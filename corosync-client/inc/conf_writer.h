#ifndef CONF_WRITER_H_DEFINED
#define CONF_WITER_H_DEFINED
#include <stdio.h>
#include <stdlib.h>
#include <corosync/corotypes.h>

cs_error_t write_config(const char *file_name);

#endif /* CONF_WRITER_H */