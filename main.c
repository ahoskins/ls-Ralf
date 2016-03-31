#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <tgmath.h>
#include <math.h>
#include "main.h"


int main(int argc, char** argv) {
	// set the printf buffer so it works
	setvbuf (stdout, NULL, _IONBF, 0);

	// start at the current working directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		displayDir(cwd, strlen(cwd));
	} else {
		perror("cwd error");
	}

	return 0;
}

/** @brief displays file info for a given filename with proper spacing
 *  @param filename the full path to the file
 *  @param pDirent the dirent structure for the file
 *  @param linkCountSize the number of characters wide to print link count
 *  @param byteSize the number of characters wide to print file size
 *  @return void
 * */
void displayFileInfo(char* filename, struct dirent* pDirent, int linkCountSize, int byteSize) {

	// get the stats
	struct stat buff;
	if (lstat(filename, &buff) < 0) {
		perror("error in stat");
		return;
	}

	// type
	if (pDirent->d_type == DT_DIR) {
		printf("d");
	} else if (pDirent->d_type == DT_LNK) {
		printf("l");
	} else {
		printf("-");
	}

	// permissions
	printf((buff.st_mode & S_IRUSR) ? "r" : "-");
	printf((buff.st_mode & S_IWUSR) ? "w" : "-");
	printf((buff.st_mode & S_IXUSR) ? "x" : "-");
	printf((buff.st_mode & S_IRGRP) ? "r" : "-");
	printf((buff.st_mode & S_IWGRP) ? "w" : "-");
	printf((buff.st_mode & S_IXGRP) ? "x" : "-");
	printf((buff.st_mode & S_IROTH) ? "r" : "-");
	printf((buff.st_mode & S_IWOTH) ? "w" : "-");
	printf((buff.st_mode & S_IXOTH) ? "x" : "-");
	printf(pDirent->d_type == DT_LNK ? " " : "+");
	printf(" ");

	// links
	printf("%*d", linkCountSize, (int) buff.st_nlink);
	printf(" ");

	// user
	struct passwd *pw;
	if ((pw = getpwuid(buff.st_uid)) == NULL) {
		perror("failed to get user");
	}
	printf("%s", pw->pw_name);
	printf(" ");

	// group
	struct group *gw;
	if ((gw = getgrgid(buff.st_gid)) == NULL) {
		perror("failed to get group");
	}
	printf("%s", gw->gr_name);
	printf(" ");

	// size
	printf("%*d", byteSize, (int) buff.st_size);
	printf(" ");

	// time last modified
	char time[20];
	strftime(time, 20, "%d %b %H:%M", localtime(&(buff.st_mtime)));
	printf("%s", time);
	printf(" ");

	// name (and maybe link)
	if (pDirent->d_type == DT_LNK) {
		char linkname[1024];
		readlink(filename, linkname, 1014);
		linkname[1023] = '\0';
		printf("%s -> %s\n", pDirent->d_name, linkname);
	} else if (pDirent->d_type == DT_DIR) {
		printf("%s/\n", pDirent->d_name);
	} else {
		printf("%s\n", pDirent->d_name);
	}

}

/**
 * @brief recursively walk directories calling displayFileInfo on all encountered files and adding extra formatting
 * @param dirname full directory path to start from
 * @return void
 * */
void displayDir(char* dirname, size_t endOfBaseDirIndex) {
	DIR* pDir;
	struct dirent* pDirent;

	pDir = opendir(dirname);
	if (pDir == NULL) {
		perror("open dir error");
	}

	// find overall stats for spacing purposes and find total block size
	int totalBlocks = 0;
	int largestLink = 0;
	int largestSize = 0;
	while ((pDirent = readdir(pDir)) != NULL) {
		struct stat buff;
		char nextName[sizeof(dirname) + sizeof(pDirent->d_name)] = "";
		strcat(nextName, dirname);
		strcat(nextName, "/");
		strcat(nextName, pDirent->d_name);

		if (lstat(nextName, &buff) < 0) {
			perror("error in stat");
			return;
		}

		totalBlocks += buff.st_blocks;
		if (buff.st_nlink > largestLink) {
			largestLink = buff.st_nlink;
		}
		if (buff.st_size > largestSize) {
			largestSize = (int) buff.st_size;
		}
	}
	printf("Total: %d\n", totalBlocks);
	rewinddir(pDir);

	// print file info for all in directory
	while ((pDirent = readdir(pDir)) != NULL) {
		char nextName[sizeof(dirname) + sizeof(pDirent->d_name)] = "";
		strcat(nextName, dirname);
		strcat(nextName, "/");
		strcat(nextName, pDirent->d_name);

		displayFileInfo(nextName, pDirent, (int) floor(log10(largestLink)) + 1, (int) floor(log10(largestSize)) + 1);
	}
	rewinddir(pDir);

	// if directory, recursively go into it
	while ((pDirent = readdir(pDir)) != NULL) {
		char nextName[sizeof(dirname) + sizeof(pDirent->d_name)] = "";
		strcat(nextName, dirname);
		strcat(nextName, "/");
		strcat(nextName, pDirent->d_name);

		if (pDirent->d_type == DT_DIR && (strcmp(pDirent->d_name, ".") != 0) && (strcmp(pDirent->d_name, "..") != 0)) {
			printf("\n.%s:\n", nextName + endOfBaseDirIndex);
			displayDir(nextName, endOfBaseDirIndex);
		}
	}

	closedir(pDir);
}