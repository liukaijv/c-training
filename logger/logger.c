#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"

static FILE *fd = NULL;

static char *level_labels[] = {
        "info", "debug", "error"
};

int log_init(const char *file_name) {
    fd = fopen(file_name, "a");
    if (fd == NULL) {
        return -1;
    }
    return 0;
}

int log_free() {
    if (fd != NULL) {
        return fclose(fd);
    }
    return 0;
}

void log_write(int level, const char *file, int line, const char *fmt, ...) {

    if (fd == NULL) {
        return;
    }

    va_list args;

    char date_buf[32];

    time_t tz = time(NULL);
    struct tm *ltm = localtime(&tz);
    strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", ltm);

    fprintf(fd, "%s level: %s %s@%d ", date_buf, level_labels[level], file, line);

    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);
    fprintf(fd, "\n");

    fflush(fd);

}
