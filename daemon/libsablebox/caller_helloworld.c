#include "sablebox.h"
#include <stdio.h>

#define CASE(STA)\
	case STA:	\
		puts( #STA );

#define deftest(pathname) \
int test##pathname ()	\
{\
	struct sablebox sbox;\
	struct profile ret;\
	sable_init(&sbox, #pathname, "t.in", "t.out", 3000, 1024*30, 0);\
	ret = sable_run(&sbox);\
	switch(ret.status){\
		CASE(PROF_SUCCESS)\
			break;\
		CASE(PROF_NOTSET)\
			break;\
		CASE(PROF_RUNTIME_ERROR)\
			printf("%s\n", ret.info);\
			break;\
		CASE(PROF_TIME_LIMIT_EXCEED)\
			break;\
		CASE(PROF_MEMORY_LIMIT_EXCEED)\
			break;\
		CASE(PROF_SYSTEM_ERROR)\
			break;\
	}\
	printf("Memory: %ld\nTime: %ld\n", ret.memory, ret.time);\
	puts("");\
}
deftest(re_seg)
deftest(re_div0)

int main(int argc, char** argv)
{
	testre_div0();
	testre_seg();
	return 0;
}
