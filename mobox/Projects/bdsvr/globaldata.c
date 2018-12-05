//#include "globaldata.h"
#include "util/util.h"

typedef struct  _epro_st{
	char fromserver[64];
	char toserver[64];
}epro_st, *epro_t;

epro_t epro_new()
{
	epro_t rtn = NULL;
	while((rtn = (epro_t)calloc(1,sizeof(epro_st))) == NULL) Sleep(10);
	return rtn;
}

void epro_free(epro_t data)
{
	free(data);
}

