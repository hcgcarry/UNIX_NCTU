
int main(void){
    printf("result:%d\n",recur(5));
}
int recur(int n){
    if (n<=0){
        return 0;
    }
    if(n==1) return 1;
    return 2*recur(n-1)+3*recur(n-2);
}