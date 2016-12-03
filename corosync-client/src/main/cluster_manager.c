#include "cluster_manager.h"

int get_local_nodeid(unsigned int *local_nodeid)
{
	int err;
	corosync_cfg_handle_t cfg_h;
	
	//initialize cfg handle
	err = corosync_cfg_initialize(&cfg_h, NULL);
	if(err != CS_OK){
		return err;
	}
	//get local id
	err = corosync_cfg_local_get(cfg_h, local_nodeid);
	if(err != CS_OK){
		corosync_cfg_finalize(cfg_h);
		return err;
	}
	//finalize handle
	corosync_cfg_finalize(cfg_h);
	//success!
	return CS_OK;
}