#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>
#include <argz.h>
#include <corosync/quorum.h>
#include <time.h>
#include "client_errors.h"
#include "client_cmapctl.h"
#include "ssh_manager.h"
#include "sftp_manager.h"
#include "print_manager.h"
#include "monitor.h"
#include "quorum_manager.h"
#include "cluster_manager.h"

struct arguments{
	char *argz;
	size_t argz_len;
};

enum client_task{
	add_option,
	remove_option,
	set_option,
	print_option,
	quorum_option,
	timed_option,
	monitor_option,
	eligibility_option,
	ssh_cmd,
	udpu_option,
} task;


const char *argp_program_bug_address = "charliemietzner@gmail.com";
const char *argp_program_version = "version 12.1.16";

enum client_task timed;
enum client_task udpu_mode;

void check_for_transport_type()
{
	int err;
	cmap_value_types_t type;
	
	err = get_cmap_value("totem.transport", NULL, &type);
	if(err != CS_ERR_NOT_EXIST){ //if we found it
		udpu_mode = udpu_option;
	}
}

void client_ssh_command(char *cmd, char *addr, char *src_file_name)
{
	int err;
	char src_file[128];
	char *dest_file;
	long cmd_start;
	Notify_Context nc;
		
	//start corosync
	if(strcmp(cmd, "start") == 0){
		nc.change = CLUSTER_JOINED;
		strncpy(nc.target_name,addr,32);
		if(timed == timed_option){
			monitor_single_dispatch(&nc);
			err = start_corosync_timed(addr, &nc.context_start);
		}
		else{
			err = start_corosync(addr);
		}
		if(err != CS_OK){
			printf("Failed to start corosync at node %s\n", addr);
		}
		//printf("corosync started on node %s ( command sent at %lu )\n", addr, cmd_start);
	}
	// Stop corosync
	else if(strcmp(cmd, "stop") == 0){
		nc.change = CLUSTER_LEFT;
		strncpy(nc.target_name,addr,32);
		if(timed == timed_option){
			monitor_single_dispatch(&nc);
			err = stop_corosync_timed(addr, &nc.context_start);
		}
		else{
			err = stop_corosync(addr);
		}
		if(err != CS_OK){
			printf("Failed to stop corosync at node %s\n", addr);
		}
		//printf("corosync stopped on node %s ( command sent at %lu )\n", addr, cmd_start);
	}
	// Reset to default conf file
	else if(strcmp(cmd, "reset_conf") == 0){
		memset(src_file, sizeof(src_file), '\0');
		strcpy(src_file, "../conf_templates/");
		strcat(src_file, src_file_name);
		dest_file = "/etc/corosync/corosync.conf";
		err = copy_conf(addr, src_file, dest_file);
		if(err != CS_OK){
			printf("Failed to reset corosync.conf to default at node %s - error: %s\n", addr, get_error(err));
		}
		err = stop_corosync(addr);
		if(err != CS_OK){
			printf("May have reset corosync.conf to default at node %s, but something went wrong restarting corosync... - error: %s\n", addr, get_error(err));
		}
		printf("corosync.conf reset to %s at node %s\n", src_file_name, addr);
	}
	else if(strcmp(cmd, "kill_conn") == 0){
		printf("killing connection at %s", addr);
		err = kill_conn(addr, &nc.context_start);
	}
	else if(strcmp(cmd, "start_conn") == 0){
		printf("starting connection at %s", addr);
		err = start_conn(addr, &nc.context_start);
	}
	else if(strcmp(cmd, "reload") == 0){
		printf("Reloading corosync at %s", addr);
		restart_corosync(addr);
	}
	
	// Unrecognized command
	else{
		printf("Command %s not recognized.\n", cmd);
	}
}

/* client_add_node() */
/* Adds a node at a given ip address */
void client_add_node(char *addr)
{	
	client_ssh_command("start", addr, NULL);
}

/* client_remove_node() */
/* Removes a node at a given ip address */
void client_remove_node(char *addr)
{
	client_ssh_command("stop", addr, NULL);
}

/* client_add_epsilon() */
void client_add_epsilon(char *addr)
{
	printf("place holder\n");
}

/* client_add_epsilon() */
void client_remove_epsilon(char *addr)
{
	printf("place holder\n");
}

/* client_change_votes() */
void client_change_votes(uint32_t nodeid, uint32_t votes)
{
	int err;
	
	err = set_votes(nodeid, votes);
	if(err != CS_OK){
		printf("failed to update node %u votes: %s\n", nodeid, get_error(err));
	}
	//success
	printf("node %u had their quorum_votes changed to %u\n", nodeid, votes);

}

void client_set_expected_votes(uint32_t votes)
{
	int err;

	err = set_expected_votes((unsigned int)votes);
	if(err != CS_OK){
		printf("failed to set expected votes: %s\n",get_error(err));
	}
	//success
	printf("the total expected votes was set to to %u\n", votes);
	
}

void client_change_eligibility(char *mode, uint32_t *id1) 
{
	int err;
	
	// mark eligible
	if(strcmp(mode, "mark_eligible") == 0){
		err = mark_eligible(*id1);
		if(err != CS_OK){
			printf("there was an error marking node eligible - error: %s\n", get_error(err));
			return;
		}
		//success
		printf("node %u has been made inelligble\n", *(uint32_t *)id1);
	}
	
	// mark ineligible
	else if(strcmp(mode, "mark_ineligible") == 0) {
		err = mark_ineligible(*id1);
		if(err != CS_OK){
			printf("there was an error marking node ineligible - error: %s\n", get_error(err));
			return;
		}
		//success
		printf("node %u has been made inelligble\n", *(uint32_t *)id1);
	}
}

void client_monitor_option(struct arguments *arguments)
{
	int err;
	char *m_target;

	m_target = argz_next(arguments->argz, arguments->argz_len, NULL);
	if(strcmp(m_target, "membership") == 0){
		err = monitor_status();
		if(err != CS_OK){
			printf("monitor recieved an error: %s\n", get_error(err));
		}
	}
	else if(strcmp(m_target, "quorum") == 0){
		err = monitor_status();
		if(err != CS_OK){
			printf("monitor recieved an error: %s\n", get_error(err));
		}
	}
}

/* the client_X_option() functions run through the list of arguments and route client instructions appropriately*/
void client_print_option(struct arguments *arguments)
{
	int err;
	char *print_target;

	print_target = argz_next(arguments->argz, arguments->argz_len, NULL);
	if(strcmp(print_target, "membership") == 0){
		err = print_membership();
		if(err != CS_OK){
			printf("print manager recieved an error: %s\n", get_error(err));
		}
	}
	else if(strcmp(print_target, "quorum") == 0){
		err = print_quorum();
		if(err != CS_OK){
			printf("print manager recieved an error: %s\n", get_error(err));
		}
	}
}

void client_quorum_option(struct arguments *arguments)
{
	int err;
	char *prev;
	char *q_option;
	char *q_target;
	uint32_t nodeid;
	int votes;
	
	prev = NULL;
	q_option = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = q_option;
	while(q_target = argz_next(arguments->argz, arguments->argz_len, prev)){
		//change votes
		if(strcmp(q_option, "set_votes") == 0){
			//get node id
			nodeid = (uint32_t)atoi(q_target);
			//get votes
			q_target = argz_next(arguments->argz, arguments->argz_len, q_target);
			votes = atoi(q_target);
			//call client_change_vote
			client_change_votes(nodeid, votes);
			break;
		}
		
		//change expected votes
		else if(strcmp(q_option, "set_expected") == 0){
			//get node id
			//get votes
			q_target = argz_next(arguments->argz, arguments->argz_len, q_target);
			q_target = argz_next(arguments->argz, arguments->argz_len, q_target);
			votes = atoi(q_target);
			//call client_change_vote
			client_set_expected_votes(votes);
			break;
		} 
		
		prev = q_target;
	}
}

void client_eligibity_option(struct arguments *arguments)
{
	int err;
	char *e_target;
	const char *prev;
	char *addr;
	uint32_t id1;
	
	prev = NULL;
	e_target = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = e_target;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		// mark eligible
		if(strcmp(e_target, "mark_eligible") == 0){
			id1 = (uint32_t)atoi(addr);
			client_change_eligibility(e_target, &id1);
			break;
		}
		// mark ineligible
		else if(strcmp(e_target, "mark_ineligible") == 0){
			id1 = (uint32_t)atoi(addr);
			client_change_eligibility(e_target, &id1);
			break;
		}
		// unknown e_target
		else{
			break;
		}
		
		prev = addr;
	}
}

void client_add_option(struct arguments *arguments)
{
	int err;
	char *add_target;
	const char *prev;
	char *addr;

	prev = NULL;
	add_target = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = add_target;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		// add a node
		if(strcmp(add_target, "node") == 0){
			client_add_node(addr);
		}
		// add epsilon
		if(strcmp(add_target, "epsilon") == 0){
			client_add_epsilon(addr);
		}
		else{
			break;
		}
		prev = addr;
	}
}

void client_remove_option(struct arguments *arguments)
{
	int err;
	char *remove_target;
	const char *prev;
	char *addr;
	
	prev = NULL;
	remove_target = argz_next(arguments->argz, arguments->argz_len, prev);
	prev = remove_target;
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		// remove a node
		if(strcmp(remove_target, "node") == 0){
			client_remove_node(addr);
		}
		else if(strcmp(remove_target, "epsilon")){
			client_remove_epsilon(addr);
		}
		else{
			break;
		}
		prev = addr;
	}
	
}

void client_ssh_option(struct arguments *arguments)
{
	int err;
	char *cmd;
	char *src_file_name;  
	const char *prev;
	char *addr;
	

	prev = NULL;
	src_file_name = NULL;
	cmd = argz_next(arguments->argz, arguments->argz_len, prev);
	if(strcmp(cmd, "reset_conf") == 0){
		src_file_name = argz_next(arguments->argz, arguments->argz_len, cmd);
		prev = src_file_name;
	}

	else{
		prev = cmd;
	}
	while(addr = argz_next(arguments->argz, arguments->argz_len, prev)){
		client_ssh_command(cmd, addr, src_file_name);
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
	
	//timed option (must preced any other option)
	case 't':
		timed = timed_option;
		break;
	
	//eligibility_option
	case 'e':
		task = eligibility_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//remove_option
	case 'r':
		task = remove_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//set_option
	case 's':
		task = set_option;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
			
	//ssh_command
	case 'c':
		task = ssh_cmd;
		argz_add(&a->argz, &a->argz_len, arg);
		break;
	
	//monitor_option
	case 'm':
		task = monitor_option;
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
			
	//Argument limit
	case ARGP_KEY_END:
		count = argz_count(a->argz, a->argz_len);
		if(count > 8){
			argp_failure(state, 1, 0, "too many arguments");
		}
		else if(count < 1){
			argp_failure(state, 1, 0, "too few arguments");
		}
		break;
			
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct argp_option options[]={
		{"set", 's', "<directive-being-set><value being set><node-ip-addr>", 0, "sets a cluster directive at a location"},
		{ "add", 'a', "<cluster_option> <node-ip-addr>", 0, "adds a cluster option (usually a node) to the cluster."},
		{"remove", 'r', "<cluster_option> <node-ip-addr>", 0, "removes a cluster option (usually a node) from the cluster."},
		{"command", 'c', "<ssh-command> <node-ip-addr>", 0, "uses ssh to remotely execute command at target node in the cluster."},
		{"print", 'p', "<corosync-item>", 0, "prints the status of corosync target item"},
		{"monitor", 'm',"<nothing>", 0, "begins monitoring locally"},
		{"quorum", 'q', "<quorum-setting>", 0, "changes a quorum setting"},
		{"timed", 't', "<nothing>", 0, "preceed other instructions with -t to time the operation"},
		{"eligibility", 'e', "<eligiblity-setting>", 0, "changes a eligibity setting"},
		{0}
	};
	
	struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};
	struct arguments arguments;
	timed = -1;
	printf("\n");
	if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0){
		switch(task){
			
			case set_option:
				client_add_option(&arguments);
				break;
				
			case add_option:
				client_add_option(&arguments);
				break;
				
			case remove_option:
				client_remove_option(&arguments);
				break;
			
			case eligibility_option:
				client_eligibity_option(&arguments);
				break;
				
			case ssh_cmd:
				client_ssh_option(&arguments);
				break;
			
			case monitor_option:
				client_monitor_option(&arguments);
				break;
				
			case print_option:
				client_print_option(&arguments);
				break;
				
			case quorum_option:
				client_quorum_option(&arguments);
				break;
				
			default:
				printf("unrecognized corosync-client command\n");
				
		}
		free(arguments.argz);
	}
	if(timed == timed_option){
		printf("Time of client command: ");
		print_time();
	}
}
