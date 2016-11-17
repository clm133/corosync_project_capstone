#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "cs_errors.h"
#include "client_errors.h"
#include "client_cmapctl.h"

const char *fake_uint32_key = "fake.fake_uint32";
uint32_t fake_uint32_val = 2016;
const char *fake_str_key = "fake.fake_str";
char *fake_str_val = "this is a fake string!";

void *val;
char **str;

void test_setup_cmapctl()
{
	
}

void test_teardown_cmapctl()
{
	if(val != NULL){
		free(val);
		val = NULL;
	}
	
	if(str != NULL){
		if(*str != NULL){
			free(*str);
			*str = NULL;
		}
		free(str);
		str = NULL;
	}
}

MU_TEST(set_key_value_tests)
{
	int err;
	
	printf("\nTesting client_set_cmap_value()\n");
	
/* Test #1: set a key with value type uint32 */
	err = client_set_cmap_value(fake_uint32_key, &fake_uint32_val, CMAP_VALUETYPE_UINT32);
	mu_check(err == CS_OK);
	printf("client_set_cmap_value() test #1 success!\n");

/* Test #2: set a key with value type string */
	err = client_set_cmap_value(fake_str_key, fake_str_val, CMAP_VALUETYPE_STRING);
	mu_check(err == CS_OK);
	printf("client_set_cmap_value() test #2 success!\n");
	
	printf("client_set_cmap_value() testing success!\n");
}

MU_TEST(get_key_type)
{
	int err;
	size_t len;
	cmap_value_types_t type;
	printf("\ntesting client_get_cmap_key_type()\n");
	
/* Test #1: get a key type of uint32 */
	err = client_get_cmap_key_type(fake_uint32_key, &type, &len);
	//check if client_get_cmap_key_type() returns success
	mu_check(err == CS_OK);
	//check if val passed to client_get_key() returns with same value set in the test above (2016)
	mu_check(type == CMAP_VALUETYPE_UINT32);
	mu_check(len == sizeof(uint32_t));
	printf("client_get_cmap_key_type() test #1 a success!\n");

/* Test #1: get a key type of string */
	err = client_get_cmap_key_type(fake_str_key, &type, &len);
	//check if client_get_cmap_key_type() returns success
	mu_check(err == CS_OK);
	//check if val passed to client_get_key() returns with same value set in the test above (2016)
	mu_check(type == CMAP_VALUETYPE_STRING);
	printf("client_get_cmap_key_type() test #2 a success!\n");
	
	printf("client_get_cmap_key_type() testing success!\n");
}


MU_TEST(get_key_value_tests)
{
	int err;
	
	printf("\ntesting client_get_cmap_value()\n");
	
/* Test #1: get a key with uint32 value  */
	//malloc room for value
	val = malloc(sizeof(uint32_t));
	err = client_get_cmap_value(fake_uint32_key, val, CMAP_VALUETYPE_UINT32);
	//check if client_get_key() returns success
	mu_check(err == CS_OK);
	//check if val passed to client_get_key() returns with same value set in the test above (2016)
	mu_check(*(uint32_t *)val == 2016);
	printf("client_get_cmap_value() test #1 a success!\n");
	
	printf("client_get_cmap_value() testing success!\n");
}

MU_TEST(get_key_string_value_tests)
{
	int err;
	
	printf("\ntesting client_get_cmap_string_value()\n");
	
/* Test #1: get a key with string value */
	//malloc room for value
	str = malloc(sizeof(char *));
	err = client_get_cmap_string_value(fake_str_key, str);
	//make sure to free val in the event of failure
	if(err != CS_OK){
		free(val);
	}
	//check for successful return value
	mu_check(err = CS_OK);
	if(strcmp(fake_str_val, *str) != 0){
		printf("Found wrong value: %s\n", *str);
	}
	//check if returned val parameter matches the fake_str_val set above ('this is a fake string!')
	mu_check(strcmp(fake_str_val, *str) == 0);
	printf("client_get_cmap_string_value() test #1 a success!\n");
	
	printf("client_get_cmap_string_value() testing success!\n");
}

MU_TEST(delete_key_value_tests)
{
	int err;
	void *val;
	
	printf("\ntesting client_delete_cmap_value()\n");

/* Test #1: deletes a key with a uint32 value */
	//malloc room for value
	val = malloc(sizeof(uint32_t));
	err = client_delete_cmap_value(fake_uint32_key, val, CMAP_VALUETYPE_UINT32);
	//check if client_delete_cmap_value() returns success
	mu_check(err == CS_OK);
	//check if val passed to client_delete_cmap_value() returns with same value set in the test above (2016)
	mu_check(*(uint32_t *)val == 2016);
	err = client_get_cmap_value(fake_uint32_key, val, CMAP_VALUETYPE_UINT32);
	//confirm key deletion with client_get_value (which should return cs_err # 12)
	mu_check(err == CS_ERR_NOT_EXIST);
	printf("client_delete_cmap_value() test #1 success!\n");

	printf("client_delete_cmap_value() testing success!\n");
}

MU_TEST(delete_key_string_value_tests)
{
	int err;
	
	printf("\ntesting client_delete_cmap_string_value()\n");
	
/* Test #1: deletes a key with a string value */
	//malloc room for value
	str = malloc(sizeof(char *));
	err = client_delete_cmap_string_value(fake_str_key, str);
	//check if client_delete_cmap_value() returns success
	mu_check(err == CS_OK);
	if(strcmp(fake_str_val, *str) != 0){
		printf("Found wrong value: %s\n", *str);
	}
	//check if returned val parameter matches the fake_str_val set above ('this is a fake string!')
	mu_check(strcmp(fake_str_val, *str) == 0);
	err = client_get_cmap_string_value(fake_str_val, str);
	//confirm key deletion with client_get_value (which should return cs_err # 12)
	mu_check(err == CS_ERR_NOT_EXIST);
	printf("client_delete_cmap_string_value() test #1 success!\n");

	printf("client_delete_cmap_string_value() testing success!\n");
}

MU_TEST(nodelist_get_total_tests)
{
	int err;
	int total;
	const char *node_1_id = "nodelist.node.1.nodeid";
	unsigned int id1 = 1;
	const char *node_2_id = "nodelist.node.2.nodeid";
	unsigned int id2 = 2;
	const char *node_1_addr = "nodelist.node.1.ring0_addr";
	char *addr1 = "11.0.0.1";
	const char *node_2_addr = "nodelist.node.2.ring0_addr";
	char *addr2 = "11.0.0.2";
	
	printf("\ntesting nodelist_get_total()\n");
	
/* Test #1: counts number of nodes in nodelist */
	//set nodelist up
	err = client_set_cmap_value(node_1_id, &id1, CMAP_VALUETYPE_UINT32);
	mu_check(err == CS_OK);
	err = client_set_cmap_value(node_2_id, &id2, CMAP_VALUETYPE_UINT32);
	mu_check(err == CS_OK);
	err = client_set_cmap_value(node_1_addr, addr1, CMAP_VALUETYPE_STRING);
	mu_check(err == CS_OK);
	err = client_set_cmap_value(node_2_addr, addr2, CMAP_VALUETYPE_STRING);
	mu_check(err == CS_OK);
	//count nodelist
	err = nodelist_get_total(&total);
	//make sure nodelist_get_total() returns success value
	mu_check(err == CS_OK);
	//make sure nodelist_get_total() sets parameter to the total of nodelist nodes
	mu_check(total == 2);
	printf("nodelist_get_total() testing success!\n");
}

MU_TEST(nodelist_get_id_array_tests)
{
	int err;
	int total;
	int i;
	int correct;
	uint32_t **node_id_list;
	const char *node_1_id_key = "nodelist.node.1.nodeid";
	unsigned int id1 = 1;
	const char *node_2_id_key = "nodelist.node.2.nodeid";
	unsigned int id2 = 2;
	
	printf("\ntesting nodelist_get_id_array()\n");
	
/* Test #1: returns a list of nodeid's */
	//get a count of the nodelist for malloc
	err = nodelist_get_total(&total);
	//make sure nodelist_get_total() returns success value
	mu_check(err == CS_OK);
	//make sure nodelist_get_total() sets parameter to the total of nodelist nodes (there should be 2 from the values set above)
	mu_check(total == 2);
	//malloc a node id list with a size = the number returned from nodelist_get_total()
	node_id_list = malloc(sizeof(uint32_t *)*total);
	err = nodelist_get_id_array(node_id_list);
	//free our malloc in case something goes wrong
	if(err != CS_OK){
		printf("got a cs_error %s\n", get_error(err));
		free(node_id_list);
	}
	//check to make sure nodelist_get_id_array() returns success value
	mu_check(err == CS_OK);
	//make sure each nodeid in the nodelist array returned is correct (should be 1 and 2)
	correct = 0;
	for(i = 0; i < total; i++){
		if(*node_id_list[i] == (i + 1)){
			correct++;
		}
		else{
			printf("Was supposed to get %u, but got %u instead\n", (i + 1), *node_id_list[i]);
		}
		free(node_id_list[i]);
	}
	free(node_id_list);
	mu_check(correct == 2);
	printf("nodelist_get_id_array() test #1 was a success!\n");
	
	printf("nodelist_get_id_array() testing success!\n");
}

MU_TEST(nodelist_get_addr_array_tests)
{
	char *addr1 = "11.0.0.1";
	char *addr2 = "11.0.0.2";
	
	int err;
	int total;
	int i;
	int correct;
	char **node_addr_list;
	
	printf("\ntesting nodelist_get_addr_array()\n");
	
/* Test #1: returns a list of node addrs */
	//get a count of the nodelist for malloc
	err = nodelist_get_total(&total);
	//make sure nodelist_get_total() returns success value
	mu_check(err == CS_OK);
	//make sure nodelist_get_total() sets parameter to the total of nodelist nodes (there should be 2 from the values set above)
	mu_check(total == 2);
	//malloc a node id list with a size = the number returned from nodelist_get_total()
	node_addr_list = malloc(sizeof(char *)*total);
	err = nodelist_get_addr_array(node_addr_list);
	//free our malloc in case something goes wrong
	if(err != CS_OK){
		printf("got a cs_error %s\n", get_error(err));
		free(node_addr_list);
	}
	//check to make sure nodelist_get_addr_array() returns success value
	mu_check(err == CS_OK);
	//make sure each node addr in the nodelist array returned is correct (should be '11.0.0.1' and '11.0.0.2')
	correct = 0;
	if(strcmp(node_addr_list[0], addr1) == 0){
		correct++;
	}
	free(node_addr_list[0]);
	if(strcmp(node_addr_list[1], addr2) == 0){
		correct++;
	}
	free(node_addr_list[1]);
	free(node_addr_list);
	mu_check(correct == 2);
	printf("nodelist_get_addr_array() test #1 was a success!\n");
	
	printf("nodelist_get_addr_array() testing success!\n");
}

MU_TEST_SUITE(test_suite_cmapctl)
{
	MU_SUITE_CONFIGURE(&test_setup_cmapctl, &test_teardown_cmapctl);
	printf("\n////////////////////////////////////////////////////");
	printf("\ntesting client_cmapctl.c \n");
	printf("////////////////////////////////////////////////////");
	MU_RUN_TEST(set_key_value_tests); //sets 2 key=value: 'fake_key.fake_string'='this is a test' and 'fake_key.fake_uint32'=2016
	MU_RUN_TEST(get_key_type);
	MU_RUN_TEST(get_key_value_tests); //gets the value of 'fake_key.fake_uint32' and confirms  it is the same as the value set above
	MU_RUN_TEST(get_key_string_value_tests);  //gets the value of 'fake_key.fake_string' and confirms  it is the same as the value set above
	MU_RUN_TEST(delete_key_value_tests); //deletes the key/value 'fake_key.fake_uint32' and confirms deletion
	MU_RUN_TEST(delete_key_string_value_tests); //deletes the key/value of 'fake_key.fake_string' and confirms deletion
	MU_RUN_TEST(nodelist_get_total_tests);
	MU_RUN_TEST(nodelist_get_id_array_tests);
	MU_RUN_TEST(nodelist_get_addr_array_tests);
	printf("client_cmapctl.c testing completed.\n");
}