#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H

#include <string.h>

#include "client_errors.h"
#include "cs_errors.h"
#include "client_cmapctl.h"
#include "conf_manager.h"

#define MAX_NODES_SUPPORTED 32 //this is totally arbitrary, but I just don't see this project scaling anywhere near this number, much less pass it.

/* mostly a helper functions for cluster_manager, but conf_manager also uses this*/
int generate_nodelist_key(char *key_buffer, uint32_t id, char *key_suffix);
/* get_id_from_addr() - sets id parameter to node id associated with provided address parameter addr */ 
int get_id_from_addr(int nodelist_size, uint32_t *id, char *addr);
/*add_node() - adds node at address addr */
int add_node(char *addr);
/*remove_node() - removes node at address addr */
int delete_node(char *addr);
/*set_node_votes() - sets the quorum-votes of node with id to the votes parameter*/
int set_node_votes(uint32_t id, uint32_t votes);
/*mark_eligible() - sets the node id == e_id eligible by setting quorum votes to 2*/
int mark_eligible(uint32_t *e_id);
/*mark_ineligible() - sets the node id == e_id ineligible by setting quorum votes to 0*/
int mark_ineligible(uint32_t *e_id);
/* move_epsilon() - moves epsilon from node ex_id to node e_id */
int move_epsilon(uint32_t e_id, uint32_t ex_id);
/* set_epsilon() - sets epsilon to node with node id == e_id. moving is 1 if called from move func*/
int set_epsilon(uint32_t e_id, int moving);
/* remove_epsilon() - removes epsilon from node where is_epsilon == "yes" */
int remove_epsilon(uint32_t e_id);
/* is_espsilon_set_on_any_node() - get if epsilon is set on a node and if it is, return the node id as well*/
int is_epsilon_set_on_any_node(int *is_set, uint32_t *e_id);

#endif /* CLUSTER_MANAGER_H */
