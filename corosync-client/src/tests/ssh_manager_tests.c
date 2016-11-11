#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "ssh_manager.h"
#include "client_errors.h"

void test_setup_ssh()
{
}

void test_teardown_ssh()
{
}

MU_TEST(stop_corosync_tests)
{
	char *local_addr;
	int err;
	
	local_addr = "127.0.0.1";
	printf("\nTesting stop_corosync()\n");
	
	/* Test #1: stop corosync at local address */
	printf("stop_corosync() test #1\n");
	//ssh corosync stop to local address: 127.0.0.1
	err = stop_corosync(local_addr);
	mu_check(err == CS_OK);
	printf("stop_corosync() test #1 success!\n");
	
	printf("stop_corosync() testing success!\n");
}


MU_TEST(start_corosync_tests)
{
	char *local_addr;
	int err;
	
	local_addr = "127.0.0.1";
	printf("\nTesting start_corosync()\n");
	
	/* Test #1: start corosync at local address */
	printf("start_corosync() test #1\n");
	//ssh corosync start to local address: 127.0.0.0
	err = start_corosync(local_addr);
	mu_check(err == CS_OK);
	printf("start_corosync() test #1 success!\n");
	
	printf("start_corosync() testing success!\n");
}

MU_TEST_SUITE(test_suite_ssh) 
{
	MU_SUITE_CONFIGURE(&test_setup_ssh, &test_teardown_ssh);
	printf("\n////////////////////////////////////////////////////");
	printf("\ntesting ssh_manager.c \n");
	printf("////////////////////////////////////////////////////");
	MU_RUN_TEST(stop_corosync_tests);
	MU_RUN_TEST(start_corosync_tests);
	printf("\nssh_manager.c testing completed.\n");
}