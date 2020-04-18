#include <stdint.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <thread>
#include <chrono>

static void handlerThreadFunc(struct evhttp_request *req, void *arg)
{
	struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        puts("failed to create response buffer \n");
        return;
    }
	printf("arg: %d\n", *(int*)arg);
	printf("request(%s) has being handling...\n", evhttp_request_get_uri(req));
	std::this_thread::sleep_for(std::chrono::seconds(10));
	printf("handle complete...\n");
    evbuffer_add_printf(buf, "Server Responsed. Requested: %s\n", evhttp_request_get_uri(req));
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}

void generic_handler(struct evhttp_request *req, void *arg)
{
/*
	std::thread([&]{
		handlerThreadFunc(req, arg);
	}).detach();
	*/

	handlerThreadFunc(req, arg);
}

int main(int argc, char **argv)
{
	uint16_t 	server_port = 1122;
	const char*	server_ip = "0.0.0.0";
	struct event_base* base = event_base_new();
	struct evhttp* http_server = evhttp_new(base);
	int ret = evhttp_bind_socket(http_server, server_ip, server_port);
    if(ret!=0)
    {
        return -1;
    }

	int data = 100;
    evhttp_set_gencb(http_server, generic_handler, &data);
    printf("http server start OK! \n");
    event_base_dispatch(base);
    evhttp_free(http_server);

	return 0;
}
