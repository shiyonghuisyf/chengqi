#include <unistd.h>
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

int main(int argc, char * argv[])
{
    
    managed_shared_memory managed_shm(open_or_create, "SharedMemory", 512);
    char * pStr = managed_shm.find_or_construct<char>("Char")[512](0);
    interprocess_mutex & mutex = *managed_shm.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition & cond = *managed_shm.find_or_construct<interprocess_condition>("cond")();
    scoped_lock<interprocess_mutex> lock(mutex);

    while(1)
    {
        GetStr(pStr);
        usleep(10);
        cond.notify_all();
    }
    shared_memory_object::remove("SharedMemory");
    return 0;
}

