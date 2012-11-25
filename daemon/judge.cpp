#include "judge_daemon.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sstream>
#include <algorithm>

#ifdef __MINGW32__
#include <windows.h>
#include <boost/thread.hpp>
namespace std {
	using boost::mutex;
	using boost::lock_guard;
	using boost::unique_lock;
}
#else
#include <mutex>
#endif

enum{CMP_tra, CMP_float, CMP_int, CMP_spj};

char DataDir[MAXPATHLEN+4], dir_name[MAXPATHLEN+16], input_filename[MAXPATHLEN+16];

extern bool lang_exist[];
extern int lang_extra_mem[];
extern std::string lang_ext[], lang_compiler[];

char target_path[MAXPATHLEN+16];
char buffer[65536];

bool clean_files()
{
	int ret;
#ifdef _WIN32
	ret = system("del /f /q /s *");
#else
	ret = system("rm -f -r *");
#endif
	return ret == 0;
}
bool solution::compile()
{
	puts("compile");
	if(!lang_exist[lang]) {
		applog("Error: Language doesn't exist.");
		throw 1;
	}
	std::string filename("target.");
	filename += lang_ext[lang];

	FILE *code_file = fopen(filename.c_str(), "wb");
	if(code_file == NULL) {
		applog("Error: Write code file failed.");
		throw 1;
	}
	
	int size = code.size();
	fwrite(code.c_str(), 1, size, code_file);
	fclose(code_file);

#ifdef _WIN32
	int ret = system((std::string("..\\win32_run_compiler.exe \"") + lang_compiler[lang] + ' ' + filename + '"').c_str());
	if(ret) {
		applog("Error: Can not run compiler");
		//printf("Can not run compiler using %s\n", (std::string("win32_run_compiler.exe ") + lang_compiler[lang] + ' ' + filename).c_str());
		throw 1;
	}
#else
	std::string command(lang_compiler[lang]);
	command += ' ' + filename + " >err.out 2>&1 && echo @~good~@ >err.out";
	//puts(command.c_str());
	system(command.c_str());
#endif
	FILE *output = fopen("err.out", "r");
	if(!output) {
		applog("Error: Can't open compiler output");
		throw 1;
	}
	int read_size = fread(buffer, 1, 65400, output);
	buffer[read_size] = '\0';
	fclose(output);	

	if(code_file = fopen("target.exe","rb")) {
		fclose(code_file);
		if(strstr(buffer, "@~good~@") == NULL) {
			applog("Info: Execute file exists, but compiler doesn't return 0.");
		}
	}else{
		if(strstr(buffer, "@~good~@") != NULL) {
			applog("Error: Compiler returned 0, but execute file doesn't exist.");
			throw 1;
		}else {
			last_state = buffer;
			score = time_limit = mem_limit = 0;
			error_code = RES_CE;
			puts("Compile Error");

			std::unique_lock<std::mutex> Lock(* (std::mutex*)mutex_for_query);
			detail_results.push_back({RES_CE, 0, 0, last_state, 0});

			return false;
		}
	}
	return true;
}
void solution::judge()
{
	puts("judge");

	sprintf(dir_name, "%s/%d", DataDir, problem);
	DIR *dp = opendir(dir_name);
	if(dp == NULL) {
		applog("Error: Can't open data dir");
		error_code = RES_SE;
		last_state = "No data files";
		throw 1;
	}
	std::vector<std::string> in_files;
	struct dirent *ep;
	ep = readdir(dp);
	while(ep) {
		int len = strlen(ep->d_name);
		if(len > 3 && 0 == strcasecmp(ep->d_name + len - 3, ".in")) {
			in_files.push_back(std::string(ep->d_name));
		}
		ep = readdir(dp);
	}
	closedir(dp);
	std::sort(in_files.begin(), in_files.end());

	int total_score = 0, total_time = 0, max_memory = 0, dir_len = strlen(dir_name);
	int status;
	std::string tips;
	for(std::string &d_name : in_files) {
		sprintf(buffer, "%s/%s", dir_name, d_name.c_str());

		puts(buffer);

		execute_info result;
		int get_score = score;

		if(run_judge(target_path, buffer, "user.out", time_limit, (lang_extra_mem[lang] + mem_limit) << 10 /*to byte*/, &result)) {
			error_code = RES_SE;
			last_state = "Cannot run target program";
			applog("Error: Cannot run target program");
			throw 1;
		}else if(result.state == 0) {
			int len = d_name.size()+dir_len+1; //dir+'/'+file
			buffer[len-2] = 'o';
			buffer[len-1] = 'u';
			buffer[len] = 't';
			buffer[len+1] = '\0';

			FILE *fanswer = fopen(buffer, "rb");
			if(fanswer) {
				FILE *foutput = fopen("user.out", "rb"), *finput;
				if(foutput) {
					validator_info info;

					switch(compare_way >> 16) {
					case CMP_tra:
						info = validator(fanswer, foutput);//traditional OI comparison (Ignore trailing space)
						break;
					case CMP_float:
						info = validator_float(fanswer, foutput, (compare_way & 0xffff)); //precision comparison
						break;
					case CMP_int:
						info = validator_int(fanswer, foutput);
						break;
					case CMP_spj:
						sprintf(input_filename, "%s/%s", dir_name, d_name.c_str());
						info = run_spj(buffer, input_filename, &get_score, dir_name);//in call_ruc.cpp
						break;
					default:
						info.ret = -1; //validator error
					}
					fclose(foutput);

					int s = info.ret;
					if(!s) {
						status = RES_AC;
						tips = "Good Job!";
						total_score += get_score;
					}else if(s == -1) {
						status = RES_VE;
						tips = "Please contact administrator.";
						get_score = 0;
					}else if(s == 4) { // for spj
						status = (get_score == score) ? RES_AC : RES_WA;
						total_score += get_score;
						tips = info.user_mismatch;
						free(info.user_mismatch);
					}else {
						status = RES_WA;
						get_score = 0;
						if(s == 1) {
							tips = "Output mismatch.\n  Your: ";
							tips += info.user_mismatch;
							tips += "\nAnswer: ";
							tips += info.std_mismatch;
							free(info.user_mismatch);
							free(info.std_mismatch);
						}
						else if(s == 2)
							tips = "Your output is longer than standard output.";
						else if(s == 3)
							tips = "Your output is shorter than standard output.";
						else //unknown result
							tips = "";
					}
				}else {
					status = RES_WA;
					get_score = 0;
					tips = "Cannot find output file.";
				}
				fclose(fanswer);
			}else {
				applog((std::string("Info: No answer file ") + buffer).c_str());
				get_score = 0;
				tips = "No answer file";
				status = RES_WA;
			}
		}else { //RE,TLE,MLE
			get_score = 0;
			status = result.state;
			if(status == RES_RE)
				tips = result.str;
			else
				tips = "";
		}
		total_time += result.time;
		if(result.memory > max_memory)
			max_memory = result.memory;

		printf("status %d %s\n", status, tips.c_str());
		if(error_code == -1 && status != RES_AC) {//only store the first error infomation
			error_code = status;
			last_state = tips;
		}

		std::unique_lock<std::mutex> Lock(* (std::mutex*)mutex_for_query);
		detail_results.push_back({status, result.time, result.memory, tips, get_score});
	}

	if(error_code == -1) //No error
		error_code = RES_AC;

	score = total_score;//use score,mem_limit,time_limit to store result
	mem_limit = max_memory;
	time_limit = total_time;
	printf("error_code %d, time %dms, memory %dkB, score %d\n", error_code, time_limit, mem_limit, score);
}
void solution::write_database()
{
	int id = get_next_solution_id();
	//printf("solution_id: %d\n", id);
	write_result_to_database(id, this);
}
