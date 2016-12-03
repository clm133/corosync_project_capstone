#include "print_manager.h"

int print_membership()
{
	int err;
	Cluster_Member **arr;
	int arr_len;
	unsigned int local_nodeid;
	int i;
	
	err = get_member_count(&arr_len);
	if(err != CS_OK){
		return err;
	}
	arr = malloc(arr_len * sizeof(Cluster_Member *));
	for(i = 0; i < arr_len; i++){
		arr[i] = malloc(sizeof(Cluster_Member));
	}
	err = get_local_nodeid(&local_nodeid);
	err = get_members(arr, arr_len);
	printf("\nMembership information\n");
	printf("----------------------\n");
	for(i = 0; i < arr_len; i++){
		printf("Node %d\t", arr[i]->nodeid);
		printf("IP: %s\t", arr[i]->ip);
		printf("STATUS: %s", arr[i]->status);
		if(arr[i]->nodeid == local_nodeid){
			printf("\t(local)");
		}
		printf("\n");
		free(arr[i]);
	}
	free(arr);
	return CS_OK;
}

int print_quorum_membership()
{
	int err;
	Quorum_Details qd;
}

int print_quorum()
{	
	int err;
	Quorum_Details qd;
	
	err = get_quorum_info(&qd);
	if(err != CS_OK){
		return err;
	}
	
	printf("\nQuorum information\n");
	printf("----------------------\n");
	printf("Quorate: %s\n", (qd.quorate == VOTEQUORUM_INFO_QUORATE) ? "yes": "no (Activity Blocked)");
	printf("Quorum: %d\n", qd.quorum);
	printf("Total Votes: %d\n", qd.total_votes);
	printf("Expected Votes: %d\n", qd.highest_expected);
	
	return CS_OK;
}