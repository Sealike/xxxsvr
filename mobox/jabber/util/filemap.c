#include "util.h"
#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

jfile_t j_fopen(const char *filename, const char *mode)
{
	jfile_t file     = NULL;
#ifdef _WIN32
	HANDLE hfilemap  = NULL;
	LPBYTE  fhead    = NULL;
	int64_t filesize = 0;
	HANDLE hfile  = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hfile) return NULL;

	if(NULL == (hfilemap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, 0, NULL))) return NULL;

	if(!GetFileSizeEx(hfile, &filesize)) return NULL;

	if(NULL == (fhead = (LPBYTE)MapViewOfFile(hfilemap, FILE_MAP_ALL_ACCESS, 0, 0, filesize))) return NULL;

	while((file = malloc(sizeof(struct _jfile_st))) == NULL) sleep(1);

	file->hfile    = hfile;
	file->hfilemap = hfilemap;
	file->filesize = filesize;
	file->fhead    = fhead;
#else 
	char   *fhead = NULL;
	int    hfile     = 0;
	struct stat status; 

	if((hfile = open(filename, O_RDWR, S_IRUSR|S_IWUSR)) == -1) return NULL;

	if (fstat(hfile, &status) == -1) return NULL;

	if(NULL == (fhead = mmap(0, status.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, hfile, 0))) return NULL;

	while((file = malloc(sizeof(struct _jfile_st))) == NULL) sleep(1);

	file->hfile    = hfile;
	file->filesize = status.st_size;
	file->fhead    = fhead;

#endif 
	return file;
}

size_t j_fread(void *buffer, size_t offset, size_t size, size_t count, jfile_t jfile)
{
	if((NULL == jfile) || (NULL == jfile->fhead)) return -1;

	memcpy(buffer, ((char *)jfile->fhead) + offset, size * count);

	return size * count;
}

size_t j_fwrite(void *buffer, size_t offset, size_t size, size_t count, jfile_t jfile)
{
	if((NULL == jfile) || (NULL == jfile->fhead)) return -1;

	memcpy(((char *)jfile->fhead) + offset, buffer, size * count);

	return size * count;
}

int  j_flush(jfile_t jfile)
{
	if(NULL == jfile) return -1;

	if(NULL != jfile->fhead) 
	{
#ifdef _WIN32
		if(!FlushViewOfFile(jfile->fhead, jfile->filesize)) return -1;
		if(!FlushFileBuffers(jfile->hfile)) return -1;
#else 
		if(msync(jfile->fhead, jfile->filesize, MS_ASYNC) == -1) return -1;
#endif 
	}
	return 0;
}

void  j_fclose(jfile_t jfile)
{
	if(NULL == jfile) return;
#ifdef _WIN32
	if(NULL != jfile->fhead) 
	{
		UnmapViewOfFile(jfile->fhead);
		jfile->fhead = NULL;
	}
	if(NULL != jfile->hfilemap) CloseHandle(jfile->hfilemap);
	if(NULL != jfile->hfile) CloseHandle(jfile->hfile);
#else
	if(NULL != jfile->fhead) 
	{
		munmap(jfile->fhead, jfile->filesize);
		jfile->fhead = NULL;
	}
	if(-1 != jfile->hfile) close(jfile->hfile);
#endif 
	free(jfile);
	return 0;
}

