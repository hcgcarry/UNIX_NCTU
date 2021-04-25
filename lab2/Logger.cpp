#include "comm.h"
extern FILE *outputFile;

class Logger
{
public:
    vector<string> argList;
    string returnValue;
    string func_name;
    Logger(const char* func_name):func_name(string(func_name)){
    }

    //template<typename T>
    template <typename T>
    void push_arg(const string &argType, T &arg)
    {
        argList.push_back(handleArg(argType, arg));
        printf("push_arg:type: %s process_result: %s\n", argType.c_str() , argList.back().c_str() );
    }
    template <typename T>
    void push_ret(string argType, T arg)
    {
        this->returnValue = handleRet(argType, arg);
        printf("push_ret:type: %s process_result: %s\n", argType.c_str() ,returnValue.c_str());
    }
    void printLog()
    {
        string output = "[logger] "+func_name +"(";
        
        for (int i = 0; i < argList.size(); i++)
        {
            output += i == 0 ? argList[i] : ", " + argList[i];
        }
        output += ") = ";
        output += returnValue + "\n";
        fprintf(stderr,"%s",output.c_str()) ;
    }
    // handle argument
    string handleArg(string argType, const char *arg)
    {
        //cout << "----- handleArg "<< argType << " const char*"<< endl;
        printf("----- handleArg argtype:%s const char*\n",argType.c_str());

        char buf[1024];
        string result;
        if (argType == "filePath")
        {
            if (realpath(arg, buf) == NULL)
            {
                return string(arg);
            }
            result = string(buf);
        }
        else
        {
            int i = 0;
            for (i = 0; arg[i] && i < 32; i++)
            {
                if (isprint(arg[i]))
                    buf[i] = arg[i];
                else
                    buf[i] = '.';
            }
            buf[i] = '\0';
            result = string(buf);
        }
        return "\"" + result + "\"";
    }
    string handleArg(string argType, FILE *arg)
    {
        printf("----- handleArg argtype:%s FILE*\n",argType.c_str());
        int fd = fileno(arg);
        printf("fd %d\n",fd);
        string fileFDPath = "/proc/self/fd/" + to_string(fd);
        char buf[1024];
        int readSize;
        if((readSize= readlink(fileFDPath.c_str(), buf, sizeof(buf)))<0){
            printf("filepath:%s\n",fileFDPath.c_str());
            perror("readlink error");
        }
        buf[readSize] = '\0';
        char buf2[1024];
        buf2[readSize] = '\0';
        if (realpath(buf, buf2) == NULL){
            printf("filepath:%s\n",fileFDPath.c_str());
            perror("realpath error");
        }
        return "\""+string(buf2)+"\"";
    }

    string handleArg(string argType, unsigned int arg)
    {
        ///cout << "----- handleArg "<< argType <<" unsigned int "<< endl;
        printf("---- handleArg %s arg:%u\n",argType.c_str(),arg);
        string result;
        if (argType == "mode_t")
        {
            char buf[1024];
            sprintf(buf, "%o", arg);
            result = string(buf);
        }
        else
        {
            result = to_string(arg);
        }
        return result;
    }
    string handleArg(string argType, int arg)
    {
        printf("----- handleArg argtype:%s int*\n",argType.c_str());
        return to_string(arg);
    }

    string handleArg(string argType, const void *arg)
    {
        printf("----- handleArg argtype:%s const void*\n",argType.c_str());
        char buf[10];
        sprintf(buf, "%p", arg);
        return string(buf);
    }
    string handleArg(string argType,unsigned long arg)
    {
        //cout << "----- handleRet "<< argType <<" int"<< endl;
        printf("----- handleArg argtype:%s unsigned long*\n",argType.c_str());
        return to_string(arg);
    }
    string handleArg(string argType,long arg)
    {
        //cout << "----- handleRet "<< argType <<" int"<< endl;
        printf("----- handleArg argtype:%s long*\n",argType.c_str());
        return to_string(arg);
    }
    string handleRet(string argType,long arg)
    {
        printf("----- handleRet argtype:%s long*\n",argType.c_str());
        return to_string(arg);
    }
    string handleRet(string argType, int arg)
    {
        printf("----- handleRet argtype:%s int*\n",argType.c_str());
        //cout << "----- handleRet "<< argType <<" int"<< endl;
        return to_string(arg);
    }
    string handleRet(string argType,unsigned long arg)
    {
        printf("----- handleRet argtype:%s unsigned long*\n",argType.c_str());
        //cout << "----- handleRet "<< argType <<" int"<< endl;
        return to_string(arg);
    }

    string handleRet(string argType, const void *arg)
    {
        printf("----- handleRet argtype:%s const void*\n",argType.c_str());
        //cout << "----- handleRet "<< argType << " const void*"<< endl;
        char buf[15];
        sprintf(buf, "%p", arg);
        return string(buf);
    }
};
