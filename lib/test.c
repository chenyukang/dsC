
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void wait_attach() {
    fprintf(stderr, "Waiting attach pid: %d\n", getpid());
    while(1) {
        if((access("./debug", F_OK)) != -1) {
            break;
        }
        else
            ;
    }
}

int test() {
    int* p = 0;
    *p = 0;
    //int* p = (int*)malloc(sizeof(int));;
//    *p = 0;
}

int main() {
    wait_attach();
    //BacktraceOnSegv();
    test();
}
