
#include<stdio.h>



int main(void){
    //FILE* file = fopen("test.txt","r");
    FILE* file = fopen("/proc/filesystems","re");
    char buf[1024];
    fgets(buf,sizeof(buf),file);
    printf("result:%s",buf);
    fclose(file);
}