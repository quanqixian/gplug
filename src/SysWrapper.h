#ifndef _SYSTEM_WRAPPER_H_
#define _SYSTEM_WRAPPER_H_

#include <string>
#include <exception>
#include <queue>
#include <vector>

#if (defined(_WIN32) || defined(_WIN64))
    #include <io.h>
    #include <direct.h>
    #include <windows.h>
#else
    #include <unistd.h>
    #include <dlfcn.h>
    #include <pthread.h>
    #include <sys/types.h>
    #include <dirent.h>
#endif

namespace SysWrapper
{

class PathWrapper
{
public:
    /**
     * @brief      stitching path
     * @param[in]  basePath : base path
     * @param[out] retPath : Full path
     * @return     true : success false : fail
     */
    static bool splicePath(const std::string & basePath, std::string & retPath)
    {
        bool ret = true;
        std::string workDir;
        std::string fullPath;

        /* Get the current working path */
        ret = getCurrentWorkDir(workDir);
        if(!ret)
        {
            GPLUGMGR_LOG_ERROR(-1, "file to getCurrentWorkDir");
            return ret;
        }

        /* splice full path */
    #if (defined(_WIN32) || defined(_WIN64))
        fullPath = workDir + std::string("\\") + basePath;
    #else
        fullPath = workDir + std::string("/") + basePath;
    #endif

        /* set return value */
        retPath = fullPath;

        /* Check if file or path exists */
        ret = isPathExist(fullPath);
        if(!ret)
        {
            GPLUGMGR_LOG_WARN(0, "file or dir is not exist, fullPath=%s", fullPath.c_str());
            return ret;
        }

        //GPLUGMGR_LOG_INFO("file or dir is exist, fullPath=%s", fullPath.c_str());

        return ret;
    }
    /**
     * @brief      Get all file paths with the same name as the specified file name in the specified directory(Breadth-first Search)
     * @param[in]  rootPath : find root path
     * @param[in]  fileName : file name
     * @param[out] retVec : used to return the found path
     * @return     true : success false : fail
     */
    static bool getFilesInDir(std::string rootPath, std::string fileName, std::vector<std::string> & retVec)
    {
    #ifdef _WIN32
        bool ret = true;
        std::queue<std::string > dirQueue;

        retVec.clear();
        ret = isPathExist(rootPath);
        if(!ret)
        {
            return false;
        }
        dirQueue.push(rootPath);
        do
        {
            std::string dirPath = dirQueue.front();
            dirQueue.pop();

            WIN32_FIND_DATAA winFindData = {0};
            std::string findArg = dirPath + "\\*.*";
            HANDLE hFind = FindFirstFileA(findArg.c_str(), &winFindData);
            if(INVALID_HANDLE_VALUE == hFind)
            {
                ret = false;
                break;
            }

            do
            {
                if (winFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if((std::string(".") != winFindData.cFileName) && (std::string("..") != winFindData.cFileName))
                    {
                        std::string fullPath = dirPath + "\\" + winFindData.cFileName;
                        dirQueue.push(fullPath);
                    }
                }
                else
                {
                    if(fileName == winFindData.cFileName)
                    {
                        std::string fullPath = dirPath + "\\" + winFindData.cFileName;
                        retVec.push_back(fullPath);
                    }
                }
            }while(FindNextFileA(hFind, &winFindData));

            if(NULL != hFind)
            {
                FindClose(hFind);
                hFind = NULL;
            }

        }while(dirQueue.size());

        return ret;
    #else
        bool ret = true;
        std::queue<std::string > dirQueue;
        DIR *dp = NULL;

        retVec.clear();
        ret = isPathExist(rootPath);
        if(!ret)
        {
            return false;
        }

        dirQueue.push(rootPath);
        do
        {
            std::string dirPath = dirQueue.front();
            dirQueue.pop();

            dp = opendir(dirPath.c_str()); 
            if(NULL == dp)
            {
                ret = false;
                break;
            }

            while(true)
            {
                struct dirent *ep = NULL;
                struct dirent ent = {0};
                int retVal = readdir_r(dp, &ent, &ep);
                if(0 != retVal)
                {
                    break;
                }
                
                if(NULL == ep)
                {
                    break;
                }

                if(ep->d_type & DT_DIR)
                {
                    if((std::string(".") == ep->d_name) || (std::string("..") == ep->d_name))
                    {
                        continue;
                    }
                    std::string fullPath = dirPath + "/" + ep->d_name;
                    dirQueue.push(fullPath);
                }
                else
                {
                    if(fileName == ep->d_name)
                    {
                        std::string fullPath = dirPath + "/" + ep->d_name;
                        retVec.push_back(fullPath);
                    }
                }
            }

            if(NULL != dp)
            {
                closedir(dp);
                dp = NULL;
            }

        }while(dirQueue.size());

        return ret;
    #endif
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
public:
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
        handler = LoadLibraryA(path);
#else
        /**
         * RTLD_DEEPBIND : Place the lookup scope of the symbols in this shared
         * object ahead of the global scope.  This means that a self-contained
         * object will use its own symbols in preference to global symbols with
         * the same name contained in objects that have already been loaded.
         */
        handler = dlopen(path, RTLD_LAZY | RTLD_DEEPBIND);
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
    static std::string getError()
    {
#if (defined(_WIN32) || defined(_WIN64))
        char buf[128] = {0};
        _snprintf_s(buf, sizeof(buf),"last error code:%ld", GetLastError());
        return buf;
#else
        return dlerror();
#endif
    }
};

#if (defined(_WIN32) || defined(_WIN64))
    class Mutex
    {
    public:
        explicit Mutex();
        ~Mutex();
        void lock();
        void unlock();
        bool isRecursive() const;
    private:
        Mutex(Mutex &);
        Mutex& operator = (const Mutex &);
        friend class LockGuard;
    private:
        HANDLE m_mutex; 
    };

    Mutex::Mutex()
    {
        m_mutex = CreateMutex(NULL, false, NULL);
        if(NULL == m_mutex)
        {
            throw std::bad_alloc();
        }
    }

    Mutex::~Mutex()
    {
        if(m_mutex)
        {
            CloseHandle(m_mutex);
            m_mutex = NULL;
        }
    }

    void Mutex::lock()
    {
        int ret = WaitForSingleObject(m_mutex, INFINITE);
        switch(ret)
        {
        case WAIT_FAILED:
            throw std::runtime_error("WaitForSingleObject error.");
            break;
        default:
            break;
        }
    }

    void Mutex::unlock()
    {
        ReleaseMutex(m_mutex);
    }
#else
    class Mutex
    {
    public:
        enum RecursionMode {NonRecursive, Recursive};
    public:
        explicit Mutex(RecursionMode mode = NonRecursive);
        ~Mutex();
        void lock();
        void unlock();
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

    bool Mutex::isRecursive() const
    {
        return (m_recursionMode == Recursive);
    }
#endif


class LockGuard
{
public:
    inline explicit LockGuard(Mutex * m) : val(NULL)
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
