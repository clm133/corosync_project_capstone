#ifndef CS_ERRORS_H_DEFINED
#define CS_ERRORS_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <corosync/cmap.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>

const char *get_cs_error(cs_error_t err);

#endif /* CS_ERRORS_H */