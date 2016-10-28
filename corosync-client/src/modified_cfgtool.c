#include "modified_cfgtool.h"

int get_addr(int nodeid, char *addr)
{
	cs_error_t result;
	corosync_cfg_handle_t handle;
	corosync_cfg_callbacks_t callbacks;
	int numaddrs;
	int i;
	corosync_cfg_node_address_t addrs[INTERFACE_MAX];


	result = corosync_cfg_initialize (&handle, &callbacks);
	if (result != CS_OK) {
		printf ("Could not initialize corosync configuration API error %d\n", result);
		return -1;
	}

	if (corosync_cfg_get_node_addrs(handle, nodeid, INTERFACE_MAX, &numaddrs, addrs) == CS_OK) {
		for (i=0; i<numaddrs; i++) {
			char buf[INET6_ADDRSTRLEN];
			struct sockaddr_storage *ss = (struct sockaddr_storage *)addrs[i].address;
			struct sockaddr_in *sin = (struct sockaddr_in *)addrs[i].address;
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)addrs[i].address;
			void *saddr;

			if (ss->ss_family == AF_INET6)
				saddr = &sin6->sin6_addr;
			else
				saddr = &sin->sin_addr;

			inet_ntop(ss->ss_family, saddr, buf, sizeof(buf));
			strcpy(addr, buf);
			return 1;
		}
	}
	(void)corosync_cfg_finalize (handle);
}