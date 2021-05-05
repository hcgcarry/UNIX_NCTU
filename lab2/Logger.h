#include "comm.h"
#include <dlfcn.h>
//#define debug
class Logger
{
public:
    vector<string> argList;
    //static int first;
    //static static FILE *outputFile;
    //FILE *outputFile;
    string returnValue;
    string func_name;
    void* open_write_result=NULL;
    int fwrite_fread_size=1;
    void* get_origin_func(string func_name);
    void setupOutputFile();
    void cleanup();
    Logger(const char* func_name);
    //template<typename T>
    FILE *set_outputFile();
    void close_outputFile(FILE *outputFile) ;
    template <typename T>
    void push_arg(const string &argType, T &arg)
    {
        argList.push_back(handleArg(argType, arg));
        #ifdef debug
        printf("push_arg:type: %s process_result: %s\n", argType.c_str() , argList.back().c_str() );
        #endif
    }
    template <typename T>
    void push_ret(string argType, T arg)
    {
        this->returnValue = handleRet(argType, arg);
        #ifdef debug
        printf("push_ret:type: %s process_result: %s\n", argType.c_str() ,returnValue.c_str());
        #endif
    }
    void printLog();
    string handleArg(string argType, const char *arg);
    string handleArg(string argType, FILE *arg);

    string handleArg(string argType, unsigned int arg);
    string handleArg(string argType, int arg);
    string handleArg(string argType, const void *arg);
    string handleArg(string argType,unsigned long arg);
    string handleArg(string argType,long arg);
    string handleRet(string argType,long arg);
    string handleRet(string argType, int arg);
    string handleRet(string argType,unsigned long arg);
    string handleRet(string argType, const void *arg);
};