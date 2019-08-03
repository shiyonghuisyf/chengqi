#include <unistd.h>
#include <signal.h>
#include <iostream>

#include "redis.h"
#include "hiredis/hiredis.h"

redis * r = new redis();

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

void sigFunc(int signum)
{
    char message[512] = {'\0'};
    message[0] = '0';
    r ->set(string(message));
    exit(0);
}

int main()
{
    if(!r -> connect("127.0.0.1", 6379))
    {
        printf("connect error!\n");
        return 0;
    }
    r -> setGroup("xgroup create codehole cg1 0-0 #");

    char str[512] = {0};
    signal(SIGINT,sigFunc);//有序退出
    while(1)
    {
        bzero(str, 512);
        GetStr(str);
        r -> set(string(str));
        usleep(10);
    }
    return 0;
}
