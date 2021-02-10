#include "RetroEngine.hpp"

#define MEMFILES	1000

MemFILE memFiles[MEMFILES];

struct {
	char *name;
	void *data;
	int active;
}memFileData[MEMFILES];

static int memfile_inited = 0;

MemFILE *mem_fopen(const char *pathname, const char *mode)
{
	if (!memfile_inited) {
		bzero(memFiles, sizeof(MemFILE) * MEMFILES);
		bzero(memFileData, sizeof(memFileData));
		memfile_inited = 1;
	}
	
	//printf("Opening %s, mode = %s...\n", pathname, mode);
	
	if (strcasecmp(mode, "rb") != 0 &&
		strcasecmp(mode, "r") != 0)
		return NULL;
	
	char *buf = NULL;
	
	//printf("Searching for %s\n", pathname);
	
	for (int x=0; x<MEMFILES; x++) {		
		if (memFileData[x].active)
		{			
			if(strcasecmp(memFileData[x].name, pathname) == 0) {
				buf = (char*)memFileData[x].data;
				break;
			}
		}
	}
		
	FILE *fp = fopen(pathname, "rb");

	if (!fp)
		return NULL;
	
	int size;
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	if (!buf) {
		printLog("Loading %s in memory...\n", pathname);
		
		buf = (char*)malloc(size);	
	
		if (!buf) {
			fclose(fp);
			return NULL;
		}
		

		fread(buf, 1, size, fp);
		
		for (int x=0; x<MEMFILES; x++) {
			if (!memFileData[x].active) {
				memFileData[x].name = strdup(pathname);
				memFileData[x].data = buf;
				memFileData[x].active = 1;
				break;
			}
		}
	}
		
	fclose(fp);
	
	MemFILE *m = NULL;
	
	for (int x=0; x<MEMFILES; x++) {
		if (!memFiles[x].active) {
			m = &memFiles[x];
			break;
		}
	}

	m->name = strdup(pathname);
	m->pos = 0;
	m->data = buf;
	m->size = size;
	m->active = 1;
	
	Engine.ResetFrameCounter();
	
	return m;
}

int mem_fclose(MemFILE *stream)
{
	//free(stream->data);
	//free(stream);
	stream->active = 0;
	
	return 0;
}

int mem_fread(void *ptr, int size, int nmemb, MemFILE *stream)
{
	int left = stream->size - stream->pos;
	int sz = size * nmemb;
	
	if (sz > left)
		sz = left;
	
	memcpy(ptr, &stream->data[stream->pos], sz);
	
	//printf("sz = %d, left = %d, pos = %d\n", sz, left, stream->pos);

	stream->pos += sz;
	
	
	return sz;
}

int mem_fwrite(const void *ptr, int size, int nmemb, MemFILE *stream)
{
// TO DO	
	int left = stream->size - stream->pos;
	int sz = size * nmemb;
	
	if (sz > left)
		sz = left;
	
	memcpy(&stream->data[stream->pos], ptr, sz);
	
	stream->pos += sz;
	
	return sz;
}

long mem_ftell(MemFILE *stream)
{
	return stream->pos;
}

int mem_fseek(MemFILE *stream, long offset, int whence)
{
	//printf("offset = %d, whence = %d\n", offset, whence);
	
	if (whence == SEEK_SET)
		stream->pos = offset;
	else if(whence == SEEK_END)
		stream->pos = stream->size + offset;
	else if(whence == SEEK_CUR)
		stream->pos += whence;
		
	if (stream->pos < 0)
		stream->pos = 0;
	else if (stream->pos > stream->size)
		stream->pos = stream->size;

	return stream->pos;
}
