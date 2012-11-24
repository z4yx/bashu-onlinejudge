#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VAL_FUCKED (-1)
#define VAL_IDENTICAL 0
#define VAL_MISMATCH 1
#define VAL_LONGER 2
#define VAL_SHORTER 3

struct validator_info validator(FILE* fstd, FILE* fuser);

struct validator_info{
	int ret;
	char* user_mismatch;
	char* std_mismatch;
};

#define should(left, op, right) \
    if(!(left op right)){ \
        puts("Failed!!!" #left #op #right);\
        printf("%s = %d, %s = %d\n\n", #left, left, #right, right); \
    }

void test_shorter()
{
    /*user output is shorter than standart output.*/
    FILE* fu = fopen("user1.txt", "r");
    FILE* fs = fopen("std1.txt", "r");
    struct validator_info info = validator(fs, fu);
    should(info.ret, ==,  VAL_SHORTER);
}

void test_longer()
{
    /*user output is shorter than longer output.*/
    FILE* fu = fopen("user2.txt", "r");
    FILE* fs = fopen("std2.txt", "r");
    struct validator_info info = validator(fs, fu);
    should(info.ret, ==,  VAL_LONGER);
}

void test_multi()
{
    FILE* fu = fopen("user_multi.txt", "r");
    FILE* fs = fopen("std_multi.txt", "r");
    struct validator_info info = validator(fs, fu);
    should(info.ret, ==,  VAL_SHORTER);
}

int main()
{
    test_shorter();
    test_longer();
    test_multi();
    return 0;
}
