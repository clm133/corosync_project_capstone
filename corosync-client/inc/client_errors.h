/* client_errors is currently only concerned with
* translating corosync error codes. perhaps it might be expanded
* to include our own clients errors as well.
*/

#ifndef CLIENT_ERRORS_H_DEFINED
#define CLIENT_ERRORS_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <corosync/cmap.h>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>

const char *get_error(cs_error_t err);

#endif /* CLIENT_ERRORS_H */