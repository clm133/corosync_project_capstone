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
	ClusterNode *next; // to store our cluster as a linked list
} ;

boolean clusterEstablished; 
ClusterNode *cluster; // a pointer to start of cluster node linked list
ClusterNode *end; // a pointer to end of cluster node linked list

int ui();
void add_node_prompt();
void add_node();
void single_node_state();
void cluster_membership();
void cluster_health();


int main(){
	//initially no cluster is established
	clusterEstablished = false; 
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
	int choice;
	
	printf("\n");
	printf("-------------------------------------\n");
	printf("           Corosync Client\n");
	printf("-------------------------------------\n");
	printf("Options:\n");
	printf("1. Show cluster's membership\n");
	printf("2. Show cluster health\n");
	if(clusterEstablished){
		printf("3. Add node to cluster");
	}
	else{
		printf("3. Establish Cluster");
	}
	printf("0. Exit\n");
	printf("-------------------------------------\n");
	printf("\n");
	scanf("%d", &choice);
	if (choice == 0){
		printf("Terminating...\n");
		return 0;
	}
	if (choice == 1) {
		cluster_membership();
	}
	if (choice == 2) {
		cluster_health();
	}
	if (choice == 3) {
		add_node_prompt();
	}
}


/*
Prompt for adding node by hostname
*/
void add_node_prompt(){
	char *buffer;
	
	buffer = malloc(sizeof(char)*64);
	if(clusterEstablished){
		printf("Please enter the hostname of node you would like to add to the cluster.");
	}
	else{
		printf("Please enter the hostname of node you would like to start the cluster with.");
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
	
	node = malloc(sizeof(ClusterNode));
	node->hostname = strdup(hostname);
	command = malloc(sizeof(char)*256);
	strcpy(command, SSH);
	strcat(command, hostname);
	strcat(command, CORO_START);
	system(command);
	
	if(clusterEstablished){
		end->next = node;
		end = end->next;
	}
	else{
		cluster = node;
		end = node;
		clusterEstablished = true;
	}
	free(command);
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
	strcat(command, cluster->hostname);
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
	strcat(command, cluster->hostname);
	strcat(command, CORO_HEALTH);
	system(command);
	printf("\n");
	free(command);
}
