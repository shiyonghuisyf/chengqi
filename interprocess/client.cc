
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp> 

using namespace boost::interprocess;
using std::cout;
using std::endl;

#define ERROR_CHECK(ret, retval, funcName) {if(ret == retval) \
    {printf("%d:", __LINE__);fflush(stdout);perror(funcName);return -1;}}

#define ARGS_CHECK(argc,val) {if(argc!=val) \
    {printf("error args\n");return -1;}}

int main(int argc, char * argv[])
{
    ARGS_CHECK(argc, 2);    
    managed_shared_memory managed_shm(open_only, argv[1]);
    char * pStr = managed_shm.find_or_construct<char>("Char")[512](0);
    interprocess_mutex & mutex = *managed_shm.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition & cond = *managed_shm.find_or_construct<interprocess_condition>("cond")();
    scoped_lock<interprocess_mutex> lock(mutex);

    while(1)
    {
        cond.wait(lock);
        if(strcmp(pStr, "0") == 0)
        {
            break;
        } else {
            cout << pStr << endl;
        }
    }
    shared_memory_object::remove(argv[1]);
    return 0;
}

