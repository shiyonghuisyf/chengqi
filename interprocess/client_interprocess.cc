
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

int main(int argc, char * argv[])
{
    
    shared_memory_object::remove("SharedMemory");
    managed_shared_memory managed_shm(open_or_create, "SharedMemory", 512);
    char * pStr = managed_shm.find_or_construct<char>("Char")[512](0);
    interprocess_mutex & mutex = *managed_shm.find_or_construct<interprocess_mutex>("mtx")();
    interprocess_condition & cond = *managed_shm.find_or_construct<interprocess_condition>("cond")();
    scoped_lock<interprocess_mutex> lock(mutex);

    while(1)
    {
        cond.wait(lock);
        cout << pStr << endl;
    }
    return 0;
}

