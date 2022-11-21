#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "logger.h"

int main(int argc, char **argv) {

    char *log_file = "/tmp/test.log";

    int ret = log_init(log_file);
    if (ret < 0) {
        perror("log init err: ");
        exit(EXIT_FAILURE);
    }

    log_info("log_info: %s", "I am info.");
    log_debug("log_debug: %s", "I am debug.");
    log_error("log_error: %s", "I am error.");

    log_free();

    return 0;
}