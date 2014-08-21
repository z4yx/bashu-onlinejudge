#ifndef SABLEBOX_H
#define SABLEBOX_H
/*
	Data structures and constants definitions for sablebox.
	Author: Yang Zhixuan
	Date: Sept.22nd, 2012
 */



/****************** constants *****************************/
#define MAX_SYSCALL_NUM 			512

#define PROF_NOTSET					-1
#define PROF_SUCCESS				0
#define PROF_RUNTIME_ERROR			1
#define PROF_TIME_LIMIT_EXCEED		2
#define PROF_MEMORY_LIMIT_EXCEED	3
#define PROF_SYSTEM_ERROR			4

#define INFO_SEGMENT_FAULT			"Segmentation fault"	
#define INFO_MATH_FAULT				"Math error"
#define INFO_SIGNAL					"Killed by a signal"

#define SCMASK_INFINITY				-1
/**********************************************************/
  
  
/****************** profile type **************************/
struct profile
{
	int status;				/*results code*/
	const char* info;		/*extra info when RE*/
	long time;				/*total usage of time*/
	long memory;			/*peak usage of memory*/
	unsigned char exit_code;
};
/**********************************************************/


/******************* syscall mask type ********************/
struct scmask{
	int set[MAX_SYSCALL_NUM];
};

/**********************************************************/


/******************* sablebox type ************************/
struct sablebox
{
	const char* pathname;	/*program tested in the sablebox*/
	const char* fin, *fout;	/*input/output file for program*/
	long cpu_lim;			/*time limit, including systime and usertime*/
	long memory_lim;			/*peak memory limit, including data, bss, text and heap*/
	struct scmask* scp;		/*syscall allowed in the sablebox*/
	char** args;
};
/**********************************************************/


/********************** function prototypes ***************/
int sable_init(struct sablebox*, const char*, char** args, const char*, const char*,
										long, long, struct scmask*);

struct profile sable_run(struct sablebox* pbox);
/**********************************************************/

#endif
