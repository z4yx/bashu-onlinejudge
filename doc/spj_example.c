//Linux平台下保存为spj.cpp放到题目的数据目录下
//Windows平台编译为spj.exe放到题目的数据目录下
#include <stdio.h>

FILE *fscore,*freport,*fstd,*fin,*fout;
int score;
int main(int argc,char *argv[])
{
	fscore=fopen("score.log","w");//打开得分文件,写入一个数字表示测试点得分
	freport=fopen("report.log","w");//打开报告文件,用于反馈信息给选手
	fstd=fopen(argv[2],"r");//打开测试点标准输出文件
	score=atoi(argv[1]);//取得测试点的满分

	fin=fopen("data.in","r");//测试数据的输入
	fout=fopen("data.out","w");//选手的输出

	//评测

	if (/*完全正确*/)
	{
		fprintf(fscore,"%d",score);//返回满分
		fprintf(freport,"right");//报告Judge结果为right
	}
	else if (/*部分正确*/)
	{
		fprintf(fscore,"%d",/*部分分*/);
		fprintf(freport,/*输出错误信息*/);
	}
	else
	{
		fprintf(fscore,"%d",0);//返回0分
		fprintf(freport,"wrong");//报告Judge结果为wrong
	}

	fclose(fscore);//关闭得分文件
	fclose(freport);//关闭报告文件
	return 0;
}