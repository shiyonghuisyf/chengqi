#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "redis.h"
#include "hiredis/hiredis.h"

int main()
{
    redis * r = new redis();
    if(!r -> connect("127.0.0.1", 6379))
    {
        printf("connect error!\n");
        return 0;
    }
    char str[512] = {0};
    string s;
    itoa(getpid(), str, 10);
    string comsumer = str;

    while(1)
    {
        s = r -> get(comsumer);
        bzero(str, 512);
        strcpy(str, s.c_str());
        if(strcmp(str, "0"))
        {
            break;
        }
    }
    return 0;
}
