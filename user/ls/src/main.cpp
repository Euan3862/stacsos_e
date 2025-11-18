#include <stacsos/dirent.h>
#include <stacsos/user-syscall.h>
#include <stacsos/console.h>
#include <stacsos/string.h>
#include <stacsos/memops.h>

using namespace stacsos;

/*
*  ls
*
*  Calls the readdir system call with a user provided path and prints
*   either a normal listing or a 'long' listing i.e. ls -l.
*
*   The system call fils an array of directory entry structures,
*   These are iterated through and the results are formatted then displayed.
*/
static void ls(int long_flag, const char *path)
{
    dirent entries[256]; // Buffer to store directory entries up to 256 entries.

    auto res = syscalls::read_dir(path, entries, 256);
    if (res.code != syscall_result_code::ok) {
        console::get().write("ls: failed to read directory\n");
        return;
    }

    for (u64 i = 0; i < res.length; i++) {

        if (long_flag) {
            // Printing type + name, and size for files.
            char type = entries[i].type == 'd' ? 'D' : 'F';

            console::get().writef("[%c] %s", type, entries[i].name);

            if (type == 'F') {
                console::get().writef(" %u", (unsigned)entries[i].size);
            }

            console::get().write("\n");
        }
        else {
            console::get().writef("%s\n", entries[i].name);
        }
    }
}

/*
*   main
*
*   Parsing the command line input into two tokens (maximum),
*   the ls command and an optional -l flag.
*
*/
int main(const char *cmdline)
{
    // If no flag is provided, default to listing the root directory.
    if (!cmdline || memops::strlen(cmdline) == 0) {
        ls(0, "");
        return 0;
    }

    char arg1[128] = {0}; // Either '-l' or a directory path.
    char arg2[128] = {0}; // Directory path if '-l' is used.

    size_t len = memops::strlen(cmdline);
    size_t pos = 0;

    /*
    *   Exctracting characters from command line input until a space is reached,
    *   the end of the string is reached, or until the buffer is full.
    */
    size_t j = 0;
    while (pos < len && cmdline[pos] != ' ' && j < sizeof(arg1)-1) {
        arg1[j++] = cmdline[pos++];
    }

    // Skips whitespace before second argument (if present)
    while (pos < len && cmdline[pos] == ' ') pos++;

    /* Extract second input word, if '-l' was the first argument,
    *   This will be the directory path.
    */
    j = 0;
    while (pos < len && cmdline[pos] != ' ' && j < sizeof(arg2)-1) {
        arg2[j++] = cmdline[pos++];
    }

    /*
    * Only one argument present, ls -l or ls <directory>
    */
    if (arg2[0] == 0) {
        if (memops::strcmp(arg1, "-l") == 0) {
            ls(1, "");
        } else {
            ls(0, arg1);
        }
    }
    else {
        /*
        * Two arguments present, ls -l <directory>
        */
        if (memops::strcmp(arg1, "-l") == 0) {
            ls(1, arg2);
        } else {
            console::get().write("usage: ls [-l] <directory>\n");
            return 1;
        }
    }

    return 0;
}
