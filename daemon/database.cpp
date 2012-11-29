#include <stdio.h>
#include <stdlib.h>
#ifdef __MINGW32__
#include <winsock2.h>
#include <boost/thread.hpp>
namespace std {
	using boost::mutex;
	using boost::recursive_mutex;
	using boost::lock_guard;
	using boost::unique_lock;
}
#else
#include <mutex>
#endif
#include <mysql.h>
#include "judge_daemon.h"

std::mutex database_mutex;
static MYSQL *hMySQL;
char statements[65536*2]; //escaped compile info becomes longer

char DATABASE_USER[128], DATABASE_PASS[128];

bool init_mysql_con()
{
	hMySQL = mysql_init(NULL);
	if(!hMySQL)
		return false;
	hMySQL = mysql_real_connect(hMySQL, "localhost", DATABASE_USER, DATABASE_PASS, "bsoj", 0, NULL, 0);
	if(!hMySQL)
		return false;
	if(mysql_set_character_set(hMySQL, "utf8"))
		return false;
	return true;
}
void Check_mysql_connection()
{
	if(mysql_ping(hMySQL)) {
		applog("Info: Try to reconnect to mysql...");
		if(!init_mysql_con()) {
			applog("Error: Can't connect to mysql.");
			throw 1;
		}
	}
}
int get_next_solution_id()
{
	std::unique_lock<std::mutex> Lock(database_mutex);
	Check_mysql_connection();

	if(mysql_query(hMySQL, "select max(solution_id) from solution"))
		throw 1;
	MYSQL_RES *result = mysql_store_result(hMySQL);
	if(NULL == result)
		throw 1;
	MYSQL_ROW row;
	int ret = 0;
	while(row = mysql_fetch_row(result)) {
		//printf("%s\n", row[0]);
		if(row[0])
			ret = atol(row[0]);
	}
	mysql_free_result(result);
	if(ret == 0)
		ret = 1000;
	else
		ret++;
	return ret;
}
bool haveSolved(int problem_id, const char *user_id)
{
	sprintf(statements, "select solution_id from solution where problem_id=%d and result=0 and user_id='%s' limit 1", problem_id, user_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
	MYSQL_RES *result = mysql_store_result(hMySQL);
	if(NULL == result)
		return false;
	if(0 == mysql_num_rows(result)) {
		mysql_free_result(result);
		return false;
	}
	mysql_free_result(result);
	return true;
}
bool haveSubmitted(int problem_id, const char *user_id, int before)
{
	sprintf(statements, "select solution_id from solution where solution_id<%d and problem_id=%d and user_id='%s' limit 1", before, problem_id, user_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
	MYSQL_RES *result = mysql_store_result(hMySQL);
	if(NULL == result)
		return false;
	if(0 == mysql_num_rows(result)) {
		mysql_free_result(result);
		return false;
	}
	mysql_free_result(result);
	return true;
}
void write_result_to_database(int solution_id, solution *data)
{
	std::unique_lock<std::mutex> Lock(database_mutex);
	Check_mysql_connection();

	int valid, delta, code_length = data->code.length();
	valid = haveSolved(data->problem, data->user.c_str()) ? 0 : 1;
	//printf("valid %d\n",valid);

	sprintf(statements, "select max(score) from solution where problem_id=%d and user_id='%s'", data->problem, data->user.c_str());
	if(mysql_query(hMySQL, statements))
		throw 1;
	MYSQL_RES *result = mysql_store_result(hMySQL);
	if(!result)
		throw 1;
	MYSQL_ROW row = mysql_fetch_row(result);
	if(!row) {
		mysql_free_result(result);
		throw 1;
	}
	if(row[0])
		delta = atol(row[0]);
	else
		delta = 0;
	mysql_free_result(result);

	if(data->score > delta)//New submit has higher score than old one
		delta = data->score - delta;
	else
		delta = 0;
	if(data->error_code == RES_CE) {
		puts("insert compileinfo");
		int len = data->last_state.length();
		char *info_escape = (char*)malloc(len*2 + 3);
		mysql_real_escape_string(hMySQL, info_escape, data->last_state.c_str(), len);
		sprintf(statements, "insert into compileinfo VALUES (%d,'%s')", solution_id, info_escape);
		free(info_escape);
		data->last_state = "";
		if(mysql_query(hMySQL, statements))
			throw 1;
		if(1 != mysql_affected_rows(hMySQL))
			throw 1;
	}
	puts("insert solution");
	sprintf(statements, "insert into solution (solution_id,problem_id,user_id,time,memory,in_date,result,score,language,valid,code_length,public_code) VALUES "
		"(%d,%d,'%s',%d,%d,NOW(),%d,%d,%d,%d,%d,%d)", solution_id, data->problem, data->user.c_str(), data->time_limit, data->mem_limit, data->error_code, data->score, data->lang, valid, code_length, (int)data->public_code);
	if(mysql_query(hMySQL, statements))
		throw 1;
	if(1 != mysql_affected_rows(hMySQL))
		throw 1;

	puts("insert source");
	//printf("code_length %d\n", code_length);
	char *code_escape = (char*)malloc(code_length*2 + 3);
	mysql_real_escape_string(hMySQL, code_escape, data->code.c_str(), code_length);
	sprintf(statements, "insert into source_code VALUES (%d,'%s')", solution_id, code_escape);
	free(code_escape);
	if(mysql_query(hMySQL, statements))
		throw 1;
	if(1 != mysql_affected_rows(hMySQL))
		throw 1;

	puts("update user info");
	int is_first_solved = (int)(valid && data->error_code == RES_AC);
	sprintf(statements, "update users set submit=submit+1,solved=solved+%d,score=score+%d,language=%d where user_id='%s'", is_first_solved, delta, data->lang, data->user.c_str());
	if(mysql_query(hMySQL, statements))
		throw 1;
	if(1 != mysql_affected_rows(hMySQL))
		throw 1;

	puts("update problem info");
	sprintf(statements, "update problem set submit=submit+1,accepted=accepted+%d,submit_user=submit_user+%d,solved=solved+%d where problem_id=%d", 
			(int)(data->error_code == RES_AC), (int)(valid && !haveSubmitted(data->problem, data->user.c_str(), solution_id)), is_first_solved, data->problem);
	if(mysql_query(hMySQL, statements))
		throw 1;
	if(1 != mysql_affected_rows(hMySQL))
		throw 1;
}

void get_exist_solution_info(int solution_id, solution* sol)
{
	std::unique_lock<std::mutex> Lock(database_mutex);
	Check_mysql_connection();

	puts("get_exist_solution_info");
	sprintf(statements, "select language from solution where solution_id=%d", solution_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if(NULL == (result = mysql_store_result(hMySQL)))
		throw 1;
	if(! (row=mysql_fetch_row(result))) {
		mysql_free_result(result);
		throw 1;
	}
	sol->lang = atol(row[0]);
	mysql_free_result(result);

	sprintf(statements, "select source from source_code where solution_id=%d", solution_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
	if(NULL == (result = mysql_store_result(hMySQL)))
		throw 1;
	if(! (row=mysql_fetch_row(result))) {
		mysql_free_result(result);
		throw 1;
	}
	sol->code = row[0];
	mysql_free_result(result);
}
void update_exist_solution_info(int solution_id, solution* data)
{
	std::unique_lock<std::mutex> Lock(database_mutex);
	Check_mysql_connection();

	puts("update_exist_solution_info");
	sprintf(statements, "update solution set time=%d,memory=%d,result=%d,score=%d where solution_id=%d",
		data->time_limit, data->mem_limit, data->error_code, data->score, solution_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
}
void refresh_users_problem(int problem_id)
{
	std::unique_lock<std::mutex> Lock(database_mutex);
	Check_mysql_connection();

	puts("refresh_users_problem");
	
	//update users solved
	sprintf(statements,"update users,(select user_id as uid,count(distinct problem_id) as s from solution where result=0 group by user_id) as cnt set users.solved=cnt.s where cnt.uid=users.user_id");
	//sprintf(statements,"update users,(select count(*) as solved,uid from (select user_id as uid,count(*) as s from solution where result=0 group by problem_id,user_id) as tmp group by uid) as cnt set users.solved=cnt.solved where cnt.uid=users.user_id");
	if(mysql_query(hMySQL, statements))
		throw 1;
	//update users score
	sprintf(statements,"update users,(select sum(tmp.s) as newsum,uid from (select user_id as uid,problem_id,max(score) as s from solution group by problem_id,user_id) as tmp group by uid) as cnt set users.score=cnt.newsum where cnt.uid=users.user_id");
	if(mysql_query(hMySQL, statements))
		throw 1;
	//update problem accepted & solved
	sprintf(statements,"update problem,(select count(distinct user_id)as s,count(*)as t from solution where problem_id=%d and result=0)as tmp set solved=tmp.s,accepted=tmp.t where problem.problem_id=%d", problem_id, problem_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
}
void get_solution_list(std::vector<int> &rejudge_list, int problem_id)
{
	std::unique_lock<std::mutex> Lock(database_mutex);
	Check_mysql_connection();

	puts("get_solution_list");
	sprintf(statements, "select solution_id from solution where problem_id=%d", problem_id);
	if(mysql_query(hMySQL, statements))
		throw 1;
	MYSQL_RES *result = mysql_store_result(hMySQL);
	MYSQL_ROW row;
	if(NULL == result)
		throw 1;
	rejudge_list.clear();
	while(row = mysql_fetch_row(result))
		rejudge_list.push_back(atol(row[0]));

	mysql_free_result(result);
}