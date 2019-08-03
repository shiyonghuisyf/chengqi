#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <zmq.hpp>
#include <time.h>

#define ERROR_CHECK(ret, retval, funcName) {if(ret == retval) \
    {printf("%d:", __LINE__);fflush(stdout);perror(funcName);return -1;}}

int main()
{
    void * context = zmq_ctx_new();
    ERROR_CHECK(context, NULL, "zmq_ctx_new");

    void * subscriber = zmq_socket(context, ZMQ_SUB);
    ERROR_CHECK(subscriber, NULL, "zmq_socket");
    
    int ret = zmq_connect(subscriber, "tcp://192.168.23.128:8888");
    ERROR_CHECK(ret, 0, "zmq_connect");
    
    ret = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    ERROR_CHECK(ret, 0, "zmq_setsockopt");
    
    char Buf[512] = {0};
    while(1)
    {
        bzero(&Buf, 512);
        ret = zmq_recv(subscriber, Buf, 512, 0);
        if(ret > 0)
        {
            if(strcmp(Buf, "0") == 0)
            {
                break;
            }else {
                printf("%s\n", Buf);
            }
        }
    }
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
    return 0;
}

