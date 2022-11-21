#include <stdio.h>
#include <stdarg.h>

enum {
    LOG_INFO, LOG_DEBUG, LOG_ERROR
};

#define log_info(...) log_write(LOG_INFO,__FILE__,__LINE__,__VA_ARGS__)
#define log_debug(...) log_write(LOG_DEBUG,__FILE__,__LINE__,__VA_ARGS__)
#define log_error(...) log_write(LOG_ERROR,__FILE__,__LINE__,__VA_ARGS__)

int log_init(const char *file_name);

int log_free();

void log_write(int level, const char *file, int line, const char *fmt, ...);