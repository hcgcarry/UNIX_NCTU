#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>



int zombies(int n,int* pids){
    if(n == 0) sleep(10);
    else{
        for(int i=0;i<n;i++){
            if((pids[i]=fork())==0){
               exit(1);
            }

        }
    }
    return n;
}
