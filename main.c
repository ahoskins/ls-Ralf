#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dirent.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "main.h"

char recursiveDirName[1024] = ".";

int main(int argc, char** argv) {
	// set the printf buffer so it works
	setvbuf (stdout, NULL, _IONBF, 0);

//	displayDir(argv[1]);

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		displayDir(cwd);
	} else {
		perror("cwd error");
	}

	return 0;
}


// Useful system calls:
// stat, lstat, getpwuid, getgrgid, readlink, strftime,
// fprintf and perror.
void displayFileInfo(char* filename, struct dirent* pDirent) {
	// readlink is for getting the symbolic link
	struct stat buff;
	if (lstat(filename, &buff) < 0) {
		perror("error in stat");
		return;
	}
	mode_t mode_t1 = buff.st_mode; // permission?
	nlink_t nlink_t1 = buff.st_nlink; // number links
	uid_t uid_t1 = buff.st_uid; // user
	gid_t gid_t1 = buff.st_gid; // group
	off_t off_t1 = buff.st_size; // bytes size
	time_t time_t1 = buff.st_mtime; // time last modified

	// type
	if (pDirent->d_type == DT_DIR) {
		printf("d");
	} else if (pDirent->d_type == DT_LNK) {
		printf("l");
	} else {
		printf("-");
	}

	// permissions
	printf((S_ISDIR(buff.st_mode)) ? "d" : "-");
	printf((buff.st_mode & S_IRUSR) ? "r" : "-");
	printf((buff.st_mode & S_IWUSR) ? "w" : "-");
	printf((buff.st_mode & S_IXUSR) ? "x" : "-");
	printf((buff.st_mode & S_IRGRP) ? "r" : "-");
	printf((buff.st_mode & S_IWGRP) ? "w" : "-");
	printf((buff.st_mode & S_IXGRP) ? "x" : "-");
	printf((buff.st_mode & S_IROTH) ? "r" : "-");
	printf((buff.st_mode & S_IWOTH) ? "w" : "-");
	printf((buff.st_mode & S_IXOTH) ? "x" : "-");

	// spacing
	printf(" ");

	// links
	printf("%d", (int) nlink_t1);

	// spacing
	printf(" ");

	// user
	struct passwd *pw;
	if ((pw = getpwuid(uid_t1)) == NULL) {
		perror("failed to get user");
	}
	printf("%s", pw->pw_name);

	// spacing
	printf(" ");

	// group
	struct group *gw;
	if ((gw = getgrgid(uid_t1)) == NULL) {
		perror("failed to get group");
	}
	printf("%s", gw->gr_name);

	// spacing
	printf(" ");

	// size
	printf("%d", (int) off_t1);

	// spacing
	printf(" ");

	// time last modified
	char time[20];
	strftime(time, 20, "%d %b %H:%M", localtime(&time_t1));
	printf("%s", time);

	// spacing
	printf(" ");

	// name
	printf("%s\n", pDirent->d_name);

}

// Useful system calls:
// chdir, opendir, closedir, readdir, rewinddir, and those from displayFileInfo.
void displayDir(char* dirname) {
	DIR* pDir;
	struct dirent* pDirent;

	pDir = opendir(dirname);
	if (pDir == NULL) {
		perror("open dir error");
	}

	while ((pDirent = readdir(pDir)) != NULL) {
		char nextName[sizeof(dirname) + sizeof(pDirent->d_name)] = "";
		strcat(nextName, dirname);
		strcat(nextName, "/");
		strcat(nextName, pDirent->d_name);

		displayFileInfo(nextName, pDirent);

		if (pDirent->d_type == DT_DIR && (strcmp(pDirent->d_name, ".") != 0) && (strcmp(pDirent->d_name, "..") != 0)) {
			strcat(recursiveDirName, "/");
			strcat(recursiveDirName, pDirent->d_name);
			printf("\n%s:\n", recursiveDirName);
			displayDir(nextName);
		}
	}

	closedir(pDir);
}