#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>
#include <argz.h>
#include "client_errors.h"
#include "client_cmapctl.h"
#include "cluster_manager.h"
#include "conf_manager.h"
#include "ssh_manager.h"
#include "sftp_manager.h"
#include "quorum_manager.h"

struct arguments{
	char *argz;
	size_t argz_len;
};

enum client_task{
	add_option,
	remove_option,
	print_option,
	quorum_option,
	ssh_cmd
} task;


const char *argp_program_bug_address = "charliemietzner@gmail.com";
const char *argp_program_version = "version 10.31.16";


/* client_add_node() */
/* Adds a node at a given ip address */
void client_add_node(char *addr)
{	
	int err;
	char *src_file;
	char *dest_file;
	int total;
	char **nodelist;

	src_file = "corosync_client.conf";
	dest_file = "/etc/corosync/corosync.conf";
	// add node to cmap
	err = add_node(addr);
	if(err != CS_OK){
		printf("There were problems adding node at %s - error: %s\n", addr, get_error(err));
		return;
	}
	//generate conf file
	printf("node at %s was added to the cmap...generating conf...\n", addr);
	err = write_conf(src_file);
	if(err != CS_OK){
		printf("There were problems generating conf - error: %s\n", get_error(err));
		return;
	}
	//sftp conf file to all nodes
	printf("conf successfuly generated... attempting to update all cluster nodes...\n");
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	//before updating all nodes, we need to get a list of those remaining
	nodelist = malloc(sizeof(char *) * total);
	err = nodelist_get_addr_array(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		free(nodelist);
		return;
	}
	//now we can update all!
	err = update_all(nodelist, total, src_file, dest_file);
	//free up our malloc
	while(total > 0){
		free(nodelist[total - 1]);
		total--;
	}
	free(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	printf("all nodes in cluster successfully updated\n");
	//success!
	printf("Node at %s successfully added!\n", addr);
}

/* client_remove_node() */
/* Removes a node at a given ip address */
void client_remove_node(char *addr)
{
	int err;
	char *src_file;
	char *dest_file;
	int total;
	char **nodelist;

	src_file = "../conf_templates/default.conf";
	dest_file = "/etc/corosync/corosync.conf";
	// remove node from cmap
	err = delete_node(addr);
	if(err != CS_OK){
		printf("There were problems deleting node at %s from cmap- error: %s\n", addr, get_error(err));
		return;
	}
	// reset and turn off deleted node
	err = copy_conf(addr, src_file, dest_file);
	if(err != CS_OK){
		printf("There may have been problems reseting removed node at %s- error: %s\n", addr, get_error(err));
	}
	err = stop_corosync(addr);
	if(err != CS_OK){
		printf("There may have been problems stopping node at %s- error: %s\n", addr, get_error(err));
	}
	//generate new conf file
	src_file = "corosync_client.conf";
	printf("node at %s was deleted from the cmap...generating new conf...\n", addr);
	err = write_conf(src_file);
	if(err != CS_OK){
		printf("There were problems generating conf - error: %s\n", get_error(err));
		return;
	}
	//sftp conf file to all nodes
	printf("conf successfuly generated... attempting to update remaining cluster nodes...\n");
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	//before updating all nodes, we need to get a list of those remaining
	nodelist = malloc(sizeof(char *) * total);
	err = nodelist_get_addr_array(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		free(nodelist);
		return;
	}
	//now we can update all!
	err = update_all(nodelist, total, src_file, dest_file);
	//free up our malloc
	while(total > 0){
		free(nodelist[total - 1]);
		total--;
	}
	free(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	printf("all nodes in cluster successfully updated\n");
	//success!
	printf("Node at %s successfully removed!\n", addr);
}

/* client_change_epsilon() */
void client_change_epsilon(char *mode, uint32_t *id1, uint32_t *id2)
{
	int err;
	int total;
	char **nodelist;
	char *src_file;
	char *dest_file;
	
	src_file = "corosync_client.conf";
	dest_file = "/etc/corosync/corosync.conf";
	// set epsilon
	if(strcmp(mode, "set_epsilon") == 0){
		err = set_epsilon(*id1, 0);
		if(err != CS_OK){
			printf("there was an error setting epsilon - error: %s\n", get_error(err));
			return;
		}
	}
	// remove_epsilon
	else if(strcmp(mode, "remove_epsilon") == 0){
		err = remove_epsilon(*id1);
		if(err != CS_OK){
			printf("there was an error removing epsilon - error: %s\n", get_error(err));
			return;
		}
	}
	// move epsilon
	else if(strcmp(mode, "move_epsilon") == 0){
		err = move_epsilon(*id1, *id2);
		if(err != CS_OK){
			printf("there was an error moving epsilon - error: %s\n", get_error(err));
			return;
		}
	}
	//generate conf file
	err = write_conf(src_file);
	if(err != CS_OK){
		printf("There were problems generating conf - error: %s\n", get_error(err));
		return;
	}
	//sftp conf file to all nodes
	printf("conf successfuly generated... attempting to update all cluster nodes...\n");
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	//before updating all nodes, we need to get a list of those remaining
	nodelist = malloc(sizeof(char *) * total);
	err = nodelist_get_addr_array(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		free(nodelist);
		return;
	}
	//now we can update all!
	err = update_all(nodelist, total, src_file, dest_file);
	//free up our malloc
	while(total > 0){
		free(nodelist[total - 1]);
		total--;
	}
	free(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	printf("all nodes in cluster successfully updated\n");
	//success
	printf("epsilon changed successfully\n");
}

/* client_change_votes() */
void client_change_votes(char *addr, uint32_t votes)
{
	int err;
	uint32_t id;
	int total;
	char **nodelist;
	char *src_file;
	char *dest_file;
	
	src_file = "corosync_client.conf";
	dest_file = "/etc/corosync/corosync.conf";
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		printf("something went wrong accessing nodelist - error: %s\n", get_error(err));
		return;
	}
	err = get_id_from_addr(total, &id, addr);
	if(err != CS_OK){
		printf("something went wrong accessing nodelist - error: %s\n", get_error(err));
		return;
	}
	err = set_node_votes(id, votes);
	if(err != CS_OK){
		printf("something went wrong setting votes - error: %s\n", get_error(err));
		return;
	}
	//generate conf file
	printf("quorum votes at %s were changed to %u in the cmap...generating conf...\n", addr, votes);
	err = write_conf(src_file);
	if(err != CS_OK){
		printf("There were problems generating conf - error: %s\n", get_error(err));
		return;
	}
	//sftp conf file to all nodes
	printf("conf successfuly generated... attempting to update all cluster nodes...\n");
	err = nodelist_get_total(&total);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	//before updating all nodes, we need to get a list of those remaining
	nodelist = malloc(sizeof(char *) * total);
	err = nodelist_get_addr_array(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		free(nodelist);
		return;
	}
	//now we can update all!
	err = update_all(nodelist, total, src_file, dest_file);
	//free up our malloc
	while(total > 0){
		free(nodelist[total - 1]);
		total--;
	}
	free(nodelist);
	if(err != CS_OK){
		printf("There were problems updating cluster nodes - error: %s\n", get_error(err));
		return;
	}
	printf("all nodes in cluster successfully updated\n");
	//success
	printf("node at %s had their quorum_votes changed to %u\n", addr, votes);
}

void client_ssh_command(char *cmd, char *addr)
{
	int err;
	char *src_file;
	char *dest_file;
	
	//start corosync
	if(strcmp(cmd, "start") == 0){
		err = start_corosync(addr);
		if(err != CS_OK){
			printf("Failed to start corosync at node %s\n", addr);
		}
		printf("corosync started at node %s\n", addr);
	}
	// Stop corosync
	else if(strcmp(cmd, "stop") == 0){
		err = stop_corosync(addr);
		if(err != CS_OK){
			printf("Failed to stop corosync at node %s\n", addr);
		}
		printf("corosync stopped at node %s\n", addr);
	}
	// Reset to default conf file
	else if(strcmp(cmd, "reset") == 0){
		src_file = "../conf_templates/default.conf";
		dest_file = "/etc/corosync/corosync.conf";
		err = copy_conf(addr, src_file, dest_file);
		if(err != CS_OK){
			printf("Failed to reset corosync.conf to default at node %s - error: %s\n", addr, get_error(err));
		}
		err = stop_corosync(addr);
		if(err != CS_OK){
			printf("May have reset corosync.conf to default at node %s, but something went wrong restarting corosync... - error: %s\n", addr, get_error(err));
		}
		printf("corosync.conf reset to default at node %s\n", addr);
	}
	// Unrecognized command
	else{
		printf("Command %s not recognized.\n", cmd);
	}
}

/* the client_X_option() functions run through the list of arguments and route client instructions appropriately*/
void client_print_option(struct arguments *arguments)
{
}

void client_quorum_option(struct arguments *arguments)
{
	int err;
	char *q_target;
	char *prev;
	char *addr;
	uint32_t id1;
	uint32_t id2;
	struct timespec half_sec;

	half_sec.tv_nsec = 500000000;
	prev = NULL;
	q_target = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = q_target;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		// mark elegible
		if(strcmp(q_target, "elegible") == 0){
		}
		// set epsilon
		else if(strcmp(q_target, "set_epsilon") == 0){
			id1 = (uint32_t)atoi(addr);
			client_change_epsilon(q_target, &id1, NULL);
			break;
		}
		// remove epsilon
		else if(strcmp(q_target, "remove_epsilon") == 0){
			id1 = (uint32_t)atoi(addr);
			client_change_epsilon(q_target, &id1, NULL);
			break;
		}
		// move epsilon
		else if(strcmp(q_target, "move_epsilon") == 0){
			id1 = (uint32_t)atoi(addr);
			prev = addr;
			addr = argz_next(arguments->argz, arguments->argz_len, prev);
			id2 = (uint32_t)atoi(addr);
			client_change_epsilon(q_target, &id1, &id2);
			break;
		}
		// change votes
		else if(strcmp(q_target, "set_votes") == 0){
			prev = addr;
			addr = argz_next(arguments->argz, arguments->argz_len, prev);
			id1 = (uint32_t)atoi(addr);//votes
			client_change_votes(prev, id1);
			prev = addr;
		}
		// unknown q_target 
		else{
			break;
		}
		
		prev = addr;
		nanosleep(&half_sec, NULL); //updating conf files and restarting corosync takes some time, so we sleep between executions
	}
}

void client_add_option(struct arguments *arguments)
{
	int err;
	char *add_target;
	const char *prev;
	char *addr;
	struct timespec half_sec;

	half_sec.tv_nsec = 500000000;
	prev = NULL;
	add_target = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = add_target;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		// add a node
		if(strcmp(add_target, "node") == 0){
			client_add_node(addr);
		}
		else{
			break;
		}
		prev = addr;
		nanosleep(&half_sec, NULL); //updating conf files and restarting corosync takes some time, so we sleep between executions
	}
}

void client_remove_option(struct arguments *arguments)
{
	int err;
	char *remove_target;
	const char *prev;
	char *addr;
	struct timespec half_sec;

	half_sec.tv_nsec = 500000000;
	prev = NULL;
	remove_target = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = remove_target;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		// remove a node
		if(strcmp(remove_target, "node") == 0){
			client_remove_node(addr);
		}
		else{
			break;
		}
		nanosleep(&half_sec, NULL); //updating conf files and restarting corosync takes some time, so we sleep between executions
		prev = addr;
	}
	
}

void client_ssh_option(struct arguments *arguments)
{
	int err;
	char *cmd;
	const char *prev;
	char *addr;
	

	prev = NULL;
	cmd = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = cmd;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		client_ssh_command(cmd, addr);
		prev = addr;
	}
}

/* parse_opt() this is argp's thing */
static int parse_opt(int key, char *arg, struct argp_state *state)
{
	size_t count;
	struct arguments *a;

	a = state->input;
	switch(key){
			
	//add_option
	case 'a':
		task = add_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//remove_option
	case 'r':
		task = remove_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//ssh_command
	case 'c':
		task = ssh_cmd;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
	
	//print_option
	case 'p':
		task = print_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//quorum_option
	case 'q':
		task = quorum_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//argp stuff
	case ARGP_KEY_ARG:
		argz_add(&a->argz, &a->argz_len, arg);
		break;

	case ARGP_KEY_INIT:
		a->argz = 0;
		a->argz_len = 0;
		break;
			
	//Argument limit (currently 5)	
	case ARGP_KEY_END:
		count = argz_count(a->argz, a->argz_len);
		if(count > 5){
			argp_failure(state, 1, 0, "too many arguments");
		}
		else if(count < 2){
			argp_failure(state, 1, 0, "too few arguments");
		}
		break;
			
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct argp_option options[]={
		{ "add", 'a', "<cluster_option> <node-ip-addr>", 0, "adds a cluster option (usually a node) to the cluster."},
		{"remove", 'r', "<cluster_option> <node-ip-addr>", 0, "removes a cluster option (usually a node) from the cluster."},
		{"command", 'c', "<ssh-command> <node-ip-addr>", 0, "uses ssh to remotely execute command at target node in the cluster."},
		{"print", 'p', "<corosync-item>", 0, "prints the status of corosync target item"},
		{"quorum", 'q', "<quorum-setting>", 0, "changes a quorum setting"},
		{0}
	};
	struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};
	struct arguments arguments;
	printf("\n");
	if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0){
		switch(task){
				
			case add_option:
				client_add_option(&arguments);
				break;
				
			case remove_option:
				client_remove_option(&arguments);
				break;
				
			case ssh_cmd:
				client_ssh_option(&arguments);
				break;
			
			case print_option:
				client_print_option(&arguments);
				break;
				
			case quorum_option:
				client_quorum_option(&arguments);
				break;
				
		}
		free(arguments.argz);
	}
	printf("\n");
}
