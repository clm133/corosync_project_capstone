#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "cs_errors.h"
#include "client_errors.h"
#include "client_cmapctl.h"
#include "cluster_manager.h"

//these are values we malloc and need to free on test teardown
void *cm_val;
char **cm_str;

void test_setup_cluster_manager()
{

}

void test_teardown_cluster_manager()
{
	if(cm_val != NULL){
		free(cm_val);
		cm_val = NULL;
	}
	if(cm_str != NULL){
		if(*cm_str != NULL){
			free(*cm_str);
			*cm_str = NULL;
		}
		free(cm_str);
		cm_str = NULL;
	}
}

MU_TEST(add_node_tests)
{
	const char *node3_nodeid_key = "nodelist.node.2.nodeid";
	const char *node3_addr_key = "nodelist.node.2.ring0_addr";
	char *fake_addr1 = "11.0.0.1";
	char *fake_addr2 = "11.0.0.2";
	char *fake_addr3 = "11.0.0.3";
	
	int err;
	
	printf("\ntesting add_node()\n");
	/* add_node() test #1 */
	printf("add_node() test #1\n");
	//we are going to start by adding 3 nodes to cluster
	//node1
	err = add_node(fake_addr1);
	if(err != CS_OK){
		printf("Got err %s\n", get_error(err));
		if(err < 0){
			printf("Got client error: %s\n", get_error(err));
		}
		else{
			printf("Got corosync error: %s\n", get_error(err));
		}
	}
	//check that add_node returned successfully
	//node2
	mu_check(err == CS_OK);
	err = add_node(fake_addr2);
	if(err != CS_OK){
		printf("Got err %s\n", get_error(err));
		if(err < 0){
			printf("Got client error: %s\n", get_error(err));
		}
		else{
			printf("Got corosync error: %s\n", get_error(err));
		}
	}
	//check that add_node returned successfully
	mu_check(err == CS_OK);
	//node3
	err = add_node(fake_addr3);
	if(err != CS_OK){
		printf("Got err %s\n", get_error(err));
		if(err < 0){
			printf("Got client error: %s\n", get_error(err));
		}
		else{
			printf("Got corosync error: %s\n", get_error(err));
		}
	}
	//check that add_node returned successfully
	mu_check(err == CS_OK);
	
	//check that the added node nodelist keys hold correct values (we are just going to check node 3 for now)
	cm_val = malloc(sizeof(uint32_t));
	err = client_get_cmap_value(node3_nodeid_key, cm_val, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		if(err < 0){
			printf("Got client error: %s\n", get_error(err));
		}
		else{
			printf("Got corosync error: %s\n", get_error(err));
		}
	}
	mu_check(err == CS_OK);
	if(*(uint32_t *)cm_val != 2){
		printf("Found value: %u\n", *(uint32_t *)cm_val);
	}
	//this is the third node added (none removed), so it should have an ID = 2
	mu_check(*(uint32_t *)cm_val == 2);
	//finally, we want to check ring0_addr
	cm_str = malloc(sizeof(char *));
	err = client_get_cmap_string_value(node3_addr_key, cm_str);
	if(err != CS_OK){
		if(err < 0){
			printf("Got client error: %s\n", get_error(err));
		}
		else{
			printf("Got corosync error: %s\n", get_error(err));
		}
	}
	mu_check(err == CS_OK);
	if(strcmp(*cm_str, fake_addr3) != 0){
		printf("Found value: %s\n", *cm_str);
	}
	mu_check(strcmp(*cm_str, fake_addr3) == 0);
	printf("add_node() test #1 success!\n");
	
	printf("add_node() testing success!\n");
}

MU_TEST(delete_node_tests)
{
	const char *nodeid_key = "nodelist.node.2.nodeid";
	const char *addr_key = "nodelist.node.2.ring0_addr";
	char *fake_addr = "11.0.0.2";
	int err;
	
	printf("\ntesting delete_node()\n");
	/* add_node() test #1 */
	printf("delete_node() test #1\n");
	cm_str = malloc(sizeof(char *));
	//first lets check and make sure node 2 is there
	err = client_get_cmap_string_value(addr_key, cm_str);
	mu_check(err == CS_OK);
	//we want to delete node 2 at address 11.0.0.2
	err = delete_node(*cm_str);
	mu_check(err == CS_OK);
	//lets make sure node 2 is no longer there
	err = client_get_cmap_string_value(addr_key, NULL);
	mu_check(err == CS_ERR_NOT_EXIST);

	printf("delete_node() test #1 success!\n");
	
	printf("delete_node() testing success!\n");
	
}

MU_TEST(one_last_test)
{
	const char *nodeid_key = "nodelist.node.2.nodeid";
	const char *addr_key = "nodelist.node.2.ring0_addr";
	char *fake_addr = "11.0.0.2";
	int err;
	uint32_t id;
	
	//I just want to make sure adding a new node will use the node id vacated by the previous test
	err = add_node(fake_addr);
	mu_check(err == CS_OK);
	err = client_get_cmap_value(nodeid_key, &id, CMAP_VALUETYPE_UINT32);
	mu_check(err == CS_OK);
	mu_check(id == 2);
}

MU_TEST_SUITE(test_suite_cluster_manager)
{
	MU_SUITE_CONFIGURE(&test_setup_cluster_manager, &test_teardown_cluster_manager);
	printf("\n////////////////////////////////////////////////////");
	printf("\ntesting cluster_manager.c \n");
	printf("////////////////////////////////////////////////////");
	MU_RUN_TEST(add_node_tests);
	MU_RUN_TEST(delete_node_tests);
	MU_RUN_TEST(one_last_test);
	printf("cluster_manager.c testing completed.\n");
}