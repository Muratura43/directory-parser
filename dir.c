#include "dir.h"

/* 
    We want POSIX.1-2008 + XSI, i.e. SuSv4, features 
*/
#define _XOPEN_SOURCE 700

/* 
    If the C library can support 64-bit file sizes
    and offsets, using the standard names,
    these defines tell the C library to do so. 
*/
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

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
        printf(" %9.3f TiB", bytes / 1099511627776.0);
    else if (bytes >= 1073741824.0)
        printf(" %9.3f GiB", bytes / 1073741824.0);
    else if (bytes >= 1048576.0)
        printf(" %9.3f MiB", bytes / 1048576.0);
    else if (bytes >= 1024.0)
        printf(" %9.3f KiB", bytes / 1024.0);
    else
        printf(" %9.0f B  ", bytes);

    int sym = 0;

    switch (typeflag)
    {
    case FTW_SL:
        sym = handle_symbolic_link(filepath);

        if (sym != 0)
            return sym;
        break;

    case FTW_SLN:
        printf(" %s (dangling symlink)\n", filepath);
        break;

    case FTW_F:
        printf(" %s\n", filepath);
        break;

    case FTW_D:
    case FTW_DP:
        printf(" %s/ (directory)\n", filepath);
        break;

    case FTW_DNR:
        printf(" %s/ (unreadable)\n", filepath);
        break;

    case FTW_NS:
    default:
        printf(" %s (unknown)\n", filepath);
        break;
    }

    return 0;
}

int print_directory_tree(const char *const dirpath)
{
    int result;

    if (dirpath == NULL || *dirpath == '\0')
    {
        return errno = EINVAL;
    }

    result = nftw(dirpath, print_entry, USE_FDS, FTW_PHYS);
    if (result >= 0)
    {
        errno = result;
    }

    return errno;
}