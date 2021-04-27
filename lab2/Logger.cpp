#include "Logger.h"
FILE* outputFile=stderr;
int first=1;

char *mygetenv(const char *name) {
    if(name == NULL || environ == NULL)
        return NULL;
    int len = strlen(name);
    for(char **p = environ ; *p ; ++p) {
        if(!strncmp(name, *p, len) && (*p)[len] == '=')
            return *p + len + 1;
    }
    return NULL;
}
void *load_sym(const char *sym) {
    void *handler = dlopen("libc.so.6", RTLD_LAZY);
    void *res = dlsym(handler, sym);
    dlclose(handler);
    return res;
} 
FILE *get_output_fd() {
    static char output_path[PATH_MAX];
    static int first = 1;
    const char *env_output_path = mygetenv("MONITOR_OUTPUT");
    static char (*old_getcwd)(char*, size_t) = NULL;
    static FILE*(*old_fopen)(const char*, const char*) = NULL;
    if(env_output_path && strcmp(env_output_path, "stderr")) {
        if(first) {
            if(env_output_path[0] == '/') {
                strcpy(output_path, env_output_path);
            } else {
                if(!old_getcwd)old_getcwd = (char(*)(char*, size_t))load_sym("getcwd"); 
                old_getcwd(output_path, PATH_MAX);
                strcat(output_path, "/");
                strcat(output_path, env_output_path);
            }
        }
        if(!old_fopen)old_fopen= (FILE*(*)(const char*,const char*))load_sym("fopen"); 
        FILE *res = old_fopen(output_path, "a");
        first = 0;
        return res;
    } else {
        return stderr;
    }
}

void close_output_fd(FILE *fd) {
    static int(*old_fclose)(FILE*) = NULL;
    if(!old_fclose)old_fclose= (int(*)(FILE*))load_sym("fclose"); 
    if(fd != stderr) old_fclose(fd);
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
            printf("dlsym error");
        }
        dlclose(handler);
    }
    else
    {
        printf("dlopen error\n");
    }
    return originFunc;
}
/*
    void setupOutputFile(){
        printf("------initianl outpufile ----\n");
        if(Logger::outputFile !=NULL)
        printf("before outputfile fileno %d\n",fileno(Logger::outputFile));
        if(Logger::first){
            char* outputFileName = getenv("OUTPUT_FILE");
            Logger::outputFile =stderr;
            printf("constructor\n");
            printf("outputfilename %s\n",outputFileName);
            if(outputFileName != NULL){
                FILE* (*origin_fopen)(const char*,const char*) =(FILE*(*)(const char*,const char*)) get_origin_func("fopen");
                printf("func address %p",origin_fopen);
                Logger::outputFile = origin_fopen(outputFileName,"a");
                printf("after outputfile fileno %d\n",fileno(Logger::outputFile));
            }
            printf("Logger::first %d\n",Logger::first);
            Logger::first = 0;
            printf("Logger::first %d\n",Logger::first);
        }
        printf("------end initianl outpufile ----\n");
    }
    */
   string myReadLink(int fd){
       char buf[1024];
       string path  = "/proc/self/fd/" + to_string(fd);
       int n=readlink(path.c_str(),buf,sizeof(buf));
       buf[n]= '\0';
       return string(buf);
   }
   void printCurFileInfo(){
        if(outputFile !=NULL){
            string filename = myReadLink(fileno(outputFile));
            printf("---outputfile fileno %d filename %s first%d\n",fileno(outputFile),filename.c_str(),first);
        }
        else{
            printf("!!!!!!!!!!!!!!--outputfile is NULL");
        }
   }
void Logger::setupOutputFile()
{
    printf("------initial outpufile ----\n");
    //outputFile = stderr;
    char *outputFileName = getenv("MONITOR_OUTPUT");
     printf("before setup\n");
     printCurFileInfo();
    if((first || outputFile == stderr || outputFile == NULL)){
        if (outputFileName != NULL)
        {
            FILE *(*origin_fopen)(const char *, const char *) = (FILE * (*)(const char *, const char *)) get_origin_func("fopen");
            printf("func address %p",origin_fopen);
            outputFile = origin_fopen(outputFileName, "a");
        }
        else{
            outputFile = stderr;
        }
        first = 0;
    }
     printf("after setup\n");
     printCurFileInfo();
     printf("------end initianl outpufile ----\n");
}

void Logger::cleanup()
{
    printf("---- before cleanup\n");
     printCurFileInfo();
    if (outputFile != stderr)
    {
        printf("!!!!!!close the file\n");
        int (*origin_fclose)(FILE *) = (int (*)(FILE *))get_origin_func("fclose");
        if(origin_fclose(outputFile) <0){
            perror("origin_fclose fail");
        }
        outputFile = stderr;
    }
    printf("---- after cleanup\n");
     printCurFileInfo();
}
Logger::Logger(const char *func_name) : func_name(string(func_name))
{
    //setupOutputFile();
}

void Logger::printLog()
{

    //setupOutputFile();
    FILE *output_fd = get_output_fd();
    string output = "[logger] " + func_name + "(";

    for (int i = 0; i < argList.size(); i++)
    {
        output += i == 0 ? argList[i] : ", " + argList[i];
    }
    output += ") = ";
    output += returnValue + "\n";
    //fprintf(stderr,"%s",output.c_str()) ;
    //kfprintf(stderr,"fjdkffjfjkfkdsfkjdsdd") ;
    ///fprintf(outputFile, "%s", output.c_str());
    fprintf(output_fd, "%s", output.c_str());
    fflush(output_fd);
    close_output_fd(output_fd);
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
        printf("filepath:%s\n", fileFDPath.c_str());
        perror("FILE readlink error");
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
            printf("filepath:%s\n", fileFDPath.c_str());
            perror("FD close readlink error");
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
