#include "INI1.26.h" // "feather-ini-parser" on googlecode
#include "judge_daemon.h"
#include "conf_items.h"
#include <sys/param.h> //for MAXPATHLEN

using std::string;

#define MAXLANG 10

int lang_extra_mem[MAXLANG];
bool lang_exist[MAXLANG];
string lang_ext[MAXLANG];
string lang_compiler[MAXLANG];  

char DATABASE_HOST[64], DATABASE_USER[128], DATABASE_PASS[128];
char HTTP_BIND_IP[32];
uint16_t HTTP_BIND_PORT;

char DataDir[MAXPATHLEN+16];

typedef INI <string, string, string> ini_t;

bool read_config()
{
	ini_t ini("config.ini", false);
	if(!ini.Parse()) {
		applog("Error: Cannot open config.ini, Exit...");
		exit(1);
	}

	ini.Select("system");
	std::string tmp = ini.Get(std::string("datadir"), std::string(""));
	if(tmp==""){
		applog("Error: We don't know your data directory.");
		return false;
	}
	strncpy(DataDir, tmp.c_str(), MAXPATHLEN);

	tmp = ini.Get(std::string("DATABASE_HOST"), std::string("localhost"));
	strncpy(DATABASE_HOST, tmp.c_str(), 62);

	tmp = ini.Get(std::string("DATABASE_USER"), std::string("root"));
	strncpy(DATABASE_USER, tmp.c_str(), 120);

	tmp = ini.Get(std::string("DATABASE_PASS"), std::string(""));
	strncpy(DATABASE_PASS, tmp.c_str(), 120);

	tmp = ini.Get(std::string("HTTP_BIND_IP"), std::string("0.0.0.0"));
	strncpy(HTTP_BIND_IP, tmp.c_str(), 30);

	HTTP_BIND_PORT = ini.Get<const char*, unsigned short>("HTTP_BIND_PORT", 8881u);

	for(auto i = ini.sections.begin(); i != ini.sections.end(); ++i) {
		const string &lang = i->first;
		if(lang.find("lang") != 0)
			continue;
		int num = atoi(lang.c_str() + 4);
		if(!num || num > MAXLANG) {
			applog("Info: Language number is not correct.");
			return false;
		}
		num--;
		std::map<string, string> &keys = *(i->second);
		if(keys.count(string("extra_mem")))
			lang_extra_mem[num] = Convert<int>(keys[string("extra_mem")]);
		else
			lang_extra_mem[num] = 0;

		if(keys.count(string("compiler")))
			lang_compiler[num] = keys[string("compiler")];
		else
			return false;

		if(keys.count(string("ext")))
			lang_ext[num] = keys[string("ext")];
		else
			return false;

		lang_exist[num] = true;
	}
	/*
	for(int i = 0; i < MAXLANG; i++) {
		if(lang_exist[i]) {
			printf("extra_mem: %d\n", lang_extra_mem[i]);
			printf("lang_compiler: %s\n", lang_compiler[i].c_str());
			printf("ext: %s\n", lang_ext[i].c_str());
		}
	}
	*/
	return true;
}