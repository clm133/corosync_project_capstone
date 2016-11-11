#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "minunit.h"
#include "cs_errors.h"
#include "client_errors.h"
#include "ssh_manager_tests.c"
#include "client_cmapctl_tests.c"
#include "cluster_manager_tests.c"
#include "conf_manager_tests.c"
#include "sftp_manager_tests.c"
#include "quorum_manager_tests.c"

struct timespec half_sec;

void start()
{
	const char *start = "corosync";
	
	printf("Restarting corosync\n");
	system(start); //corosync needs to start up again after ssh_testing.
	nanosleep(&half_sec, NULL); // but minunit tests a bit too fast for corosync to keep up, so we sleep a second before continuing
}

void stop()
{
	const char *end = "corosync-cfgtool -H";
	
	nanosleep(&half_sec, NULL); // but minunit tests a bit too fast for corosync to keep up, so we sleep a second before continuing
	system(end); //same timing issue as before... need to give corosync time to catch up
}

void reboot()
{
	stop();
	start();
}

int main(int argc, char *argv[])
{
	//corosync and ssh is a bit slower than minunit tests... so we need to sleep sometimes
	half_sec.tv_nsec = 500000000;
	
	//copy a default conf file to /etc/corosync/corosync.conf
	system("cp conf_templates/default.conf /etc/corosync/corosync.conf");
	//starts corosync
	start();
	//run tests
	MU_RUN_SUITE(test_suite_ssh);
	reboot();  // I reboot corosync between tests in case any cmap key/values linger after tests
	MU_RUN_SUITE(test_suite_cmapctl);
	reboot();
	MU_RUN_SUITE(test_suite_cluster_manager);
	reboot();
	MU_RUN_SUITE(test_suite_conf_manager);
	reboot();
	MU_RUN_SUITE(test_suite_sftp_manager);
	reboot();
	MU_RUN_SUITE(test_suite_quorum_manager);
	//stop corosync before report
	stop(); 
	system("cp conf_templates/default.conf /etc/corosync/corosync.conf");
	printf("\n////////////////////////////////////////////////////");
	printf("\nReport\n");
	printf("////////////////////////////////////////////////////");
	MU_REPORT();
	return 0;
}