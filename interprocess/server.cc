#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp> 

using namespace boost::interprocess;

#define ERROR_CHECK(ret, retval, funcName) {if(ret == retval) \
    {printf("%d:", __LINE__);fflush(stdout);perror(funcName);return -1;}}

#define ARGS_CHECK(argc,val) {if(argc!=val) \
    {printf("error args\n");return -1;}}

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
int exitfds[2];

void sigFunc(int signum)
{
    write(exitfds[1],&signum,1);
}

int main(int argc, char * argv[])
{
    pipe(exitfds);
    if(fork())
    {    
        close(exitfds[0]);
        signal(SIGINT, sigFunc);
        wait(NULL);
        exit(0);
    }
    close(exitfds[1]);

    ARGS_CHECK(argc, 2);
    managed_shared_memory managed_shm(open_or_create, argv[1], 512);
    char * pStr = managed_shm.find_or_construct<char>("Char")[512](0);
    interprocess_mutex & mutex = *managed_shm.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition & cond = *managed_shm.find_or_construct<interprocess_condition>("cond")();
    scoped_lock<interprocess_mutex> lock(mutex);

    int epfd = epoll_create(1), num = 0;
    struct epoll_event event, evs[1];
    event.events = EPOLLIN;
    event.data.fd = exitfds[0];
    epoll_ctl(epfd, EPOLL_CTL_ADD, exitfds[0], &event);

    while(1)
    {

        GetStr(pStr);
        num = epoll_wait(epfd, evs, 1, 5);
        if(evs[0].data.fd == exitfds[0])
        {
            bzero(pStr, 512);
            pStr[0] = '0';
            cond.notify_all();
            shared_memory_object::remove(argv[1]);
            exit(0); 
        }
        usleep(5);
        cond.notify_all();
    }
    return 0;
}

