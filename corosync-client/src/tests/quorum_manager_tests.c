#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "minunit.h"
#include "quorum_manager.h"

void test_setup_quorum()
{
	
}

void test_teardown_quorum()
{
	
}

MU_TEST(set_elegible_tests)
{
	int err;
	
	printf("\ntesting set_elegible()\n");
	
	/* Test #1: set_elegible() */
	err = set_elegible();
	mu_check(err == CS_OK);
	printf("set_elegible() test #1 success!\n");
	
	printf("set_elegible() testing success!\n");
}

MU_TEST(set_inelegible_tests)
{
	int err;
	
	printf("\ntesting set_inelegible()\n");
	
	/* Test #1: set_inelegible() */
	err = set_inelegible();
	mu_check(err == CS_OK);
	printf("set_inelegible() test #1 success!\n");
	
	printf("set_inelegible() testing success!\n");
}



MU_TEST_SUITE(test_suite_quorum_manager) 
{
	MU_SUITE_CONFIGURE(&test_setup_quorum, &test_teardown_quorum);
	printf("\n////////////////////////////////////////////////////");
	printf("\ntesting quorum_manager.c \n");
	printf("////////////////////////////////////////////////////");
	MU_RUN_TEST(set_elegible_tests);
	MU_RUN_TEST(set_inelegible_tests);
	printf("\nquorum_manager.c testing completed.\n");
}