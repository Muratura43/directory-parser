#include "parser.h"

struct OPTIONS opt;

int handle_symbolic_link(const char *filepath)
{
    char *target;
    size_t maxlen = 1023;
    ssize_t len;

    while (1)
    {
        target = malloc(maxlen + 1);

        if (target == NULL)
            return ENOMEM;

        len = readlink(filepath, target, maxlen);

        if (len == (ssize_t)-1)
        {
            const int saved_errno = errno;
            free(target);
            return saved_errno;
        }

        if (len >= (ssize_t)maxlen)
        {
            free(target);
            maxlen += 1024;
            continue;
        }

        target[len] = '\0';
        break;
    }

    printf(" %s -> %s\n", filepath, target);
    free(target);

    return 0;
}

int print_entry(const char *filepath, const struct stat *info, const int typeflag, struct FTW *pathinfo)
{
    if (opt.verbose)
    {
        const double bytes = (double)info->st_size;
        struct tm mtime;

        localtime_r(&(info->st_mtime), &mtime);

        printf("%04d-%02d-%02d %02d:%02d:%02d",
               mtime.tm_year + 1900,
               mtime.tm_mon + 1,
               mtime.tm_mday,
               mtime.tm_hour,
               mtime.tm_min,
               mtime.tm_sec);

        if (bytes >= 1099511627776.0)
            printf(" %9.3f TiB ", bytes / 1099511627776.0);
        else if (bytes >= 1073741824.0)
            printf(" %9.3f GiB ", bytes / 1073741824.0);
        else if (bytes >= 1048576.0)
            printf(" %9.3f MiB ", bytes / 1048576.0);
        else if (bytes >= 1024.0)
            printf(" %9.3f KiB ", bytes / 1024.0);
        else
            printf(" %9.0f B   ", bytes);
    }

    switch (typeflag)
    {
    case FTW_SL:
        if (opt.include_links)
        {
            int sym = 0;
            sym = handle_symbolic_link(filepath);

            if (sym != 0)
                return sym;
        }
        break;

    case FTW_SLN:
        if (opt.include_links)
        {
            if (opt.verbose)
                printf("%s (dangling symlink)\n", filepath);
            else
                printf("%s\n", filepath);
        }
        break;

    case FTW_F:
        printf("%s\n", filepath);
        break;

    case FTW_D:
    case FTW_DP:
        if (opt.include_directories)
        {
            if (opt.verbose)
                printf("%s/ (directory)\n", filepath);
            else
                printf("%s\n", filepath);
        }
        break;

    case FTW_DNR:
        if (opt.include_unreadable)
        {
            if (opt.verbose)
                printf("%s/ (unreadable)\n", filepath);
            else
                printf("%s\n", filepath);
        }
        break;

    case FTW_NS:
    default:
        if (opt.include_unknown)
        {
            if (opt.verbose)
                printf("%s (unknown)\n", filepath);
            else
                printf("%s\n", filepath);
        }
        break;
    }

    return 0;
}

int print_directory_tree(struct OPTIONS *options)
{
    opt = *options;
    int result;

    if (opt.root_file_path == NULL || *opt.root_file_path == '\0')
    {
        return errno = EINVAL;
    }

    result = nftw(opt.root_file_path, print_entry, USE_FDS, FTW_PHYS);
    if (result >= 0)
    {
        errno = result;
    }

    return errno;
}