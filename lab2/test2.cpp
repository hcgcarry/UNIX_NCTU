#include <iostream>

void fail() {
    int *p1;
    int *p2(NULL);
    int *p3 = NULL;
    *p3 = 1;
    if (p3) {
        std::cout << *p3 << std::endl;
    } 
}

int main() { 
    fail();
}