//
// Created by Andrew Hoskins on 2016-03-30.
//

#ifndef LS_RALF_MAIN_H
#define LS_RALF_MAIN_H

void displayFileInfo(char* filename, struct dirent* pDirent, int linkCountSize, int byteSize);
void displayDir(char* dirname, size_t endOfBaseDirIndex);

#endif //LS_RALF_MAIN_H
