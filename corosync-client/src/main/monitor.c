#include "monitor.h"

//cmap stuff
static cmap_handle_t cmap_handle;

//quourm stuff
static void quorum_notification_fn( //q_notification
	quorum_handle_t handle,
	uint32_t quorate,
	uint64_t ring_id,
	uint32_t view_list_entries,
	uint32_t *view_list);

static quorum_handle_t q_handle; //quorum handle
static uint32_t q_type; //quorum type
static quorum_callbacks_t q_callbacks = {
	.quorum_notify_fn = quorum_notification_fn
};


//quorum call back vars
typedef struct {
      struct votequorum_info *vq_info; /* Might be NULL if votequorum not present */
      char *name;  /* Might be IP address or NULL */
      int node_id; /* Always present */
} view_list_entry_t;

static view_list_entry_t *g_view_list;
static uint32_t g_quorate;
static uint64_t g_ring_id;
static uint32_t g_ring_id_rep_node;
static uint32_t g_view_list_entries;
static uint32_t g_called;
static uint32_t g_vq_called;

//votequorum stuff
static void votequorum_notification_fn(
	votequorum_handle_t handle,
	uint64_t context,
	votequorum_ring_id_t ring_id,
	uint32_t node_list_entries,
	uint32_t node_list[]);

static votequorum_handle_t v_handle;

static votequorum_callbacks_t v_callbacks = {
	.votequorum_quorum_notify_fn = NULL,
	.votequorum_expectedvotes_notify_fn = NULL,
	.votequorum_nodelist_notify_fn = votequorum_notification_fn,
};

static uint32_t our_nodeid = 0;

//cfg stuff
static corosync_cfg_handle_t c_handle;
static corosync_cfg_callbacks_t c_callbacks = {
	.corosync_cfg_shutdown_callback = NULL
};

//our client stuff
static Notify_Context *n_context;
static long n_time_received;

static void votequorum_notification_fn(
	votequorum_handle_t handle,
	uint64_t context,
	votequorum_ring_id_t ring_id,
	uint32_t node_list_entries,
	uint32_t node_list[])
{
	get_microtime(&n_time_received);
	n_context->context_end = n_time_received;
	g_ring_id_rep_node = ring_id.nodeid;
	g_vq_called = 1;
}

static void quorum_notification_fn(
	quorum_handle_t handle,
	uint32_t quorate,
	uint64_t ring_id,
	uint32_t view_list_entries,
	uint32_t *view_list)
{
    int i;

	g_called = 1;
	g_quorate = quorate;
	g_ring_id = ring_id;
	g_view_list_entries = view_list_entries;
	if (g_view_list) {
		free(g_view_list);
	}
	g_view_list = malloc(sizeof(view_list_entry_t) * view_list_entries);
	if (g_view_list) {
	        for (i=0; i< view_list_entries; i++) {
		    g_view_list[i].node_id = view_list[i];
			g_view_list[i].name = NULL;
			g_view_list[i].vq_info = NULL;
		}
	}
}

static int get_quorum_type(char *quorum_type, size_t quorum_type_len)
{
	int err;
	char *str = NULL;

	if ((!quorum_type) || (quorum_type_len <= 0)) {
		return -1;
	}

	if (q_type == QUORUM_FREE) {
		return -1;
	}

	if ((err = cmap_get_string(cmap_handle, "quorum.provider", &str)) != CS_OK) {
		goto out;
	}

	if (!str) {
		return -1;
	}

	strncpy(quorum_type, str, quorum_type_len - 1);
	free(str);

	return 0;
out:
	return err;
}

//returns a 1 if votequorum is active (which allows us to perform votequorum actions)
static int using_votequorum(void)
{
	char quorumtype[256];
	int using_voteq;

	memset(quorumtype, 0, sizeof(quorumtype));

	if (get_quorum_type(quorumtype, sizeof(quorumtype))) {
		return -1;
	}

	if (strcmp(quorumtype, "corosync_votequorum") == 0) {
		using_voteq = 1;
	} else {
		using_voteq = 0;
	}

	return using_voteq;
}


static int init_all(void) {
	cmap_handle = 0;
	q_handle = 0;
	v_handle = 0;
	c_handle = 0;

	if (cmap_initialize(&cmap_handle) != CS_OK) {
		fprintf(stderr, "Cannot initialize CMAP service\n");
		cmap_handle = 0;
		goto out;
	}

	if (quorum_initialize(&q_handle, &q_callbacks, &q_type) != CS_OK) {
		fprintf(stderr, "Cannot initialize QUORUM service\n");
		q_handle = 0;
		goto out;
	}

	if (corosync_cfg_initialize(&c_handle, &c_callbacks) != CS_OK) {
		fprintf(stderr, "Cannot initialize CFG service\n");
		c_handle = 0;
		goto out;
	}

	if (using_votequorum() <= 0) {
		return 0;
	}

	if (votequorum_initialize(&v_handle, &v_callbacks) != CS_OK) {
		fprintf(stderr, "Cannot initialize VOTEQUORUM service\n");
		v_handle = 0;
		goto out;
	}

	if (cmap_get_uint32(cmap_handle, "runtime.votequorum.this_node_id", &our_nodeid) != CS_OK) {
		fprintf(stderr, "Unable to retrieve this_node_id\n");
		goto out;
	}

	return 0;
out:
	return -1;
}

static void close_all(void) {
	if (cmap_handle) {
		cmap_finalize(cmap_handle);
	}
	if (q_handle) {
		quorum_finalize(q_handle);
	}
	if (c_handle) {
		corosync_cfg_finalize(c_handle);
	}
	if (v_handle) {
		votequorum_finalize(v_handle);
	}
}

static int loop()
{
	int err;
	int i;
	
	if (q_type == QUORUM_FREE) {
		printf("\nQuorum is not configured - cannot monitor\n");
		return -1;
	}

	err=quorum_trackstart(q_handle, CS_TRACK_CHANGES);
	if (err != CS_OK) {
		fprintf(stderr, "Unable to start quorum status tracking: %s\n", cs_strerror(err));
		return -1;
	}

	if (using_votequorum()) {
		if ( (err=votequorum_trackstart(v_handle, 0LL, CS_TRACK_CHANGES)) != CS_OK) {
			fprintf(stderr, "Unable to start votequorum status tracking: %s\n", cs_strerror(err));
			return -1;
		}
	}
	i = 0;
	while(1){
		err = votequorum_dispatch(v_handle, CS_DISPATCH_ONE);
		if (err != CS_OK) {
			fprintf(stderr, "Unable to dispatch quorum status: %s\n", cs_strerror(err));
			return -1;
		}
		err = print_quorum();
		if(err != CS_OK){
			return err;
		}
		err = print_membership();
		if(err != CS_OK){
			return err;
		}
		printf("\nloop %d\n", i);
	}
}

static int single_dispatch()
{
	int err;
	
	if (q_type == QUORUM_FREE) {
		printf("\nQuorum is not configured - cannot monitor\n");
		return -1;
	}

	err=quorum_trackstart(q_handle, CS_TRACK_CHANGES);
	if (err != CS_OK) {
		fprintf(stderr, "Unable to start quorum status tracking: %s\n", cs_strerror(err));
		return -1;
	}

	if (using_votequorum()) {
		if ( (err=votequorum_trackstart(v_handle, 0LL, CS_TRACK_CHANGES)) != CS_OK) {
			fprintf(stderr, "Unable to start votequorum status tracking: %s\n", cs_strerror(err));
			return -1;
		}
	}
	err = votequorum_dispatch(v_handle, CS_DISPATCH_ONE);
	if (err != CS_OK) {
		fprintf(stderr, "Unable to dispatch quorum status: %s\n", cs_strerror(err));
		return -1;
	}
	
	print_notification(n_context);
}

int monitor_single_dispatch(Notify_Context *nc)
{
	int err;
	if (init_all()) {
		close_all();
		return -1;
	}
	n_context = nc;
	err = single_dispatch();
	close_all();
	return CS_OK;
}

int monitor_status()
{
	int err;
	
	if (init_all()) {
		close_all();
		return -1;
	}
	err = loop();
	close_all();
	return CS_OK;
}
