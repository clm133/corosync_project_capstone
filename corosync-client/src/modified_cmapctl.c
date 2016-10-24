/*
 * Copyright (c) 2011-2012 Red Hat, Inc.
 *
 * All rights reserved.
 *
 * Author: Jan Friesse (jfriesse@redhat.com)
 *
 * This software licensed under BSD license, the text of which follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Red Hat, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <ctype.h>
#include <stdio.h>
#include <poll.h>

#include "modified_cmapctl.h"
#include "client_errors.h"


#ifndef INFTIM
#define INFTIM -1
#endif

#define MAX_TRY_AGAIN 10

struct name_to_type_item {
	const char *name;
	cmap_value_types_t type;
};

struct name_to_type_item name_to_type[] = {
	{"i8", CMAP_VALUETYPE_INT8},
	{"u8", CMAP_VALUETYPE_UINT8},
	{"i16", CMAP_VALUETYPE_INT16},
	{"u16", CMAP_VALUETYPE_UINT16},
	{"i32", CMAP_VALUETYPE_INT32},
	{"u32", CMAP_VALUETYPE_UINT32},
	{"i64", CMAP_VALUETYPE_INT64},
	{"u64", CMAP_VALUETYPE_UINT64},
	{"flt", CMAP_VALUETYPE_FLOAT},
	{"dbl", CMAP_VALUETYPE_DOUBLE},
	{"str", CMAP_VALUETYPE_STRING},
	{"bin", CMAP_VALUETYPE_BINARY}};

int convert_name_to_type(const char *name)
{
	int i;

	for (i = 0; i < sizeof(name_to_type) / sizeof(*name_to_type); i++) {
		if (strcmp(name, name_to_type[i].name) == 0) {
			return (name_to_type[i].type);
		}
	}

	return (-1);
}

void print_key(cmap_handle_t handle,const char *key_name,size_t value_len,const void *value,cmap_value_types_t type)
{
	char *str;
	char *bin_value = NULL;
	cs_error_t err;
	int8_t i8;
	uint8_t u8;
	int16_t i16;
	uint16_t u16;
	int32_t i32;
	uint32_t u32;
	int64_t i64;
	uint64_t u64;
	float flt;
	double dbl;
	int end_loop;
	int no_retries;
	size_t bin_value_len;

	end_loop = 0;
	no_retries = 0;

	err = CS_OK;

	while (!end_loop) {
		switch (type) {
		case CMAP_VALUETYPE_INT8:
			if (value == NULL) {
				err = cmap_get_int8(handle, key_name, &i8);
			} else {
				i8 = *((int8_t *)value);
			}
			break;
		case CMAP_VALUETYPE_INT16:
			if (value == NULL) {
				err = cmap_get_int16(handle, key_name, &i16);
			} else {
				i16 = *((int16_t *)value);
			}
			break;
		case CMAP_VALUETYPE_INT32:
			if (value == NULL) {
				err = cmap_get_int32(handle, key_name, &i32);
			} else {
				i32 = *((int32_t *)value);
			}
			break;
		case CMAP_VALUETYPE_INT64:
			if (value == NULL) {
				err = cmap_get_int64(handle, key_name, &i64);
			} else {
				i64 = *((int64_t *)value);
			}
			break;
		case CMAP_VALUETYPE_UINT8:
			if (value == NULL) {
				err = cmap_get_uint8(handle, key_name, &u8);
			} else {
				u8 = *((uint8_t *)value);
			}
			break;
		case CMAP_VALUETYPE_UINT16:
			if (value == NULL) {
				err = cmap_get_uint16(handle, key_name, &u16);
			} else {
				u16 = *((uint16_t *)value);
			}
			break;
		case CMAP_VALUETYPE_UINT32:
			if (value == NULL) {
				err = cmap_get_uint32(handle, key_name, &u32);
			} else {
				u32 = *((uint32_t *)value);
			}
			break;
		case CMAP_VALUETYPE_UINT64:
			if (value == NULL) {
				err = cmap_get_uint64(handle, key_name, &u64);
			} else {
				u64 = *((uint64_t *)value);
			}
			break;
		case CMAP_VALUETYPE_FLOAT:
			if (value == NULL) {
				err = cmap_get_float(handle, key_name, &flt);
			} else {
				flt = *((float *)value);
			}
			break;
		case CMAP_VALUETYPE_DOUBLE:
			if (value == NULL) {
				err = cmap_get_double(handle, key_name, &dbl);
			} else {
				dbl = *((double *)value);
			}
			break;
		case CMAP_VALUETYPE_STRING:
			if (value == NULL) {
				err = cmap_get_string(handle, key_name, &str);
			} else {
				str = (char *)value;
			}
			break;

		if (err == CS_OK) {
			end_loop = 1;
		} else if (err == CS_ERR_TRY_AGAIN) {
			sleep(1);
			no_retries++;

			if (no_retries > MAX_TRY_AGAIN) {
				end_loop = 1;
			}
		} else {
			end_loop = 1;
		}
	};

	if (err != CS_OK) {
		fprintf(stderr, "Can't get value of %s. Error %s\n", key_name, cs_strerror(err));

		return ;
	}

	printf("%s (", key_name);

	switch (type) {
	case CMAP_VALUETYPE_INT8:
		printf("%s) = %"PRId8, "i8", i8);
		break;
	case CMAP_VALUETYPE_UINT8:
		printf("%s) = %"PRIu8, "u8", u8);
		break;
	case CMAP_VALUETYPE_INT16:
		printf("%s) = %"PRId16, "i16", i16);
		break;
	case CMAP_VALUETYPE_UINT16:
		printf("%s) = %"PRIu16, "u16", u16);
		break;
	case CMAP_VALUETYPE_INT32:
		printf("%s) = %"PRId32, "i32", i32);
		break;
	case CMAP_VALUETYPE_UINT32:
		printf("%s) = %"PRIu32, "u32", u32);
		break;
	case CMAP_VALUETYPE_INT64:
		printf("%s) = %"PRId64, "i64", i64);
		break;
	case CMAP_VALUETYPE_UINT64:
		printf("%s) = %"PRIu64, "u64", u64);
		break;
	case CMAP_VALUETYPE_FLOAT:
		printf("%s) = %f", "flt", flt);
		break;
	case CMAP_VALUETYPE_DOUBLE:
		printf("%s) = %lf", "dbl", dbl);
		break;
	case CMAP_VALUETYPE_STRING:
		printf("%s) = %s", "str", str);
		if (value == NULL) {
			free(str);
		}
		break;
	}
	}

	printf("\n");
}

void print_iter(cmap_handle_t handle, const char *prefix)
{
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	size_t value_len;
	cmap_value_types_t type;
	cs_error_t err;

	err = cmap_iter_init(handle, prefix, &iter_handle);
	if (err != CS_OK) {
		fprintf (stderr, "Failed to initialize iteration. Error %s\n", cs_strerror(err));
		exit (EXIT_FAILURE);
	}

	while ((err = cmap_iter_next(handle, iter_handle, key_name, &value_len, &type)) == CS_OK) {
		print_key(handle, key_name, value_len, NULL, type);
	}
	cmap_iter_finalize(handle, iter_handle);
}

void delete_with_prefix(cmap_handle_t handle, const char *prefix)
{
	cmap_iter_handle_t iter_handle;
	char key_name[CMAP_KEYNAME_MAXLEN + 1];
	size_t value_len;
	cmap_value_types_t type;
	cs_error_t err;
	cs_error_t err2;

	err = cmap_iter_init(handle, prefix, &iter_handle);
	if (err != CS_OK) {
		fprintf (stderr, "Failed to initialize iteration. Error %s\n", cs_strerror(err));
		exit (EXIT_FAILURE);
	}

	while ((err = cmap_iter_next(handle, iter_handle, key_name, &value_len, &type)) == CS_OK) {
		err2 = cmap_delete(handle, key_name);
		if (err2 != CS_OK) {
			fprintf(stderr, "Can't delete key %s. Error %s\n", key_name, cs_strerror(err2));
		}
	}
	cmap_iter_finalize(handle, iter_handle);
}

void set_key(cmap_handle_t handle, const char *key_name, const char *key_type_s, const char *key_value_s)
{
	int64_t i64;
	uint64_t u64;
	double dbl;
	float flt;
	cs_error_t err = CS_OK;
	int scanf_res = 0;

	cmap_value_types_t type;

	if (convert_name_to_type(key_type_s) == -1) {
		fprintf(stderr, "Unknown type %s\n", key_type_s);
		exit (EXIT_FAILURE);
	}

	type = convert_name_to_type(key_type_s);

	switch (type) {
	case CMAP_VALUETYPE_INT8:
	case CMAP_VALUETYPE_INT16:
	case CMAP_VALUETYPE_INT32:
	case CMAP_VALUETYPE_INT64:
		scanf_res = sscanf(key_value_s, "%"PRId64, &i64);
		break;
	case CMAP_VALUETYPE_UINT8:
	case CMAP_VALUETYPE_UINT16:
	case CMAP_VALUETYPE_UINT32:
	case CMAP_VALUETYPE_UINT64:
		scanf_res = sscanf(key_value_s, "%"PRIu64, &u64);
		break;
	case CMAP_VALUETYPE_FLOAT:
		scanf_res = sscanf(key_value_s, "%f", &flt);
		break;
	case CMAP_VALUETYPE_DOUBLE:
		scanf_res = sscanf(key_value_s, "%lf", &dbl);
		break;
	case CMAP_VALUETYPE_STRING:
	case CMAP_VALUETYPE_BINARY:
		/*
		 * Do nothing
		 */
		scanf_res = 1;
		break;
	}

	if (scanf_res != 1) {
		fprintf(stderr, "%s is not valid %s type value\n", key_value_s, key_type_s);
		exit(EXIT_FAILURE);
	}
	/*
	 * We have parsed value, so insert value
	 */

	switch (type) {
	case CMAP_VALUETYPE_INT8:
		if (i64 > INT8_MAX || i64 < INT8_MIN) {
			fprintf(stderr, "%s is not valid i8 integer\n", key_value_s);
			exit(EXIT_FAILURE);
		}
		err = cmap_set_int8(handle, key_name, i64);
		break;
	case CMAP_VALUETYPE_INT16:
		if (i64 > INT16_MAX || i64 < INT16_MIN) {
			fprintf(stderr, "%s is not valid i16 integer\n", key_value_s);
			exit(EXIT_FAILURE);
		}
		err = cmap_set_int16(handle, key_name, i64);
		break;
	case CMAP_VALUETYPE_INT32:
		if (i64 > INT32_MAX || i64 < INT32_MIN) {
			fprintf(stderr, "%s is not valid i32 integer\n", key_value_s);
			exit(EXIT_FAILURE);
		}
		err = cmap_set_int32(handle, key_name, i64);
		break;
	case CMAP_VALUETYPE_INT64:
		err = cmap_set_int64(handle, key_name, i64);
		break;

	case CMAP_VALUETYPE_UINT8:
		if (u64 > UINT8_MAX) {
			fprintf(stderr, "%s is not valid u8 integer\n", key_value_s);
			exit(EXIT_FAILURE);
		}
		err = cmap_set_uint8(handle, key_name, u64);
		break;
	case CMAP_VALUETYPE_UINT16:
		if (u64 > UINT16_MAX) {
			fprintf(stderr, "%s is not valid u16 integer\n", key_value_s);
			exit(EXIT_FAILURE);
		}
		err = cmap_set_uint16(handle, key_name, u64);
		break;
	case CMAP_VALUETYPE_UINT32:
		if (u64 > UINT32_MAX) {
			fprintf(stderr, "%s is not valid u32 integer\n", key_value_s);
			exit(EXIT_FAILURE);
		}
		err = cmap_set_uint32(handle, key_name, u64);
		break;
	case CMAP_VALUETYPE_UINT64:
		err = cmap_set_uint64(handle, key_name, u64);
		break;
	case CMAP_VALUETYPE_FLOAT:
		err = cmap_set_float(handle, key_name, flt);
		break;
	case CMAP_VALUETYPE_DOUBLE:
		err = cmap_set_double(handle, key_name, dbl);
		break;
	case CMAP_VALUETYPE_STRING:
		err = cmap_set_string(handle, key_name, key_value_s);
		break;
	}

	if (err != CS_OK) {
		fprintf (stderr, "Failed to set key %s. Error %s\n", key_name, cs_strerror(err));
		exit (EXIT_FAILURE);
	}
}