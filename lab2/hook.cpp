#include"marco.h"
#include"comm.h"
#include <dlfcn.h>



/*
void* get_origin_func(string func_name){
    void* handler = dlopen("libc.so.6",RTLD_LAZY); 
    void* originFunc = dlsym(handler, func_name.c_str()); 
    dlclose(handler);                                                   
    return originFunc;
}
__attribute__((constructor)) static void setupOutputFile(){
    outputFile = stderr;
    char* outputFileName = getenv("OUTPUT_FILE");
    printf("constructor\n");
    if(outputFileName != NULL){
        FILE* (*origin_fopen)(const char*,const char*) =(FILE*(*)(const char*,const char*)) get_origin_func("fopen");
        outputFile = origin_fopen(outputFileName,"w");
    }
}

*/

/*
void *get_origin_func(string func_name)
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
__attribute__((destructor)) static void cleanup(){
    printf("deconstructor\n");
    int (*origin_close)(int) =(int(*)(int)) get_origin_func("close");
    origin_close(3);
}
*/


extern "C"{
    MODIFY_FUNC_2ARG(0,FILE*,fopen,filePath,pathname,const char*,mode);
    MODIFY_FUNC_2ARG(0,FILE*,fopen64,filePath,pathname,const char*,mode);
    MODIFY_FUNC_2ARG(0,int,chmod,filePath,pathname,mode_t,mode);
    MODIFY_FUNC_3ARG(1,int,chown,filePath,pathname,uid_t,owner,gid_t,group);
    MODIFY_FUNC_1ARG(1,int,close,FD,fd);
    MODIFY_FUNC_2ARG(0,int,creat,filePath,path,mode_t,mode);
    MODIFY_FUNC_2ARG(0,int,creat64,filePath,path,mode_t,mode);
    MODIFY_FUNC_1ARG(1,int,fclose,FILE*,stream);
    MODIFY_FUNC_4ARG(0,size_t,fread,void*,ptr,size_t,size,size_t,nmemb,FILE*,stream);
    MODIFY_FUNC_4ARG(0,size_t,fwrite,const void*,ptr,size_t,size,size_t,nmemb,FILE*,stream);
    MODIFY_FUNC_3ARG(0,ssize_t,read,FD,fd,void*,buf,size_t,_count);
    MODIFY_FUNC_1ARG(1,int,remove,filePath,pathname);
    MODIFY_FUNC_2ARG(0,int,rename,filePath,oldpath,filePath,newpath);
    MODIFY_FUNC_0ARG(FILE*,tmpfile);
    MODIFY_FUNC_0ARG(FILE*,tmpfile64);
    MODIFY_FUNC_3ARG(0,ssize_t,write,FD,fd,const void*,buf,size_t,_count);
    MODIFY_FUNC_OPEN(0,int,open,const char*,pathname,FLAG,flag,mode_t,mode);
    MODIFY_FUNC_OPEN(0,int,open64,const char*,pathname,FLAG,flag,mode_t,mode);
};
