#ifndef _dir_h
#define _dir_h

#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int print_directory_tree(const char *const dirpath);

#endif