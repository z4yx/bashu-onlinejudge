#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "judge_daemon.h"

#ifdef _WIN32
#include <windows.h>
#define RUC_NAME "..\\win32_ruc "
/*characters cmd.exe interprets in a command line*/
#define SHELL_SPECIAL_CHARS "\n\r\v\f&|<>^%!\"*?"
#else
#define RUC_NAME "../runner_linux "
/*characters the shell interprets in a command line*/
#define SHELL_SPECIAL_CHARS "\n\r\v\f|&$`\\!\"*?"
#endif

#define SPJ_MAX_TIME 5000
#define SPJ_MAX_MEM 1073741824
#define MAX_REPORT_LENGTH 64

/*A file name containing a shell special character could break out of the
  command line built below and inject an arbitrary command, so such names are
  refused.  Whitespace and control characters are included as well: some of
  the names are handed to the shell without any quoting.*/
static bool has_special_char(const char *name)
{
	if(!name)
		return false;
	return strpbrk(name, SHELL_SPECIAL_CHARS) != NULL;
}

/*Check a file name before it is put into a command line.  The reason is
  logged when the name cannot be used; false tells the caller to abort.*/
static bool check_filename(const char *what, const char *name)
{
	if(!has_special_char(name))
		return true;
	char message[512];
	snprintf(message, sizeof(message),
		"Error: Special character in %s \"%s\", possible command injection",
		what, name);
	applog(message);
	return false;
}

int run_judge(const char *target, const char *input, const char *output, int time, int mem, execute_info* info)
{
	std::ostringstream command;
	static char info_str[128];

	if(!check_filename("target program", target)
		|| !check_filename("input file name", input)
		|| !check_filename("output file name", output))
		return 1;

	command << RUC_NAME << target << ' ' << time 
		<< ' ' << mem << " \"" << input << "\" \"" << output << "\"";
	const std::string &str = command.str();
	//puts(str.c_str());
	int ret = system(str.c_str());
	if(ret) {
		sprintf(info_str, "Error: RUC exited with %d", ret);
		applog(info_str);
		return ret;
	}
	FILE *run_info = fopen("run.info", "r");
	char type[20];
	int val;
	if(!run_info) {
		applog("Can't open run.info");
		return 1;
	}
	info->str = NULL;
	while(fscanf(run_info, "%s%*s", type) > 0) {
		if(!strcmp(type, "Info")) {
			fgets(info_str, 100, run_info);
			info->str = info_str;
		}else if(!strcmp(type, "Time")) {
			fscanf(run_info, "%d", &val);
			info->time = val;
		}else if(!strcmp(type, "Memory")) {
			fscanf(run_info, "%d", &val);
			info->memory = val;
		}else if(!strcmp(type, "State")) {
			fscanf(run_info, "%d", &val);
			info->state = val;
		}
	}
	
	fclose(run_info);
	return ret;
}
validator_info run_spj(char *datafile_out, char *datafile_in, int *score, char *data_dir)
{
	std::ostringstream cmd;
	validator_info info;
	info.ret = -1;

	if(!check_filename("answer file name", datafile_out)
		|| !check_filename("input file name", datafile_in)
		|| !check_filename("data directory", data_dir))
		return info;	/*info.ret stays -1: validator error*/

#ifdef _WIN32
	system("del /f /q /s score.log report.log");
	CopyFile(datafile_in, "user.in", 0);
	CopyFile(datafile_out, "std.ans", 0);

	cmd << RUC_NAME << data_dir << "\\spj.exe " << SPJ_MAX_TIME << ' ' << SPJ_MAX_MEM << " \"\" \"\" -spj " << *score << " std.ans";
#else
	system("rm -f -r score.log report.log");
	system((std::string("cp \"")+datafile_in+"\" user.in").c_str());
	system((std::string("cp \"")+datafile_out+"\" std.ans").c_str());
	cmd << "test -x spj.exe || g++ \"" << data_dir << "/spj.cpp\" -o spj.exe";
	if(system(cmd.str().c_str())) {
		applog("Error: Cannot find spj");
		return info;
	}
	cmd.str("");
	cmd << RUC_NAME << "./spj.exe " << SPJ_MAX_TIME << ' ' << SPJ_MAX_MEM << " '' '' -spj " << *score << " std.ans";
#endif

	int ret = system(cmd.str().c_str());
	if(ret) {
		char msg[64];
		sprintf(msg, "Error: RUC for spj exited with %d", ret);
		applog(msg);
		return info;
	}

	{
		FILE *fscore = fopen("score.log", "r");
		if(!fscore) {
			applog("Error: Cannot open score.log");
			return info;
		}

		double t;
		if(1 != fscanf(fscore, "%lf", &t)) {
			applog("Error: Invalid score.log");
			fclose(fscore);
			return info;
		}
		info.ret = 4; //must be 4 for spj
		*score = (int)t;
		fclose(fscore);
	}

	{
		info.user_mismatch = (char *)malloc(MAX_REPORT_LENGTH);
		info.user_mismatch[0] = '\0';
		FILE *freport = fopen("report.log", "r");
		if(freport) {
			fgets(info.user_mismatch, MAX_REPORT_LENGTH, freport);
			fclose(freport);
		}
	}

	return info;
}
