#include <string>
#include <vector>
#include <ctime>

struct case_info
{
	int error_code, time, memory;
	std::string info;
	int score;
};

struct solution
{
	int problem, compare_way, lang, time_limit, mem_limit, score, error_code;
	bool public_code;
	unsigned char type;
	std::string code, user, key, last_state;
	std::vector<case_info> detail_results;
	void *mutex_for_query;//use void* to avoid including <mutex>
	time_t timestamp;
#ifdef DUMP_FOR_DEBUG
	std::string raw_post_data;
#endif

	solution();
	~solution();
	void copy_setting(const solution &) throw ();
	bool compile() throw (const char *);
	void judge() throw (const char *);
	void write_database() throw (const char *);
};

struct execute_info
{
	int state, time, memory;
	char *str;
};

struct validator_info{
	int ret;
	char* user_mismatch;
	char* std_mismatch;
};

enum {TYPE_normal=0, TYPE_rejudge=1};
enum {MSG_problem, MSG_lang, MSG_time, MSG_mem, MSG_score, MSG_code, MSG_user, MSG_key, MSG_share, MSG_compare, MSG_rejudge};
enum {RES_AC=0, RES_CE=7, RES_TLE=2, RES_MLE=3, RES_WA=4, RES_RE=5, RES_VE=99, RES_SE=100};

typedef int (*run_compiler_def)(const char *, char *, int);
typedef int (*run_judge_def)(const char *, const char *, const char *, int, int, execute_info*);

void applog(const char *str, const char *info = "") throw ();
bool read_config();
bool start_http_interface();
bool init_mysql_con() throw ();
char *JUDGE_get_progress(const char*);
char *JUDGE_accept_submit(solution *&sol);
char *JUDGE_start_rejudge(solution *&sol);
bool clean_files() throw ();
int get_next_solution_id() throw (const char *);
void write_result_to_database(int, solution*) throw (const char *);
void get_exist_solution_info(int solution_id, solution* sol) throw (const char *);
void update_exist_solution_info(int solution_id, solution* sol) throw (const char *);
void refresh_users_problem(int problem_id) throw (const char *);
void get_solution_list(std::vector<int> &rejudge_list, int problem_id) throw (const char *);

struct validator_info validator_cena(FILE* fstd, FILE* fuser);
extern "C" {
	struct validator_info validator(FILE* fstd, FILE* fuser);
	struct validator_info validator_int(FILE* fstd, FILE* fuser);
	struct validator_info validator_float(FILE* fstd, FILE* fuser, int);
}

int run_judge(const char *, const char *, const char *, int, int, execute_info*);
validator_info run_spj(char *datafile_out, char *datafile_in, int *score, char *data_dir);


