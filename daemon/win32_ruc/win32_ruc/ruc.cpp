#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <string>
#pragma comment(lib,"Psapi.lib")

const char INFO_SEGMENT_FAULT[]="Segmentation fault";
const char INFO_DIVIDE_BY_ZERO[]="Divide by zero";
const char INFO_STACK_OVERFLOW[]="Stack overflow";
const char INFO_NO_MEMORY[]="No memory";
const char INFO_UNKNOWN_REASON[]="Unknown reason";

struct execute_info
{
	int state, time, memory;
	const char *info;
};
HANDLE hJOB;
HANDLE create_child_process(const char *cmd, const char *file_in, const char *file_out, LONGLONG ms, SIZE_T memory_limit)
{
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {sizeof(si)};

	hJOB = CreateJobObject(NULL, NULL);
	if(!hJOB) {
		return NULL;
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit = {0};
	JOBOBJECT_BASIC_LIMIT_INFORMATION &basic_limit = limit.BasicLimitInformation;

	basic_limit.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_TIME | JOB_OBJECT_LIMIT_PROCESS_MEMORY | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION ;
	limit.ProcessMemoryLimit = memory_limit;
	basic_limit.PerProcessUserTimeLimit.QuadPart = ms*10000;

	if(!SetInformationJobObject(hJOB, JobObjectExtendedLimitInformation, &limit, sizeof limit)) {
		CloseHandle(hJOB);
		return NULL;
	}

	/*JOBOBJECT_SECURITY_LIMIT_INFORMATION security_info = {0};
	security_info.SecurityLimitFlags = JOB_OBJECT_SECURITY_NO_ADMIN;
	if(!SetInformationJobObject(hJOB, JobObjectSecurityLimitInformation, &security_info, sizeof security_info)) {
		CloseHandle(hJOB);
		return NULL;
	}*/
	if(*file_in)
		freopen(file_in, "r", stdin);
	if(*file_out)
		freopen(file_out, "w", stdout);
	freopen("user.err", "w", stderr);

	BOOL bSuccess = CreateProcessA(NULL, (LPSTR)(void*)cmd, NULL, NULL, TRUE, CREATE_SUSPENDED|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi);
	if(!bSuccess) {
		CloseHandle(hJOB);
		return NULL;
	}
	if(!AssignProcessToJobObject(hJOB, pi.hProcess)) {
		CloseHandle(hJOB);
		CloseHandle(pi.hThread);
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);
		return NULL;
	}
	if(-1 == ResumeThread(pi.hThread)) {
		CloseHandle(hJOB);
		CloseHandle(pi.hThread);
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);
		return NULL;
	}
	CloseHandle(pi.hThread);
	return pi.hProcess;
}
void get_time_memory(HANDLE hChild, execute_info *usage)
{
	/*FILETIME CreateTime, ExitTime, KernalTime, UserTime;
	GetProcessTimes(hChild, &CreateTime, &ExitTime, &KernalTime, &UserTime)) {
		usage->time = 1;
	}else {
		ULONGLONG tmp;
		tmp = ((ULONGLONG)UserTime.dwHighDateTime << 32) | UserTime.dwLowDateTime;
		usage->time = tmp/10000;
	}*/
	JOBOBJECT_BASIC_ACCOUNTING_INFORMATION job_info;
	if(!QueryInformationJobObject(hJOB, JobObjectBasicAccountingInformation, &job_info, sizeof(job_info), NULL)) {
		usage->time = 1;
	}else {
		usage->time = job_info.TotalUserTime.QuadPart / 10000;
	}


	PROCESS_MEMORY_COUNTERS mem_usage;
	mem_usage.cb = sizeof(PROCESS_MEMORY_COUNTERS);
	if(!GetProcessMemoryInfo(hChild, &mem_usage, mem_usage.cb)) {
		usage->memory = 0;
	}else {
		usage->memory = (mem_usage.PeakWorkingSetSize + mem_usage.PeakPagefileUsage) >> 10;////
	}

}
bool running = true;
DWORD state = 0;
VOID CALLBACK time_out_callback(PVOID, BOOLEAN)
{
	if(running) {
		running = false;
		state = 2;
		TerminateJobObject(hJOB, 1);
	}
}
int run_judge(const char *cmd, const char *file_in, const char *file_out, int time_limit, int mem_limit, execute_info *usage)
{
	HANDLE hChild = create_child_process(cmd, file_in, file_out, time_limit, mem_limit);
	if(!hChild) {
		return 2;
	}

	HANDLE hTimer;

	if(!CreateTimerQueueTimer(&hTimer, NULL, time_out_callback, 0, time_limit+100, 0, WT_EXECUTEDEFAULT)) {
		fprintf(stderr, "Failed to create timer, GetLastError()=%d", GetLastError());
		return 3;
	}
	usage->info = " ";
	for(; running;) {
		DEBUG_EVENT de;
		if(WaitForDebugEvent(&de, 400)) {
			//fprintf(stderr, "%d\n", de.dwDebugEventCode);
			switch(de.dwDebugEventCode) {
				case CREATE_PROCESS_DEBUG_EVENT:
					CloseHandle(de.u.CreateProcessInfo.hFile);
					break;
				case LOAD_DLL_DEBUG_EVENT:
					CloseHandle(de.u.LoadDll.hFile);
					break;
				case EXIT_PROCESS_DEBUG_EVENT:
					running = false;
					break;
				case EXCEPTION_DEBUG_EVENT:
					//fprintf(stderr, "ExceptionCode %u\n", de.u.Exception.ExceptionRecord.ExceptionCode);
					switch(de.u.Exception.ExceptionRecord.ExceptionCode) {
						case EXCEPTION_SINGLE_STEP:
						case EXCEPTION_BREAKPOINT:
							break;
						case EXCEPTION_ACCESS_VIOLATION:
						case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
						case EXCEPTION_ILLEGAL_INSTRUCTION:
						case EXCEPTION_IN_PAGE_ERROR:
						case EXCEPTION_PRIV_INSTRUCTION:
							state = 5;
							usage->info = INFO_SEGMENT_FAULT;
							TerminateJobObject(hJOB, 1);
							running = false;
							break;
						case EXCEPTION_INT_DIVIDE_BY_ZERO:
							state = 5;
							usage->info = INFO_DIVIDE_BY_ZERO;
							TerminateJobObject(hJOB, 1);
							running = false;
							break;
						case EXCEPTION_STACK_OVERFLOW:
							state = 5;
							usage->info = INFO_STACK_OVERFLOW;
							TerminateJobObject(hJOB, 1);
							running = false;
							break;
						case STATUS_NO_MEMORY:
							state = 3;
							usage->info = INFO_NO_MEMORY;
							TerminateJobObject(hJOB, 1);
							running = false;
							break;
						default:
							state = 5;
							usage->info = INFO_UNKNOWN_REASON;
							TerminateJobObject(hJOB, 1);
							running = false;
							break;
					}
					break;
			}
			ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE);
			continue;
		}		
		if(WaitForSingleObject(hChild, 0) == WAIT_OBJECT_0)
			running = false;
	}
	DeleteTimerQueueTimer(NULL, hTimer, NULL);
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);

	get_time_memory(hChild, usage);
	if(state == 0 && usage->memory > (mem_limit>>10))
		state = 3;

	if(state == 0 && usage->time > time_limit)
		state = 2;

	/*if(state == 0) { //No TLE,MLE,RE
		GetExitCodeProcess(hChild, &state);
		if(state)
			state = 5;
	}*/
	CloseHandle(hChild);
	CloseHandle(hJOB);

	usage->state = state;
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 6) {
		puts("usage: ruc program time_ms memory_byte file_in file_out");
		return 1;
	}
	int time_ms = atol(argv[2]);
	int memory = atol(argv[3]);
	if(argc>7 && strcmp("-spj", argv[6])==0) {
		std::string cmd(argv[1]);
		for(int i=7; i<argc; i++) {
			cmd += ' ';
			cmd += argv[i];
		}
		execute_info info;
		int ret = run_judge(cmd.c_str(), argv[4], argv[5], time_ms, memory, &info);
		if(ret || info.state)
			return 1;
		else
			return 0;
	}else {
		execute_info info;
		int ret = run_judge(argv[1], argv[4], argv[5], time_ms, memory, &info);
		if(ret)
			return ret;
		freopen("run.info", "w", stderr);
		fprintf(stderr, "Time = %d\n", info.time);
		fprintf(stderr, "Memory = %d\n", info.memory);
		fprintf(stderr, "State = %d\n", info.state);
		fprintf(stderr, "Info = %s\n", info.info);

		fclose(stderr);
	}
	return 0;
}
