#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"
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
	val = malloc(sizeof(uint32_t));
	str = malloc(sizeof(char *));
	*str = NULL;
}

void test_teardown_cmapctl()
{
	free(val);
	if(*str != NULL){
		free(*str);
	}
	free(str);
}

MU_TEST(set_cmap_value_tests)
{
	int err;
	
	printf("\ntesting set_cmap_value()\n");
	//we are going to set a uint32 key/value and a string value
	err = set_cmap_value(fake_uint32_key, &fake_uint32_val, CMAP_VALUETYPE_UINT32);
	if(err != CS_OK){
		printf("Received error: %s\n", get_error(err));
	}
	mu_check(err == CS_OK);
	err = set_cmap_value(fake_str_key, fake_str_val, CMAP_VALUETYPE_STRING);
	if(err != CS_OK){
		printf("Received error: %s\n", get_error(err));
	}
	mu_check(err == CS_OK);
	printf("set_cmap_value() testing success!\n");
}

MU_TEST(get_cmap_value_tests)
{
	int err;
	cmap_value_types_t type;
	
	
	printf("\ntesting get_cmap_value()\n");
	//now we need to check if those values and their types were set correctly
	//uint32 tests
	err = get_cmap_value(fake_uint32_key, NULL,&type);
	mu_check(err == CS_OK);
	mu_check(type == CMAP_VALUETYPE_UINT32);
	err = get_cmap_value(fake_uint32_key, val, &type);
	mu_check(err == CS_OK);
	mu_check(*(uint32_t *)val == fake_uint32_val);
	//string tests
	err = get_cmap_value(fake_str_key, NULL, &type);
	mu_check(err == CS_OK);
	mu_check(type == CMAP_VALUETYPE_STRING);
	err = get_cmap_value(fake_str_key, str, &type);
	mu_check(strcmp(*str, fake_str_val) == 0);

	printf("get_cmap_value() testing success!\n");
}

MU_TEST(delete_cmap_value_tests)
{
	int err;
	cmap_value_types_t type;
	
	printf("\ntesting delete_cmap_value()\n");
	
	//uint32 tests
	err = delete_cmap_value(fake_uint32_key, NULL, &type);
	mu_check(err == CS_OK);
	mu_check(type == CMAP_VALUETYPE_UINT32);
	err = get_cmap_value(fake_uint32_key, NULL, &type);
	mu_check(err == CS_ERR_NOT_EXIST);
	//string tests
	err = delete_cmap_value(fake_str_key, str, &type);
	mu_check(err == CS_OK);
	mu_check(type == CMAP_VALUETYPE_STRING);
	mu_check(strcmp(*str, fake_str_val) == 0);
	err = get_cmap_value(fake_str_key, NULL, &type);
	mu_check(err == CS_ERR_NOT_EXIST);
	
	printf("delete_cmap_value() testing success!\n");
}

MU_TEST_SUITE(test_suite_cmapctl)
{
	MU_SUITE_CONFIGURE(&test_setup_cmapctl, &test_teardown_cmapctl);
	printf("\n////////////////////////////////////////////////////\n");
	printf("testing client_cmapctl.c \n");
	printf("////////////////////////////////////////////////////\n");
	MU_RUN_TEST(set_cmap_value_tests);
	MU_RUN_TEST(get_cmap_value_tests);
	MU_RUN_TEST(delete_cmap_value_tests);
	printf("\nclient_cmapctl.c testing completed.\n");
}