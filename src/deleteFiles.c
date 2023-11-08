/** @file deleteFiles.c 
 *  @brief deletes files or dies trying. 
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h> 
#include "../include/iocomp.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);
    if (rv)
        perror(fpath);
    return rv;
}
#pragma GCC diagnostic pop 

/**
 * @file deleteFiles.c
 * @brief deletes files or directories.
 * @param fileName name of file to delete
 * @return void
 */
void deleteFiles(char* fileName)
{
	int ierr; 
	ierr = nftw(fileName, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
	mpi_error_check(ierr); 
} 
