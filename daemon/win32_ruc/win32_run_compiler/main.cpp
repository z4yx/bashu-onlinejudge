#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#pragma comment(lib,"Psapi.lib")

#define compiler_max_time 20000
//#define compiler_max_memory 1024*1024*1024 //1GB
HANDLE hJOB;
HANDLE create_child_process(const char *cmd)
{
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {sizeof(si)};

	hJOB = CreateJobObject(NULL, NULL);
	if(!hJOB) {
		return NULL;
	}

	/*JOBOBJECT_SECURITY_LIMIT_INFORMATION security_info = {0};
	security_info.SecurityLimitFlags = JOB_OBJECT_SECURITY_NO_ADMIN;
	if(!SetInformationJobObject(hJOB, JobObjectSecurityLimitInformation, &security_info, sizeof security_info)) {
		CloseHandle(hJOB);
		return NULL;
	}*/
	freopen("err.out", "w", stderr);
	freopen("cmpr.out", "w", stdout);

	BOOL bSuccess = CreateProcess(NULL, (LPSTR)(void*)cmd, NULL, NULL, TRUE, CREATE_SUSPENDED | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
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
int run_compiler(const char *cmd, int *exit_code)
{
	HANDLE hChild = create_child_process(cmd);
	if(!hChild) {
		return 1;
	}
	*exit_code = 0;
	if(WAIT_TIMEOUT == WaitForSingleObject(hChild, compiler_max_time + 200)) {
		*exit_code = 2;
		TerminateJobObject(hJOB, 1);
		//WaitForSingleObject(hChild, INFINITE);
	}
	if(! *exit_code)
		GetExitCodeProcess(hChild, (LPDWORD)exit_code);

	CloseHandle(hChild);
	CloseHandle(hJOB);

	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		puts("usage: win32_run_compiler cmd");
		return 1;
	}
	int exit_code;
	int ret = run_compiler(argv[1], &exit_code);
	if(ret)
		return ret;
	if(exit_code == 0)
		fprintf(stderr, "@~good~@\n");
	fclose(stderr);
	return 0;
}
