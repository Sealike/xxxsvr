#include "instapp.h"

instapp_t instapp_new()
{
	instapp_t rtn  = NULL;
	while((rtn = (calloc(1,sizeof(instapp_st)))) == NULL) 
		Sleep(1);
	return rtn;
}

void instapp_free(instapp_t instapp)
{
	if(instapp == NULL) return;

	if (instapp->log)
	log_write(instapp->log, LOG_NOTICE, "server shutting down.");

	if(instapp->bdjob_ip)        
		free(instapp->bdjob_ip);

	if(instapp->frontsvr_ip)    	
		free(instapp->frontsvr_ip);

	if(instapp->https_ip)  	
		free(instapp->https_ip);

	/*if (instapp->httpc) 
		httpc_free(instapp->httpc);*/

	if(instapp->p2p_frontsvr)    	
		free(instapp->p2p_frontsvr);

	if(instapp->log)          	
		log_free(instapp->log);

	if(instapp->config)       	
		config_free(instapp->config);

	if(instapp->mio)          	
		mio_free(instapp->mio);

	free(instapp);
}