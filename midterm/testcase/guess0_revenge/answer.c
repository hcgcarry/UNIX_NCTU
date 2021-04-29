#include<stdio.h>


int last=1;
int rand(){
    last ^= 1;
    return last;
}

size_t read(int fd,void *buf,size_t size){
    char* buf1  = (char*) buf;
    buf1[0] = 1+last + '0';
    buf1[1] = '\0';
    return 6;
}