#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <microhttpd.h>
#include "judge_daemon.h"

char robots_txt[] = "User-agent: *\nDisallow: /\n";

typedef std::pair<MHD_PostProcessor*, solution*> pair;
int ignore_requst(struct MHD_Connection *connection)
{
	struct MHD_Response *response = 
		MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
	// MHD_add_response_header(response, "Connection", "close");
	int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
	MHD_destroy_response(response);
	return ret;
}
static int server_handler_get(
	void *cls, struct MHD_Connection *connection, const char *url, const char *method,
	const char *version, const char *upload_data, size_t *upload_size, void **con_cls) 
{
	if(strcmp(method, "GET") == 0) {
		char *result;
		applog(url);
		if(strstr(url, "/query_") == url) { 
			if(result=JUDGE_get_progress(url)) {
				struct MHD_Response *response = 
					MHD_create_response_from_buffer(strlen(result), result, MHD_RESPMEM_MUST_FREE);
				// MHD_add_response_header(response, "Connection", "close");
				int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
				return ret;
			}
		}else if(strcmp(url, "/robots.txt") == 0) {
			struct MHD_Response *response = 
				MHD_create_response_from_buffer(sizeof(robots_txt)-1, robots_txt, MHD_RESPMEM_PERSISTENT);
			// MHD_add_response_header(response, "Connection", "close");
			int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
			MHD_destroy_response(response);
			return ret;
		}
	}
	return ignore_requst(connection);
}
template<class T>
void numcat(T &left, const char *right)
{
	int len = strlen(right);
	while(len--)
		left *= 10;
	left += atol(right);
}
static int iterate_post(void *arg, enum MHD_ValueKind, const char *name,
	const char*, const char*, const char*, const char *data, uint64_t offset, size_t size)
{
	solution *p = ((pair*)arg) -> second;
	//printf("[%s]->[%s] %lld %d\n", name, data, offset, size);
	switch(*name - 'a') {
		//A number may be separated into two parts
		case MSG_problem:
			numcat(p->problem, data);
			break;
		case MSG_lang:
			numcat(p->lang, data);
			break;
		case MSG_time:
			numcat(p->time_limit, data);
			break;
		case MSG_mem:
			numcat(p->mem_limit, data);
			break;
		case MSG_score:
			numcat(p->score, data);
			break;
		case MSG_code:
			p->code += data;
			break;
		case MSG_user:
			p->user += data;
			break;
		case MSG_key:
			p->key += data;
			break;
		case MSG_share:
			p->public_code = atol(data);
			break;
		case MSG_compare:
			numcat(p->compare_way, data);
			break;
		case MSG_rejudge:
			p->type = atol(data);
			break;
	}
	return MHD_YES;
}
static int server_handler_post(
	void *cls, struct MHD_Connection *connection, const char *url, const char *method,
	const char *version, const char *upload_data, size_t *upload_size, void **con_cls) 
{
	if(NULL == *con_cls) { //first time, read header
		//puts("first");
		//printf("%s %s\n", method, url);
		if(strcmp(method, "POST") == 0 && strcmp(url, "/submit_prob") == 0) {
			//puts("accept");

			pair *p = new pair;
			if(NULL == p)
				return MHD_NO;
			*con_cls = p;

			solution *body = new solution;
			if(NULL == body)
				return MHD_NO;
			p->second = body;
			
			MHD_PostProcessor *processor 
				= MHD_create_post_processor(connection, 768, iterate_post, (void*)p);
			if(NULL == processor)
				return MHD_NO;
			p->first = processor;
			//puts("success");
			return MHD_YES;
		}else {
			return MHD_NO;
		}
	}else{
		pair *p = (pair*)*con_cls;
		if(0 != *upload_size) { //next, read body
			MHD_post_process(p->first, upload_data, *upload_size);
			*upload_size = 0;
			//puts("next");
			return MHD_YES;
		}else{ //last time, finish reading
			//puts("last");
			char *result;
			if(p->second->type == TYPE_rejudge)
				result = JUDGE_start_rejudge(p->second);
			else
				result = JUDGE_accept_submit(p->second);
			struct MHD_Response *response = 
				MHD_create_response_from_buffer(strlen(result), result, MHD_RESPMEM_MUST_FREE);
			int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
			MHD_destroy_response(response);
			return ret;
		}
	}
}
static void request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe)
{
	//puts("completed");
	pair *p = (pair*)*con_cls;
	if(NULL != p) {
		MHD_destroy_post_processor(p->first);
		if(p->second)
			delete p->second;
		delete p;
		*con_cls = NULL;
	}
}
static int on_client_connect(void *cls, const struct sockaddr * addr, socklen_t addrlen)
{
	sockaddr_in *addr1 = (sockaddr_in*)addr;
#ifndef __MINGW32__
	uint32_t ip = addr1->sin_addr.s_addr;
#else
	uint32_t ip = addr1->sin_addr.S_un.S_addr;
#endif
	//printf("ip: %x\n", ip);
	if(ip == 0x00000000 || ip == 0x0100007f) //0.0.0.0 or 127.0.0.1
		return MHD_YES;
	return MHD_NO;
}
bool start_http_interface()
{
	struct MHD_Daemon *handle =
		MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8888, NULL, NULL, &server_handler_get, NULL, MHD_OPTION_END);
	if(handle == NULL){
		applog("Error: Unable to start http server on 8888.");
		return false;
	}
	handle = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8881, &on_client_connect, NULL, &server_handler_post, NULL, MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL, MHD_OPTION_END);
	if(handle == NULL){
		applog("Error: Unable to start http server on 8881.");
		return false;
	}	
	return true;
}