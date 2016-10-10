/*
 This is the file Brian provided us as a reference.
*/


#include <iostream>
#include <corosync/quorum.h>
#include <corosync/votequorum.h>
#include <corosync/cmap.h>
 
#include <pthread.h>
 
using namespace std;
 
 
quorum_handle_t q_h;
bool quorum_connected = false;
int num_q_cb = 0;
quorum_callbacks_t q_cb;
 
votequorum_handle_t vq_h;
bool votequorum_connected = false;
int num_vq_cb = 0;
votequorum_callbacks_t vq_cb;
 
cmap_handle_t cmap_h;
bool cmap_connected = true;
 
uint32_t our_nodeid = 0;
string   cluster_name;
 
string
get_cluster_name() {
    return cluster_name;
}
bool
has_cluster_name() {
    return cluster_name.size() > 0;
}
 
 
void
quorum_notification (
    quorum_handle_t handle,
    uint32_t quorate,
    uint64_t ring_seq,
    uint32_t view_list_entries,
    uint32_t *view_list)
{
    num_q_cb++;
 
    cout << endl
         << "-----------------------------------------------------" << endl
         << "*** QUORUM NOTIFY:"
         << "  " << (quorate? "online" : "offline")
         << "  ring_seq : " << ring_seq
         << "  cb_count : " << num_q_cb
         << " ***"
         << endl;
 
    for(unsigned int i=0; i<view_list_entries; i++) {
      cout << "  Node: " << view_list[i] << endl;
    }
    cout << "-----------------------------------------------------";
 
    cout << endl << (has_cluster_name()? get_cluster_name().c_str() : "") << "::*> ";
    cout.flush();
}
 
 
const char * get_error(int err) {
    switch(err) {
    case CS_OK: return "Success";
    case CS_ERR_LIBRARY: return "Library error";
    case CS_ERR_VERSION: return "Version error";
    case CS_ERR_INIT: return "Initialization error";
    case CS_ERR_TIMEOUT: return "Timeout";
    case CS_ERR_TRY_AGAIN: return "Resource temporarily unavailable";
    case CS_ERR_INVALID_PARAM: return "Invalid argument";
    case CS_ERR_NO_MEMORY: return "Not enough memory to completed the requested task";
    case CS_ERR_BAD_HANDLE: return "Bad handle";
    case CS_ERR_BUSY: return "Busy";
    case CS_ERR_ACCESS: return "Permission denied";
    case CS_ERR_NOT_EXIST: return "Not found";
    case CS_ERR_NAME_TOO_LONG: return "Name is too long";
    case CS_ERR_EXIST: return "Already exists";
    case CS_ERR_NO_SPACE: return "Insufficient memory";
    case CS_ERR_INTERRUPT: return "System call inturrupted by a signal";
    case CS_ERR_NOT_SUPPORTED: return "The requested protocol/functuality not supported";
    case CS_ERR_MESSAGE_ERROR: return "Incorrect auth message received";
    case CS_ERR_NAME_NOT_FOUND: return "Name not found";
    case CS_ERR_NO_RESOURCES: return "No resources";
    case CS_ERR_BAD_OPERATION: return "Bad operation";
    case CS_ERR_FAILED_OPERATION: return "Failed operation";
    case CS_ERR_QUEUE_FULL: return "Queue full";
    case CS_ERR_QUEUE_NOT_AVAILABLE: return "Queue not available";
    case CS_ERR_BAD_FLAGS: return "Bad flags";
    case CS_ERR_TOO_BIG: return "Too big";
    case CS_ERR_NO_SECTIONS: return "No sections";
    case CS_ERR_CONTEXT_NOT_FOUND: return "Context not found";
    case CS_ERR_TOO_MANY_GROUPS: return "Too many groups";
    case CS_ERR_SECURITY: return "Security error";
    default: return "Unknown error";     
    }
}
 
void
track_begin() {
    int ret = quorum_trackstart(q_h, CS_TRACK_CURRENT|CS_TRACK_CHANGES|CS_TRACK_CHANGES_ONLY);
    if (ret == CS_OK) {
        cout << "Quorum tracking started" << endl;
    } else {
        cout << "Quorum tracking initialization failed: " << get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void start_dispatch();
 
void
quorum_init() {
    q_cb.quorum_notify_fn = &quorum_notification;
 
    uint32_t quorum_type;
    int ret = quorum_initialize(&q_h, &q_cb, &quorum_type);
    if (ret == CS_OK) {
        cout << "Quorum initialized: " << ((quorum_type==QUORUM_SET)? "in-quorum" : "no quorum") << endl;
        quorum_connected = true;
        track_begin();
        start_dispatch();
    } else {
        cout << "Quorum initialization failed: " << get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void get_quorate() {
    int qr = 0;
    int ret = quorum_getquorate(q_h, &qr);
    if (ret == CS_OK) {
        cout << "Quorate: " << qr;
    } else {
        cout << "Quorum initialization failed: " << get_error(ret) << " (" << ret << ")";
    }
}
 
void *
dispatch_fn(void *) {
    cs_dispatch_flags_t disp_type = CS_DISPATCH_BLOCKING;
    int ret = quorum_dispatch(q_h, disp_type);
    if (ret == CS_OK) {
#ifdef DEBUG
        cout << "Quorum dispatch finished" << endl;
#endif
    } else {
        cout << "Quorum dispatch failed: " << get_error(ret) << " (" << ret << ")" << endl;
        cout << (has_cluster_name()? get_cluster_name().c_str() : "") << "::*> ";
        cout.flush();
    }
    return NULL;
}
 
void
start_dispatch() {
    pthread_t thr;
    int ret = pthread_create(&thr, NULL, &dispatch_fn, NULL);
    if (ret==0) {
      (void)pthread_detach(thr);
    } else {
        cout << "Failed to create the dispatch thread: " << ret << endl;
    }
}
 
void
quorum_close() {
    if (quorum_connected) {
        int ret = quorum_finalize(q_h);
        if (ret == CS_OK) {
#ifdef DEBUG
            cout << "Quorum finalized. Callbacks fired: " << num_q_cb;
#endif
        } else {
            cout << "Quorum finalization failed: " << ret;
        }
        quorum_connected = false;
    }
}
 
void *
votequorum_dispatch_fn(void *) {
    cs_dispatch_flags_t disp_type = CS_DISPATCH_BLOCKING;
    int ret = votequorum_dispatch(vq_h, disp_type);
    if (ret == CS_OK) {
#ifdef DEBUG
        cout << "Votequorum dispatch finished" << endl;
#endif
    } else {
        cout << "Votequorum dispatch failed: " << get_error(ret) << " (" << ret << ")" << endl;
        cout << (has_cluster_name()? get_cluster_name().c_str() : "") << "::*> ";
        cout.flush();
    }
    return NULL;
}
 
void
votequorum_start_dispatch() {
    pthread_t thr;
    int ret = pthread_create(&thr, NULL, &votequorum_dispatch_fn, NULL);
    if (ret==0) {
      (void)pthread_detach(thr);
    } else {
        cout << "Failed to create the votequorum dispatch thread: " << ret << endl;
    }
}
 
void
votequorum_track_begin() {
    int ret = votequorum_trackstart(vq_h, 0, CS_TRACK_CURRENT|CS_TRACK_CHANGES|CS_TRACK_CHANGES_ONLY);
    if (ret == CS_OK) {
        cout << "Votequorum tracking started" << endl;
    } else {
        cout << "Votequorum tracking initialization failed: " << get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void
votequorum_notification (
    votequorum_handle_t handle,
    uint64_t context,
    uint32_t quorate,
    uint32_t node_list_entries,
    votequorum_node_t node_list[]) {
 
    num_vq_cb++;
 
    cout << endl
         << "-----------------------------------------------------" << endl
         << "*** VOTEQUORUM NOTIFY:"
         << "  " << (quorate==1? "online" : "offline")
         << "  cb_count : " << num_vq_cb
         << " ***"
         << endl;
 
    for(unsigned int i=0; i<node_list_entries; i++) {
      cout << "  Node: " << node_list[i].nodeid
           << "  state: " << (node_list[i].state==1? "online" : "offline") <<  endl;
    }
    cout << "-----------------------------------------------------";
 
    cout << endl << (has_cluster_name()? get_cluster_name().c_str() : "") << "::*> ";
    cout.flush();
}
 
void votequorum_expectedvotes_notification (
    votequorum_handle_t handle,
    uint64_t context,
    uint32_t expected_votes) {
}
 
void
votequorum_init() {
    vq_cb.votequorum_notify_fn = &votequorum_notification;
    vq_cb.votequorum_expectedvotes_notify_fn = votequorum_expectedvotes_notification;
 
    uint32_t quorum_type;
    int ret = votequorum_initialize(&vq_h, &vq_cb);
    if (ret == CS_OK) {
#ifdef DEBUG
        cout << "Votequorum initialized" << endl;
#endif
        votequorum_connected = true;
        votequorum_track_begin();
        votequorum_start_dispatch();
    } else {
        cout << "Voequorum initialization failed: " << get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void
votequorum_close() {
    if (votequorum_connected) {
      (void)votequorum_finalize(vq_h);
      votequorum_connected = false;
    }
}
 
int
has_epsilon( unsigned int node_id, bool &epsilon ) {
    struct votequorum_info info;
    int ret = votequorum_getinfo(vq_h, node_id, &info);
    if (ret == CS_OK) {
        epsilon = (info.node_votes == 2);
        return CS_OK;
    } else {
      return ret;
    }
}
 
void
assign_epsilon( unsigned int node_id ) {
    bool already_epsilon = false;
    int ret = has_epsilon(node_id, already_epsilon);
    if (ret == CS_OK) {
        if (!already_epsilon) {
            ret = votequorum_setvotes(vq_h, node_id, 2);
            if (ret == CS_OK) {
                cout << "Assigned epsilon to node " << node_id << endl;
            } else {
                cout << "Failed to assign epsilon to node " << node_id
                     << ": "<< get_error(ret) << " (" << ret << ")" << endl;
            }
        } else {
            cout << "Node " << node_id << " already has epsilon" << endl;
        }
    } else {
        cout << "Failed to assign epsilon: unable to determine current configuration for node "
             << node_id << ": "<< get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void
remove_epsilon( unsigned int node_id ) {
    bool is_epsilon = false;
    int ret = has_epsilon(node_id,is_epsilon);
    if (ret == CS_OK) {
        if (is_epsilon) {
            ret = votequorum_setvotes(vq_h, node_id, 1);
            if (ret == CS_OK) {
                cout << "Removed epsilon from node " << node_id << endl;
            } else {
                cout << "Failed to remove epsilon from node " << node_id
                     << ": "<< get_error(ret) << " (" << ret << ")" << endl;
            }                
        } else {
            cout << "Node " << node_id << " does not have epsilon" << endl;
        }
    } else {
        cout << "Failed to remove epsilon: unable to determine current status for node "
             << node_id << ": "<< get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void
mark_ineligible( unsigned int node_id ) {
    bool have_epsilon = false;
    int ret = has_epsilon(node_id,have_epsilon);
    if (ret == CS_OK) {
      if (!have_epsilon) {
            struct votequorum_info info;
            ret = votequorum_getinfo(vq_h, node_id, &info);
            if (ret == CS_OK) {
                ret = votequorum_setexpected(vq_h, info.node_expected_votes-1);
            }
            if (ret == CS_OK) {
                ret = votequorum_setvotes(vq_h, node_id, 0);
            }
            if (ret == CS_OK) {
                cout << "Marked ineligible node " << node_id << endl;
            }
        } else {
            cout << "Unable to mark node " << node_id
                 << " ineligible: reassign epsilon first" << endl;
        }
    }
    if (ret != CS_OK) {
        cout << "Failed to mark ineligible node " << node_id
             << ": "<< get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
int
is_eligible( unsigned int node_id, bool &eligible ) {
    struct votequorum_info info;
    int ret = votequorum_getinfo(vq_h, node_id, &info);
    if (ret == CS_OK) {
        eligible = (info.node_votes > 0);
        return CS_OK;
    } else {
      return ret;
    }
}
 
void
mark_eligible( unsigned int node_id ) {
    bool already_eligible = false;
    int ret = is_eligible(node_id,already_eligible);
    if (ret == CS_OK) {
        if (!already_eligible) {
            ret = votequorum_setvotes(vq_h, node_id, 1);
            if (ret == CS_OK) {
                cout << "Marked eligible node " << node_id << endl;
            } else {
                cout << "Failed to mark eligible node " << node_id
                     << ": "<< get_error(ret) << " (" << ret << ")" << endl;
            }
        } else {
            cout << "Node " << node_id << " is already eligible" << endl;
        }
    } else {
        cout << "Failed to mark node eligible: unable to determine current status for node "
             << node_id << ": "<< get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
/*
* Display all the nodes known to this instance of corosync.
*
* Uses cmap to query the list of nodes and their attributes. Alternatively,
* the same can be gathered using quorum callbacks.
* For node online status and active vote count, query the votequorum subsystem.
*/
void
node_show() {
    cmap_iter_handle_t iter_h;
    int ret = cmap_iter_init(cmap_h, "nodelist.node", &iter_h);
    if (ret != CS_OK) {
        cout << "Failed to read nodelist: "<< get_error(ret) << " (" << ret << ")" << endl;
        return;
    }
 
    const unsigned int MAX_NODES = 128;
    struct node_info {
        uint32_t node_id;
        char node_name[128];
    } nodes[MAX_NODES];
    for(uint32_t i=0; i<MAX_NODES; i++) {
        nodes[i].node_id = 0;
        nodes[i].node_name[0] = '\0';
    }
 
    uint32_t num_nodes = 0;
    do {
        char key_name[CMAP_KEYNAME_MAXLEN];
        size_t val_len = 0;
        cmap_value_types_t val_type;
        ret = cmap_iter_next(cmap_h, iter_h, key_name, &val_len, &val_type);
        if (ret != CS_OK) {
            break;
        }
        int node_idx = atoi(key_name+strlen("nodelist.node."));
        if (strcmp(key_name+(strlen(key_name)-strlen("nodeid")), "nodeid") == 0) {
            uint32_t node_id = 0;
            ret = cmap_get_uint32(cmap_h, key_name, &node_id);
            if (ret == CS_OK) {
                num_nodes++;
                nodes[node_idx].node_id = node_id;
            }
        } else if (nodes[node_idx].node_name[0]=='\0' &&
                   (strcmp(key_name+(strlen(key_name)-strlen("name")), "name") == 0 ||
                    strcmp(key_name+(strlen(key_name)-strlen("ring0_addr")), "ring0_addr") == 0)) {
            char *node_name = NULL;
            ret = cmap_get_string(cmap_h, key_name, &node_name);
            if (ret == CS_OK) {
                strncpy(nodes[node_idx].node_name, node_name, 127);
                free(node_name);
            }
        }
    } while(true);
 
    (void)cmap_iter_finalize (cmap_h, iter_h);
 
 
    cout << " Node                 Health  Eligibility   Epsilon     " << endl
         << " -------------------- ------- ------------  ------------" << endl;
        
            //rha1-b1              true    true          true
            //rha1-b2              true    true          false
 
    int in_quorum = false;
    (void)quorum_getquorate(q_h, &in_quorum);
 
    for(uint32_t i=0; i<num_nodes; i++) {
        votequorum_info info;
        ret = votequorum_getinfo(vq_h, nodes[i].node_id, &info);
        if (ret != CS_OK) {
            cout << "Failed to get info for node " << nodes[i].node_id << ": "
                 << get_error(ret) << " (" << ret << ")" << endl;
            continue;
        }
 
        bool is_eligible = info.node_votes>0;
        bool is_online   = in_quorum && is_eligible &&
                          (info.node_state == VOTEQUORUM_NODESTATE_MEMBER);
 
        cout << (info.node_id==our_nodeid? "*" : " ")
             //<< info.node_id
             << "" << nodes[i].node_name << "         "
             << "    "      << (is_online?   "true " : "false")
             << "   "       << (is_eligible? "true " : "false")
             << "         " << (info.node_votes==2? "true " : "false")
             << endl;
 
/********
        struct votequorum_info {
        unsigned int node_id;
        unsigned int node_state;
        unsigned int node_votes;
        unsigned int node_expected_votes;
        unsigned int highest_expected;
        unsigned int total_votes;
        unsigned int quorum;
        unsigned int flags;
        unsigned int qdevice_votes;
        char qdevice_name[VOTEQUORUM_QDEVICE_MAX_NAME_LEN];
        };
 
The node_state is defined as:
 
#define VOTEQUORUM_NODESTATE_MEMBER     1
#define VOTEQUORUM_NODESTATE_DEAD       2
#define VOTEQUORUM_NODESTATE_LEAVING    3
 
The flags are defined as:
 
#define VOTEQUORUM_INFO_TWONODE                 1
#define VOTEQUORUM_INFO_QUORATE                 2
#define VOTEQUORUM_INFO_WAIT_FOR_ALL            4
#define VOTEQUORUM_INFO_LAST_MAN_STANDING       8
#define VOTEQUORUM_INFO_AUTO_TIE_BREAKER       16
#define VOTEQUORUM_INFO_ALLOW_DOWNSCALE        32
#define VOTEQUORUM_INFO_QDEVICE_REGISTERED     64
#define VOTEQUORUM_INFO_QDEVICE_ALIVE         128
#define VOTEQUORUM_INFO_QDEVICE_CAST_VOTE     256
#define VOTEQUORUM_INFO_QDEVICE_MASTER_WINS   512
}
***************/
    }
}
 
void
cmap_init() {
    int ret = cmap_initialize(&cmap_h);
    if (ret == CS_OK) {
#ifdef DEBUG
        cout << "cmap initialized" << endl;
#endif
        cmap_connected = true;
 
        ret = cmap_get_uint32(cmap_h, "runtime.votequorum.this_node_id", &our_nodeid);
        if (ret == CS_OK) {
            // Got the node id
            cout << "Node id: " << our_nodeid << endl;
        } else {
            cout << "Unable to get this node's id: " << get_error(ret) << " (" << ret << ")" << endl;
        }
        char *cluster = NULL;
        ret = cmap_get_string(cmap_h, "totem.cluster_name", &cluster);
        if (ret == CS_OK) {
            cluster_name = cluster;
            free(cluster);
        }
    } else {
        cout << "Unable to initialize cmap: " << get_error(ret) << " (" << ret << ")" << endl;
    }
}
 
void cmap_close() {
    if (cmap_connected) {
      (void)cmap_finalize(cmap_h);
      cmap_connected = false;
    }
}
 
void coro_init() {
    quorum_init();
    votequorum_init();
    cmap_init();
}
 
void coro_close() {
    quorum_close();
    votequorum_close();
    cmap_close();
}
 
void print_help() {
    cout << "  c, connect        - Connect to corosync"         << endl
         << "  d, disconnect     - Disconnect from corosync"    << endl
         << "  s, node show      - Show all nodes"              << endl
         << "  u, quorum show    - Quorum status"               << endl
         << "  a, epsilon assign - Assign epsilon to this node" << endl
         << "  r, epsilon remove - Remove epsilon from node"    << endl
         << "  l, eligible       - Mark node eligible"          << endl
         << "  n, ineligible     - Mark node ineligible"        << endl
         << "  h, help           - Show help"                   << endl
         << "  q, quit           - Exit"                        << endl
         << "  e, exit           - Exit"                        << endl
         << endl;
}
 
int main() {
    bool done = false;
    do {
        cout << endl << (has_cluster_name()? get_cluster_name().c_str() : "") << "::*> ";
 
        char command[128];
        cin.getline(command,128);
 
        if(strcmp("connect",command)==0 ||
           strcmp("c",command)==0) {
            coro_init();
        } else if (strcmp("disconnect",command)==0 ||
                  strcmp("d",command)==0) {
            coro_close();
        } else if (strcmp("epsilon assign",command)==0 ) {
            uint32_t node_id = our_nodeid;
            assign_epsilon(node_id);
        } else if (strcmp("a",command)==0) {
            assign_epsilon(our_nodeid);
        } else if (strcmp("epsilon remove",command)==0 ) {
            uint32_t node_id = our_nodeid;
            remove_epsilon(node_id);
        } else if (strcmp("r",command)==0) {
            remove_epsilon(our_nodeid);
        } else if(strcmp("eligible",command)==0 ||
                  strcmp("l",command)==0) {
            mark_eligible(our_nodeid);
        } else if(strcmp("ineligible",command)==0 ||
                  strcmp("n",command)==0) {
            mark_ineligible(our_nodeid);
        } else if(strcmp("quorum show",command)==0) {
            get_quorate();
        } else if (strcmp("u",command)==0) {
            get_quorate();
        } else if(strcmp("cluster show",command)==0) {
            node_show();
        } else if(strcmp("node show",command)==0) {
            node_show();
        } else if (strcmp("s",command)==0) {
            node_show();
        } else if(strcmp("exit",command)==0 ||
                  strcmp("quit",command)==0 ||
                  strcmp("q",command)==0) {
            done = true;
        } else if(strcmp("help",command)==0 ||
                  strcmp("h",command)==0 ||
                  strcmp("?",command)==0) {
            cout << "\nHelp: \n";
            print_help();
        } else {
            cout << command << ": Unknown command\n\n";
            print_help();
        }
    } while(!done);
 
    coro_close();
 
    cout << "\nExiting\n";
    return 0;
}
