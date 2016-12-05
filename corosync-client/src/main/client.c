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
	print_option,
	quorum_option,
	monitor_option,
	ssh_cmd,
} task;


const char *argp_program_bug_address = "charliemietzner@gmail.com";
const char *argp_program_version = "version 12.1.16";

/*client_ssh_command*/
//processes several different kinds of instructions:
//start:	"./ctc -c start <ip-address-of-where-you-want-to-start-node>
//stop:	 	"./ctc -c stop <ip address of where you want to stop a node>" (note this is a graceful stop for corosync)
//kill_conn:	"./ctc -c kill_conn <ip-addr of a node you want to bring down>" (note this is a NOT a graceful stop)
//reset_conf:	"./ctc -c reset_conf <name of a conf file in the conf_templates folder> <ip address of node you want to set conf at>"
void client_ssh_command(char *cmd, char *addr, char *src_file_name)
{
	int err;
	char src_file[128];
	char *dest_file;
	long cmd_start;
		
	//start corosync
	if(strcmp(cmd, "start") == 0){
		err = start_corosync(addr);
		printf("node started\n");
		if(err != CS_OK){
			printf("Failed to start corosync at node %s\n", addr);
		}
	}
	// Stop corosync
	else if(strcmp(cmd, "stop") == 0){
			err = stop_corosync(addr);
			printf("node stopped\n");
		if(err != CS_OK){
			printf("Failed to stop corosync at node %s\n", addr);
		}
	}
	// Reset to a node to a conf file located in the conf_templates folder
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
	//kills the eth1 connection (11.0.0.X) at the specified address (note, once eth1 is down you will need to communicate to this node by its alternative interface (12.0.0.X))
	else if(strcmp(cmd, "kill_conn") == 0){
		printf("killing connection at %s\nTime:", addr);
		print_time();
		err = kill_conn(addr);
	}
	//starts the eth1 connection (11.0.0.X) at the specified address
	else if(strcmp(cmd, "start_conn") == 0){
		printf("starting connection at %s\nTime:", addr);
		print_time();
		err = start_conn(addr);
	}
	// Unrecognized command
	else{
		printf("Command %s not recognized.\n", cmd);
	}
}

/* client_change_votes() */
//processes instructions of the format "./ctc -q set_votes <nodeid, votes>"
void client_change_votes(uint32_t nodeid, uint32_t votes)
{
	int err;
	
	err = set_votes(nodeid, (unsigned int)votes);
	if(err != CS_OK){
		printf("failed to update node %u votes: %s\n", nodeid, get_error(err));
	}
	//success
	printf("node %u had their quorum_votes changed to %u\n", nodeid, votes);

}

/* client_monitor_option() */
// to monitor at a node, use the command: "./ctc -m quorum"
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

/*client_print_option()*/
// to print membership:	"./ctc -p membership"
// to print quorum: "./ctc -p membership"
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

/*client_quorum_option*/
//sorts through quorum option arguments and routes their input appropriately
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
		prev = q_target;
	}
}

/* the client_X_option() functions run through the list of arguments and route client instructions appropriately*/
// see client_ssh_command above for the arguments availible
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
		{"command", 'c', "<ssh-command> <node-ip-addr>", 0, "uses ssh to remotely execute command at target node in the cluster."},
		{"print", 'p', "-p <quorum/membership>", 0, "prints the status of corosync target item"},
		{"monitor", 'm',"-m <quorum>", 0, "begins monitoring locally"},
		{"quorum", 'q', "-q set_votes <nodeid> <votes>", 0, "changes a quorum setting"},
		{0}
	};
	
	struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};
	struct arguments arguments;
	printf("\n");
	if(argp_parse(&argp, argc, argv, 0, 0, &arguments) == 0){
		switch(task){
				
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
	printf("\n");
}
