#ifndef _dir_h
#define _dir_h

#define _GNU_SOURCE

/* 
    If the C library can support 64-bit file sizes
    and offsets, using the standard names,
    these defines tell the C library to do so. 
*/
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* 
    POSIX.1 says each process has at least 20 file descriptors.
    Three of those belong to the standard streams.
    Here, we use a conservative estimate of 15 available;
    assuming we use at most two for other uses in this program,
    we should never run into any problems.
    Most trees are shallower than that, so it is efficient.
    Deeper trees are traversed fine, just a bit slower.
    (Linux allows typically hundreds to thousands of open files,
    so you'll probably never see any issues even if you used
    a much higher value, say a couple of hundred, but
    15 is a safe, reasonable value.)
*/
#ifndef USE_FDS
#define USE_FDS 15
#endif

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