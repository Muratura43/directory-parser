#include "lib/parser/parser.h"

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

    int c, index;

    while ((c = getopt(argc, argv, "vldku")) != -1)
    {
        switch (c)
        {
        case 'v':
            opt.verbose = true;
            break;
        case 'l':
            opt.include_links = true;
            break;
        case 'd':
            opt.include_directories = true;
            break;
        case 'u':
            opt.include_unreadable = true;
            break;
        case 'k':
            opt.include_unknown = true;
            break;
        default:
            abort();
        }
    }

    for (index = optind; index < argc; index++)
    {
        opt.root_file_path = argv[index];
    }

    if (print_directory_tree(&opt))
    {
        fprintf(stderr, "%s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}