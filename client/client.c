#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//some constants for commands
#define SSH "ssh root@"
#define CORO_START "corosync"
#define CORO_MEMBERSHIP "corosync-cmapctl | grep member"
#define CORO_HEALTH "corosync-cfgtool -s"
#define CORO_QUORUM "corosync-quorumtool"
#define CORO_SHUTDOWN "corosync-cfgtool -H"

//boolean stuff
#define true 1
#define false 0

/* a simple boolean struct (true/false defined above) */
typedef int boolean;

/* This struct holds node information */
typedef struct ClusterNode ClusterNode;
struct ClusterNode {
	unsigned int node_id;
	char *hostname; //relative to client machine
	ClusterNode *prev; 
	ClusterNode *next; // to store our cluster as a linked list
} ;

boolean clusterEstablished; 
ClusterNode *cluster_start; // a pointer to start of cluster node linked list
ClusterNode *cluster_end; // a pointer to end of cluster node linked list

int ui();
void add_node_prompt();
void add_node();
void remove_node(ClusterNode *node); //the parameter here should probably eventually change to node_id;
void cluster_destroy();
void single_node_state();
void cluster_membership();
void cluster_health();


int main(){
	//initially no cluster is established
	clusterEstablished = false; 
	//display ui
	while(1){
		if (ui() == 0){
			if(clusterEstablished){
				cluster_destroy();
			}
			exit(0);
		}
	}
}

/*
Text-based UI for the client
*/
int ui(){
	int choice;
	
	printf("\n");
	printf("-------------------------------------\n");
	printf("           Corosync Client\n");
	printf("-------------------------------------\n");
	printf("Options:\n");
	printf("1. Show cluster's membership\n");
	printf("2. Show cluster health\n");
	if(clusterEstablished){
		printf("3. Add node to cluster\n");
	}
	else{
		printf("3. Establish Cluster\n");
	}
	printf("4. Destroy Cluster\n");
	printf("0. Exit\n");
	printf("-------------------------------------\n");
	printf("\n");
	scanf("%d", &choice);
	if (choice == 0){
		printf("Terminating...\n");
		return 0;
	}
	else if (choice == 1) {
		cluster_membership();
	}
	else if (choice == 2) {
		cluster_health();
	}
	else if (choice == 3) {
		add_node_prompt();
	}
	else if (choice == 4) {
		cluster_destroy();
	}
	else{
		printf("invalid selection\n");
	}
	
	return choice;
}

/*
Prompt for adding node by hostname
*/
void add_node_prompt(){
	char *buffer;
	
	buffer = malloc(sizeof(char)*64);
	if(clusterEstablished){
		printf("\nPlease enter the hostname of node you would like to add to the cluster.\n");
	}
	else{
		printf("\nPlease enter the hostname of node you would like to start the cluster with.\n");
	}
	scanf("%s", buffer);
	//eventually we will want to add some input handling here
	add_node(buffer);
	free(buffer);
}

/*
Adds a node to the cluster (or establishes one if no cluster exists)
*/
void add_node(char *hostname){
	ClusterNode *node;
	char *command;
	//malloc for node to add to cluster linked list (remeber the hostname must also be freed when freeing node)
	node = malloc(sizeof(ClusterNode));
	node->hostname = strdup(hostname);
	//create a string to pass to system()
	command = malloc(sizeof(char)*256);
	strcpy(command, SSH);
	strcat(command, hostname);
	strcat(command, " ");
	strcat(command, CORO_START);
	system(command);
	//Check if we have established cluster yet
	if(clusterEstablished){
		//double link list
		cluster_end->next = node;
		node->prev = cluster_end;
		cluster_end = cluster_end->next;
	}
	else{
		cluster_start = node;
		cluster_end = node;
		clusterEstablished = true;
	}
	
	free(command);
}

/*
Removes node from the cluster
*/
void remove_node(ClusterNode *node)
{
	char *command;
	
	//shut down corosync on node 
	command = malloc(sizeof(char)*256);
	strcpy(command, SSH);
	strcat(command, node->hostname);
	strcat(command, " ");
	strcat(command, CORO_SHUTDOWN);
	system(command);
	free(command);
	
	//removes node record from linked list, caller function responsible for updating linked list
	free(node->hostname);
	free(node);
}

/*
Shuts down and removes all nodes in entire cluster
*/
void cluster_destroy()
{
	ClusterNode *cur;
	ClusterNode *next;
	
	cur = cluster_start;
	while(cur != cluster_end){
		next = cur->next;
		remove_node(cur);
		cur = next;
	}
	remove_node(cur);
	clusterEstablished = false;
	printf("Cluster destroyed.\n");
}


/*
Get the status of a single node
*/
void single_node_state(){
	printf("\n");
}

/*
Print the cluster membership
*/
void cluster_membership()
{
	char *command;
	command = malloc(sizeof(char)*256);
	strcpy(command, SSH);
	strcat(command, cluster_start->hostname);
	strcat(command, " ");
	strcat(command, CORO_MEMBERSHIP);
	system(command);
	printf("\n");
	free(command);
}

/*
Print cluster's health
*/
void cluster_health(){
	char *command;
	command = malloc(sizeof(char)*256);
	strcpy(command, SSH);
	strcat(command, cluster_start->hostname);
	strcat(command, " ");
	strcat(command, CORO_HEALTH);
	system(command);
	printf("\n");
	free(command);
}
