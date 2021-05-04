#include "Logger.h"

// 這邊後來改良後沒用到
FILE *Logger::set_outputFile() {
    //把這個static 丟到 class的member也不行 不知道為甚麼
    static int first = 1;
    static char output_path[4096];
    const char *output_file_name = getenv("OUTPUT_FILE");
    static FILE* outputFile;
    if(output_file_name && strcmp(output_file_name, "stderr")) {
        //if(first){} 這邊加了可以防止 ./logger -o output -- bash segmentation fault
        if(first) {
            if(output_file_name[0] == '/') {
                strcpy(output_path, output_file_name);
            } else {
                //if(!old_getcwd)old_getcwd = (char(*)(char*, size_t))get_origin_func("getcwd"); 
                getcwd(output_path, 4096);
                strcat(output_path, "/");
                strcat(output_path, output_file_name);
            }
            FILE*(*old_fopen)(const char*, const char*) = (FILE*(*)(const char*,const char*))get_origin_func("fopen"); 
            outputFile= old_fopen(output_path, "w");
            first = 0;
        }
        return outputFile;
    } else {
        return stderr;
    }
}

void Logger::close_outputFile(FILE *outputFile) {
    //int(*old_fclose)(FILE*) = (int(*)(FILE*))get_origin_func("fclose"); 
    //if(outputFile != stderr) old_fclose(outputFile);
}

void *Logger::get_origin_func(string func_name)
{
    void *handler = dlopen("libc.so.6", RTLD_LAZY);
    //void *handler = dlopen("/lib/x86_64-linux-gnu/libc-2.27.so", RTLD_LAZY);
    void *originFunc;
    if (handler != NULL)
    {
        if ((originFunc = dlsym(handler, func_name.c_str())) == NULL)
        {
            // printf("dlsym error");
        }
        dlclose(handler);
    }
    else
    {
        // printf("dlopen error\n");
    }
    return originFunc;
}
   string myReadLink(int fd){
       char buf[1024];
       string path  = "/proc/self/fd/" + to_string(fd);
       int n=readlink(path.c_str(),buf,sizeof(buf));
       buf[n]= '\0';
       return string(buf);
   }
Logger::Logger(const char *func_name) : func_name(string(func_name))
{
    //setupOutputFile();
}

void Logger::printLog()
{

    //setupOutputFile();
    //FILE *output_fd = set_outputFile();
    string output = "[logger] " + func_name + "(";

    for (int i = 0; i < argList.size(); i++)
    {
        output += i == 0 ? argList[i] : ", " + argList[i];
    }
    output += ") = ";
    output += returnValue + "\n";
    dprintf(3 , "%s", output.c_str());
    //fflush(output_fd);
    //close_outputFile(output_fd);
    //cleanup();
}
// handle argument
string Logger::handleArg(string argType, const char *arg)
{
//cout << "----- handleArg "<< argType << " const char*"<< endl;
#ifdef debug
    printf("----- handleArg argtype:%s const char*\n", argType.c_str());
#endif

    char buf[1024];
    string result;
    if (argType == "filePath")
    {
        if (realpath(arg, buf) == NULL)
        {
            result = string(arg);
        }
        else
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
string Logger::handleArg(string argType, FILE *arg)
{
#ifdef debug
    printf("----- handleArg argtype:%s FILE*\n", argType.c_str());
#endif
    int fd = fileno(arg);
    string fileFDPath = "/proc/self/fd/" + to_string(fd);
    char buf[1024];
    int readSize;
    if ((readSize = readlink(fileFDPath.c_str(), buf, sizeof(buf))) < 0)
    {
        // printf("filepath:%s\n", fileFDPath.c_str());
        // perror("FILE readlink error");
    }
    buf[readSize] = '\0';
    char buf2[1024];
    buf2[readSize] = '\0';
    if (realpath(buf, buf2) == NULL)
    {
        //printf("filepath:%s\n",fileFDPath.c_str());
        //perror("FILE realpath error");
        return "\"" + string(buf) + "\"";
    }
    return "\"" + string(buf2) + "\"";
}

string Logger::handleArg(string argType, unsigned int arg)
{
///cout << "----- handleArg "<< argType <<" unsigned int "<< endl;
#ifdef debug
    printf("---- handleArg %s arg:%u\n", argType.c_str(), arg);
#endif
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
string Logger::handleArg(string argType, int arg)
{
#ifdef debug
    printf("----- handleArg argtype:%s int*\n", argType.c_str());
    fprintf(stderr, "----- handleArg argtype:%s int*\n", argType.c_str());
#endif

    if (argType == "FLAG")
    {
        char buf[1024];
        sprintf(buf, "%o", arg);
        return string(buf);
    }
    if (argType == "FD")
    {
        string fileFDPath = "/proc/self/fd/" + to_string(arg);
        char buf[4024];
        int readSize;
        if ((readSize = readlink(fileFDPath.c_str(), buf, sizeof(buf))) < 0)
        {
            // printf("filepath:%s\n", fileFDPath.c_str());
            // perror("FD close readlink error");
        }
        buf[readSize] = '\0';
        char buf2[4024];
        if (realpath(buf, buf2) == NULL)
        {
            return "\"" + string(buf) + "\"";
            //printf("filepath:%s\n",fileFDPath.c_str());
            //perror("FD close realpath error");
        }
        return "\"" + string(buf2) + "\"";
    }
    return to_string(arg);
}

string Logger::handleArg(string argType, const void *arg)
{
#ifdef debug
    printf("----- handleArg argtype:%s const void*\n", argType.c_str());
    fprintf(stderr, "----- handleArg argtype:%s const void*\n", argType.c_str());
#endif
    const char *arg2 = (const char *)arg;
    char buf[1024];
    int i = 0;
    for (i = 0; arg2[i] && i < 32; i++)
    {
        if (isprint(arg2[i]))
            buf[i] = arg2[i];
        else
            buf[i] = '.';
    }
    buf[i] = '\0';
    string result = "\"" + string(buf) + "\"";
    //fprintf(stderr,"----- result %s\n",result.c_str());
    return result;
}
string Logger::handleArg(string argType, unsigned long arg)
{
//cout << "----- handleRet "<< argType <<" int"<< endl;
//fprintf(stderr,"----- handleArg argtype:%s unsigned long %lu\n",argType.c_str(),arg);
#ifdef debug
#endif
    return to_string(arg);
}
string Logger::handleArg(string argType, long arg)
{
//cout << "----- handleRet "<< argType <<" int"<< endl;
#ifdef debug
    printf("----- handleArg argtype:%s long*\n", argType.c_str());
#endif
    return to_string(arg);
}
string Logger::handleRet(string argType, long arg)
{
#ifdef debug
    printf("----- handleRet argtype:%s long*\n", argType.c_str());
#endif
    return to_string(arg);
}
string Logger::handleRet(string argType, int arg)
{
#ifdef debug
    printf("----- handleRet argtype:%s int*\n", argType.c_str());
#endif
    //cout << "----- handleRet "<< argType <<" int"<< endl;
    return to_string(arg);
}
string Logger::handleRet(string argType, unsigned long arg)
{
#ifdef debug
    printf("----- handleRet argtype:%s unsigned long*\n", argType.c_str());
#endif
    //cout << "----- handleRet "<< argType <<" int"<< endl;
    return to_string(arg);
}

string Logger::handleRet(string argType, const void *arg)
{
#ifdef debug
    printf("----- handleRet argtype:%s const void*\n", argType.c_str());
#endif
    //cout << "----- handleRet "<< argType << " const void*"<< endl;
    char buf[1024];
    sprintf(buf, "%p", arg);
    return string(buf);
}
