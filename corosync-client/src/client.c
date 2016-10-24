#include <stdio.h>
#include <argp.h>
#include <argz.h>
#include <stdlib.h>
#include <string.h>
#include "cluster_manager.h"
#include "client_errors.h"
#include "modified_cmapctl.h"

const char *argp_program_bug_address = "charliemietzner@gmail.com";
const char *argp_program_version = "version 1.0";

enum client_task {
	status,
	add_option
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
		err = add_node(value);
		if(err != CS_OK){
			printf("something went wrong adding node! Error#%d: %s\n", err, get_error(err));
			return -1;
		}
		printf("Node added successfully.\n");
	}
	else{
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
		case 'a':
			task = add_option;
			argz_add(&a->argz, &a->argz_len, arg);
			break;
		
		case 's':
			task = status;
			argz_add(&a->argz, &a->argz_len, arg);
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

int main(int argc, char **argv)
{	
	struct argp_option options[]={
		{ "add", 'a', "node", 0, "add a node with address arguement to the cluster"},
		{ "status", 's', "members/quorum/ring/node", 0, "prints status of supplied arguments"},
		{0}
	};
	
	struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};
	struct arguments arguments;
	printf("\n");
	if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && task == status){
		const char *prev = NULL;
		char *item;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			status_print(item);
			prev = item;
		}
		free(arguments.argz);
	}
	
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
	return 0;
} 