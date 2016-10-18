#include <stdio.h>
#include <argp.h>
#include <argz.h>
#include <stdlib.h>
#include <string.h>
#include "cluster_manager.h"
#include "client_errors.h"

const char *argp_program_bug_address = "charliemietzner@gmail.com";
const char *argp_program_version = "version 1.0";

int func;

int func_1(char *item)
{
	int err;
	//printing ring status
	if(strcasecmp(item, "ring") == 0){
		err = print_ring();
	}
	else{
		err = -1;
		return err;
	}
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
			func = 2;
			argz_add(&a->argz, &a->argz_len, arg);
			break;
		
		case 's':
			func = 1;
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
		{ "add", 'a', "address", 0, "add a node to the cluster"},
		{ "status", 's', "membership/quorum/ring/node", 0, "prints status of arguments"},
		{0}
	};
	
	struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};
	struct arguments arguments;
	if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0 && func == 1){
		const char *prev = NULL;
		char *item;
		while((item = argz_next(arguments.argz, arguments.argz_len, prev))){
			func_1(item);
			prev = item;
		}
		free(arguments.argz);
	}
	return 0;
} 