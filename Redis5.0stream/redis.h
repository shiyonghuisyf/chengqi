#ifndef __REDIS_H__
#define __REDIS_H__

#include <iostream>
#include <string.h>
#include <string>

#include <hiredis/hiredis.h>
using std::cout;
using std::endl;
using std::string;

class redis
{
public:
    redis()
    : _connect(NULL)
    , _reply(NULL)
    {}


    ~redis() 
    {
        save();
        if(_connect)
            redisFree(_connect);
        if(_reply)
            freeReplyObject(_reply);
        _connect = NULL;
        _reply = NULL;
    }

    bool save()
    {
        _reply = (redisReply *)redisCommand(_connect, "SAVE");
        if(NULL == _reply) {
            cout << "Execute SAVE failure" << endl;
            return 0;
        }
        if(!(_reply->type == REDIS_REPLY_STATUS && strcasecmp(_reply->str, "OK") == 0))
        {
            cout << "Failed to execute command [SAVE]:" << _reply->str << endl;
            freeReplyObject(_reply);
            _reply = NULL;
            return 0;
        }
        cout << "Succeed to execute command [SAVE] " << endl;
        return 1;
    }
    bool setGroup(const string & str)
    {
        _reply = (redisReply *)redisCommand(_connect, str.c_str());
        if(NULL == _reply)
        {
            cout << str.c_str() << "failure" << endl;
            return 0;
        } else if(!(strcasecmp(_reply->str, "OK") == 0))
        {
            cout << str.c_str() << "failure" << endl;
            freeReplyObject(_reply);
            _reply = NULL;
            return 0;
        }
        cout << str.c_str() << "success" << endl;
        return 1;
    }

    bool connect(std::string host, int port)
    {
        _connect = redisConnect(host.c_str(), port);
        if(_connect != NULL && _connect->err)
        {
            redisFree(_connect);
            cout << "Connect error: " << _connect->errstr << endl;
            return 0;
        }
        std::cout << "Connect to redisServer Success" << std::endl;
        return 1;
    }
    
    string get(std::string key)
    {
        _reply = (redisReply *)redisCommand(_connect, "xreadgroup GROUP cg1 %s count 1 streams codehole ", key.c_str());
        if(_reply->type == REDIS_REPLY_NIL )
        {
            _reply = (redisReply *)redisCommand(_connect, "xreadgroup GROUP cg1 %s block 0 count 1 streams codehole", key.c_str());
        }
        std::string str1 = _reply->element[4]->str;
        cout << "Succeed to execute command [xreadgroup group cg1 " << key << " count 1 streams codehole ]" << endl;
        freeReplyObject(_reply);
        _reply = NULL;
        return str1;
    }

    void set(string value)
    {
        _reply = (redisReply *)redisCommand(_connect, "xadd codehole * messages %s", value.c_str());
        if(_reply == NULL)
        {
            cout << "Execut xadd codehole * messages " << value << " failure" << endl;
            return;
        }
        cout << "Succeed to execute command [XADD codehole * message " << value << "]" << endl;
        freeReplyObject(_reply);
        _reply = NULL;
    }

private:
    redisContext * _connect;
    redisReply * _reply;
};

#endif
