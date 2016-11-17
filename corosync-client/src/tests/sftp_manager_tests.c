#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "minunit.h"
#include "conf_manager.h"
#include "cluster_manager.h"
#include "sftp_manager.h"

void test_setup_sftp()
{
	add_node("127.0.0.1");
	add_node("11.0.0.2");
	add_node("11.0.0.3");
}

void test_teardown_sftp()
{
	delete_node("127.0.0.1");
	delete_node("11.0.0.2");
	delete_node("11.0.0.3");
	system("rm test.conf");
}

MU_TEST(copy_conf_tests)
{
	char *local_addr;
	char *src_file_path;
	char *dir_file_path;
	int total;
	int err;
	
	printf("\ntesting copy_conf()\n");
	
	/* Test #1: copy_conf() at local address */
	printf("copy_conf() test #1\n");
	src_file_path = "test.conf";
	err = write_conf(src_file_path);
	mu_check(err == CS_OK);
	local_addr = "127.0.0.1";
	dir_file_path =  "/etc/corosync/corosync.conf";
	err = copy_conf(local_addr, src_file_path, dir_file_path);
	mu_check(err == CS_OK);
	//restart corosync and make sure changes are in effect
	printf("please wait while corosync reboots for conf changes...\n");
	system("corosync-cfgtool -H"); //corosync needs to start up again after ssh_testing.
	sleep(1); // but minunit tests a bit too fast for corosync to keep up, so we sleep a second before continuing
	system("corosync");
	sleep(1);
	err = nodelist_get_total(&total);
	mu_check(err == CS_OK);
	mu_check(total == 3);
	src_file_path = "conf_templates/default.conf";
	err = copy_conf(local_addr, src_file_path, dir_file_path);
	mu_check(err == CS_OK);
	printf("copy_conf() test #1 success!\n");
	
	printf("copy_conf() testing success!\n");
}



MU_TEST_SUITE(test_suite_sftp_manager) 
{
	MU_SUITE_CONFIGURE(&test_setup_sftp, &test_teardown_sftp);
	printf("\n////////////////////////////////////////////////////");
	printf("\ntesting sftp_manager.c \n");
	printf("////////////////////////////////////////////////////");
	MU_RUN_TEST(copy_conf_tests);
	printf("\nsftp_manager.c testing completed.\n");
}