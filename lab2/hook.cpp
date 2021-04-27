#include"marco.h"
#include"comm.h"
#include <dlfcn.h>

//FILE* outputFile=stderr;
//int Logger::first= 1;


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

__attribute__((destructor)) static void cleanup(){
    printf("deconstructor\n");
    if(outputFile != stderr){
        int (*origin_fclose)(FILE*) =(int(*)(FILE*)) get_origin_func("fclose");
        origin_fclose(outputFile);
    }
}
*/



extern "C"{
    MODIFY_FUNC_2ARG(0,FILE*,fopen,filePath,pathname,const char*,mode);
    MODIFY_FUNC_2ARG(0,int,chmod,filePath,pathname,mode_t,mode);
    MODIFY_FUNC_1ARG(1,int,close,FD,fd);
    MODIFY_FUNC_2ARG(0,int,creat,filePath,path,mode_t,mode);
    MODIFY_FUNC_1ARG(1,int,fclose,FILE*,stream);
    MODIFY_FUNC_4ARG(0,size_t,fread,void*,ptr,size_t,size,size_t,nmemb,FILE*,stream);
    MODIFY_FUNC_4ARG(0,size_t,fwrite,const void*,ptr,size_t,size,size_t,nmemb,FILE*,stream);
    MODIFY_FUNC_3ARG(0,ssize_t,read,FD,fd,void*,buf,size_t,_count);
    MODIFY_FUNC_1ARG(1,int,remove,filePath,pathname);
    MODIFY_FUNC_2ARG(0,int,rename,filePath,oldpath,filePath,newpath);
    MODIFY_FUNC_0ARG(FILE*,tmpfile);
    MODIFY_FUNC_3ARG(0,ssize_t,write,FD,fd,const void*,buf,size_t,_count);
    MODIFY_FUNC_OPEN(0,int,open,const char*,pathname,FLAG,flag,mode_t,mode);
    //chmod chown close creat fclose fopen fread fwrite open read remove rename tmpfile write
};
