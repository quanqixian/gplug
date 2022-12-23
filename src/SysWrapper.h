#ifndef _SYSTEM_WRAPPER_H_
#define _SYSTEM_WRAPPER_H_

#include <string>

#if (defined(_WIN32) || defined(_WIN64))
    #include <io.h>
    #include <direct.h>
    #include <windows.h>
#else
    #include <unistd.h>
    #include <dlfcn.h>
    #include <pthread.h>
#endif

namespace SysWrapper
{

class PathWrapper
{
public:
    /**
     * @brief      拼接路径
     * @param[in]  basePath : 基础路径
     * @param[out] retPath : 全路径
     * @return     true : success false : fail
     */
    static bool splicePath(std::string basePath, std::string & retPath)
    {
        bool ret = true;
        std::string workDir;
        std::string fullPath;

        /* 获取当前工作路径 */
        ret = getCurrentWorkDir(workDir);
        if(!ret)
        {
            GPLUG_LOG_ERROR(-1, "file to getCurrentWorkDir");
            return ret;
        }

        /* 拼接全路径 */
    #if (defined(_WIN32) || defined(_WIN64))
        fullPath = workDir + std::string("\\") + basePath;
    #else
        fullPath = workDir + std::string("/") + basePath;
    #endif

        /* 设置返回值 */
        retPath = fullPath;

        /* 检查是否存在文件或路径 */
        ret = isPathExist(fullPath);
        if(!ret)
        {
            GPLUG_LOG_WARN(0, "file or dir is not exist, fullPath=%s", fullPath.c_str());
            return ret;
        }

        GPLUG_LOG_INFO("file or dir is exist, fullPath=%s", fullPath.c_str());

        return ret;
    }
private:
    static bool isPathExist(std::string path)
    {
        bool ret = true;

    #if (defined(_WIN32) || defined(_WIN64))
        ret = (0 == _access(path.c_str(), 0));
    #else
        ret = (0 == access(path.c_str(), F_OK));
    #endif

        return ret;
    }

    static bool getCurrentWorkDir(std::string & path)
    {
        bool ret = true;
        char buf[512] = {0};

    #if (defined(_WIN32) || defined(_WIN64))
        ret = (NULL != _getcwd(buf, sizeof(buf)));
    #else
        ret = (NULL != getcwd(buf, sizeof(buf)));
    #endif

        path = buf;
        return ret;
    }
};

class DLWrapper
{
#if (defined(_WIN32) || defined(_WIN64))
    typedef HMODULE DLHandle;  
#else
    typedef void* DLHandle;  
#endif

public:
    static DLHandle open(const char * path)
    {
        DLHandle handler = NULL;
#if (defined(_WIN32) || defined(_WIN64))
        handler = LoadLibrary(path);
#else
        handler = dlopen(path, RTLD_LAZY);
#endif
        return handler;
    }
    static void* getSym(DLHandle handler, const char * sym)
    {
        void * ret = NULL;
#if (defined(_WIN32) || defined(_WIN64))
        ret = GetProcAddress(handler, sym);
#else
        ret = dlsym(handler, sym);
#endif
        return ret;
    }
    static int close(DLHandle handler)
    {
#if (defined(_WIN32) || defined(_WIN64))
        return FreeLibrary(handler) ? 0 : 1;
#else
        return dlclose(handler);
#endif
    }
};

class Mutex
{
public:
    enum RecursionMode {NonRecursive, Recursive};
public:
    explicit Mutex(RecursionMode mode = NonRecursive);
    ~Mutex();
    void lock();
    void unlock();
    bool tryLock();
    bool isRecursive() const;
private:
    Mutex(Mutex &);
    Mutex& operator = (const Mutex &);
    friend class LockGuard;
private:
    pthread_mutex_t m_mutex;
    RecursionMode m_recursionMode;
};

Mutex::Mutex(RecursionMode mode) : m_recursionMode(mode)
{
    if(m_recursionMode == Recursive)
    {
        pthread_mutexattr_t mutexattr;
        pthread_mutexattr_init(&mutexattr);
        pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &mutexattr);
        pthread_mutexattr_destroy(&mutexattr);
    }
    else
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

bool Mutex::tryLock()
{
    return (0 == pthread_mutex_trylock(&m_mutex));
}

bool Mutex::isRecursive() const
{
    return (m_recursionMode == Recursive);
}

class LockGuard
{
public:
    inline explicit LockGuard(Mutex * m) : val(nullptr)
    {
        if(m)
        {
            val = m;
            val->lock();
        }
    }
    inline ~LockGuard()
    {
        if(val)
        {
            val->unlock();
        }
    }
    inline void unlock()
    {
        if(val)
        {
            val->unlock();
        }
    }
    inline void relock()
    {
        if(val)
        {
            val->lock();
        }
    }

    inline Mutex * mutex() const
    {
        return val;
    }
private:
    LockGuard(const LockGuard &);
    LockGuard & operator = (const LockGuard &);
    Mutex* val;
};

}/* end of namespace SysWrapper */

#endif
