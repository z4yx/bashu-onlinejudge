#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
extern "C"{
extern int inet_aton(const char *cp_arg, struct in_addr *addr);
}
#endif
#include <microhttpd.h>
#include "judge_daemon.h"
#include "conf_items.h"

static char robots_txt[] = "User-agent: *\nDisallow: /\n";

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
template<class T>
void numcat(T &left, const char *right, size_t sz)
{
	int len = sz;
	while(len--)
		left = left * 10 + *(right++) - '0';
}
static int iterate_post(void *arg, enum MHD_ValueKind, const char *name,
	const char*, const char*, const char*, const char *data, uint64_t offset, size_t size)
{
	solution *p = ((pair*)arg) -> second;
	if(!size)
		return MHD_YES;
	//printf("[%s]->[%s] %lld %d\n", name, data, offset, size);
	switch(*name - 'a') {
		//A number may be separated into two parts
		case MSG_problem:
			numcat(p->problem, data, size);
			break;
		case MSG_lang:
			numcat(p->lang, data, size);
			break;
		case MSG_time:
			numcat(p->time_limit, data, size);
			break;
		case MSG_mem:
			numcat(p->mem_limit, data, size);
			break;
		case MSG_score:
			numcat(p->score, data, size);
			break;
		case MSG_code:
			p->code.append(data, size);
			break;
		case MSG_user:
			p->user.append(data, size);
			break;
		case MSG_key:
			p->key.append(data, size);
			break;
		case MSG_share:
			p->public_code = *data-'0';
			break;
		case MSG_compare:
			numcat(p->compare_way, data, size);
			break;
		case MSG_rejudge:
			p->type = *data-'0';
			break;
	}
	return MHD_YES;
}
static int server_handler(
	void *cls, struct MHD_Connection *connection, const char *url, const char *method,
	const char *version, const char *upload_data, size_t *upload_size, void **con_cls) 
{
	if(NULL == *con_cls) { //first time, read header
		//puts("first");
		//printf("%s %s\n", method, url);

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
			}else if(strcmp(url, "/get_datapath") == 0){
				struct MHD_Response *response = 
					MHD_create_response_from_buffer(strlen(DataDir), DataDir, MHD_RESPMEM_PERSISTENT);
				// MHD_add_response_header(response, "Connection", "close");
				int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
				return ret;
			}else if(strcmp(url, "/robots.txt") == 0) {
				struct MHD_Response *response = 
					MHD_create_response_from_buffer(sizeof(robots_txt)-1, robots_txt, MHD_RESPMEM_PERSISTENT);
				// MHD_add_response_header(response, "Connection", "close");
				int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
				return ret;
			}
			return ignore_requst(connection);
		}
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
		}
		return ignore_requst(connection);
	}else{
		pair *p = (pair*)*con_cls;
		if(0 != *upload_size) { //next, read body
#ifdef DUMP_FOR_DEBUG
			p->second->raw_post_data.append(upload_data, *upload_size);
#endif
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
	// printf("ip: %x\n", ip);
	if((ip & 0xff) == 0x7f) //127.x.x.x
		return MHD_YES;
	return MHD_NO;
}
bool start_http_interface()
{
	struct MHD_Daemon *handle;
	struct sockaddr_in sock_addr;

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(HTTP_BIND_PORT);
	if(!inet_aton(HTTP_BIND_IP, &sock_addr.sin_addr)) {
		applog("Error: Invalid IP address.", HTTP_BIND_IP);
		return false;
	}
	// printf("listen %s:%d\n", HTTP_BIND_IP, HTTP_BIND_PORT);

	handle = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, HTTP_BIND_PORT,
		&on_client_connect, NULL, &server_handler, NULL, MHD_OPTION_NOTIFY_COMPLETED,
		request_completed, NULL, MHD_OPTION_SOCK_ADDR, &sock_addr, MHD_OPTION_END);
	if(handle == NULL){
		applog("Error: Unable to start http server.");
		return false;
	}	
	return true;
}
