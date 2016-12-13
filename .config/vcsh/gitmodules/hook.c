#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

static void subst(const char* func, const char* path, char** result, int *should_free) {
    *should_free = 0;
    *result = path;

    char *workdir = getenv("VCSH_BASE");
    if(!workdir)
        workdir = getenv("HOME");

    if (workdir) {
        char *reponame = getenv("VCSH_REPO_NAME");
        if(!reponame) return;
        char *gitmodules = malloc(PATH_MAX+1);
        strcpy(gitmodules, workdir);
        strcat(gitmodules, "/.gitmodules.d/");
        strcat(gitmodules, reponame);
        char modulespath[PATH_MAX+1];
        strcpy(modulespath, workdir);
        strcat(modulespath, "/.gitmodules");
        //fprintf(stderr, "MODULESPATH: %s %s %s\n", func, path, gitmodules);
        if(gitmodules && (strcmp(path, modulespath) == 0 || strcmp(path, ".gitmodules") == 0)) {
            *result = gitmodules;
            *should_free = 1;
            return;
        }
    }
}

#define SUBST(ret_t, f_name, fargt, farg, path) \
    static ret_t (*original_##f_name) fargt; \
    if(!original_##f_name) original_##f_name = dlsym(RTLD_NEXT, #f_name); \
    int should_free=0; \
    char* subpath; \
    subst(#f_name, path, &subpath, &should_free); \
    ret_t retval = (*original_##f_name) farg; \
    if(should_free) free(subpath); \
    return retval;

int open(const char *path, int flags, ...) {
    va_list args;
    mode_t mode;

    va_start (args, flags);
    mode = va_arg (args, mode_t);
    va_end (args);

    SUBST(int, open, (const char*, int, mode_t), (subpath, flags, mode), path)
}

int open64(const char *path, int flags, ...) {
    va_list args;
    mode_t mode;

    va_start (args, flags);
    mode = va_arg (args, mode_t);
    va_end (args);

    SUBST(int, open64, (const char*, int, mode_t), (subpath, flags, mode), path)
}

FILE* fopen(const char *path, const char* mode) {
    SUBST(FILE*, fopen, (const char*, const char*), (subpath, mode), path)
}

FILE* fopen64(const char *path, const char* mode) {
    SUBST(FILE*, fopen64, (const char*, const char*), (subpath, mode), path)
}

int stat(const char *path, void *buf) {
    SUBST(int, stat, (const char*, void*), (subpath, buf), path)
}

int lstat(const char *path, void *buf) {
    SUBST(int, lstat, (const char*, void*), (subpath, buf), path)
}

int stat64(const char *path, void *buf) {
    SUBST(int, stat64, (const char*, void*), (subpath, buf), path)
}

int lstat64(const char *path, void *buf) {
    SUBST(int, lstat64, (const char*, void*), (subpath, buf), path)
}
