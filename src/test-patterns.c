#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "patterns.h"

#define TARGET_FILE "hammerplusplus_dll.dll"

static unsigned char *buffer = NULL;
static size_t buffer_cap = 0;

static void process_file(const char *path, const char *dir_under_versions) {
    FILE *f = fopen(path, "rb");
    if (!f) return;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return;
    }

    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return;
    }

    rewind(f);

    if ((size_t)size > buffer_cap) {
        unsigned char *newbuf = realloc(buffer, (size_t)size);
        if (!newbuf) {
            fclose(f);
            return;
        }
        buffer = newbuf;
        buffer_cap = (size_t)size;
    }

    if (fread(buffer, 1, (size_t)size, f) != (size_t)size) {
        fclose(f);
        return;
    }

    printf("checking build %s\n", dir_under_versions);
    if (scan_all(buffer, (size_t)size)) {
        printf("all patterns found\n");
    }
    printf("\n\n");

    fclose(f);
}

static void recurse(const char *path, const char *dir_under_versions) {
    DIR *d = opendir(path);
    if (!d) return;

    struct dirent *ent;

    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, ent->d_name);

        struct stat st;
        if (stat(fullpath, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode)) {
            recurse(fullpath, dir_under_versions);
        } else if (strcmp(ent->d_name, TARGET_FILE) == 0) {
            process_file(fullpath, dir_under_versions);
        }
    }

    closedir(d);
}

int main(void) {
    DIR *versions = opendir("./versions");
    if (!versions) {
        perror("opendir");
        return 1;
    }

    struct dirent *ent;

    while ((ent = readdir(versions)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char path[4096];
        snprintf(path, sizeof(path), "./versions/%s", ent->d_name);

        struct stat st;
        if (stat(path, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode)) {
            recurse(path, ent->d_name);
        }
    }

    closedir(versions);
    free(buffer);
    return 0;
}
