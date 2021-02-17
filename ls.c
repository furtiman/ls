/**
 * @file ls.c
 * @author Ivan Turasov
 * @brief Test version of ls utility with -l flag
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

#define DEBUG 0

/**
 * @brief Display file type
 *
 * @param mode file mode (struct stat)
 */
void print_filetype(mode_t mode)
{
    switch (mode & S_IFMT)
    {
    case S_IFREG:
        putchar('-');
        break;
    case S_IFDIR:
        putchar('d');
        break;
    case S_IFLNK:
        putchar('l');
        break;
    case S_IFCHR:
        putchar('c');
        break;
    case S_IFBLK:
        putchar('b');
        break;
    case S_IFSOCK:
        putchar('s');
        break;
    case S_IFIFO:
        putchar('f');
        break;
    }
}

/**
 * @brief Display file permissions
 *
 * @param mode file mode (struct stat)
 */
void print_permissions(mode_t mode)
{
    putchar((mode & S_IRUSR) ? 'r' : '-');
    putchar((mode & S_IWUSR) ? 'w' : '-');
    putchar((mode & S_IXUSR) ? 'x' : '-');
    putchar((mode & S_IRGRP) ? 'r' : '-');
    putchar((mode & S_IWGRP) ? 'w' : '-');
    putchar((mode & S_IXGRP) ? 'x' : '-');
    putchar((mode & S_IROTH) ? 'r' : '-');
    putchar((mode & S_IWOTH) ? 'w' : '-');
    putchar((mode & S_IXOTH) ? 'x' : '-');
}

/**
 * @brief Print time of file creation
 * 
 * @param mod_time file time (struct stat)
 */
void print_time(time_t mod_time)
{
    // get current time with year
    time_t curr_time;
    time(&curr_time);
    struct tm *t = localtime(&curr_time);
    const int curr_mon = t->tm_mon;
    const int curr_yr = 1970 + t->tm_year;

    // get mod time and year
    t = localtime(&mod_time);
    const int mod_mon = t->tm_mon;
    const int mod_yr = 1970 + t->tm_year;

    // determine format based on years
    const char *format = ((mod_yr == curr_yr) && (mod_mon >= (curr_mon - 6)))
                             ? "%b %e %H:%M"
                             : "%b %e  %Y";

    char time_buf[128];
    strftime(time_buf, sizeof(time_buf), format, t);
    printf("%s   ", time_buf);
}

/**
 * @brief Show extended attributes of the file
 * 
 * @param path Path to the file
 */
void list_extended(char *path)
{
    struct stat file_stat;

    if(lstat(path, &file_stat) != 0)
    {
        if (DEBUG)
        {
            printf("lstat error\n");
        }
        exit(0);
    }

    print_filetype(file_stat.st_mode);
    print_permissions(file_stat.st_mode);

    printf("  %d ", file_stat.st_nlink);
    printf("%7s ", getpwuid(file_stat.st_uid)->pw_name);
    printf("%7s    ", getgrgid(file_stat.st_gid)->gr_name);
    printf("%7ld ", (long)file_stat.st_size);
    print_time(file_stat.st_mtime);
}

/**
 * @brief Print contents of the directory
 * 
 * @param path Path to the directory
 * @param lflag Extended attributes flag
 */
void print_dir_content(char *path, int lflag)
{
    DIR *current_dir = opendir(path);
    struct dirent *dir;

    if (current_dir)
    {
        while ((dir = readdir(current_dir)) != NULL)
        {
            if (lflag)
            {
                list_extended(dir->d_name);
            }
            switch (dir->d_type)
            {
            case DT_DIR:
                printf(BLUE);
                printf("%s\n", dir->d_name);
                break;
            case DT_REG:
                printf(NORMAL_COLOR);
                printf("%s\n", dir->d_name);
                break;
            case DT_LNK:
                printf(GREEN);
                printf("%s\n", dir->d_name);
                break;
            default:
                printf(NORMAL_COLOR);
                printf("%s\n", dir->d_name);
                break;
            }
            printf(NORMAL_COLOR);
        }
        closedir(current_dir);
    }
}

/**
 * @brief Print usage instructions
 * 
 * @param argv0 
 */
static void usage(char *argv0)
{
    fprintf(stdout, "\nDisplay contents of the directory\n"
                    "Usage: %s [options] <path>\n\n"
                    "Options:\n"
                    "\t-l                List in long format.\n"
                    "\t-h                Print this help message.\n",
            argv0);
    exit(-1);
}

int main(int argc, char *argv[])
{
    int option_index = 1, c;
    int lflag = 0;
    while ((c = getopt(argc, argv, ":lh")) != -1)
    {
        switch (c)
        {
        case 'l':
            lflag = 1;
            option_index ++;
            break;
        case 'h':
            usage(argv[0]);
            break;
        default:
            printf("Unknown option '%c'\n", c);
            usage(argv[0]);
            break;
        }
    }

    print_dir_content(argv[option_index], lflag);

    return 0;
}
