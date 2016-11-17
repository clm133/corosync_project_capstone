# Refactored Client Overview
*a lot has changed, this is to help folks readjust 

## new architecture
### the old client's functionality has been broken apart further into the following files:
* client.c is where user input processing occurs, as well as client program output.
* client_errors.c, in conjuction with cs_errors.c, is where the constants for our errors (and corosync's) are defined and where processing those errors .toString() occurs
* ssh_manager.c is where our ssh functionality is defined. These are the functions you call when you want to start corosync on a remote machine, for instance.
* sftp_manager.c is where our sftp functionality resides, that is just one important function that copies a conf file to a remote location.
* conf_manager.c is where we define fuctions that translate current cmap conditions and write them to a conf file
* client_cmapctl.c is where we define functions which abstract away the messier code involved in interacting with the cmap
* cluster_manager.c is where we defined functions which change conditions of the cluster (right now, this is largely adding a node and removing a node). That said, cluster manager just updates the local cmap, and should not be writing conf files or updating conf files on other nodes (leave that to client.c)
* quorum_manager.c is empty at the moment. I see it as a space to perform similar tasks as cluster_manager.c which are more specifically quorum focused.

## helpful functions
### I reccomend checking out the header files to get a sense of the important functions, but these are some I think will be helpful adding more features going forward
* get_error(int err) in client_errors.c will help in returning a string representation of any error that may be returned by any other function in the client. On that note, when creating new functions, I suggest using an int return value to represent any error that may occur and to use corosync's success return value CS_OK to communicate success.
* client_set_cmap_value(const char *key_name, void *value, cmap_value_types_t type) in client_cmapctl.c will set any key/value in the cmap for you (no more initializing handles and all that mess)
* client_get_cmap_value(const char *key_name, void *value, cmap_value_types_t type) will set the value parameter to any (non-string... see below) cmap value stored with the key_name parameter
* client_get_cmap_string_value(const char *key_name, char **value) essentially performs the same function as above, but strings in C are tricky, and the easiest thing to do here was to split get_cmap() functionality accross two functions. Also, remember to free your strings if necessary, corosync always allocates memory at *value so be sure to at least free that when you are done with it.
* you might have noticed client_delete_cmap_value() and client_delete_cmap_string_value() still have a value parameter. in this way, you can the function to return the value you instructed to delete (this may be helpful sometimes). That being said, if you have no need of the deleted value, you can always pass these functions NULL and nothing will be returned or allocated
* nodelist_get_addr_array(char **list) in client_cmapctl.c is a helpful function that allocates an array of node ring0_addresses
* copy_conf(char *src_file, char *dest_file, char *addr) in sftp_manager.c is the function you want to call when updating a conf file at a remote location
* int generate_nodelist_key(char *key_buffer, uint32_t id, char *key_suffix) in cluster_manager.c is a pretty helpful function which will generate a nodelist key for you (e.g. you want to make a nodelist key for key 'nodelist.node.2.ring0_addr' you would call generate_nodelist_key(pointer_to_some_string_buffer, 2, "ring0_addr"))

## testing
### minuinit framework testing has been implemented and it rocks. tests are found in the tests folder in src. 
* to run tests, you must do so from the root directory (where you normally call 'make') and call 'make', then 'make tests' (to compile the test files), and then 'testrun' to run those tests
* for some reason, 'make tests' doesn't always recognize changes to test files, so to recompile tests you will have to 'make' first and then 'make tests'
* right now, some of those tests require access to corosync to work, so be sure to call "sudo make testrun" if you are not already root user

## a note on changing things
* some of the new implementations of things are not optimal, but I really think this is a really large improvement over what existed originally
* feel free to change to / add functions to the existing source files (espcially client.c, cluster_manager.c, and quorum_manger.c). But if you want to change something in client_cmapctl.c, conf_manager.c, ssh_manager.conf, or sftp_manager.c be sure to let everyone know, because those files are the base of what everthing else is built from
* please change tests willy-nilly if you want, the tests I made aren't perfect

