#include "sablebox.h"
#include <stdlib.h>
#include <stdio.h>

#define CASE(STA)\
	case STA:	\
		puts( #STA );

#define deftest(name, pathname, fin, fout) \
int test_##name ()	\
{\
	if(fork() == 0){\
		struct sablebox sbox;\
		struct profile ret;\
		puts("------------" #name "-------------------\n");\
		sable_init(&sbox, #pathname, NULL, #fin, #fout, 1000, 1024*50, 0);\
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
		puts("----------------------------------------\n");\
		exit(0);\
	}else{\
		wait(0);\
	}\
}
deftest(re_seg, ./re_seg, t.in, t.out)
deftest(re_div0, ./re_div0, t.in, t.out)
deftest(helloworld, ./helloworld, t.in, t.out)
deftest(mle_static, ./mle_static, t.in, t.out)
deftest(mle_dyn, ./mle_dyn, t.in, t.out)
deftest(1003re, ./1003re, 1003.in, 1003.out)
deftest(1500ac, ./1500ac, 1500.in, 1500.out)
deftest(2000mle, ./2000mle, 2000.in, 2000.out)
deftest(tle, ./tle, t.in, t.out)
deftest(block, ./block, t.in, t.out)
deftest(futex_test, ./futex_test, 3172.in, t.out)

int main(int argc, char** argv)
{
	test_tle();
	test_helloworld();
	test_re_div0();
	test_re_seg();
	test_mle_static();
	test_mle_dyn();
	test_1003re();
	test_1500ac();
	test_2000mle();
    test_block();
    test_futex_test();
	return 0;
}
