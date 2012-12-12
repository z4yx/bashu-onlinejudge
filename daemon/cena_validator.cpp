/*
from: https://cena.googlecode.com/svn/trunk/lib/diffu.pas
translated into C by zyx
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct validator_info{
	int ret;
	char* user_mismatch;
	char* std_mismatch;
};


#define VAL_FUCKED (-1)
#define VAL_IDENTICAL 0
#define VAL_MISMATCH 1
#define VAL_LONGER 2
#define VAL_SHORTER 3

static int fs[2], line, col[2];
static FILE *f[2];
struct Tchar
{
	bool have, he;
	int eml;
	char c;
}chars[2];

void GetChar(int fi)
{
	char ch, last;
	last = 0;
	for(;;) {
		ch = fgetc(f[fi]);
		if(ch == EOF)
			return;
		if(ch == 10) {
			if(last != 13)
				chars[fi].eml++;
			col[fi] = 0;
			chars[fi].he = false;
		}else if(ch == 13) {
			chars[fi].eml++;
			col[fi] = 0;
			chars[fi].he = false;
		}else {
			col[fi]++;
			if(ch==' ') {
				if(chars[fi].eml == 0)
					chars[fi].he = true;
			}else {
				chars[fi].c = ch;
				break;
			}
		}
		last = ch;
	}
	chars[fi].have = true;
	if(fi == 0)
		line += chars[fi].eml;
}
int CheckNow()
{
	int result;
	col[0] = col[1] = fs[0] = fs[1] = line = 0;
	memset(chars, 0, sizeof(chars));

	result = VAL_IDENTICAL;
	chars[0].eml = chars[1].eml = 1;

	for(;;) {
		GetChar(0);
		GetChar(1);

		if(!chars[0].have && !chars[1].have)
			return result;

		if(chars[0].have && !chars[1].have) {
			result = VAL_LONGER;
			return result;
		}
		if(!chars[0].have && chars[1].have) {
			result = VAL_SHORTER;
			return result;
		}
		if(chars[0].eml > chars[1].eml) {
			result = VAL_MISMATCH;
			return result;
		}
		if(chars[1].eml > chars[0].eml) {
			result = VAL_MISMATCH;
			return result;
		}
		if(chars[0].he != chars[1].he || chars[0].c != chars[1].c) {
			result =VAL_MISMATCH;
			return result;
		}

		memset(chars, 0 ,sizeof(chars));
	}
	return VAL_FUCKED;
}

struct validator_info validator_cena(FILE* fstd, FILE* fuser)
{
	validator_info result;

	f[0] = fuser;
	f[1] = fstd;
/*	
	if(!fstd || fseek(fstd, 0, SEEK_END) || (fs[1]=ftell(fstd))==-1)
		return VAL_FUCKED;
	if(!fuser || fseek(fuser, 0, SEEK_END) || (fs[0]=ftell(fuser))==-1)
		return VAL_FUCKED;

	rewind(fstd);
	rewind(fuser);
*/
	result.ret = CheckNow();
	if(result.ret == VAL_MISMATCH) {
		result.user_mismatch = (char*) malloc(3);
		result.std_mismatch = (char*) malloc(3);

		*result.user_mismatch = *result.std_mismatch = 0;
	}

	return result;
}
/*
int main()
{
	FILE* f1 = fopen("A.txt", "r");
	FILE* f2 = fopen("B.txt", "r");
	struct validator_info ret = validator_cena(f1, f2);

	if(ret.ret == VAL_FUCKED)
		printf("Error\n");
	else if(ret.ret == VAL_IDENTICAL)
		printf("Identical\n");
	else if(ret.ret == VAL_MISMATCH){
		printf("Mismatch\n");
		printf("user output: %s\n", ret.user_mismatch);
		printf("std output: %s\n", ret.std_mismatch);
	}else if(ret.ret == VAL_SHORTER)
		printf("Shorter\n");
	else
		printf("Longer\n");
	return 0;
}
*/
