# posixtest
Test the posix interface.

"statfs file                       - Show the status of file system\n"
"statfs64 file                     - Show the status of file system\n"
"getdents directory                - Show the files of directory\n"
"mkdir directory  mode             - Make new directory(mode is octonary)\n"
"access directory  mode            - Check the access mode(R_OK,W_OK,X_OK or F_OK) of directory\n"
"stat file                         - Show the status of file\n"
"fstat file                        - Show the status of file\n"
"rename file file                  - Rename the path/file\n"
"unlink file                       - Delete a file\n"
"open file flags                   - Open a file with flags\n"
"                                    flags:O_RDONLY,O_WRONLY,O_RDWR,O_CLOEXEC,O_CREAT,\n"
"                                          O_DIRECTORY,O_EXCL,O_NOCTTY,O_NOFOLLOW,\n"
"                                          O_TMPFILE,and O_TRUNC\n"
"openmode file flags mode          - Open a file with flags and mode\n"
"                                    flags:according to open\n"
"openat directory file flags       - Open a file at a directory with flags\n"
"                                    flags:according to open\n"
"mmap file prot flags              - Create a new mapping in the virtual address space\n"
"                                    prot:PROT_EXEC,PROT_READ,PROT_WRITE and PROT_NONE\n"
"                                    flags:MAP_SHARED,MAP_PRIVATE,MAP_32BIT,MAP_ANON,\n"
"                                          MAP_FIXED,MAP_ANONYMOUS,MAP_LOCKED...\n"
"read file                         - Read a file\n"
"pread file                        - Read a file\n"
"write file context                - Write context to a file\n"
"pwrite file context               - Write context to a file\n"
"fsetxattr file name value         - Set extended attributes of a file with name:value\n"
"fgetxattr file name               - Get extended attributes of a file\n"
"fcntl file cmd                    - Perform the operations described of a file\n"
"                                    cmd: O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME and O_NONBLOCK\n"
"fcntllock file type               - Lock a file.Type can be F_RDLCK, F_WRLCK, and F_UNLCK\n"
"lockf file cmd len                - Lock a file.Cmd can be F_LOCK, F_TLOCK, F_ULOCK, and F_TEST\n"
"testall directory                 - Test all functions.Directory must be new.\n"
