#ifndef _dir_h
#define _dir_h

#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

struct OPTIONS
{
    char *root_file_path;
    bool verbose;

    bool include_links;
    bool include_directories;
    bool include_unreadable;
    bool include_unknown;
};

int print_directory_tree(struct OPTIONS *opt);

#endif