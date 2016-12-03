#include "quorum_manager.h"

int get_quorum_info(Quorum_Details *qd)
{
	int err;
	unsigned int id;
	votequorum_handle_t vq_h;
	quorum_handle_t q_h;
	struct votequorum_info info;
	
	//initialize handle
	err = votequorum_initialize(&vq_h, NULL);
	if(err != CS_OK){
		return err;
	}
	//attempt to retrieve info
	err = get_local_nodeid(&id);
	err = votequorum_getinfo(vq_h, 1, &info);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//copy values to qd struct
	qd->highest_expected = info.highest_expected;
	qd->total_votes = info.total_votes;
	qd->quorum = info.quorum;
	qd->quorate = info.flags & VOTEQUORUM_INFO_QUORATE;
	//finalize handle
	votequorum_finalize(vq_h);
	return CS_OK;
}

int get_expected_votes(unsigned int *expected_votes)
{
	int err;
	unsigned int local_id;
	uint32_t nodeid;
	votequorum_handle_t vq_h;
	quorum_handle_t q_h;
	struct votequorum_info info;
	
	err = get_local_nodeid(&local_id);
	if(err != CS_OK){
		return err;
	}
	nodeid = (uint32_t)local_id;
	
	//initialize handle
	err = votequorum_initialize(&vq_h, NULL);
	if(err != CS_OK){
		return err;
	}
	//attempt to retrieve info
	err = votequorum_getinfo(vq_h, nodeid, &info);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//copy values to votes
	*expected_votes = (int) info.highest_expected;
	//finalize handle
	votequorum_finalize(vq_h);
	return CS_OK;
}

int set_expected_votes(unsigned int expected_votes)
{
	int err;
	unsigned int local_id;
	uint32_t nodeid;
	votequorum_handle_t vq_h;
	quorum_handle_t q_h;
	struct votequorum_info info;
	
	err = get_local_nodeid(&local_id);
	if(err != CS_OK){
		return err;
	}
	nodeid = (uint32_t)local_id;
	
	//initialize handle
	err = votequorum_initialize(&vq_h, NULL);
	if(err != CS_OK){
		return err;
	}
	//attempt to retrieve info
	err = votequorum_setexpected(vq_h, expected_votes);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//finalize handle
	votequorum_finalize(vq_h);
	return CS_OK;
}

int get_votes(uint32_t nodeid, unsigned int *votes)
{
	int err;
	unsigned int id;
	votequorum_handle_t vq_h;
	quorum_handle_t q_h;
	struct votequorum_info info;
	
	//initialize handle
	err = votequorum_initialize(&vq_h, NULL);
	if(err != CS_OK){
		return err;
	}
	//attempt to retrieve info
	err = get_local_nodeid(&id);
	err = votequorum_getinfo(vq_h, nodeid, &info);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//copy values to votes
	*votes = (int) info.node_votes;
	//finalize handle
	votequorum_finalize(vq_h);
	return CS_OK;
}

int set_votes(uint32_t nodeid, unsigned int votes)
{
	int err;
	unsigned int cur_votes;
	unsigned int expected_votes;
	int delta;
	votequorum_handle_t vq_h;
	quorum_handle_t q_h;
	
	//initialize handle
	err = votequorum_initialize(&vq_h, NULL);
	if(err != CS_OK){
		return err;
	}
	//get the current node votes (we may need to change expected votes)
	err = get_votes(nodeid, &cur_votes);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//get the current expected votes
	err = get_expected_votes(&expected_votes);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//set the votes
	err=votequorum_setvotes(vq_h, nodeid, votes);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//change expected votes accordingly
	delta = votes - cur_votes;
	expected_votes = expected_votes + delta;
	err = set_expected_votes(expected_votes);
	if(err != CS_OK){
		votequorum_finalize(vq_h);
		return err;
	}
	//success
	//finalize handle
	votequorum_finalize(vq_h);
	return CS_OK;
}