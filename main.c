#include "dir.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int arg;

    if (argc < 2)
    {
        if (print_directory_tree("."))
        {
            fprintf(stderr, "%s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }
    else
    {
        for (arg = 1; arg < argc; arg++)
        {
            if (print_directory_tree(argv[arg]))
            {
                fprintf(stderr, "%s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}