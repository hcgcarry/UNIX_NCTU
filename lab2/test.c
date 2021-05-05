
#include<stdio.h>
#include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>




int main(void){
    //FILE* file = fopen("test.txt","r");
    char buf[10];
    char filepath[1023] = "3test.txt";
    char filepath2[1023] = "test2.txt";

    printf("-----test.txt\n");
    FILE* file = fopen("test.txt","r");
    FILE* file2 = fopen64("test16.txt","w");
    fread(buf,1,sizeof(buf),file);
    fwrite(buf,1,strlen(buf),stdout);
    fread(buf,2,3,file);
    fclose(file);

    printf("-----test2.txt\n");
    int fd = open("test2.txt",O_RDWR);
    read(fd,buf,sizeof(buf));
    write(1,buf,strlen(buf));
    close(fd);

    printf("-----test3.txt\n");
    int fd2 = open("test3.txt",O_WRONLY|O_CREAT, O_APPEND);
    close(fd2);

    fd2 = open64("test11.txt",O_WRONLY|O_CREAT, O_APPEND);

    printf("-----test4.txt\n");
    creat("test4.txt",S_IRUSR);
    creat64("test14.txt",S_IRUSR);
    chmod("test4.txt",S_IWUSR);

    printf("-----test5.txt\n");
    creat("test5.txt",S_IWUSR);
    remove("test5.txt");


    printf("-----test6.txt\n");
    creat("test6.txt",S_IRUSR);
    rename("test6.txt","test7.txt");
    chown("test7.txt",getuid(),1 );
    tmpfile();
    tmpfile64();

    
}