#include "sablebox.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static struct scmask safe_syscall;

static const int sc_safe[] = {
#ifdef __x86_64__
    0,    1,      3,     5,    8,     9,     10,      11,    12,   13,          16,    19,    20,    21,      25,    63,       89,      97,       100,   158,        231
#else
    3,    4,      6,    108,   19,    90,    125,     91,    45,   174,         54,    145,   146,   33,      163,   122,      85,      76,        43,               252,         140,    192,    197,    224,     243
#endif
//read, write, close, fstat, lseek, mmap, mprotect, munmap, brk, rt_sigaction, ioctl, readv, writev, access, mremap, uname, readlink, getrlimit, times, arch_prctl, exit_group, _llseek, mmap2, fstat64, gettid, set_thread_area
};

static void init_safe_syscall()
{
	memset(&safe_syscall, 0, sizeof(struct scmask));
	int i, len = sizeof(sc_safe) / sizeof(int);
	for(i=0; i<len; i++)
		safe_syscall.set[sc_safe[i]] = SCMASK_INFINITY;
}

int main(int argc, char **argv)
{
	FILE *run_info;
	int state = 0, sys_error = 0;
	struct sablebox sbox;
	struct profile ret;
	char *file_in = NULL, *file_out = NULL, **args = NULL, isspj = 0;

	/*
	int i;
	for(i = 0; i<argc; i++)
		printf("%s ", argv[i]);
	printf("\n");
	*/

	if(argc < 6) {
		puts("usage: runner_linux program time_ms memory_byte file_in file_out");
		return 1;
	}
	init_safe_syscall();

	if(*(argv[4])) //not empty string
		file_in = argv[4];
	if(*(argv[5]))
		file_out = argv[5];
	if(argc > 7 && strcmp(argv[6], "-spj")==0) { // to run spj validator
		int cnt = argc - 7, i;
		args = (char**)malloc((cnt + 2) * sizeof(char*));
		args[0] = argv[1];
		for(i = 7; i<argc; i++)
			args[i-6] = argv[i];
		args[argc-6] = NULL;

		isspj = 1;
#ifdef __x86_64__ //sys_open
		safe_syscall.set[2] = SCMASK_INFINITY;
#else
		safe_syscall.set[5] = SCMASK_INFINITY;
#endif
	}
	dup2(fileno(stdout), fileno(stderr));

	sable_init(&sbox, argv[1], args, file_in, file_out, atol(argv[2]), atol(argv[3])>>10, &safe_syscall);
	ret = sable_run(&sbox);

	switch(ret.status){
		case PROF_SUCCESS:
			state = 0;
			break;
		case PROF_NOTSET:
			sys_error = 1;
			break;
		case PROF_RUNTIME_ERROR:
			state = 5;
			break;
		case PROF_TIME_LIMIT_EXCEED:
			state = 2;
			break;
		case PROF_MEMORY_LIMIT_EXCEED:
			state = 3;
			break;
		case PROF_SYSTEM_ERROR:
			sys_error = 1;
			break;
		default:
			sys_error = 1;
			break;
	}
	if(sys_error)
		return 2;

	if(isspj) {
		if(ret.info)
			printf("spj: %s\n", ret.info);
		return (state == 0) ? 0 : 10+state;//whether validator exited successfully
	}
	run_info = fopen("run.info", "w");
	if(!run_info)
		return 3;

	fprintf(run_info, "Time = %ld\n", ret.time);
	fprintf(run_info, "Memory = %ld\n", ret.memory);
	fprintf(run_info, "State = %d\n", state);
	if(ret.info)
		fprintf(run_info, "Info = %s\n", ret.info);
	fclose(run_info);

	return 0;
}
