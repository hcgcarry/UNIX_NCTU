#include<stdio.h>
/// 使用LD_PRELOAD 把rand 換掉


int rand(){
    return 1;
}