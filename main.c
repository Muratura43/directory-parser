#include "dir.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    struct OPTIONS opt;
    opt.root_file_path = ".";
    opt.verbose = false;
    opt.include_links = false;
    opt.include_directories = false;
    opt.include_unreadable = false;
    opt.include_unknown = false;

    if (print_directory_tree(&opt))
    {
        fprintf(stderr, "%s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}