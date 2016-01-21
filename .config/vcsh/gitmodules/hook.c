#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

static const char* subst(const char* path) {
    char *workdir = getenv("VCSH_BASE");
    if(!workdir)
        workdir = getenv("HOME");

    if (workdir) {
        char *gitmodules = getenv("VCSH_GITMODULES");
        char modulespath[PATH_MAX+1];
        strcpy(modulespath, workdir);
        strcat(modulespath, "/.gitmodules");
        if(gitmodules && strcmp(path, modulespath) == 0) {
            return gitmodules;
        }
    }
    return path;
}

int open(const char *path, int flags, ...) {
    va_list args;
    mode_t mode;

    va_start (args, flags);
    mode = va_arg (args, mode_t);
    va_end (args);

    static int (*original_open)(const char*, int, mode_t);
    if(!original_open)
        original_open = dlsym(RTLD_NEXT, "open");
    return (*original_open)(subst(path), flags, mode);
}

int open64(const char *path, int flags, ...) {
    va_list args;
    mode_t mode;

    va_start (args, flags);
    mode = va_arg (args, mode_t);
    va_end (args);

    static int (*original_open)(const char*, int, mode_t);
    if(!original_open)
        original_open = dlsym(RTLD_NEXT, "open64");
    return (*original_open)(subst(path), flags, mode);
}

FILE* fopen(const char *path, const char* mode) {
    static FILE* (*original_open)(const char*, const char*);
    if(!original_open)
        original_open = dlsym(RTLD_NEXT, "fopen");
    return (*original_open)(subst(path), mode);
}

FILE* fopen64(const char *path, const char* mode) {
    static FILE* (*original_open)(const char*, const char*);
    if(!original_open)
        original_open = dlsym(RTLD_NEXT, "fopen64");
    return (*original_open)(subst(path), mode);
}
