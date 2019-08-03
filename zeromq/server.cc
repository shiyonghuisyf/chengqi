#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <assert.h>
#include <unistd.h>
#include <zmq.hpp>

#define ERROR_CHECK(ret, retval, funcName) {if(ret == retval) \
    {printf("%d:", __LINE__);fflush(stdout);perror(funcName);return -1;}}

void GetStr(char* random)//生成随机内容
{
    char str[512] = {0};
    int i,flag;
    srand(time(NULL));
    for(i = 0; i < 511; ++i)
    {
        flag = rand()%3;
        switch(flag)
        {
        case 0: 
            str[i] = rand()%26+'a';
            break;
        case 1:
            str[i] = rand()%26+'A';
            break;
        case 2:
            str[i] = rand()%10+'0';
            break;
        }
    }
    str[511] = '\0';
    sprintf(random, "%s", str);
}


int main()
{
    void * context = zmq_ctx_new();
    ERROR_CHECK(context, NULL, "zmq_ctx_new");
    
    int ret = zmq_ctx_set(context, ZMQ_MAX_SOCKETS, 1);
    ERROR_CHECK(ret, 0, "zmq_ctx_set");

    void * publisher = zmq_socket(context, ZMQ_PUB);
    ERROR_CHECK(publisher, NULL, "zmq_socket");

    ret = zmq_bind(publisher, "tcp://192.168.23.128:8888");
    ERROR_CHECK(ret, 0, "zmq_bind");

    char Buf[512] = {0};
    while(1)
    {
        bzero(&Buf, 512);
        GetStr(Buf);
        ret = zmq_send(publisher, Buf, 512, 0);
        usleep(10);
    }
    zmq_close(publisher);
    zmq_ctx_destroy(context);
    return 0;
}
