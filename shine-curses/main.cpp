#include <iostream>
#include <zconf.h>
#include <signal.h>
#include <execinfo.h>
#include "module_menu.h"

void handler(int sig) {
    void *array[20];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 20);

    // print out all the frames to stderr
    std::cerr << "Segfault. Size is: " << size << std::endl;
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main() {
    signal(SIGSEGV, handler);
    new module_menu();
    std::cout << "Solaris Aerospace would like to thank you for using Shine! - \"Thera\"" << std::endl;
    return 0;
}