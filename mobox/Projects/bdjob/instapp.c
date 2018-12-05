#include "instapp.h"

instapp_t instapp_new()
{
	instapp_t rtn  = NULL;
	while((rtn = (calloc(1,sizeof(instapp_st)))) == NULL) Sleep(1);
	return rtn;
}

void instapp_free(instapp_t instapp)
{
	if(instapp == NULL) return;

	if (instapp->log)
	log_write(instapp->log, LOG_NOTICE, "server shutting down.");

	if (instapp->bdsvr_ip)   free(instapp->bdsvr_ip);
	if (instapp->p2pbdsvr)    free(instapp->p2pbdsvr);
	if(instapp->log)          log_free(instapp->log);

	if(instapp->config)       config_free(instapp->config);

	if(instapp->mio)          mio_free(instapp->mio);

	free(instapp);
}