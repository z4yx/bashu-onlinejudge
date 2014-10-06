#ifndef CONF_ITEM_H_
#define CONF_ITEM_H_

//Configure variables defined in readconf.cpp

extern char DATABASE_HOST[], DATABASE_USER[], DATABASE_PASS[];
extern char HTTP_BIND_IP[];
extern uint16_t HTTP_BIND_PORT;

extern bool lang_exist[];
extern int lang_extra_mem[];
extern std::string lang_ext[], lang_compiler[];

extern char DataDir[];

#endif
