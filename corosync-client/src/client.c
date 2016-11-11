#include <stdio.h>
#include <argp.h>
#include <argz.h>
#include <stdlib.h>
#include <string.h>
#include "cluster_manager.h"
#include "client_errors.h"
#include "modified_cmapctl.h"
#include "modified_cfgtool.h"
#include "ssh_manager.h"
#include <pthread.h>

const char *argp_program_bug_address = "charliemietzner@gmail.com";
const char *argp_program_version = "version 1.0";

enum client_task {
	status,
	add_option,
	remove_option,
	ssh_cmd,
	mark_elig,
	mark_inelig,
	epsilon_options
}task;

/* -s option prints out information regarding the following arguments: "ring", "members", "node"*/
int status_print(char *item)
{
	int err;
	//printing ring status
	if(strcasecmp(item, "ring") == 0){
		err = print_ring();
		printf("\n");
	}
	//printing members
	if(strcasecmp(item, "members") == 0){
		err = print_members();
		printf("\n");
	}
	else{
		err = -1;
		return err;
	}
}

int add_options(char *item, char *value)
{
	int err;

	if(strcmp(item, "node") == 0){
		printf("adding node %s...\n", value);
		err = add_node(value);
		if(err != CS_OK){
			printf("something went wrong adding node! Error#%d: %s\n", err, get_error(err));
			return -1;
		}
		printf("success!\n\n");
	}
	else{
		err = -1;
	}
	return err;
}

//item = type of command, value = ip address
int ssh_command(char *item, char *value)
{
	int err;
	char addr[INET6_ADDRSTRLEN];
	//start corosync
	if(strcmp(item, "start") == 0){
		printf("starting corosync at node %s...\n", value);
		err = start_corosync(value);
		if(err != 1){
			printf("something went wrong with ssh\n");
			return -1;
		}
		printf("success!\n\n");
	}
	if(strcmp(item, "stop") == 0){
		printf("shutting off corosync at node %s...\n", value);
		err = stop_corosync(value);
		if(err != 1){
			printf("something went wrong with ssh!\n");
			return -1;
		}
		printf("success!\n\n");
	}
	if(strcmp(item, "reset") == 0){
		printf("Resetting cluster...\n");
		err = reset_cluster(value);
		if(err != 1){
			printf("error resetting cluster\n");
			return -1;
		}
		printf("success!\n");
	}
	
	return 1;
}

// item -> "node", value -> node id
int remove_options(char *item, char *value)
{
	int err;
	uint32_t id = (uint32_t)atoi(value);
	if(strcmp(item, "node") == 0){
		printf("removing nodeid %s...\n", value);
		err = remove_node(id);
		if (err != CS_OK){
			printf("Something went wrong removing node! Error#%d: %s\n", err, get_error(err));
			return -1;
		}
		printf("success!\n\n");
	}
	else{
		err = -1;
	}
	return err;
}

int mark_ineligible (char *value){
	int ret;
	uint32_t id = (uint32_t)atoi(value);
	votequorum_handle_t vq_h;
	votequorum_callbacks_t vq_cb;
	votequorum_initialize(&vq_h, &vq_cb);
	struct votequorum_info info;
	ret = votequorum_getinfo( vq_h, id, &info); 
	if(ret == CS_OK){
		ret = votequorum_setexpected(vq_h, info.node_expected_votes-1);
	}
	if(ret == CS_OK) {
		ret = votequorum_setvotes( vq_h, info.node_id, 0);
	}
	if( ret == CS_OK ){
		ret = votequorum_getinfo( vq_h, id, &info); 
		printf("expected votes: %d\n", info.node_expected_votes);
		printf("number votes: %d\n", info.node_votes);	
	}
		
	return ret; 
}

int mark_eligible ( char *value){
	int ret;
	uint32_t id =(uint32_t)atoi(value);
	votequorum_callbacks_t vq_cb;
	votequorum_handle_t vq_h;
	votequorum_initialize( &vq_h, &vq_cb);
	struct votequorum_info info;
	ret = votequorum_getinfo( vq_h, id, &info);
	if( ret == CS_OK){
		printf("previous votes for node were: %d\n", info.node_votes);
		printf("previous expected votes were: %d\n", info.node_expected_votes);
		ret = votequorum_setvotes(vq_h, id, 1);
		if (ret == CS_OK){
			votequorum_setexpected( vq_h, info.node_expected_votes+1);
		}
		
		if (ret == CS_OK){
			ret = votequorum_getinfo( vq_h, id, &info);
					
		printf("votes now set at: %d\n", info.node_votes);
		printf("expected votes now set at: %d\n", info.node_expected_votes);
		}
		return ret;
	}
	else
		return ret;
}

int epsilon_options(char *item, char *value)
{
	int err;
	uint32_t id = (uint32_t) atoi(value);
	if (strcmp(item, "add") == 0) {
		printf("adding epsilon to node %s...\n", id);
		err = add_epsilon(id);
		if (err != CS_OK) {
			printf("something went wrong adding epsilon! Error#%d: %s\n", err, get_error(err));
			return -1;
		}
		printf("success!\n\n");
	}
	else {
		err = -1;
	}
	return err;
}

struct arguments
{
	char *argz;
	size_t argz_len;
};

static int parse_opt(int key, char *arg, struct argp_state *state)
{
	size_t count;
	struct arguments *a;

	a = state->input;
	switch(key){
		//adding nodes
		case 'a':
			task = add_option;
			argz_add(&a->argz, &a->argz_len, arg);
			break;
		//sending ssh commands (currently just "start" or "stop")	
		case 'c':
			task = ssh_cmd;
			argz_add(&a->argz, &a->argz_len, arg);
			break;

		//removing nodes
		case 'r':
			task = remove_option;
			argz_add(&a->argz, &a->argz_len, arg);
			break;

		//check status
		case 's':
			task = status;
			argz_add(&a->argz, &a->argz_len, arg);
			break;

		//mark node eligible
		case 'e':
			task = mark_elig;
			argz_add(&a->argz, &a->argz_len, arg);
			break;

		//mark node ineligible			
		case 'i':
			task = mark_inelig;					
			argz_add(&a->argz, &a->argz_len, arg);
			break;

		//epsilon options
		case 'l':
			task = epsilon_options;
			argz_add(&a->argz, &a->argz_len, atg);
			break;
			
		case ARGP_KEY_ARG:
			argz_add(&a->argz, &a->argz_len, arg);
			break;

		case ARGP_KEY_INIT:
			a->argz = 0;
			a->argz_len = 0;
			break;

		case ARGP_KEY_END:
			count = argz_count(a->argz, a->argz_len);
			if(count > 2){
				argp_failure(state, 1, 0, "too many arguments");
			}
			else if(count < 0){
				argp_failure(state, 1, 0, "too few arguments");
			}
			break;
	}
	return 0;
}

// example for adding node: sudo ./corosync-client -a node 11.0.0.2
// for removing node: sudo ./corosync-client -r node 1
int main(int argc, char **argv)
{
	struct argp_option options[]={
		{"add", 'a', "node", 0, "add a node with address arguement to the cluster"},
		{"remove", 'r', "node", 0, "remove a node according to nodeid"},
		{"command", 'c', "start/stop", 0, "starts or stops corosync at specified ip address"},
		{"status", 's', "members/quorum/ring/node", 0, "prints status of supplied arguments"},
		{"mark_elig", 'e', "node", 0,"mark node eligible in cluster"},
		{"mark_inelig", 'i',"node", 0, "mark node ineligible in cluster"},
		{"epsilon_options", 'l',"add", 0, "add epsilon to a node"},
		{0}
	};
	struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};
	struct arguments arguments;
	printf("\n");
	//status
	if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == status){
		const char *prev = NULL;
		char *item;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			status_print(item);
			prev = item;
		}
		free(arguments.argz);
	}
	//add_option
	else if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == add_option){
		const char *prev = NULL;
		char *item;
		char *value;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			value = argz_next(arguments.argz, arguments.argz_len, item);
			add_options(item, value);
			prev = value;
		}
		free(arguments.argz);
	}
	//remove_option
	else if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == remove_option){
		const char *prev = NULL;
		char *item;
		char *value;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			value = argz_next(arguments.argz, arguments.argz_len, item);
			remove_options(item, value);
			prev = value;
		}
		free(arguments.argz);
	}
	//ssh_cmd
	else if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == ssh_cmd){
		const char *prev = NULL;
		char *item;
		char *value;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			ssh_command(item, value);
			prev = value;
		}
		free(arguments.argz);
	}
	else if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == mark_elig){
		const char *prev = NULL;
		char *item;
		char *value;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			value = argz_next(arguments.argz, arguments.argz_len, item);
			mark_eligible(value);
			prev = value;
		}
		free(arguments.argz);
	}
	else if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == mark_inelig){
		const char *prev = NULL;
		char *item;
		char *value;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			value = argz_next(arguments.argz, arguments.argz_len, item);
			mark_ineligible(value);
			prev = value;
		}
		free(arguments.argz);
	}
	else if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == epsilon_options){
		const char *prev = NULL;
		char *item;
		char *value;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			value = argz_next(arguments.argz, arguments.argz_len, item);
			epsilon_options(value);
			prev = value;
		}
		free(arguments.argz);
	}
	return 0;
}
