/*Sprint1*/

#include <stdio.h>
#include <stdlib.h>

int ui();
void single_node_state();
void cluster_health();
void connect_corosync();

int main(){
	//display ui
	while(1){
		if (ui() == 0){
			exit(0);
		}
	}
}

/*
Text-based UI for the client
*/
int ui(){
	connect_corosync();
	printf("\n");
	printf("-------------------------------------\n");
	printf("           Corosync Client\n");
	printf("-------------------------------------\n");
	printf("Options:\n");
	printf("1. Show cluster's membership\n");
	printf("2. Show cluster health\n");
	printf("0. Exit\n");
	printf("-------------------------------------\n");
	printf("\n");
	int choice;
	scanf("%d", &choice);
	if (choice == 0){
		printf("Terminating...\n");
		return 0;
	}
	if (choice == 1) {
		single_node_state();
	}

	if (choice == 2) {

		cluster_health();
	}
}

/*Connect to the Corosync process*/
void connect_corosync(){




}
/*
Get the status of a single node
*/
void single_node_state(){
	// see pacemaker documents
	system("crm_mon");
	printf("\n");


}

/*
Get the cluster's health
*/
void cluster_health(){
	// see pacemaker documents
	system("crm_mon -n");
	printf("\n");



}
