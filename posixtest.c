#include <stdio.h>
#include <sys/vfs.h>
#include <string.h>
#include <unistd.h>
#include <linux/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/mman.h>
#include <sys/ioctl.h> 

#define WAIT_INPUT (1)

#define numberof(x)        (sizeof(x) / sizeof(x[0]))

typedef struct {
    const char    *name;
    int        (*func)(const char *);
} CMDINFO;

typedef struct {
	const char *name;
	int flg;
}FLAGS;

typedef struct {
	const char *name;
	int result;
}TESTRESULT;

struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[];
};

FLAGS openflgs[] = {
	{"O_RDONLY", O_RDONLY },
	{"O_WRONLY", O_WRONLY },
	{"O_RDWR", O_RDWR },
	{"O_CREAT", O_CREAT },
	{"O_EXCL", O_EXCL },
	{"O_APPEND", O_APPEND  },
	{"O_TRUNC", O_TRUNC },
	{"O_NOCTTY", O_NOCTTY },
	{"O_NONBLOCK", O_NONBLOCK },
	{"O_NDELAY", O_NDELAY },
	{"O_SYNC", O_SYNC  },
	{"O_NOFOLLOW", O_NOFOLLOW },
	{"O_FSYNC", O_FSYNC },
	{"O_DIRECTORY", O_DIRECTORY },
};

FLAGS protflgs[] = {
	{"PROT_EXEC", PROT_EXEC },
	{"PROT_READ", PROT_READ },
	{"PROT_WRITE", PROT_WRITE },
	{"PROT_NONE", PROT_NONE },
};

FLAGS mapflgs[] = {
	{"MAP_SHARED", MAP_SHARED },
	{"MAP_PRIVATE", MAP_PRIVATE },
	{"MAP_32BIT", MAP_32BIT },
	{"MAP_ANON", MAP_ANON },
	{"MAP_ANONYMOUS", MAP_ANONYMOUS },
	{"MAP_DENYWRITE", MAP_DENYWRITE },
	{"MAP_EXECUTABLE", MAP_EXECUTABLE },
	{"MAP_FILE", MAP_FILE },
	{"MAP_FIXED", MAP_FIXED },
	{"MAP_GROWSDOWN", MAP_GROWSDOWN },
	{"MAP_HUGETLB", MAP_HUGETLB },
	{"MAP_LOCKED", MAP_LOCKED },
	{"MAP_NONBLOCK", MAP_NONBLOCK },
	{"MAP_NORESERVE", MAP_NORESERVE },
	{"MAP_POPULATE", MAP_POPULATE },
	{"MAP_STACK", MAP_STACK },
};

FLAGS lockflgs[] = {
	{"F_RDLCK", F_RDLCK },
	{"F_WRLCK", F_WRLCK },
	{"F_UNLCK", F_UNLCK },
	{"F_LOCK", F_LOCK },
	{"F_TLOCK", F_TLOCK },
	{"F_ULOCK", F_ULOCK },
	{"F_TEST", F_TEST },
};
	
#define BUF_SIZE 1024
#define PATH_SIZE 1024

int getParam(const char *arg, char *param) {
	int index = 0;
	
	while (*arg != '\0' && *arg != ' ') {
		param[index] = *arg;
		index++;
		arg++;
	}
	
	return 0;
}

int getFlag(FLAGS *flags, int size, char *flag) {
	int i = 0;
	
	for (i = 0; i < size; i++) {
		if (strcasecmp(flags[i].name, flag) == 0) {
			return flags[i].flg;
		}
	}
	
	return 0;
}

int Statfs(
    const char *arg
    )
{
    const char *path = arg;
    int ret = 0;
    struct statfs stat = {};
    
    ret = statfs(path, &stat);
    if (ret != 0) {
        printf("statfs %s error:%d\n", path, ret);
        return -1;
    }
    
    printf("the result of statfs %s:\n", path);
    printf("f_type=%ld\n", stat.f_type);
    printf("f_bsize=%ld\n", stat.f_bsize);
    printf("f_blocks=%ld\n", stat.f_blocks);
    printf("f_bfree=%ld\n", stat.f_bfree);
    printf("f_bavail=%ld\n", stat.f_bavail);
    printf("f_files=%ld\n", stat.f_files);
    printf("f_ffree=%ld\n", stat.f_ffree);
    printf("f_fsid=%ld\n", stat.f_fsid);
    printf("f_namelen=%ld\n", stat.f_namelen);
	
	return 0;
}

int Statfs64(
    const char *arg
    )
{
    const char *path = arg;
    int ret = 0;
    struct statfs stat = {};
    
    ret = statfs64(path, &stat);
    if (ret != 0) {
        printf("statfs64 %s error:%d\n", path, ret);
        return -1;
    }
    
    printf("the result of statfs64 %s:\n", path);
    printf("f_type=%ld\n", stat.f_type);
    printf("f_bsize=%ld\n", stat.f_bsize);
    printf("f_blocks=%ld\n", stat.f_blocks);
    printf("f_bfree=%ld\n", stat.f_bfree);
    printf("f_bavail=%ld\n", stat.f_bavail);
    printf("f_files=%ld\n", stat.f_files);
    printf("f_ffree=%ld\n", stat.f_ffree);
    printf("f_fsid=%ld\n", stat.f_fsid);
    printf("f_namelen=%ld\n", stat.f_namelen);
	
	return 0;
}

int Getdents(
    const char *arg
    )
{
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;
    
    fd = open(arg, O_RDONLY | O_DIRECTORY);
    if (fd < 0) {
        printf("Open %s error:%d\n", arg, fd);
        return -1;
    }
    for ( ; ; ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1) {
            printf("SYS_getdents %s error:%d\n", arg, nread);
            break;
        }

        if (nread == 0)
            break;

        printf("--------------- nread=%d ---------------\n", nread);
        printf("inode#    file type  d_reclen  d_off   d_name\n");
        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            printf("%8ld  ", d->d_ino);
            d_type = *(buf + bpos + d->d_reclen - 1);
            printf("%-10s ", (d_type == DT_REG) ?  "regular" :
                            (d_type == DT_DIR) ?  "directory" :
                            (d_type == DT_FIFO) ? "FIFO" :
                            (d_type == DT_SOCK) ? "socket" :
                            (d_type == DT_LNK) ?  "symlink" :
                            (d_type == DT_BLK) ?  "block dev" :
                            (d_type == DT_CHR) ?  "char dev" : "???");
            printf("%4d %10lld  %s\n", d->d_reclen,
                   (long long) d->d_off, d->d_name);
            bpos += d->d_reclen;
        }
    }
    
    close(fd);
	
    return 0;
}

int Mkdir(
    const char *arg
    )
{
	int ret = -1;
	char path[PATH_SIZE] = {};
	int index = 0;
	int mode = 0;

	getParam(arg, path);
	
	arg += strlen(path) + 1;
	mode = 0;
	while (*arg >= '0' && *arg <= '7') {
		mode = mode * 8 + (*arg - '0');
		arg++;
	}
		
	
	ret = mkdir(path, mode);
	if (ret < 0) {
        printf("Mkdir %s error:%d\n", arg, ret);
		return -1;
    }
	printf("Make new directory %s success.\n", path);
	
	return 0;
}

int Access(
    const char *arg
    )
{
	int ret = -1;
	char path[PATH_SIZE] = {};
	char mode[10] = {};
	int index = 0;
	
	getParam(arg, path);
	getParam(arg+strlen(path)+1, mode);

    if (strncmp(mode, "R_OK", 4) == 0) {
		ret = access(path, R_OK);
		if (ret == 0) {
		    printf("%s can be read.\n", path);
		} else {
			printf("%s can not be read.\n", path);
		}
	} else if (strncmp(mode, "W_OK", 4) == 0) {
		ret = access(path, W_OK);
		if (ret == 0) {
		    printf("%s can be write.\n", path);
		} else {
			printf("%s can not be write.\n", path);
		}
	} else if (strncmp(mode, "X_OK", 4) == 0) {
		ret = access(path, X_OK);
		if (ret == 0) {
		    printf("%s can be execute.\n", path);
		} else {
			printf("%s can not be execute.\n", path);
		}
	} else if (strncmp(mode, "F_OK", 4) == 0) {
		ret = access(path, F_OK);
		if (ret == 0) {
		    printf("%s is exist.\n", path);
		} else {
			printf("%s is not exist.\n", path);
		}
	} else {
		printf("mode %s is not exist.\n", mode);
	}
	
	return 0;
}

int Stat(
    const char *arg
    )
{
	struct stat sts = {};
	int ret = stat(arg, &sts);
	if (ret != 0) {
		printf("stat %s fail:%d.\n", arg, ret);
		return -1;
	}
	
	printf("stat %s:\n", arg);
	printf("st_dev=%d\n", sts.st_dev);
	printf("st_ino=%d\n", sts.st_ino);
	printf("st_mode=%d\n", sts.st_mode);
	printf("st_nlink=%d\n", sts.st_nlink);
	printf("st_uid=%d\n", sts.st_uid);
	printf("st_gid=%d\n", sts.st_gid);
	printf("st_rdev=%d\n", sts.st_rdev);
	printf("st_size=%d\n", sts.st_size);
	printf("st_blksize=%d\n", sts.st_blksize);
	printf("st_blocks=%d\n", sts.st_blocks);
	printf("st_atime=%d\n", sts.st_atime);
	printf("st_mtime=%d\n", sts.st_mtime);
	printf("st_ctime=%d\n", sts.st_ctime);

	return 0;
}

int Fstat(
    const char *arg
    )
{
	struct stat sts = {};
	int fd = open(arg, O_RDONLY);
	if (fd < 0) {
		printf("open %s fail:%d.\n", arg, fd);
		return -1;
	}
	int ret = fstat(fd, &sts);
	if (ret != 0) {
		printf("fstat %s fail:%d.\n", arg, ret);
		close(fd);
		return -1;
	}
	
	printf("stat %s:\n", arg);
	printf("st_dev=%d\n", sts.st_dev);
	printf("st_ino=%d\n", sts.st_ino);
	printf("st_mode=%d\n", sts.st_mode);
	printf("st_nlink=%d\n", sts.st_nlink);
	printf("st_uid=%d\n", sts.st_uid);
	printf("st_gid=%d\n", sts.st_gid);
	printf("st_rdev=%d\n", sts.st_rdev);
	printf("st_size=%d\n", sts.st_size);
	printf("st_blksize=%d\n", sts.st_blksize);
	printf("st_blocks=%d\n", sts.st_blocks);
	printf("st_atime=%d\n", sts.st_atime);
	printf("st_mtime=%d\n", sts.st_mtime);
	printf("st_ctime=%d\n", sts.st_ctime);

	close(fd);
	return 0;
}

int Rename(
    const char *arg
    )
{
	int ret = -1;
	char path[PATH_SIZE] = {};
	char newpath[PATH_SIZE] = {};
	int index = 0;
	
	getParam(arg, path);
	getParam(arg+strlen(path)+1, newpath);
	
	ret = rename(path, newpath);
	if (ret != 0) {
		printf("rename %s to %s fail.\n", path, newpath);
		return -1;
	}
	
	printf("rename %s to %s success.\n", path, newpath);
	return 0;
}

int Unlink(
    const char *arg
    )
{
	int ret = unlink(arg);
	if (ret == 0) {
		printf("unlink %s success.\n", arg);
		return 0;
	}
	
	printf("unlink %s fail:%d.\n", arg, ret);
	return -1;
}

int Open(
    const char *arg
    )
{
	char path[PATH_SIZE] = {};
	char flag[20] = {};
	int index = 0;
	int flags = 0;
	
	getParam(arg, path);
	
	arg += strlen(path)+1;
	while (*arg != '\0' && *arg != ' ') {
		if (*arg == '|') {
			flags |= getFlag(openflgs, (int)numberof(openflgs), flag);
			arg++;
			index = 0;
			memset(flag, 0, sizeof(flag));
			continue;
		}
		flag[index] = *arg;
		index++;
		arg++;
	}
	
	flags |= getFlag(openflgs, (int)numberof(openflgs), flag);
	
	int fd = open(path, flags);
	if (fd < 0) {
		printf("open %s fail:%d\n", path, fd);
		return -1;
	}
	
	printf("open %s with %d success.\n", path, flags);
	
	int ret = close(fd);
	if (ret < 0) {
		printf("close %s fail:%d\n", path, ret);
		return -1;
	}
	
	printf("close %s success.\n", path);
	return 0;
}

int OpenMode(
    const char *arg
    )
{
	char path[PATH_SIZE] = {};
	char flag[20] = {};
	int index = 0;
	int flags = 0;
	
	getParam(arg, path);
	
	arg += strlen(path)+1;
	while (*arg != '\0' && *arg != ' ') {
		if (*arg == '|') {
			flags |= getFlag(openflgs, (int)numberof(openflgs), flag);
			arg++;
			index = 0;
			memset(flag, 0, sizeof(flag));
			continue;
		}
		flag[index] = *arg;
		index++;
		arg++;
	}
	
	flags |= getFlag(openflgs, (int)numberof(openflgs), flag);
	
    arg++;
	int mode = 0;
	while (*arg >= '0' && *arg <= '7') {
		mode = mode * 8 + (*arg - '0');
		arg++;
	}
	
	int fd = open(path, flags, mode);
	if (fd < 0) {
		printf("open %s fail:%d\n", path, fd);
		return -1;
	}
	
	printf("open %s with %d %d success.\n", path, flags, mode);
	
	int ret = close(fd);
	if (ret < 0) {
		printf("close %s fail:%d\n", path, ret);
		return -1;
	}
	
	printf("close %s success.\n", path);
	return 0;
}

int OpenAt(
    const char *arg
    )
{
	DIR *dirptr;
	int pathfd;
	char dir[PATH_SIZE] = {};
	char file[PATH_SIZE] = {};
	char flag[20] = {};
	int index = 0;
	int flags = 0;
	
	getParam(arg, dir);
	
	arg += strlen(dir) + 1;
	getParam(arg, file);

	arg += strlen(file) + 1;
	while (*arg != '\0' && *arg != ' ') {
		if (*arg == '|') {
			flags |= getFlag(openflgs, (int)numberof(openflgs), flag);
			arg++;
			index = 0;
			memset(flag, 0, sizeof(flag));
			continue;
		}
		flag[index] = *arg;
		index++;
		arg++;
	}
	
	flags |= getFlag(openflgs, (int)numberof(openflgs), flag);

	dirptr = opendir(dir);
	if(NULL == dirptr)
	{
		printf("open directory %s error.\n", dir);
		return -1;
	}
	pathfd = dirfd(dirptr);
	if(-1 == pathfd)
	{
		printf("dirfd %s error.\n", dir);
		return -1;
	}
	
	int fd = openat(pathfd, file, flags);
	if (fd < 0) {
		printf("open %s fail:%d\n", file, fd);
		return -1;
	}
	
	printf("open %s at %s with %d success.\n", file, dir, flags);
	
	int ret = close(fd);
	if (ret < 0) {
		printf("close file %s fail:%d\n", file, ret);
		return -1;
	}
	
	ret = closedir(dirptr);
	if (ret < 0) {
		printf("close directory %s fail:%d\n", dir, ret);
		return -1;
	}
	
	return 0;
}

int Mmap(
    const char *arg
    )
{
	char file[PATH_SIZE] = {};
	char buf[100] = {};
	int index = 0;
	
	getParam(arg, file);
	
	arg += strlen(file) + 1;
	int prot = 0;
	index = 0;
	while (*arg != '\0' && *arg != ' ') {
		if (*arg == '|') {
			prot |= getFlag(protflgs, (int)numberof(protflgs), buf);
			arg++;
			index = 0;
			memset(buf, 0, sizeof(buf));
			continue;
		}
		buf[index] = *arg;
		index++;
		arg++;
	}
	prot |= getFlag(protflgs, (int)numberof(protflgs), buf);
	
	arg++;
	int flags = 0;
	index = 0;
	memset(buf, 0, sizeof(buf));
	while (*arg != '\0' && *arg != ' ') {
		if (*arg == '|') {
			flags |= getFlag(mapflgs, (int)numberof(mapflgs), buf);
			arg++;
			index = 0;
			memset(buf, 0, sizeof(buf));
			continue;
		}
		buf[index] = *arg;
		index++;
		arg++;
	}
	flags |= getFlag(mapflgs, (int)numberof(mapflgs), buf);
	
	int fd = open(file, O_RDWR);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}
	
	struct stat sts = {};
	fstat(fd, &sts);
	void *start = mmap(NULL, sts.st_size, prot, flags, fd, 0);
	if(start == MAP_FAILED) {
		printf("mmap %s fail.\n", file);
		close(fd);
		return -1;
	}
	printf("mmap %s success.\n", file);
	printf("============show the memory=============\n");
	printf("%s", start);
	printf("============end show=============\n");
	
	int ret = munmap(start, sts.st_size);
	if (ret != 0) {
		printf("munmap %s fail.\n", file);
	}
	
	close(fd);
	return 0;
}

int Read(
    const char *arg
    )
{
	int fd = open(arg, O_RDONLY);
	if (fd < 0) {
		printf("Open %s fail.\n", arg);
		return -1;
	}
	
	int cur = lseek(fd, 0, SEEK_CUR);
	if (cur == -1) {
		printf("lseek get current position fail.\n");
		close(fd);
		return -1;
	} else {
		printf("The current position of file point is %d\n", cur);
	}
	
	int setpos = lseek(fd, 0, SEEK_SET);
	if (setpos == -1) {
		printf("lseek set position fail.\n");
		close(fd);
		return -1;
	} else {
		printf("Set position of file point success at %d\n", setpos);
	}
	
	char buf[BUF_SIZE] = {};
	ssize_t readlen = 0;
	printf("============read start==================\n");
	while ((readlen = read(fd, buf, BUF_SIZE)) > 0) {
		printf("%s", buf);
		memset(buf, 0, BUF_SIZE);
	}
	printf("============read end==================\n");
	
	close(fd);
	printf("Read %s success.\n", arg);
	return 0;
}

int Pread(
    const char *arg
    )
{
	int fd = open(arg, O_RDONLY);
	if (fd < 0) {
		printf("Open %s fail.\n", arg);
		return -1;
	}
	
	char buf[BUF_SIZE] = {};
	ssize_t readlen = 0;
	off_t offset = 0;
	printf("============pread start==================\n");
	while ((readlen = pread(fd, buf, BUF_SIZE, offset)) > 0) {
		printf("%s", buf);
		offset += readlen;
		memset(buf, 0, BUF_SIZE);
	}
	printf("============pread end==================\n");
	
	close(fd);
	printf("Pread %s success.\n", arg);
	return 0;
}

int Write(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char *context = NULL;
	
	getParam(arg, file);
	
	arg += strlen(file) + 1;
	context = (char *)arg;
	
	int fd = open(file, O_RDWR|O_CREAT);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}

	int cur = lseek(fd, 0, SEEK_CUR);
	if (cur == -1) {
		printf("lseek get current position fail.\n");
		close(fd);
		return -1;
	} else {
		printf("The current position of file point is %d\n", cur);
	}
	
	int setpos = lseek(fd, 0, SEEK_END);
	if (setpos == -1) {
		printf("lseek set position fail.\n");
		close(fd);
		return -1;
	} else {
		printf("Set position of file point success at %d\n", setpos);
	}
	
	ssize_t wlen = write(fd, context, strlen(context));
	printf("Then length of writing context is %d\n", wlen);
	
	int ret = fdatasync(fd);
	if (ret != 0) {
		printf("fsync %s fail.\n", file);
	} else {
		printf("fsync %s success.\n", file);
	}
	
	close(fd);
	return 0;
}

int Pwrite(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char *context = NULL;
	
	getParam(arg, file);
	
	arg += strlen(file) + 1;
	context = (char *)arg;
	
	int fd = open(file, O_RDWR|O_CREAT);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}

	int cur = lseek(fd, 0, SEEK_CUR);
	if (cur == -1) {
		printf("lseek get current position fail.\n");
		close(fd);
		return -1;
	} else {
		printf("The current position of file point is %d\n", cur);
	}
	
	int setpos = lseek(fd, 0, SEEK_END);
	if (setpos == -1) {
		printf("lseek set position fail.\n");
		close(fd);
		return -1;
	} else {
		printf("Set position of file point success at %d\n", setpos);
	}
	
	ssize_t wlen = pwrite(fd, context, strlen(context), setpos);
	printf("Then length of writing context is %d\n", wlen);
	
	int ret = fsync(fd);
	if (ret != 0) {
		printf("fsync %s fail.\n", file);
	} else {
		printf("fsync %s success.\n", file);
	}
	
	close(fd);
	return 0;
}

int Fsetxattr(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char name[100] = {};
	char value[100] = {};
	
	getParam(arg, file);
	
	arg += strlen(file) + 1;
	getParam(arg, name);
	
	arg += strlen(name) + 1;
	getParam(arg, value);

	int fd = open(file, O_RDONLY);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}
	
	int ret = fsetxattr(fd, name, value, strlen(value), 0);
	if (ret < 0) {
		printf("fsetxattr %s fail.\n", file);
		close(fd);
		return -1;
	}

	printf("fsetxattr %s success.\n", file);
	close(fd);
	return 0;
}

int Fgetxattr(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char name[100] = {};
	char value[100] = {};
	
	getParam(arg, file);
	arg += strlen(file) + 1;
	getParam(arg, name);
	
	int fd = open(file, O_RDONLY);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}
	
	int ret = fgetxattr(fd, name, value, sizeof(value));
	if (ret < 0) {
		printf("fgetxattr %s fail.\n", file);
		close(fd);
		return -1;
	}

	printf("fgetxattr %s of %s success.The value is %s\n", name, file, value);
	close(fd);
	return 0;
}

int Fcntl(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char cmd[100] = {};
	
	getParam(arg, file);
	arg += strlen(file) + 1;
	getParam(arg, cmd);
	
	int fd = open(file, O_RDONLY);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}
	
	int val = fcntl(fd, F_GETFL, 0);
    if (val == -1) {
		printf("fcntl F_GETFL fail.\n");
		return -1;
	}
	
	printf("fcntl F_GETFL success.The value is %d\n", val);
	
	val |= getFlag(openflgs, (int)numberof(openflgs), cmd);
	if (fcntl(fd, F_SETFL, val) < 0) {
		printf("fcntl F_SETFL fail.\n");
		return -1;
	}
	printf("fcntl F_SETFL success.The value is %d\n", val);
	
	val = fcntl(fd, F_GETFL, 0);
    if (val == -1) {
		printf("fcntl F_GETFL fail.\n");
		return -1;
	}
	
	printf("fcntl F_GETFL success.The value is %d\n", val);
	
	close(fd);
	return 0;
}

int FcntlLock(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char type[100] = {};
	
	getParam(arg, file);
	arg += strlen(file) + 1;
	getParam(arg, type);
	
	int fd = open(file, O_RDWR);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}
	
	struct flock lock = {};
	lock.l_start=0;
	lock.l_whence=SEEK_SET;
	lock.l_len=0;
	int ret = fcntl(fd, F_GETLK, &lock);
    if (ret < 0) {
		printf("fcntl F_GETLK fail.\n");
		return -1;
	}

	if(lock.l_type == F_WRLCK) {
		printf("%s is already locked.\n", file);
		return 0;
	}
	
	memset(&lock, 0, sizeof(lock));
	lock.l_type=getFlag(lockflgs, (int)numberof(lockflgs), type);
	lock.l_start=0;
	lock.l_whence=SEEK_SET;
	lock.l_len=0;
    ret = fcntl(fd, F_SETLK, &lock);
	if (ret < 0) {
		printf("fcntl F_SETLK fail.\n");
		return -1;
	}
	
	printf("fcntl F_SETLK success.\n");
#if WAIT_INPUT
	printf("press any key to unlock\n");
	getchar();
#endif
	lock.l_type = F_UNLCK;
	if(fcntl(fd, F_SETLK, &lock) == 0) {
		printf("F_UNLCK success.\n");
	} else {
		printf("F_UNLCK fail.");
	}
	
	close(fd);
	return 0;
}

int Lockf(
    const char    *arg
    )
{
	char file[PATH_SIZE] = {};
	char cmd[100] = {};
	char length[100] = {};
	
	getParam(arg, file);
	arg += strlen(file) + 1;
	getParam(arg, cmd);
	arg += strlen(cmd) + 1;
	getParam(arg, length);
	
	int fd = open(file, O_RDWR);
	if (fd < 0) {
		printf("Open %s fail.\n", file);
		return -1;
	}
	
	int type = getFlag(lockflgs, (int)numberof(lockflgs), cmd);
	int ret = lockf(fd, type, atol(length));

	switch(type) {
	case F_LOCK:
	    if (ret == -1) {
			printf("lockf %s with F_LOCK fail.\n", file);
			close(fd);
			return -1;
		}
		printf("lockf %s with F_LOCK success.\n", file);
#if WAIT_INPUT
		printf("press any key to unlock\n");
		getchar();
#endif
		ret = lockf(fd, F_ULOCK, atol(length));
		if (ret == -1) {
			printf("lockf %s with F_ULOCK fail.\n", file);
			close(fd);
			return -1;
		}
		printf("lockf %s with F_ULOCK success.\n", file);
		break;
	case F_TLOCK:
	    if (ret == -1) {
			printf("lockf %s with F_TLOCK fail.\n", file);
			return -1;
		}
		printf("lockf %s with F_TLOCK success.\n", file);
#if WAIT_INPUT
		printf("press any key to unlock\n");
		getchar();
#endif
		ret = lockf(fd, F_ULOCK, atol(length));
		if (ret == -1) {
			printf("lockf %s with F_ULOCK fail.\n", file);
			close(fd);
			return -1;
		}
		printf("lockf %s with F_ULOCK success.\n", file);
		break;
	case F_ULOCK:
		printf("lockf %s with F_ULOCK success.\n", file);
		break;
	case F_TEST:
	    if (ret == -1) {
			printf("%s is locked.\n", file);
		} else {
			printf("%s is not locked.\n", file);
		}
		break;
	default:
	    printf("Command error.");
	    break;
	}
	
	return 0;
}

int TestAll(
    const char    *arg
    )
{
	char dir[PATH_SIZE] = {};
	char newarg[BUF_SIZE] = {};
	char *file = "test.txt";
	TESTRESULT testresult[100] = {};
	int idx = 0;
	getParam(arg, dir);
	
	sprintf(newarg, "%s %s", dir, "700");
	testresult[idx].name = "mkdir";
	testresult[idx].result = Mkdir((const char *)newarg);
	
	idx++;
	testresult[idx].name = "statfs";
	testresult[idx].result = Statfs(arg);
	
	idx++;
	testresult[idx].name = "statfs64";
	testresult[idx].result = Statfs64(arg);
	
	idx++;
	testresult[idx].name = "getdents";
	testresult[idx].result = Getdents(arg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s %s", dir, "W_OK");
	testresult[idx].name = "Access";
	testresult[idx].result = Access((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s", dir, "a.txt", "O_RDWR|O_CREAT");
	testresult[idx].name = "open";
	testresult[idx].result = Open((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s/%s", dir, "a.txt", dir, file);
	testresult[idx].name = "Rename";
	testresult[idx].result = Rename((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s", dir, file);
	testresult[idx].name = "stat";
	testresult[idx].result = Stat((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s", dir, file);
	testresult[idx].name = "fstat";
	testresult[idx].result = Fstat((const char *)newarg);

	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s %s", dir, file, "O_RDWR", "600");
	testresult[idx].name = "openmode";
	testresult[idx].result = OpenMode((const char *)newarg);	
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s %s %s", dir, file, "O_RDWR");
	testresult[idx].name = "openat";
	testresult[idx].result = OpenAt((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s", dir, file, "context from the function of write.\n");
	testresult[idx].name = "write";
	testresult[idx].result = Write((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s", dir, file, "context from the function of pwrite.\n");
	testresult[idx].name = "pwrite";
	testresult[idx].result = Pwrite((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s", dir, file);
	testresult[idx].name = "read";
	testresult[idx].result = Read((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s", dir, file);
	testresult[idx].name = "pread";
	testresult[idx].result = Pread((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s %s", dir, file, "PROT_READ|PROT_WRITE", "MAP_SHARED");
	testresult[idx].name = "mmap";
	testresult[idx].result = Mmap((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s %s", dir, file, "user.test", "testabc");
	testresult[idx].name = "fsetxattr";
	testresult[idx].result = Fsetxattr((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s", dir, file, "user.test");
	testresult[idx].name = "fgetxattr";
	testresult[idx].result = Fgetxattr((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s", dir, file, "O_NONBLOCK");
	testresult[idx].name = "fcntl";
	testresult[idx].result = Fcntl((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s", dir, file, "F_WRLCK");
	testresult[idx].name = "fcntllock";
	testresult[idx].result = FcntlLock((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s %s %s", dir, file, "F_LOCK", "0");
	testresult[idx].name = "lockf";
	testresult[idx].result = Lockf((const char *)newarg);
	
	idx++;
	memset(newarg, 0, sizeof(newarg));
	sprintf(newarg, "%s/%s", dir, file);
	testresult[idx].name = "unlink";
	testresult[idx].result = Unlink((const char *)newarg);
	
	printf("\n=========================\n");
	int i = 0;
	for (; i <= idx; i++) {
		printf("%s    =>  %s\n", testresult[i].name, testresult[i].result==0?"success":"fail");
	}
	printf("\n=========================\n");
	
	return 0;
}

int Help(
    const char    *arg
    )
{
    printf(
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
        "help                              - Show This Help\n"
        );
	return 0;
}

CMDINFO            CmdTable[] = {
    { "statfs",       Statfs              },
    { "statfs64",     Statfs64            },
    { "getdents",     Getdents            },
	{ "mkdir",        Mkdir               },
	{ "access",       Access              },
	{ "stat",         Stat                },
	{ "fstat",        Fstat               },
	{ "rename",       Rename              },
	{ "unlink",       Unlink              },
	{ "open",         Open                },
	{ "openmode",     OpenMode            },
	{ "openat",       OpenAt              },
	{ "mmap",         Mmap                },
	{ "read",         Read                },
	{ "pread",        Pread               },
	{ "write",        Write               },
	{ "pwrite",       Pwrite              },
	{ "fsetxattr",    Fsetxattr           },
	{ "fgetxattr",    Fgetxattr           },
	{ "fcntl",        Fcntl               },
	{ "fcntllock",    FcntlLock           },
	{ "lockf",        Lockf               },
	{ "testall",      TestAll             },
    { "help",         Help                },
    { "?",            Help                },
};

int main(
    int            argc,
    char        *argv[]
    )
{
    int            i;
    char        *cmd;
    char        *arg;
    char        buffer[256];

    for (;;) {
        printf("%% ");
        fflush(stdout);
        if (fgets(buffer, numberof(buffer), stdin) == NULL)
            break;
        arg = &buffer[strlen(buffer)];
        while (arg > &buffer[0]) {
            arg--;
            if (*arg > ' ')
                break;
            *arg = '\0';
        }
        cmd = &buffer[0];
        while (*cmd != '\0') {
            if (*cmd > ' ')
                break;
            cmd++;
        }
        if (*cmd == '\0')
            continue;
        arg = cmd;
        while (*arg > ' ')
            arg++;
        if (*arg != '\0') {
            *arg++ = '\0';
            while (*arg != '\0') {
                if (*arg > ' ')
                    break;
                arg++;
            }
        }
        if (strcasecmp(cmd, "exit") == 0 ||
            strcasecmp(cmd, "quit") == 0)
            break;
        for (i = 0; i < (int) numberof(CmdTable); i++) {
            if (strcasecmp(CmdTable[i].name, cmd) == 0) {
                CmdTable[i].func(arg);
                break;
            }
        }
        if (i == numberof(CmdTable))
            printf("Unknown command: %s\n", cmd);
    }

    return 0;
}