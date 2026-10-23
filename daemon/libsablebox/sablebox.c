/*
	Core implementation of sablebox.
	Author: Yang Zhixuan
    Date: Sept. 22nd, 2012

 */

#include "sablebox.h"
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/reg.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include "syscallnum.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

//#define PRINT_FOR_DEBUG
//#define ALLOW_FOR_DEBUG

/********************default syscall set******************************/
static struct scmask MINIMAL_SCSET;

static const int sc_safe[] = {
    __NR_read, __NR_write, __NR_open, __NR_close, __NR_fstat, __NR_lseek, __NR_mmap, __NR_mprotect, 
    __NR_munmap, __NR_brk, __NR_rt_sigaction, __NR_ioctl, __NR_readv, __NR_writev, __NR_access,
    __NR_mremap, __NR_uname, __NR_readlink, __NR_getrlimit, __NR_times, __NR_arch_prctl, __NR_futex, 
    __NR_exit_group, __NR_gettid, __NR_set_thread_area, __NR_set_tid_address, __NR_set_robust_list,
    __NR_rt_sigprocmask, __NR_time
};

static void init_defualt_scmask()
{
	memset(&MINIMAL_SCSET, 0, sizeof(struct scmask));
	int i, len = sizeof(sc_safe) / sizeof(int);
	for(i=0; i<len; i++)
		MINIMAL_SCSET.set[sc_safe[i]] = SCMASK_INFINITY;
}
/**********************************************************************/


/********************sable_init****************************************/
int sable_init(struct sablebox* pbox, 
					const char* path,
					char** args,
					const char* fin,
					const char* fout,
					long cpu,
					long memory,
					struct scmask* scp)
{
	static int inited = 0;
	if(!inited)
		init_defualt_scmask();

	pbox->pathname = path;
	pbox->args = args;
	pbox->cpu_lim = cpu;
	pbox->memory_lim = memory;
	pbox->fin = fin;
	pbox->fout = fout;

	if(scp)
		pbox->scp = scp;
	else
		pbox->scp = &MINIMAL_SCSET;

	return 0;
}
/**********************************************************************/





/******************* sable_run ****************************************/


/***** helper data structures ******************************/
static jmp_buf my_jmpbuf;
struct __child_t{
	pid_t pid;
	int dead;
	int insyscall;
	long program_break;
};
/**********************************************************/

/*******prototypes of helper functions*********************/
static void set_resource_limits(struct sablebox* pbox);	
static void redirect_io(struct sablebox* pbox);			 
static void setup_listener(struct __child_t* pson);
static void message_loop(struct sablebox* pbox, struct __child_t* pson
												, struct profile* ret);
/**********************************************************/

struct profile sable_run(struct sablebox* pbox)
{
	struct profile ret;
	struct __child_t son;

	ret.status = PROF_NOTSET;
	ret.time = ret.memory = 0;
	ret.info = NULL;
	ret.exit_code = 0;

	/* simple exception handler*/
	if(setjmp(my_jmpbuf)){
		ret.status = PROF_SYSTEM_ERROR; 
		return ret;
	};

	if(son.pid = fork()){
		/*parent process*/
		stderr = stdout;
		
		son.dead = son.insyscall = son.program_break = 0;
		setup_listener(&son);
		message_loop(pbox, &son, &ret);
		//printf("Hello %d\n\n", ret.status);
		return ret;

	}else{
		/*child process*/

		set_resource_limits(pbox);
		redirect_io(pbox);
		
		if(ptrace(PTRACE_TRACEME, 0, 0, 0)){
			perror("ptrace:"TOSTRING(__LINE__));
			_exit(1);
		}

		/*stop to wait parent getting the listener ready*/
		kill(getpid(), SIGSTOP);

		if(!pbox->args){
			char *tmp[2]={pbox->pathname, NULL};
			execvp(pbox->pathname, tmp);
		}else{		
			execvp(pbox->pathname, pbox->args);
		}
		
		/*exec failed*/
		_exit(1);
	}
}

/**********************************************************************/

/********* helper functions for sable_run, child process *************/
static void set_resource_limits(struct sablebox* pbox)
{
	struct rlimit rlbuf;
	int ret = 0;

	/*CPU time limit, one more second allowed for accuracy*/
	rlbuf.rlim_cur = rlbuf.rlim_max = (pbox->cpu_lim + 999) / 1000;
	ret += setrlimit(RLIMIT_CPU, &rlbuf);

	/*Memory limit*/
	rlbuf.rlim_cur = rlbuf.rlim_max = (pbox->memory_lim << 10);
	ret += setrlimit(RLIMIT_AS, &rlbuf);

	if(ret)
		/*setting resource limits failed*/
		_exit(0);
}

static void redirect_io(struct sablebox* pbox)
{
	if((pbox->fin && !freopen(pbox->fin, "r", stdin))
		|| (pbox->fout && !freopen(pbox->fout, "w", stdout)))
			_exit(0);
	freopen("/dev/null", "w", stderr);
}

/**********************************************************************/




/*********** helper functions for sable_run, parent process ***********/
#define THROW_SYSERR ptrace(PTRACE_KILL, pson->pid, 0, 0), longjmp(my_jmpbuf, 1)
static void setup_listener(struct __child_t* pson)
{
	int status;

	if(waitpid(pson->pid, &status, 0) < 0){
		perror("waitpid:"TOSTRING(__LINE__));
		THROW_SYSERR;
	}

	/*check it is the first STOP signal sent by the child*/
	if(!((WIFSTOPPED(status)) && ((WSTOPSIG(status)) == SIGSTOP)))
		THROW_SYSERR;

	if(ptrace(PTRACE_SETOPTIONS, pson->pid, 0, PTRACE_O_TRACESYSGOOD | PTRACE_EVENT_EXEC)){
		perror("ptrace:"TOSTRING(__LINE__));
		THROW_SYSERR;
	}

	
	/*resume the child*/
	if(ptrace(PTRACE_SYSCALL, pson->pid, 0, 0)){
		perror("ptrace:"TOSTRING(__LINE__));
		THROW_SYSERR;
	}
}


static const char* scnames[] = {
	#include "sable-syscallent.h"
};

/*number of names in the table above: it is shorter than MAX_SYSCALL_NUM, so
  it must be bounds checked before being indexed*/
#define SCNAMES_COUNT	(sizeof(scnames) / sizeof(scnames[0]))

/*name of a syscall; a generic name is returned for the numbers the table does
  not know (syscalls newer than the table, e.g. rseq, are most of them)*/
static const char* scname(long scno)
{
	if(scno < 0 || (unsigned long)scno >= SCNAMES_COUNT || !scnames[scno])
		return INFO_UNKNOWN_SYSCALL;
	return scnames[scno];
}

/*********** helper functions for telling the syscall ABI *************/
/*ptrace request number of PTRACE_GET_SYSCALL_INFO, available since Linux 5.3.
  Request numbers belong to the kernel ABI, so it is defined here to keep the
  code compilable with old system headers.*/
#ifndef PTRACE_GET_SYSCALL_INFO
#define PTRACE_GET_SYSCALL_INFO		0x420e
#endif

/*AUDIT_ARCH_X86_64, i.e. EM_X86_64 | __AUDIT_ARCH_64BIT | __AUDIT_ARCH_LE*/
#define SABLE_AUDIT_ARCH_X86_64		0xc000003eU

/*values of the op field of struct sable_syscall_info*/
#define SABLE_SYSCALL_INFO_NONE		0

/*__USER_CS, code segment selector of a 64-bit user process*/
#define SABLE_USER_CS_64			0x33

/*Minimal image of the kernel's struct ptrace_syscall_info.*/
struct sable_syscall_info {
	unsigned char op;
	unsigned int arch __attribute__((aligned(4)));
	unsigned long long instruction_pointer;
	unsigned long long stack_pointer;
	union {
		struct {
			unsigned long long nr;
			unsigned long long args[6];
		} entry;
		struct {
			long long rval;
			unsigned char is_error;
		} exit;
		struct {
			unsigned long long nr;
			unsigned long long args[6];
			unsigned int ret_data;
		} seccomp;
	} u;
};

/*Read the two bytes of the instruction a 64-bit process used to enter the
  kernel and tell whether it is the 64-bit entry (0f 05, i.e. syscall) rather
  than one of the 32-bit entries (cd 80, i.e. `int $0x80', or 0f 34, i.e.
  sysenter).  Returns 1 for the 64-bit entry, 0 for the others and -1 when the
  bytes cannot be read.  Called on kernels that cannot tell the ABI by
  themselves (see below).*/
static int sc_insn_is_64bit(pid_t pid)
{
	unsigned long rip, word, addr, offset;
	unsigned char bytes[2];
	int i;

	errno = 0;
	rip = ptrace(PTRACE_PEEKUSER, pid, RIP * 8, 0);
	if(rip == (unsigned long)-1 && errno)
		return -1;

	/*every one of these instructions is 2 bytes long and may begin in the
	  page before the one RIP points to, so the bytes are fetched one by one*/
	for(i = 0; i < 2; i++){
		addr = (rip - 2 + i) & ~(sizeof(long) - 1);
		offset = (rip - 2 + i) & (sizeof(long) - 1);
		errno = 0;
		word = ptrace(PTRACE_PEEKTEXT, pid, (void*)addr, 0);
		if(word == (unsigned long)-1 && errno)
			return -1;
		bytes[i] = (word >> (8 * offset)) & 0xff;
	}
	return bytes[0] == 0x0f && bytes[1] == 0x05;
}

/*Tell whether the syscall the child has just entered is issued through the
  32-bit (i386) system call ABI.
  Such a syscall is either made by a program running in 32-bit compatibility
  mode, or by a 64-bit program using `int $0x80'.  Both of them must be
  rejected: the syscall numbers stored in the mask are the 64-bit ones, and a
  32-bit number means something completely different (i386's open() is 5,
  which is x86_64's fstat()), so a black-listed 32-bit syscall would sneak
  through the mask under the name of a harmless 64-bit one.*/
static int is_32bit_syscall(pid_t pid)
{
	struct sable_syscall_info info;
	long ret;

	/*asking the kernel for the ABI of the current syscall; this is the only
	  reliable way to catch `int $0x80' issued by a 64-bit process, because
	  such a process still runs with the 64-bit code segment*/
	memset(&info, 0, sizeof(info));
	ret = ptrace(PTRACE_GET_SYSCALL_INFO, pid, (void*)sizeof(info), &info);
	if(ret > 0 && info.op != SABLE_SYSCALL_INFO_NONE)
		return info.arch != SABLE_AUDIT_ARCH_X86_64;

	/*kernels older than 5.3 know nothing about PTRACE_GET_SYSCALL_INFO, so
	  fall back to the code segment selector: it is 0x33 for 64-bit code and
	  __USER32_CS (0x23) while the process runs in 32-bit compatibility mode*/
	errno = 0;
	ret = ptrace(PTRACE_PEEKUSER, pid, CS * 8, 0);
	if(ret == -1 && errno)
		return 0;	/*the child is gone, nothing to decide*/
	if((unsigned long)ret != SABLE_USER_CS_64)
		return 1;

	/*the 64-bit code segment does not prove a 64-bit syscall: `int $0x80'
	  and `sysenter' enter the 32-bit ABI without switching the segment, so
	  on those kernels the instruction itself is the only clue left*/
	ret = sc_insn_is_64bit(pid);
	if(ret < 0)
		return 1;	/*unreadable instruction, refuse it to be safe*/
	return !ret;
}
/**********************************************************************/

static int sc_handle(struct sablebox* pbox, struct __child_t* pson, struct profile* ret)
{
	static long scno = -1;
	if(pson->insyscall){
		pson->insyscall = 0;
		if(scno == __NR_brk){
			long newbrk;
			newbrk = ptrace(PTRACE_PEEKUSER, pson->pid, RAX * 8, 0); 
			if(newbrk == -1){
				if(errno != ESRCH){
					perror("ptrace:"TOSTRING(__LINE__));
					ret->status = PROF_SYSTEM_ERROR;
				}
				return 1;
			}		
			if(pson->program_break)
				ret->memory += newbrk - pson->program_break;
			pson->program_break = newbrk;
		}// else if(scno == __NR_open){
		// 	ptrace(PTRACE_POKEUSER, pson->pid, RAX * 8, EACCES);
		// }
	}else{
		pson->insyscall = 1;

		/*a 32-bit mode syscall is refused whatever its number is, before
		  looking at the syscall number at all*/
		if(is_32bit_syscall(pson->pid)){
			ret->status = PROF_RUNTIME_ERROR;
			ret->info = INFO_32BIT_SYSCALL;
			return 1;
		}

		scno = ptrace(PTRACE_PEEKUSER, pson->pid, ORIG_RAX * 8, 0);
		if(scno == -1 || scno >= MAX_SYSCALL_NUM){
			if(errno != ESRCH){
				perror("ptrace:"TOSTRING(__LINE__));
				ret->status = PROF_SYSTEM_ERROR;
			}
			return 1;
		}

#ifdef PRINT_FOR_DEBUG
		printf("Call %ld %s\n", scno, scname(scno));
#endif
#ifndef ALLOW_FOR_DEBUG
		if(!pbox->scp->set[scno]){
			if(scno == __NR_open) {
				// int i;
				long addr;
				union{
					long x;
					char c[8];
				}data;
				// puts("call open");
				addr = ptrace(PTRACE_PEEKUSER, pson->pid, RDI * 8, 0);
				// printf("addr %ld\n", addr);
				data.x = ptrace(PTRACE_PEEKDATA, pson->pid, addr, 0);
				// printf("char " );
				// for(i=0; i<sizeof(long); i++)
					// putchar(data.c[i]);
				// puts("");
				data.c[0] = '\0';
				if(addr != -1 && data.x != -1 && ptrace(PTRACE_POKEDATA, pson->pid, addr, data.x) != -1){
					puts("modify the first argument of open");
				}
			}else {
				ret->status = PROF_RUNTIME_ERROR;
				ret->info = scname(scno);
				return 1;
			}
		}
#endif
		if(pbox->scp->set[scno] > 0)
			pbox->scp->set[scno]--;
	}
	return 0;
}


int to_microseconds(struct timeval val)
{
	return val.tv_sec * 1000 + val.tv_usec/1000;
}


/***************WARNING! THIS PART MAY BE NOT PORTABLE!!!**********************/
/*Use size utils to count static memory.*/
long static_memory(const char* p)
{
	char cmdbuf[4096];
	sprintf(cmdbuf, "size %s | awk '{if(NR==2)print $4;}'\n", p);
	FILE* pipe = popen(cmdbuf, "r");
	if(!pipe)
		return 0;
	long ret;
	fscanf(pipe, "%ld", &ret);
	return ret;
}

/***************WARNING! THIS PART MAY BE NOT PORTABLE!!!**********************/


static void profile_info(struct sablebox* pbox, struct __child_t* pson, 
									int exit_status, struct profile* ret)
{
	static long last_cpu = 0;

	struct rusage rusage_buf;
	getrusage(RUSAGE_CHILDREN, &rusage_buf);
	
	long new_cpu =  to_microseconds(rusage_buf.ru_utime) 
					+ to_microseconds(rusage_buf.ru_stime);
	ret->time += new_cpu - last_cpu;
	last_cpu = new_cpu;

	ret->memory += static_memory(pbox->pathname);
	ret->memory >>= 10;

	if(ret->status != PROF_NOTSET)
		return;
	
	if(ret->time > pbox->cpu_lim)
		ret->status = PROF_TIME_LIMIT_EXCEED;
	else if(ret->memory > pbox->memory_lim)
		ret->status = PROF_MEMORY_LIMIT_EXCEED;
	else if(WIFSIGNALED(exit_status)){
		int sig = WTERMSIG(exit_status);
		if(sig == SIGXCPU || sig == SIGKILL)
			ret->status = PROF_TIME_LIMIT_EXCEED;
		else{
			ret->status = PROF_RUNTIME_ERROR;
			if(sig == SIGFPE)
				ret->info = INFO_MATH_FAULT;
			else if(sig == SIGSEGV || sig == SIGBUS)
				ret->info = INFO_SEGMENT_FAULT;
			else
				ret->info = INFO_SIGNAL;
		}
	}else{
		ret->status = PROF_SUCCESS;
		ret->exit_code = WEXITSTATUS(exit_status);
	}

}

static void message_loop(struct sablebox* pbox, struct __child_t* pson, struct profile* ret)
{
	int status, sig;

	/*execve will be called once by the system*/
	if(pbox->scp->set[__NR_execve] != SCMASK_INFINITY)
		pbox->scp->set[__NR_execve]++;

	while(!pson->dead){
		sig = 0;
		if(waitpid(pson->pid, &status, 0) < 0){
			perror("waitpid:"TOSTRING(__LINE__));
			THROW_SYSERR;
		}

		if(WIFSTOPPED(status)){
			switch(WSTOPSIG(status)){
				case SIGTRAP | 0x80:
					/*child made or returned from a syscall*/
					if(sc_handle(pbox, pson, ret)){
						ptrace(PTRACE_KILL, pson->pid, 0, 0);
						pson->dead = 1;
					}
					break;
				case SIGTRAP:
					break;
				default:
					/*otherwise, we pass the signal to child*/
					sig = WSTOPSIG(status);
					break;
			}
		}else if(WIFEXITED(status) || WIFSIGNALED(status)){
			/*child has exited*/
			pson->dead = 1;
		}else{
			sig = WSTOPSIG(status);
		}

		/*resume the child*/
		ptrace(PTRACE_SYSCALL, pson->pid, 0, sig);
	}
	
	profile_info(pbox, pson, status, ret);
}
#undef THROW_SYSERR
/**********************************************************************/
