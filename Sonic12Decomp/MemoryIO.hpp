#ifndef _MEMORYIO_H
#define _MEMORYIO_H

typedef struct
{
	char *name;
	int size;
	int pos;
	char *data;
	int active;
}MemFILE;

MemFILE *mem_fopen(const char *pathname, const char *mode);
int mem_fclose(MemFILE *stream);
int mem_fread(void *ptr, int size, int nmemb, MemFILE *stream);
int mem_fwrite(const void *ptr, int size, int nmemb, MemFILE *stream);
long mem_ftell(MemFILE *stream);
int mem_fseek(MemFILE *stream, long offset, int whence);

#endif
