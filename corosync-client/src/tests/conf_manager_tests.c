#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "cluster_manager.h"
#include "conf_manager.h"

void test_setup_conf()
{
	delete_node("127.0.0.1");
	add_node("11.0.0.1");
	add_node("11.0.0.2");
	add_node("11.0.0.3");
}

void test_teardown_conf()
{
	delete_node("11.0.0.1");
	delete_node("11.0.0.2");
	delete_node("11.0.0.3");
	system("rm test.conf");
}

MU_TEST(create_conf_line_tests)
{
	int err;
	char *local_addr;
	char *value_name;
	char *uint32_key = "nodelist.node.0.nodeid";
	char *str_key = "nodelist.node.0.ring0_addr";
	char line[256];
	
	printf("\ntesting create_conf_line() \n");
	
	/* Test #1: write a line with a uint32_t value */
	printf("create_conf_line() test #1\n");
	value_name = "nodeid";
	err = create_conf_line(uint32_key, value_name, line);
	mu_check(err == CS_OK);
	err = strcmp(line, "nodeid: 0");
	mu_check(err == 0);
	printf("create_conf_line() test #1 success!\n");
	
	
	/* Test #1: write a line with a string value */
	printf("create_conf_line() test #2\n");
	value_name = "ring0_addr";
	err = create_conf_line(str_key, value_name, line);
	mu_check(err == CS_OK);
	err = strcmp(line, "ring0_addr: 11.0.0.1");
	mu_check(err == 0);
	printf("create_conf_line() test #2 success!\n");
	
	printf("create_conf_line() testing success!\n");
}

MU_TEST(write_conf_tests)
{
	int err;
	
	printf("\ntesting write_conf() \n");
	
	/* Test #1: write a test conf in the bin folder */
	printf("write_conf() test #1\n");
	err = write_conf("test.conf");
	mu_check(err == CS_OK);
	printf("write_conf() test #1 success!\n");
	
	printf("write_conf() testing success!\n");
}

MU_TEST_SUITE(test_suite_conf_manager) 
{
	MU_SUITE_CONFIGURE(&test_setup_conf, &test_teardown_conf);
	printf("\n////////////////////////////////////////////////////");
	printf("\ntesting conf_manager.c \n");
	printf("////////////////////////////////////////////////////");
	MU_RUN_TEST(create_conf_line_tests);
	MU_RUN_TEST(write_conf_tests);
	printf("\nconf_manager.c testing completed.\n");
}