struct as_cluster_s;
typedef struct aerospike_s {
	bool _free;
	struct as_cluster_s * cluster;
} aerospike;
aerospike* aerospike_init(aerospike* as, as_config* config);
aerospike* aerospike_new(as_config* config);
void aerospike_init_lua(as_config_lua* config);
void aerospike_destroy(aerospike* as);
as_status aerospike_connect(aerospike* as, as_error* err);
as_status aerospike_close(aerospike* as, as_error* err);
bool aerospike_cluster_is_connected(aerospike* as);
 bool aerospike_cluster_is_connected(aerospike* as);
 void aerospike_stop_on_interrupt(bool stop);
typedef struct as_batch_read_record_s {
	as_key key;
	char** bin_names;
	uint32_t n_bin_names;
	bool read_all_bins;
	as_status result;
	as_record record;
} as_batch_read_record;
typedef struct as_batch_read_records_s {
	as_vector list;
} as_batch_read_records;
typedef bool (*aerospike_batch_read_callback)(const as_batch_read* results, uint32_t n, void* udata);
typedef bool (*as_batch_callback_xdr)(as_key* key, as_record* record, void* udata);
typedef void (*as_async_batch_listener)(as_error* err, as_batch_read_records* records, void* udata, as_event_loop* event_loop);
#define as_batch_read_inita(__records, __capacity) \
	as_vector_inita(&((__records)->list), sizeof(as_batch_read_record), __capacity);
static inline void as_batch_read_init(as_batch_read_records* records, uint32_t capacity)
{
	as_vector_init(&records->list, sizeof(as_batch_read_record), capacity);
}
static inline as_batch_read_records* as_batch_read_create(uint32_t capacity)
{
	return (as_batch_read_records*) as_vector_create(sizeof(as_batch_read_record), capacity);
}
static inline as_batch_read_record* as_batch_read_reserve(as_batch_read_records* records)
{
	return (as_batch_read_record*)as_vector_reserve(&records->list);
}
void as_batch_read_destroy(as_batch_read_records* records);
 bool aerospike_has_batch_index(aerospike* as);
as_status aerospike_batch_read(aerospike* as, as_error* err, const as_policy_batch* policy, as_batch_read_records* records );
as_status aerospike_batch_read_async(aerospike* as, as_error* err, const as_policy_batch* policy, as_batch_read_records* records,
	as_async_batch_listener listener, void* udata, as_event_loop* event_loop );
as_status aerospike_batch_get(aerospike * as, as_error * err, const as_policy_batch * policy, const as_batch * batch,
	aerospike_batch_read_callback callback, void * udata );
as_status aerospike_batch_get_xdr(aerospike* as, as_error* err, const as_policy_batch* policy, const as_batch* batch,
	as_batch_callback_xdr callback, void* udata );
as_status aerospike_batch_get_bins(
	aerospike* as, as_error* err, const as_policy_batch* policy, const as_batch* batch,
	const char** bins, uint32_t n_bins, aerospike_batch_read_callback callback, void* udata );
as_status aerospike_batch_exists(
	aerospike * as, as_error * err, const as_policy_batch * policy, const as_batch * batch,
	aerospike_batch_read_callback callback, void * udata );
#define AS_INDEX_POSITION_MAX_SZ 256
typedef char as_index_position[AS_INDEX_POSITION_MAX_SZ];
typedef enum as_index_type_s {
	AS_INDEX_TYPE_DEFAULT,
	AS_INDEX_TYPE_LIST,
	AS_INDEX_TYPE_MAPKEYS,
	AS_INDEX_TYPE_MAPVALUES
} as_index_type;
typedef enum as_index_datatype_s {
	AS_INDEX_STRING,
	AS_INDEX_NUMERIC,
	AS_INDEX_GEO2DSPHERE
} as_index_datatype;
typedef struct as_index_task_s {
	aerospike * as;
	as_namespace ns;
	char name[64];
	bool done;
} as_index_task;
as_status aerospike_index_create_complex(
	aerospike * as, as_error * err, as_index_task * task, const as_policy_info * policy,
	const as_namespace ns, const as_set set, const as_index_position position, const char * name,
	as_index_type itype, as_index_datatype dtype);
static inline as_status aerospike_index_create(
	aerospike * as, as_error * err, as_index_task * task, const as_policy_info * policy,
	const as_namespace ns, const as_set set, const as_bin_name bin, const char * name,
	as_index_datatype dtype)
{
	return aerospike_index_create_complex(as, err, task, policy, ns, set, bin, name, AS_INDEX_TYPE_DEFAULT, dtype);
}


as_status aerospike_index_create_wait(as_error * err, as_index_task * task, uint32_t interval_ms);


as_status aerospike_index_remove(
	aerospike * as, as_error * err, const as_policy_info * policy,
	const as_namespace ns, const char * name);




static inline as_status aerospike_index_integer_create(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const as_namespace ns, const as_set set, const as_bin_name bin, const char * name)
{
	return aerospike_index_create_complex(as, err, 0, policy, ns, set, bin, name, AS_INDEX_TYPE_DEFAULT, AS_INDEX_NUMERIC);
}


static inline as_status aerospike_index_string_create(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const as_namespace ns, const as_set set, const as_bin_name bin, const char * name)
{
	return aerospike_index_create_complex(as, err, 0, policy, ns, set, bin, name, AS_INDEX_TYPE_DEFAULT, AS_INDEX_STRING);
}


typedef bool (* aerospike_info_foreach_callback)(const as_error * err, const as_node * node, const char * req, char * res, void * udata);

as_status aerospike_info_host(aerospike * as, as_error * err, const as_policy_info * policy, const char * addr, uint16_t port, const char * req,
	char ** res );
as_status aerospike_info_socket_address(aerospike * as, as_error * err, const as_policy_info * policy, struct sockaddr_in* sa_in, const char * req,
	char ** res );
as_status aerospike_info_any(aerospike * as, as_error * err, const as_policy_info * policy, const char * req, char ** res );
as_status aerospike_info_foreach(aerospike * as, as_error * err, const as_policy_info * policy, const char * req, aerospike_info_foreach_callback callback, void * udata );
as_status aerospike_key_get(aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, as_record** rec );
as_status aerospike_key_get_async(aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, as_async_record_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
as_status aerospike_key_select(aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, const char* bins[], as_record** rec );
as_status aerospike_key_select_async(aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, const char* bins[], as_async_record_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
as_status aerospike_key_exists(aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, as_record** rec );
as_status aerospike_key_exists_async(aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, as_async_record_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
as_status aerospike_key_put(aerospike* as, as_error* err, const as_policy_write* policy, const as_key* key, as_record* rec );
as_status aerospike_key_put_async(aerospike* as, as_error* err, const as_policy_write* policy, const as_key* key, as_record* rec, as_async_write_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
as_status aerospike_key_remove(aerospike* as, as_error* err, const as_policy_remove* policy, const as_key* key );
as_status aerospike_key_remove_async(aerospike* as, as_error* err, const as_policy_remove* policy, const as_key* key, as_async_write_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
as_status aerospike_key_operate(aerospike* as, as_error* err, const as_policy_operate* policy, const as_key* key, const as_operations* ops, as_record** rec );
as_status aerospike_key_operate_async(aerospike* as, as_error* err, const as_policy_operate* policy, const as_key* key, const as_operations* ops, as_async_record_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
as_status aerospike_key_apply(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const char * module, const char * function, as_list * arglist, as_val ** result );
as_status aerospike_key_apply_async(aerospike* as, as_error* err, const as_policy_apply* policy, const as_key* key, const char* module, const char* function, as_list* arglist, as_async_value_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener );
bool aerospike_has_double(aerospike* as);
bool aerospike_has_geo(aerospike* as);
as_status aerospike_llist_add(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_val * val);
as_status aerospike_llist_update(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_val * val);
as_status aerospike_llist_add_all(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_list * vals);
as_status aerospike_llist_update_all(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_list * vals);
as_status aerospike_llist_find(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_val * search_val, as_list ** elements );
as_status aerospike_llist_find_first(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, uint32_t count, as_list ** elements);
as_status aerospike_llist_find_first_filter(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, uint32_t count, const as_udf_function_name filter, const as_list *filter_args, as_list ** elements);
as_status aerospike_llist_find_last(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, uint32_t count, as_list ** elements);
as_status aerospike_llist_find_last_filter(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, uint32_t count, const as_udf_function_name filter, const as_list *filter_args, as_list ** elements);
as_status aerospike_llist_find_from(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_val * from_val, uint32_t count, as_list ** elements);
as_status aerospike_llist_find_from_filter(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, const as_val * from_val, uint32_t count, const as_udf_function_name filter, const as_list *filter_args, as_list ** elements);
as_status aerospike_llist_scan(aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key, const as_ldt * ldt, as_list ** elements );

as_status aerospike_llist_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );


as_status aerospike_llist_range_limit(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_val * min_value, const as_val * max_value, uint32_t count,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );


static inline as_status aerospike_llist_range(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_val * min_value, const as_val * max_value,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements )
{
	return aerospike_llist_range_limit(as, err, policy, key, ldt, min_value, max_value, 0, filter, filter_args, elements);
}


as_status aerospike_llist_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);


as_status aerospike_llist_remove(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val *element
	);


as_status aerospike_llist_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);


as_status aerospike_llist_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t ldt_capacity
	);


as_status aerospike_llist_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);


as_status aerospike_llist_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);


as_status aerospike_llist_set_page_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t page_size);








as_status aerospike_lmap_put(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_val * mkey, const as_val * mval
	);


as_status aerospike_lmap_put_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_map * vals
	);


as_status aerospike_lmap_get(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * mkey,
	as_val ** mval
	);


as_status aerospike_lmap_get_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_map ** elements
	);


as_status aerospike_lmap_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_udf_function_name filter, const as_list *filter_args,
	as_map ** elements
	);


as_status aerospike_lmap_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);


as_status aerospike_lmap_remove(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val *mkey
	);


as_status aerospike_lmap_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);


as_status aerospike_lmap_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t ldt_capacity
	);


as_status aerospike_lmap_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);


as_status aerospike_lmap_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);








as_status aerospike_lset_add(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val);


as_status aerospike_lset_add_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_list * vals);


as_status aerospike_lset_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val,
	as_boolean *exists);


as_status aerospike_lset_get(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val,
	as_val ** pp_return_val );


as_status aerospike_lset_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );


as_status aerospike_lset_scan(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_list ** elements );


as_status aerospike_lset_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);


as_status aerospike_lset_remove(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val *element
	);


as_status aerospike_lset_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);


as_status aerospike_lset_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t ldt_capacity
	);


as_status aerospike_lset_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);


as_status aerospike_lset_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);








as_status aerospike_lstack_push(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val);


as_status aerospike_lstack_push_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_list * vals);


as_status aerospike_lstack_peek(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t peek_count,
	as_list ** elements );


as_status aerospike_lstack_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t peek_count,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );


as_status aerospike_lstack_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);


as_status aerospike_lstack_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);


as_status aerospike_lstack_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t ldt_capacity
	);


as_status aerospike_lstack_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);


as_status aerospike_lstack_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);








typedef bool (*aerospike_query_foreach_callback)(const as_val* val, void* udata);


typedef bool (*as_async_query_record_listener)(as_error* err, as_record* record, void* udata, as_event_loop* event_loop);




as_status
aerospike_query_foreach(
	aerospike* as, as_error* err, const as_policy_query* policy, const as_query* query,
	aerospike_query_foreach_callback callback, void* udata
	);


as_status
aerospike_query_async(
	aerospike* as, as_error* err, const as_policy_query* policy, const as_query* query,
	as_async_query_record_listener listener, void* udata, as_event_loop* event_loop
	);


as_status
aerospike_query_background(
	aerospike* as, as_error* err, const as_policy_write* policy,
	const as_query* query, uint64_t* query_id
	);


static inline as_status
aerospike_query_wait(
   aerospike* as, as_error* err, const as_policy_info* policy,
   const as_query* query, uint64_t query_id, uint32_t interval_ms
   )
{
	const char* module = (query->where.size > 0)? "query" : "scan";
	return aerospike_job_wait(as, err, policy, module, query_id, interval_ms);
}


static inline as_status
aerospike_query_info(
	aerospike* as, as_error* err, const as_policy_info* policy,
	const as_query* query, uint64_t query_id, as_job_info* info
	)
{
	const char* module = (query->where.size > 0)? "query" : "scan";
	return aerospike_job_info(as, err, policy, module, query_id, false, info);
}








typedef bool (*aerospike_scan_foreach_callback)(const as_val* val, void* udata);


typedef bool (*as_async_scan_listener)(as_error* err, as_record* record, void* udata, as_event_loop* event_loop);




as_status
aerospike_scan_background(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan,
	uint64_t* scan_id
	);


as_status
aerospike_scan_wait(
	aerospike* as, as_error* err, const as_policy_info* policy, uint64_t scan_id,
	uint32_t interval_ms
	);


as_status
aerospike_scan_info(
	aerospike* as, as_error* err, const as_policy_info* policy, uint64_t scan_id, as_scan_info* info
	);


as_status
aerospike_scan_foreach(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan,
	aerospike_scan_foreach_callback callback, void* udata
	);


as_status
aerospike_scan_node(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan,
	const char* node_name, aerospike_scan_foreach_callback callback, void* udata
	);


as_status
aerospike_scan_async(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan, uint64_t* scan_id,
	as_async_scan_listener listener, void* udata, as_event_loop* event_loop
	);


as_status
aerospike_scan_node_async(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan, uint64_t* scan_id,
	const char* node_name, as_async_scan_listener listener, void* udata, as_event_loop* event_loop
	);








as_status aerospike_udf_list(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	as_udf_files * files
	);


as_status aerospike_udf_get(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * filename, as_udf_type type, as_udf_file * file
	);


as_status aerospike_udf_put(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * filename, as_udf_type type, as_bytes * content
	);


as_status aerospike_udf_put_wait(
	aerospike * as, as_error * err, const as_policy_info * policy,
	const char * filename, uint32_t interval_ms);


as_status aerospike_udf_remove(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * filename
	);






#define AS_ROLE_SIZE 32




typedef enum as_privilege_code_e {
	

	AS_PRIVILEGE_USER_ADMIN = 0,
	

	AS_PRIVILEGE_SYS_ADMIN = 1,
	

	AS_PRIVILEGE_DATA_ADMIN = 2,
	

	AS_PRIVILEGE_READ = 10,
	

	AS_PRIVILEGE_READ_WRITE = 11,
	

	AS_PRIVILEGE_READ_WRITE_UDF = 12
} as_privilege_code;


typedef struct as_privilege_s {
	

	as_namespace ns;
	

	as_set set;
	

	as_privilege_code code;
} as_privilege;


typedef struct as_role_s {
	

	char name[AS_ROLE_SIZE];
	

	int privileges_size;
	

	as_privilege privileges[];
} as_role;


typedef struct as_user_s {
	

	char name[AS_USER_SIZE];
	

	int roles_size;
	

	char roles[][AS_ROLE_SIZE];
} as_user;




as_status
aerospike_create_user(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char* password, const char** roles, int roles_size);


as_status
aerospike_drop_user(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name);


as_status
aerospike_set_password(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char* password);


as_status
aerospike_change_password(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char* password);


as_status
aerospike_grant_roles(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char** roles, int roles_size);


as_status
aerospike_revoke_roles(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char** roles, int roles_size);


as_status
aerospike_create_role(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role, as_privilege** privileges, int privileges_size);


as_status
aerospike_drop_role(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role);


as_status
aerospike_grant_privileges(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role, as_privilege** privileges, int privileges_size);


as_status
aerospike_revoke_privileges(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role, as_privilege** privileges, int privileges_size);


as_status
aerospike_query_user(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, as_user** user);


void
as_user_destroy(as_user* user);


as_status
aerospike_query_users(aerospike* as, as_error* err, const as_policy_admin* policy, as_user*** users, int* users_size);


void
as_users_destroy(as_user** users, int users_size);


as_status
aerospike_query_role(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role_name, as_role** role);


void
as_role_destroy(as_role* role);


as_status
aerospike_query_roles(aerospike* as, as_error* err, const as_policy_admin* policy, as_role*** roles, int* roles_size);


void
as_roles_destroy(as_role** roles, int roles_size);


as_status
as_authenticate(as_error* err, int fd, const char* user, const char* credential, uint64_t deadline_ms);


uint32_t
as_authenticate_set(const char* user, const char* credential, uint8_t* buffer);


#include <citrusleaf/alloc.h>


#define AS_ASYNC_TYPE_WRITE 0
#define AS_ASYNC_TYPE_RECORD 1
#define AS_ASYNC_TYPE_VALUE 2
#define AS_ASYNC_TYPE_BATCH 3
#define AS_ASYNC_TYPE_SCAN 4
#define AS_ASYNC_TYPE_QUERY 5
#define AS_AUTHENTICATION_MAX_SIZE 158
#define AS_ASYNC_CONNECTION_COMPLETE 0
#define AS_ASYNC_CONNECTION_PENDING 1
#define AS_ASYNC_CONNECTION_ERROR 2
typedef struct as_async_write_command {
	as_event_command command;
	as_async_write_listener listener;
	uint8_t space[];
} as_async_write_command;
typedef struct as_async_record_command {
	as_event_command command;
	as_async_record_listener listener;
	uint8_t space[];
} as_async_record_command;
typedef struct as_async_value_command {
	as_event_command command;
	as_async_value_listener listener;
	uint8_t space[];
} as_async_value_command;


static inline as_event_command*
as_async_write_command_create(
	as_cluster* cluster, as_node* node, uint32_t timeout_ms, bool deserialize,
	as_async_write_listener listener, void* udata, as_event_loop* event_loop,
	as_pipe_listener pipe_listener, size_t size, as_event_parse_results_fn parse_results
	)
{
	// Allocate enough memory to cover: struct size + write buffer size + auth max buffer size
	// Then, round up memory size in 1KB increments.
	size_t s = (sizeof(as_async_write_command) + size + AS_AUTHENTICATION_MAX_SIZE + 1023) & ~1023;
	as_event_command* cmd = cf_malloc(s);
	as_async_write_command* wcmd = (as_async_write_command*)cmd;
	cmd->event_loop = as_event_assign(event_loop);
	cmd->conn = 0;
	cmd->cluster = cluster;
	cmd->node = node;
	cmd->udata = udata;
	cmd->parse_results = parse_results;
	cmd->buf = wcmd->space;
	cmd->capacity = (uint32_t)(s - sizeof(as_async_write_command));
	cmd->len = 0;
	cmd->pos = 0;
	cmd->auth_len = 0;
	cmd->timeout_ms = timeout_ms;
	cmd->type = AS_ASYNC_TYPE_WRITE;
	cmd->state = AS_ASYNC_STATE_UNREGISTERED;
	cmd->pipe_listener = pipe_listener;
	cmd->deserialize = deserialize;
	cmd->free_buf = false;
	wcmd->listener = listener;
	return cmd;
}
static inline as_event_command*
as_async_record_command_create(
	as_cluster* cluster, as_node* node, uint32_t timeout_ms, bool deserialize,
	as_async_record_listener listener, void* udata, as_event_loop* event_loop,
	as_pipe_listener pipe_listener, size_t size, as_event_parse_results_fn parse_results
	)
{
	// Allocate enough memory to cover: struct size + write buffer size + auth max buffer size
	// Then, round up memory size in 1KB increments to reduce fragmentation and to allow socket
	// read to reuse buffer for small socket write sizes.
	size_t s = (sizeof(as_async_record_command) + size + AS_AUTHENTICATION_MAX_SIZE + 1023) & ~1023;
	as_event_command* cmd = cf_malloc(s);
	as_async_record_command* rcmd = (as_async_record_command*)cmd;
	cmd->event_loop = as_event_assign(event_loop);
	cmd->conn = 0;
	cmd->cluster = cluster;
	cmd->node = node;
	cmd->udata = udata;
	cmd->parse_results = parse_results;
	cmd->buf = rcmd->space;
	cmd->capacity = (uint32_t)(s - sizeof(as_async_record_command));
	cmd->len = 0;
	cmd->pos = 0;
	cmd->auth_len = 0;
	cmd->timeout_ms = timeout_ms;
	cmd->type = AS_ASYNC_TYPE_RECORD;
	cmd->state = AS_ASYNC_STATE_UNREGISTERED;
	cmd->pipe_listener = pipe_listener;
	cmd->deserialize = deserialize;
	cmd->free_buf = false;
	rcmd->listener = listener;
	return cmd;
}
static inline as_event_command*
as_async_value_command_create(
   as_cluster* cluster, as_node* node, uint32_t timeout_ms, bool deserialize,
   as_async_value_listener listener, void* udata, as_event_loop* event_loop,
   as_pipe_listener pipe_listener, size_t size, as_event_parse_results_fn parse_results
   )
{
	// Allocate enough memory to cover: struct size + write buffer size + auth max buffer size
	// Then, round up memory size in 1KB increments to reduce fragmentation and to allow socket
	// read to reuse buffer for small socket write sizes.
	size_t s = (sizeof(as_async_value_command) + size + AS_AUTHENTICATION_MAX_SIZE + 1023) & ~1023;
	as_event_command* cmd = cf_malloc(s);
	as_async_value_command* vcmd = (as_async_value_command*)cmd;
	cmd->event_loop = as_event_assign(event_loop);
	cmd->conn = 0;
	cmd->cluster = cluster;
	cmd->node = node;
	cmd->udata = udata;
	cmd->parse_results = parse_results;
	cmd->buf = vcmd->space;
	cmd->capacity = (uint32_t)(s - sizeof(as_async_value_command));
	cmd->len = 0;
	cmd->pos = 0;
	cmd->auth_len = 0;
	cmd->timeout_ms = timeout_ms;
	cmd->type = AS_ASYNC_TYPE_VALUE;
	cmd->state = AS_ASYNC_STATE_UNREGISTERED;
	cmd->pipe_listener = pipe_listener;
	cmd->deserialize = deserialize;
	cmd->free_buf = false;
	vcmd->listener = listener;
	return cmd;
}


#include <stdint.h>


uint32_t
as_async_get_pending(as_cluster* cluster);
void
as_async_get_connections(as_cluster* cluster, uint32_t* async_conn, uint32_t* async_conn_pool);


#include <stdint.h>
#include <stdbool.h>




typedef struct as_batch_s {
	

	bool _free;
	

	struct {
		

		bool _free;
		

		uint32_t size;
		

		as_key * entries;
	} keys;
} as_batch;


typedef struct as_batch_read_s {
	

	const as_key * key;
	

	as_status result;
	

	as_record record;
} as_batch_read;




#define as_batch_inita(__batch, __size) \
	if ( (__batch) != NULL ) {\
		(__batch)->_free = false;\
		(__batch)->keys.entries = (as_key *) alloca(sizeof(as_key) * __size);\
		if ( (__batch)->keys.entries ) { \
			(__batch)->keys._free = false;\
			(__batch)->keys.size = __size;\
		}\
 	}




as_batch * as_batch_new(uint32_t size);


as_batch * as_batch_init(as_batch * batch, uint32_t size);


void as_batch_destroy(as_batch * batch);


static inline as_key * as_batch_keyat(const as_batch * batch, uint32_t i)
{
	return (batch != NULL && batch->keys.entries != NULL && batch->keys.size > i) ? &batch->keys.entries[i] : NULL;
}






#define AS_BIN_NAME_MAX_SIZE 15


#define AS_BIN_NAME_MAX_LEN (AS_BIN_NAME_MAX_SIZE - 1)




typedef char as_bin_name[AS_BIN_NAME_MAX_SIZE];


typedef union as_bin_value_s {
	as_val nil;
	as_integer integer;
	as_double dbl;
	as_string string;
	as_bytes bytes;
	as_list list;
	as_map map;
} as_bin_value;


typedef struct as_bin_s {
	

	as_bin_name name;
	

	as_bin_value value;
	

	as_bin_value * valuep;
} as_bin;


typedef struct as_bins_s {
	

	bool _free;
	

	uint16_t capacity;
	

	uint16_t size;
	

	as_bin * entries;
} as_bins;




static inline char * as_bin_get_name(const as_bin * bin) {
	return (char *) bin->name;
}


static inline as_bin_value * as_bin_get_value(const as_bin * bin) {
	return bin->valuep;
}


static inline as_val_t as_bin_get_type(const as_bin * bin) {
	return as_val_type(bin->valuep);
}


// Concurrency kit needs to be under extern "C" when compiling C++.




typedef struct as_seed_s {
	

	char* name;
	

	in_port_t port;
} as_seed;


typedef struct as_seeds_s {
	

	uint32_t ref_count;
	

	uint32_t size;
	

	as_seed array[];
} as_seeds;


typedef struct as_nodes_s {
	

	uint32_t ref_count;
	

	uint32_t size;
	

	as_node* array[];
} as_nodes;


typedef struct as_addr_maps_s {
	

	uint32_t ref_count;
	

	uint32_t size;
	

	as_addr_map array[];
} as_addr_maps;


typedef void (*as_release_fn) (void* value);


typedef struct as_gc_item_s {
	

	void* data;
	

	as_release_fn release_fn;
} as_gc_item;


typedef struct as_cluster_s {
	

	as_nodes* nodes;
	

	as_partition_tables* partition_tables;
	

	as_vector* 
 gc;
	

	struct as_shm_info_s* shm_info;
	

	char* user;
	

	char* password;
	

	as_seeds* seeds;
	

	uint32_t version;
	

	as_addr_maps* ip_map;
	

	as_thread_pool thread_pool;
	

	pthread_t tend_thread;
	

	pthread_mutex_t tend_lock;
	

	pthread_cond_t tend_cond;
	

	uint32_t tend_interval;
	

	uint32_t conn_queue_size;
	

	uint32_t async_max_conns_per_node;
	

	uint32_t pipe_max_conns_per_node;
	

	uint32_t async_pending;
	

	uint32_t async_conn_count;
	

	uint32_t async_conn_pool;
	

	uint32_t conn_timeout_ms;
	

	uint32_t max_socket_idle;
	

	uint32_t node_index;
	

	uint16_t n_partitions;
	

	bool use_services_alternate;
	

	volatile bool valid;
} as_cluster;




as_status
as_cluster_create(as_config* config, as_error* err, as_cluster** cluster);


void
as_cluster_destroy(as_cluster* cluster);


bool
as_cluster_is_connected(as_cluster* cluster);


void
as_cluster_get_node_names(as_cluster* cluster, int* n_nodes, char** node_names);


static inline as_nodes*
as_nodes_reserve(as_cluster* cluster)
{
	as_nodes* nodes = (as_nodes *)ck_pr_load_ptr(&cluster->nodes);
	//ck_pr_fence_acquire();
	ck_pr_inc_32(&nodes->ref_count);
	return nodes;
}


static inline void
as_nodes_release(as_nodes* nodes)
{
	//ck_pr_fence_release();
	bool destroy;
	ck_pr_dec_32_zero(&nodes->ref_count, &destroy);
	if (destroy) {
		cf_free(nodes);
	}
}


static inline as_seeds*
as_seeds_reserve(as_cluster* cluster)
{
	as_seeds* seeds = (as_seeds *)ck_pr_load_ptr(&cluster->seeds);
	ck_pr_inc_32(&seeds->ref_count);
	return seeds;
}


static inline void
as_seeds_release(as_seeds* seeds)
{
	bool destroy;
	ck_pr_dec_32_zero(&seeds->ref_count, &destroy);
	if (destroy) {
		for (uint32_t i = 0; i < seeds->size; i++) {
			cf_free(seeds->array[i].name);
		}
		cf_free(seeds);
	}
}


void
as_seeds_add(as_cluster* cluster, as_seed* seed_list, uint32_t size);


void
as_seeds_update(as_cluster* cluster, as_seed* seed_list, uint32_t size);


static inline as_addr_maps*
as_ip_map_reserve(as_cluster* cluster)
{
	as_addr_maps* ip_map = (as_addr_maps *)ck_pr_load_ptr(&cluster->ip_map);
	if (ip_map == NULL) {
		return NULL;
	}
	ck_pr_inc_32(&ip_map->ref_count);
	return ip_map;
}


static inline void
as_ip_map_release(as_addr_maps* ip_map)
{
	bool destroy;
	ck_pr_dec_32_zero(&ip_map->ref_count, &destroy);
	if (destroy) {
		for (uint32_t i = 0; i < ip_map->size; i++) {
			cf_free(ip_map->array[i].orig);
			cf_free(ip_map->array[i].alt);
		}
		cf_free(ip_map);
	}
}


void
as_ip_map_update(as_cluster* cluster, as_addr_map* ip_map_list, uint32_t size);


void
as_cluster_set_async_max_conns_per_node(as_cluster* cluster, uint32_t async_size, uint32_t pipe_size);


void
as_cluster_change_password(as_cluster* cluster, const char* user, const char* password);


as_node*
as_node_get_random(as_cluster* cluster);


as_node*
as_node_get_by_name(as_cluster* cluster, const char* name);


static inline as_partition_tables*
as_partition_tables_reserve(as_cluster* cluster)
{
	as_partition_tables* tables = (as_partition_tables *)ck_pr_load_ptr(&cluster->partition_tables);
	ck_pr_inc_32(&tables->ref_count);
	return tables;
}


static inline void
as_partition_tables_release(as_partition_tables* tables)
{
	bool destroy;
	ck_pr_dec_32_zero(&tables->ref_count, &destroy);
	if (destroy) {
		cf_free(tables);
	}
}


static inline as_partition_table*
as_cluster_get_partition_table(as_cluster* cluster, const char* ns)
{
	// Partition tables array size does not currently change after first cluster tend.
	// Also, there is a one second delayed garbage collection coupled with as_partition_tables_get()
	// being very fast.  Reference counting the tables array is not currently necessary, but do it
	// anyway in case the server starts supporting dynamic namespaces.
	as_partition_tables* tables = as_partition_tables_reserve(cluster);
	as_partition_table* table = as_partition_tables_get(tables, ns);
	as_partition_tables_release(tables);
	return table;
}


as_node*
as_partition_table_get_node(as_cluster* cluster, as_partition_table* table, const uint8_t* digest, bool write, as_policy_replica replica);


as_node*
as_shm_node_get(as_cluster* cluster, const char* ns, const uint8_t* digest, bool write, as_policy_replica replica);


static inline as_node*
as_node_get(as_cluster* cluster, const char* ns, const uint8_t* digest, bool write, as_policy_replica replica)
{
}


#include <citrusleaf/cf_byte_order.h>
#include <citrusleaf/cf_digest.h>


// Field IDs
#define AS_FIELD_NAMESPACE 0
#define AS_FIELD_SETNAME 1
#define AS_FIELD_KEY 2
#define AS_FIELD_DIGEST 4
#define AS_FIELD_DIGEST_ARRAY 6
#define AS_FIELD_TASK_ID 7
#define AS_FIELD_SCAN_OPTIONS 8
#define AS_FIELD_INDEX_RANGE 22
#define AS_FIELD_INDEX_FILTER 23
#define AS_FIELD_INDEX_LIMIT 24
#define AS_FIELD_INDEX_ORDER 25
#define AS_FIELD_INDEX_TYPE 26
#define AS_FIELD_UDF_PACKAGE_NAME 30
#define AS_FIELD_UDF_FUNCTION 31
#define AS_FIELD_UDF_ARGLIST 32
#define AS_FIELD_UDF_OP 33
#define AS_FIELD_QUERY_BINS 40
#define AS_FIELD_BATCH_INDEX 41
#define AS_FIELD_BATCH_INDEX_WITH_SET 42
// Message info1 bits
#define AS_MSG_INFO1_READ				(1 << 0) // contains a read operation
#define AS_MSG_INFO1_GET_ALL			(1 << 1) // get all bins, period
// (Note:  Bit 2 is unused.)
#define AS_MSG_INFO1_BATCH_INDEX		(1 << 3) // batch read
#define AS_MSG_INFO1_XDR				(1 << 4) // operation is being performed by XDR
#define AS_MSG_INFO1_GET_NOBINDATA		(1 << 5) // do not get information about bins and its data
#define AS_MSG_INFO1_CONSISTENCY_ALL	(1 << 6) // read consistency level - bit 0
// (Note:  Bit 7 is unused.)
// Message info2 bits
#define AS_MSG_INFO2_WRITE				(1 << 0) // contains a write semantic
#define AS_MSG_INFO2_DELETE				(1 << 1) // delete record
#define AS_MSG_INFO2_GENERATION			(1 << 2) // pay attention to the generation
#define AS_MSG_INFO2_GENERATION_GT		(1 << 3) // apply write if new generation >= old, good for restore
// (Note:  Bit 4 is unused.)
#define AS_MSG_INFO2_CREATE_ONLY		(1 << 5) // write record only if it doesn't exist
// (Note:  Bit 6 is unused.)
// (Note:  Bit 7 is unused.)
// Message info3 bits
#define AS_MSG_INFO3_LAST				(1 << 0) // this is the last of a multi-part message
#define AS_MSG_INFO3_COMMIT_MASTER  	(1 << 1) // write commit level - bit 0
// (Note:  Bit 2 is unused.)
#define AS_MSG_INFO3_UPDATE_ONLY		(1 << 3) // update existing record only, do not create new record
#define AS_MSG_INFO3_CREATE_OR_REPLACE	(1 << 4) // completely replace existing record, or create new record
#define AS_MSG_INFO3_REPLACE_ONLY		(1 << 5) // completely replace existing record, do not create new record
// (Note:  Bit 6 is unused.)
// (Note:  Bit 7 is unused.)
// Transaction message
#define AS_MESSAGE_VERSION 2L
#define AS_MESSAGE_TYPE 3L
#define AS_COMPRESSED_MESSAGE_TYPE 4L
// Info message
#define AS_INFO_MESSAGE_VERSION 2L
#define AS_INFO_MESSAGE_TYPE 1L
// Misc
#define AS_HEADER_SIZE 30
#define AS_FIELD_HEADER_SIZE 5
#define AS_OPERATION_HEADER_SIZE 8
#define AS_STACK_BUF_SIZE (1024 * 16)


static inline void*
local_malloc(size_t size)
{
	return cf_malloc(size);
}
static inline void
local_free(void* memory)
{
	return cf_free(memory);
}


#define as_command_init(_sz) (_sz > AS_STACK_BUF_SIZE) ? (uint8_t*)local_malloc(_sz) : (uint8_t*)alloca(_sz)


#define as_command_free(_buf, _sz) if (_sz > AS_STACK_BUF_SIZE) {local_free(_buf);}




typedef struct as_command_node_s {
	as_node* node;
	const char* ns;
	const uint8_t* digest;
	as_policy_replica replica;
	bool write;
} as_command_node;


typedef struct as_command_parse_result_data_s {
	as_record** record;
	bool deserialize;
} as_command_parse_result_data;


typedef as_status (*as_parse_results_fn) (as_error* err, int fd, uint64_t deadline_ms, void* user_data);




size_t
as_command_key_size(as_policy_key policy, const as_key* key, uint16_t* n_fields);


static inline size_t
as_command_string_field_size(const char* value)
{
	return strlen(value) + AS_FIELD_HEADER_SIZE;
}


static inline size_t
as_command_field_size(size_t size)
{
	return size + AS_FIELD_HEADER_SIZE;
}


size_t
as_command_value_size(as_val* val, as_buffer* buffer);


static inline size_t
as_command_bin_size(const as_bin* bin, as_buffer* buffer)
{
	return strlen(bin->name) + as_command_value_size((as_val*)bin->valuep, buffer) + 8;
}


static inline as_status
as_command_bin_name_size(as_error* err, const char* name, size_t* size)
{
	size_t s = strlen(name);
	if (s > AS_BIN_NAME_MAX_LEN) {
		return as_error_update(err, AEROSPIKE_ERR_PARAM, "Bin name too long: %s", name);
	}
	(*size) += s + AS_OPERATION_HEADER_SIZE;
	return AEROSPIKE_OK;
}


static inline size_t
as_command_string_operation_size(const char* value)
{
	return strlen(value) + AS_OPERATION_HEADER_SIZE;
}


uint8_t*
as_command_write_header(uint8_t* cmd, uint8_t read_attr, uint8_t write_attr,
	as_policy_commit_level commit_level, as_policy_consistency_level consistency,
	as_policy_exists exists, as_policy_gen gen_policy, uint32_t gen, uint32_t ttl,
	uint32_t timeout_ms, uint16_t n_fields, uint16_t n_bins);


static inline uint8_t*
as_command_write_header_read(uint8_t* cmd, uint8_t read_attr, as_policy_consistency_level consistency,
	uint32_t timeout_ms, uint16_t n_fields, uint16_t n_bins)
{
	if (consistency == AS_POLICY_CONSISTENCY_LEVEL_ALL) {
		read_attr |= AS_MSG_INFO1_CONSISTENCY_ALL;
	}
	cmd[8] = 22;
	cmd[9] = read_attr;
	memset(&cmd[10], 0, 12);
	*(uint32_t*)&cmd[22] = cf_swap_to_be32(timeout_ms);
	*(uint16_t*)&cmd[26] = cf_swap_to_be16(n_fields);
	*(uint16_t*)&cmd[28] = cf_swap_to_be16(n_bins);
	return cmd + AS_HEADER_SIZE;
}


static inline uint8_t*
as_command_write_field_header(uint8_t* p, uint8_t id, uint32_t size)
{
	*(uint32_t*)p = cf_swap_to_be32(size+1);
	p += 4;
	*p++ = id;
	return p;
}


static inline uint8_t*
as_command_write_field_string(uint8_t* begin, uint8_t id, const char* val)
{
	uint8_t* p = begin + AS_FIELD_HEADER_SIZE;
	// Copy string, but do not transfer null byte.
	while (*val) {
		*p++ = *val++;
	}
	as_command_write_field_header(begin, id, (uint32_t)(p - begin - AS_FIELD_HEADER_SIZE));
	return p;
}


static inline uint8_t*
as_command_write_field_uint64(uint8_t* p, uint8_t id, uint64_t val)
{
	p = as_command_write_field_header(p, id, sizeof(uint64_t));
	*(uint64_t*)p = cf_swap_to_be64(val);
	return p + sizeof(uint64_t);
}


static inline uint8_t*
as_command_write_field_buffer(uint8_t* p, uint8_t id, as_buffer* buffer)
{
	p = as_command_write_field_header(p, id, buffer->size);
	memcpy(p, buffer->data, buffer->size);
	return p + buffer->size;
}


static inline uint8_t*
as_command_write_field_digest(uint8_t* p, const as_digest* val)
{
	p = as_command_write_field_header(p, AS_FIELD_DIGEST, AS_DIGEST_VALUE_SIZE);
	memcpy(p, val->value, AS_DIGEST_VALUE_SIZE);
	return p + AS_DIGEST_VALUE_SIZE;
}


uint8_t*
as_command_write_key(uint8_t* p, as_policy_key policy, const as_key* key);


static inline uint8_t*
as_command_write_bin_name(uint8_t* cmd, const char* name)
{
	uint8_t* p = cmd + AS_OPERATION_HEADER_SIZE;
	// Copy string, but do not transfer null byte.
	while (*name) {
		*p++ = *name++;
	}
	uint8_t name_len = p - cmd - AS_OPERATION_HEADER_SIZE;
	*(uint32_t*)cmd = cf_swap_to_be32((uint32_t)name_len + 4);
	cmd += 4;
	*cmd++ = AS_OPERATOR_READ;
	*cmd++ = 0;
	*cmd++ = 0;
	*cmd++ = name_len;
	return p;
}


uint8_t*
as_command_write_bin(uint8_t* begin, uint8_t operation_type, const as_bin* bin, as_buffer* buffer);


static inline size_t
as_command_write_end(uint8_t* begin, uint8_t* end)
{
	uint64_t len = end - begin;
	uint64_t proto = (len - 8) | (AS_MESSAGE_VERSION << 56) | (AS_MESSAGE_TYPE << 48);
	*(uint64_t*)begin = cf_swap_to_be64(proto);
	return len;
}


static inline size_t
as_command_compress_write_end(uint8_t* begin, uint8_t* end, uint64_t uncompressed_sz)
{
	uint64_t len = end - begin;
	uint64_t proto = (len - 8) | (AS_MESSAGE_VERSION << 56) | (AS_COMPRESSED_MESSAGE_TYPE << 48);
	*(uint64_t*)begin = cf_swap_to_be64(proto);
	// TODO: We are not passing this in network byte order because of a mistake
	// in the past. Should be fixed in unison with server code.
	((as_compressed_proto *)begin)->uncompressed_sz = uncompressed_sz;
	return len;
}


size_t
as_command_compress_max_size(size_t cmd_sz);


as_status
as_command_compress(as_error* err, uint8_t* cmd, size_t cmd_sz, uint8_t* compressed_cmd, size_t* compressed_size);


as_status
as_command_execute(as_cluster* cluster, as_error* err, as_command_node* cn, uint8_t* command, size_t command_len,
   uint32_t timeout_ms, uint32_t retry,
   as_parse_results_fn parse_results_fn, void* parse_results_data);


as_status
as_command_parse_header(as_error* err, int fd, uint64_t deadline_ms, void* user_data);


as_status
as_command_parse_result(as_error* err, int fd, uint64_t deadline_ms, void* user_data);


as_status
as_command_parse_success_failure(as_error* err, int fd, uint64_t deadline_ms, void* user_data);


as_status
as_command_parse_success_failure_bins(uint8_t** pp, as_error* err, as_msg* msg, as_val** value);


uint8_t*
as_command_parse_bins(as_record* rec, uint8_t* buf, uint32_t n_bins, bool deserialize);


as_status
as_command_parse_udf_failure(uint8_t* p, as_error* err, as_msg* msg, as_status status);


uint8_t*
as_command_ignore_fields(uint8_t* p, uint32_t n_fields);


uint8_t*
as_command_ignore_bins(uint8_t* p, uint32_t n_bins);


uint8_t*
as_command_parse_key(uint8_t* p, uint32_t n_fields, as_key* key);






#define AS_CONFIG_PATH_MAX_SIZE 256


#define AS_CONFIG_PATH_MAX_LEN 	(AS_CONFIG_PATH_MAX_SIZE - 1)


#define AS_CONFIG_HOSTS_SIZE 256




typedef struct as_config_host_s {
	

	const char * addr;
	

	uint16_t port;
} as_config_host;


typedef struct as_addr_map_s {
	

    char * orig;
	

    char * alt;
} as_addr_map;


typedef struct as_config_lua_s {
	

	bool cache_enabled;
	

	char system_path[AS_CONFIG_PATH_MAX_SIZE];
	

	char user_path[AS_CONFIG_PATH_MAX_SIZE];
} as_config_lua;


typedef struct as_config_s {
	

	char user[AS_USER_SIZE];
	

	char password[AS_PASSWORD_HASH_SIZE];
	

	as_addr_map * ip_map;
	

	uint32_t ip_map_size;
	

	uint32_t max_conns_per_node;
	

	uint32_t async_max_conns_per_node;
	

	uint32_t pipe_max_conns_per_node;
	

	uint32_t conn_timeout_ms;
	

	uint32_t tender_interval;
	

	uint32_t thread_pool_size;
	

	uint32_t hosts_size;
	

	as_config_host hosts[AS_CONFIG_HOSTS_SIZE];
	

	as_policies policies;
	

	as_config_lua lua;
	

	bool fail_if_not_connected;
	

	bool use_services_alternate;
	

	bool use_shm;
	

	int shm_key;
	

	uint32_t shm_max_nodes;
	

	uint32_t shm_max_namespaces;
	

	uint32_t shm_takeover_threshold_sec;
} as_config;




as_config * as_config_init(as_config * c);


static inline void
as_config_add_host(as_config* config, const char* addr, uint16_t port)
{
	as_config_host* host = &config->hosts[config->hosts_size++];
	host->addr = addr;
	host->port = port;
}


bool
as_config_set_user(as_config* config, const char* user, const char* password);


static inline void
as_config_lua_init(as_config_lua* lua)
{
	lua->cache_enabled = false;
	strcpy(lua->system_path, AS_CONFIG_LUA_SYSTEM_PATH);
	strcpy(lua->user_path, AS_CONFIG_LUA_USER_PATH);
}


#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>




#define AS_ERROR_MESSAGE_MAX_SIZE 	1024


#define AS_ERROR_MESSAGE_MAX_LEN 	(AS_ERROR_MESSAGE_MAX_SIZE - 1)




typedef struct as_error_s {
	

	as_status code;
	

	char message[AS_ERROR_MESSAGE_MAX_SIZE];
	

	const char * func;
	

	const char * file;
	

	uint32_t line;
} as_error;




#define as_error_update(__err, __code, __fmt, ...) \
	as_error_setallv( __err, __code, __func__, __FILE__, __LINE__, __fmt, ##__VA_ARGS__ );


#define as_error_set_message(__err, __code, __msg) \
	as_error_setall( __err, __code, __msg, __func__, __FILE__, __LINE__ );




static inline as_error * as_error_init(as_error * err) {
	err->code = AEROSPIKE_OK;
	err->message[0] = '\0';
	err->func = NULL;
	err->file = NULL;
	err->line = 0;
	return err;
}


static inline as_status as_error_reset(as_error * err) {
	err->code = AEROSPIKE_OK;
	err->message[0] = '\0';
	err->func = NULL;
	err->file = NULL;
	err->line = 0;
	return err->code;
}


static inline as_status as_error_setall(as_error * err, as_status code, const char * message, const char * func, const char * file, uint32_t line) {
	err->code = code;
	as_strncpy(err->message, message, AS_ERROR_MESSAGE_MAX_SIZE);
	err->func = func;
	err->file = file;
	err->line = line;
	return err->code;
}


static inline as_status as_error_setallv(as_error * err, as_status code, const char * func, const char * file, uint32_t line, const char * fmt, ...) {
	if ( fmt != NULL ) {
		va_list ap;
		va_start(ap, fmt);
		vsnprintf(err->message, AS_ERROR_MESSAGE_MAX_LEN, fmt, ap);
		err->message[AS_ERROR_MESSAGE_MAX_LEN] = '\0';
		va_end(ap);   
	}
	err->code = code;
	err->func = func;
	err->file = file;
	err->line = line;
	return err->code;
}


static inline as_status as_error_set(as_error * err, as_status code, const char * fmt, ...) {
	if ( fmt != NULL ) {
		va_list ap;
		va_start(ap, fmt);
		vsnprintf(err->message, AS_ERROR_MESSAGE_MAX_LEN, fmt, ap);
		err->message[AS_ERROR_MESSAGE_MAX_LEN] = '\0';
		va_end(ap);   
	}
	err->code = code;
	return err->code;
}


static inline void as_error_copy(as_error * trg, const as_error * src) {
	trg->code = src->code;
	strcpy(trg->message, src->message);
	trg->func = src->func;
	trg->file = src->file;
	trg->line = src->line;
}


char*
as_error_string(as_status status);


#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>


#if defined(AS_USE_LIBEV)
#include <ev.h>
#elif defined(AS_USE_LIBUV)
#include <uv.h>
#else
extern "C" {




typedef struct {
#if defined(AS_USE_LIBEV)
	struct ev_loop* loop;
	struct ev_async wakeup;
#elif defined(AS_USE_LIBUV)
	uv_loop_t* loop;
	uv_async_t* wakeup;
#else
	void* loop;
	pthread_mutex_t lock;
	as_queue queue;
	as_queue pipe_cb_queue;
	pthread_t thread;
	uint32_t index;
	bool pipe_cb_calling;
} as_event_loop;


extern as_event_loop* as_event_loops;
extern uint32_t as_event_loop_size;
extern uint32_t as_event_loop_current;




as_event_loop*
as_event_create_loops(uint32_t capacity);


bool
as_event_set_external_loop_capacity(uint32_t capacity);


as_event_loop*
as_event_set_external_loop(void* loop);


as_event_loop*
as_event_loop_find(void* loop);


static inline as_event_loop*
as_event_loop_get_by_index(uint32_t index)
{
	return index < as_event_loop_size ? &as_event_loops[index] : NULL;
}


static inline as_event_loop*
as_event_loop_get()
{
	// Increment is not atomic because it doesn't need to be exactly accurate.
	uint32_t current = as_event_loop_current++;
	return &as_event_loops[current % as_event_loop_size];
}


void
as_event_close_loops();
} // end extern "C"


#include <citrusleaf/cf_ll.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#if defined(AS_USE_LIBEV)
#include <ev.h>
#elif defined(AS_USE_LIBUV)
#include <uv.h>
#else
extern "C" {


#define AS_ASYNC_STATE_UNREGISTERED 0
#define AS_ASYNC_STATE_AUTH_WRITE 1
#define AS_ASYNC_STATE_AUTH_READ_HEADER 2
#define AS_ASYNC_STATE_AUTH_READ_BODY 4
#define AS_ASYNC_STATE_WRITE 8
#define AS_ASYNC_STATE_READ_HEADER 16
#define AS_ASYNC_STATE_READ_BODY 32
#define AS_ASYNC_AUTH_RETURN_CODE 1
#define AS_EVENT_CONNECTION_COMPLETE 0
#define AS_EVENT_CONNECTION_PENDING 1
#define AS_EVENT_CONNECTION_ERROR 2
#define AS_EVENT_QUEUE_INITIAL_CAPACITY 256
struct as_event_command;
struct as_event_executor;
typedef struct {
#if defined(AS_USE_LIBEV)
	struct ev_io watcher;
	int fd;
#elif defined(AS_USE_LIBUV)
	uv_tcp_t socket;
	// Reuse memory for requests, because only one request is active at a time.
	union {
		uv_connect_t connect;
		uv_write_t write;
	} req;
#else
	bool pipeline;
} as_event_connection;
typedef struct {
	as_event_connection base;
	struct as_event_command* cmd;
} as_async_connection;
typedef struct {
	as_pipe_listener listener;
	void* udata;
} as_queued_pipe_cb;
typedef bool (*as_event_parse_results_fn) (struct as_event_command* cmd);
typedef void (*as_event_executor_complete_fn) (struct as_event_executor* executor, as_error* err);
typedef void (*as_event_executor_destroy_fn) (struct as_event_executor* executor);
typedef struct as_event_command {
#if defined(AS_USE_LIBEV)
	struct ev_timer timer;
#elif defined(AS_USE_LIBUV)
	uv_timer_t timer;
#else
	as_event_loop* event_loop;
	as_event_connection* conn;
	as_cluster* cluster;
	as_node* node;
	void* udata;
	as_event_parse_results_fn parse_results;
	as_pipe_listener pipe_listener;
	cf_ll_element pipe_link;
	uint8_t* buf;
	uint32_t capacity;
	uint32_t len;
	uint32_t pos;
	uint32_t auth_len;
	uint32_t timeout_ms;
	uint8_t type;
	uint8_t state;
	bool deserialize;
	bool free_buf;
} as_event_command;
typedef struct as_event_executor {
	pthread_mutex_t lock;
	struct as_event_command** commands;
	as_event_loop* event_loop;
	as_event_executor_complete_fn complete_fn;
	void* udata;
	uint32_t max_concurrent;
	uint32_t max;
	uint32_t count;
	bool valid;
} as_event_executor;
typedef enum as_connection_status_e {
	AS_CONNECTION_FROM_POOL = 0,
	AS_CONNECTION_NEW = 1,
	AS_CONNECTION_TOO_MANY = 2
} as_connection_status;


extern as_event_loop* as_event_loops;
extern uint32_t as_event_loop_size;
extern uint32_t as_event_loop_current;


as_status
as_event_command_execute(as_event_command* cmd, as_error* err);
void
as_event_executor_complete(as_event_command* cmd);
void
as_event_executor_cancel(as_event_executor* executor, int queued_count);
as_connection_status
as_event_get_connection(as_event_command* cmd);
int
as_event_create_socket(as_event_command* cmd);
void
as_event_connect_error(as_event_command* cmd, as_error* err, int fd);
void
as_event_error_callback(as_event_command* cmd, as_error* err);
void
as_event_socket_error(as_event_command* cmd, as_error* err);
void
as_event_response_error(as_event_command* cmd, as_error* err);
void
as_event_timeout(as_event_command* cmd);
bool
as_event_command_parse_result(as_event_command* cmd);
bool
as_event_command_parse_header(as_event_command* cmd);
bool
as_event_command_parse_success_failure(as_event_command* cmd);


bool
as_event_create_loop(as_event_loop* event_loop);
void
as_event_register_external_loop(as_event_loop* event_loop);
bool
as_event_send(as_event_command* cmd);
void
as_event_command_begin(as_event_command* cmd);
void
as_event_close_connection(as_event_connection* conn);
void
as_event_node_destroy(as_node* node);
bool
as_event_send_close_loop(as_event_loop* event_loop);
void
as_event_close_loop(as_event_loop* event_loop);
static inline void
as_event_command_free(as_event_command* cmd)
{
	ck_pr_dec_32(&cmd->cluster->async_pending);
	as_node_release(cmd->node);
	if (cmd->free_buf) {
		cf_free(cmd->buf);
	}
	cf_free(cmd);
}


#if defined(AS_USE_LIBEV)
static inline int
as_event_validate_connection(as_event_connection* conn)
{
	return as_socket_validate(conn->fd);
}
static inline void
as_event_stop_timer(as_event_command* cmd)
{
	if (cmd->timeout_ms) {
		ev_timer_stop(cmd->event_loop->loop, &cmd->timer);
	}
}
static inline void
as_event_stop_watcher(as_event_command* cmd, as_event_connection* conn)
{
	ev_io_stop(cmd->event_loop->loop, &conn->watcher);
}
static inline void
as_event_command_release(as_event_command* cmd)
{
	as_event_command_free(cmd);
}


#elif defined(AS_USE_LIBUV)
static inline int
as_event_validate_connection(as_event_connection* conn)
{
	// Libuv does not have a peek function, so use fd directly.
	uv_os_fd_t fd;
	if (uv_fileno((uv_handle_t*)&conn->socket, &fd) == 0) {
		return as_socket_validate(fd);
	}
	return false;
}
static inline void
as_event_stop_timer(as_event_command* cmd)
{
	// Timer is stopped in libuv by uv_close which occurs later in as_event_command_release().
}
static inline void
as_event_stop_watcher(as_event_command* cmd, as_event_connection* conn)
{
	// Watcher already stopped by design in libuv.
}
void
as_uv_timer_closed(uv_handle_t* handle);
static inline void
as_event_command_release(as_event_command* cmd)
{
	if (cmd->timeout_ms) {
		// libuv requires that cmd can't be freed until timer is closed.
		uv_close((uv_handle_t*)&cmd->timer, as_uv_timer_closed);
	}
	else {
		as_event_command_free(cmd);
	}
}


#else
static inline int
as_event_validate_connection(as_event_connection* conn)
{
	return -1;
}
static inline void
as_event_stop_timer(as_event_command* cmd)
{
}
static inline void
as_event_stop_watcher(as_event_command* cmd, as_event_connection* conn)
{
}
static inline void
as_event_command_release(as_event_command* cmd)
{
}


static inline void
as_event_command_execute_in_loop(as_event_command* cmd)
{
	// Check if command timed out after coming off queue.
	if (cmd->timeout_ms && (cf_getms() - *(uint64_t*)cmd) > cmd->timeout_ms) {
		as_error err;
		as_error_set_message(&err, AEROSPIKE_ERR_TIMEOUT, as_error_string(AEROSPIKE_ERR_TIMEOUT));
		// Tell the libuv version of as_event_command_release() to not try to close the uv_timer_t.
		cmd->timeout_ms = 0;
		as_event_error_callback(cmd, &err);
		return;
	}
	// Start processing.
	as_event_command_begin(cmd);
}
static inline as_event_loop*
as_event_assign(as_event_loop* event_loop)
{
	if (! event_loop) {
		// Assign event loop using round robin distribution.
		// Not atomic because doesn't need to be exactly accurate.
		uint32_t current = as_event_loop_current++;
		event_loop = &as_event_loops[current % as_event_loop_size];
	}
	return event_loop;
}
static inline void
as_event_set_auth_write(as_event_command* cmd)
{
	// The command buffer was already allocated with enough space for max authentication size,
	// so just use the end of the write buffer for authentication bytes.
	cmd->pos = cmd->len;
	cmd->auth_len = as_authenticate_set(cmd->cluster->user, cmd->cluster->password, &cmd->buf[cmd->pos]);
	cmd->len = cmd->pos + cmd->auth_len;
}
static inline void
as_event_set_auth_read_header(as_event_command* cmd)
{
	// Authenticate response buffer is at end of write buffer.
	cmd->pos = cmd->len - cmd->auth_len;
	cmd->auth_len = sizeof(as_proto);
	cmd->len = cmd->pos + cmd->auth_len;
	cmd->state = AS_ASYNC_STATE_AUTH_READ_HEADER;
}
static inline void
as_event_set_auth_parse_header(as_event_command* cmd)
{
	// Authenticate response buffer is at end of write buffer.
	cmd->pos = cmd->len - cmd->auth_len;
	as_proto* proto = (as_proto*)&cmd->buf[cmd->pos];
	as_proto_swap_from_be(proto);
	cmd->auth_len = (uint32_t)proto->sz;
	cmd->len = cmd->pos + cmd->auth_len;
	cmd->state = AS_ASYNC_STATE_AUTH_READ_BODY;
}
static inline void
as_event_release_connection(as_cluster* cluster, as_event_connection* conn, as_queue* queue)
{
	as_event_close_connection(conn);
	ck_pr_dec_32(&cluster->async_conn_count);
	as_queue_decr_total(queue);
}
static inline void
as_event_decr_connection(as_cluster* cluster, as_queue* queue)
{
	ck_pr_dec_32(&cluster->async_conn_count);
	as_queue_decr_total(queue);
}
static inline void
as_event_decr_conn(as_event_command* cmd)
{
	as_queue* queue = cmd->pipe_listener != NULL ?
		&cmd->node->pipe_conn_qs[cmd->event_loop->index] :
		&cmd->node->async_conn_qs[cmd->event_loop->index];
	as_event_decr_connection(cmd->cluster, queue);
}
} // end extern "C"


extern "C" {




typedef struct as_name_value_s {
	char* name;
	char* value;
} as_name_value;
struct sockaddr_in;




as_status
as_info_command_node(as_error* err, as_node* node, char* command, bool send_asis, uint64_t deadline_ms, char** response);


as_status
as_info_command_host(as_cluster* cluster, as_error* err, struct sockaddr_in* sa_in, char* command,
	 bool send_asis, uint64_t deadline_ms, char** response);


as_status
as_info_command(as_error* err, int fd, char* names, bool send_asis, uint64_t deadline_ms,
				uint64_t max_response_length, char** values);


as_status
as_info_create_socket(as_cluster* cluster, as_error* err, struct sockaddr_in* sa_in,
					  uint64_t deadline_ms, int* fd_out);


as_status
as_info_parse_single_response(char *values, char **value);


void
as_info_parse_multi_response(char* buf, as_vector* 
 values);
} // end extern "C"


extern "C" {




typedef enum as_job_status_e {
	

	AS_JOB_STATUS_UNDEF,
	

	AS_JOB_STATUS_INPROGRESS,
	

	AS_JOB_STATUS_COMPLETED,
} as_job_status;


typedef struct as_job_info_s {
	

	as_job_status status;
	

	uint32_t progress_pct;
	

	uint32_t records_read;
} as_job_info;




as_status
aerospike_job_wait(
	aerospike* as, as_error* err, const as_policy_info* policy, const char* module, uint64_t job_id,
	uint32_t interval_ms
	);


as_status
aerospike_job_info(
	aerospike* as, as_error* err, const as_policy_info* policy, const char* module, uint64_t job_id,
	bool stop_if_in_progress, as_job_info * info
	);
} // end extern "C"


#include <stdbool.h>
#include <stdint.h>
extern "C" {




#define AS_DIGEST_VALUE_SIZE 20


#define AS_NAMESPACE_MAX_SIZE 32


#define AS_SET_MAX_SIZE 64




typedef char as_namespace[AS_NAMESPACE_MAX_SIZE];


typedef char as_set[AS_SET_MAX_SIZE];


typedef uint8_t as_digest_value[AS_DIGEST_VALUE_SIZE];


typedef struct as_digest_s {
	

	bool init;
	

	as_digest_value value;
} as_digest;


typedef union as_key_value_u {
	

	as_integer integer;
	

	as_string string;
	

	as_bytes bytes;
} as_key_value;


typedef struct as_key_s {
	

	bool _free;
	

	as_namespace ns;
	

	as_set set;
	

	as_key_value value;
	

	as_key_value * valuep;
	

	as_digest digest;
} as_key;




as_key * as_key_init(as_key * key, const as_namespace ns, const as_set set, const char * value);


as_key * as_key_init_int64(as_key * key, const as_namespace ns, const as_set set, int64_t value);


as_key * as_key_init_strp(as_key * key, const as_namespace ns, const as_set set, const char * value, bool free);


static inline as_key * as_key_init_str(as_key * key, const as_namespace ns, const as_set set, const char * value)
{
	return as_key_init_strp(key, ns, set, value, false);
}


as_key * as_key_init_rawp(as_key * key, const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size, bool free);


static inline as_key * as_key_init_raw(as_key * key, const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size)
{
	return as_key_init_rawp(key, ns, set, value, size, false);
}


as_key * as_key_init_digest(as_key * key, const as_namespace ns, const as_set set, const as_digest_value digest);


as_key * as_key_init_value(as_key * key, const as_namespace ns, const as_set set, const as_key_value * value);


as_key * as_key_new(const as_namespace ns, const as_set set, const char * value);


as_key * as_key_new_int64(const as_namespace ns, const as_set set, int64_t value);


as_key * as_key_new_strp(const as_namespace ns, const as_set set, const char * value, bool free);


static inline as_key * as_key_new_str(const as_namespace ns, const as_set set, const char * value)
{
	return as_key_new_strp(ns, set, value, false);
}


as_key * as_key_new_rawp(const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size, bool free);


static inline as_key * as_key_new_raw(const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size)
{
	return as_key_new_rawp(ns, set, value, size, false);
}


as_key * as_key_new_digest(const as_namespace ns, const as_set set, const as_digest_value digest);


as_key * as_key_new_value(const as_namespace ns, const as_set set, const as_key_value * value);


void as_key_destroy(as_key * key);


as_digest * as_key_digest(as_key * key);


as_status
as_key_set_digest(as_error* err, as_key* key);
} // end extern "C"


extern "C" {




typedef enum as_ldt_type_e {
	AS_LDT_LLIST,
	AS_LDT_LMAP,
	AS_LDT_LSET,
	AS_LDT_LSTACK
} as_ldt_type;




typedef struct as_ldt_s {
	

	bool _free;
	

	as_bin_name name;
	

	as_ldt_type type;
	

	as_udf_module_name module;
} as_ldt;




as_ldt * as_ldt_new(const as_bin_name name, const as_ldt_type type, const as_udf_module_name module);


as_ldt * as_ldt_init(as_ldt * ldt, const as_bin_name name, const as_ldt_type type, const as_udf_module_name module);


void as_ldt_destroy(as_ldt * ldt);
} // end extern "C"


extern "C" {


typedef void (*as_async_write_listener) (as_error* err, void* udata, as_event_loop* event_loop);


typedef void (*as_async_record_listener) (as_error* err, as_record* record, void* udata, as_event_loop* event_loop);


typedef void (*as_async_value_listener) (as_error* err, as_val* val, void* udata, as_event_loop* event_loop);


typedef void (*as_pipe_listener) (void* udata, as_event_loop* event_loop);
} // end extern "C"


extern "C" {




as_status
as_lookup(as_error* err, char* hostname, uint16_t port, as_vector* 
 addresses);
} // end extern "C"


#include <citrusleaf/cf_queue.h>
#include <netinet/in.h>
#include <sys/uio.h>
extern "C" {
// Concurrency kit needs to be under extern "C" when compiling C++.




#define AS_HOSTNAME_SIZE 256


#define AS_NODE_NAME_SIZE 20
// Leave this is in for backwards compatibility.
#define AS_NODE_NAME_MAX_SIZE AS_NODE_NAME_SIZE




typedef struct as_address_s {
	

	struct sockaddr_in addr;
	

	char name[INET_ADDRSTRLEN];
} as_address;
struct as_cluster_s;


typedef struct as_node_s {
	

	uint32_t ref_count;
	

	uint32_t partition_generation;
	

	char name[AS_NODE_NAME_SIZE];
	

	uint32_t address_index;
	

	as_vector 
 addresses;
	

	as_vector 
 aliases;
	struct as_cluster_s* cluster;
	

	cf_queue* conn_q;
	

	as_queue* async_conn_qs;
	

	as_queue* pipe_conn_qs;
	

	int info_fd;
	

	uint32_t conn_count;
	

	uint32_t friends;
	

	uint32_t failures;
	

	uint32_t index;
	

	uint8_t active;
	

	uint8_t has_batch_index;
	

	uint8_t has_replicas_all;
	

	uint8_t has_double;
	

	uint8_t has_geo;
} as_node;


typedef struct as_node_info_s {
	

	char name[AS_NODE_NAME_SIZE];
	

	int fd;
	

	uint8_t has_batch_index;
	

	uint8_t has_replicas_all;
	

	uint8_t has_double;
	

	uint8_t has_geo;
} as_node_info;


typedef struct as_host_s {
	

	char name[AS_HOSTNAME_SIZE];
	

	in_port_t port;
} as_host;




as_node*
as_node_create(struct as_cluster_s* cluster, as_host* host, struct sockaddr_in* addr, as_node_info* node_info);


void
as_node_destroy(as_node* node);


static inline void
as_node_deactivate(as_node* node)
{
	// Make volatile write so changes are reflected in other threads.
	ck_pr_store_8(&node->active, false);
}


static inline void
as_node_reserve(as_node* node)
{
	//ck_pr_fence_acquire();
	ck_pr_inc_32(&node->ref_count);
}


static inline void
as_node_release(as_node* node)
{
	//ck_pr_fence_release();
	bool destroy;
	ck_pr_dec_32_zero(&node->ref_count, &destroy);
	if (destroy) {
		as_node_destroy(node);
	}
}


void
as_node_add_address(as_node* node, as_host* host, struct sockaddr_in* addr);


static inline struct sockaddr_in*
as_node_get_address(as_node* node)
{
	as_address* address = (as_address *)as_vector_get(&node->addresses, node->address_index);
	return &address->addr;
}


static inline as_address*
as_node_get_address_full(as_node* node)
{
	return (as_address *)as_vector_get(&node->addresses, node->address_index);
}


as_status
as_node_get_connection(as_error* err, as_node* node, uint64_t deadline_ms, int* fd);


static inline void
as_node_close_connection(as_node* node, int fd) {
	close(fd);
	ck_pr_dec_32(&node->conn_count);
}


static inline void
as_node_put_connection(as_node* node, int fd)
{
	if (cf_queue_push(node->conn_q, &fd) != CF_QUEUE_OK) {
		as_node_close_connection(node, fd);
	}
}


static inline bool
as_host_equals(as_host* h1, as_host* h2)
{
	return strcmp(h1->name, h2->name) == 0 && h1->port == h2->port;
}
} // end extern "C"


#include <stdbool.h>
#include <stdint.h>
extern "C" {


typedef enum as_cdt_paramtype_e {
	AS_CDT_PARAM_INDEX = 3,
	AS_CDT_PARAM_COUNT = 4,
	AS_CDT_PARAM_PAYLOAD = 5,
	AS_CDT_PARAM_LAST_INDEX = 6,
	AS_CDT_PARAM_MAP_KEY = 11,
	AS_CDT_PARAM_MAP_VALUE = 12,
} as_cdt_paramtype;
typedef enum as_cdt_optype_e {
	// ------------------------------------------------------------------------
	// List Operation
	// Add to list
	AS_CDT_OP_LIST_APPEND        = 1,
	AS_CDT_OP_LIST_APPEND_ITEMS  = 2,
	AS_CDT_OP_LIST_INSERT        = 3,
	AS_CDT_OP_LIST_INSERT_ITEMS  = 4,
	// Remove from list
	AS_CDT_OP_LIST_POP           = 5,
	AS_CDT_OP_LIST_POP_RANGE     = 6,
	AS_CDT_OP_LIST_REMOVE        = 7,
	AS_CDT_OP_LIST_REMOVE_RANGE  = 8,
	// Other list modifies
	AS_CDT_OP_LIST_SET           = 9,
	AS_CDT_OP_LIST_TRIM          = 10,
	AS_CDT_OP_LIST_CLEAR         = 11,
	AS_CDT_OP_LIST_INCREMENT_BY  = 12,
	// Read from list
	AS_CDT_OP_LIST_SIZE          = 16,
	AS_CDT_OP_LIST_GET           = 17,
	AS_CDT_OP_LIST_GET_RANGE     = 18,
	// ------------------------------------------------------------------------
	// Map Operation
	// Adding <key, value> to the Map
	AS_CDT_OP_MAP_PUT            = 32,
	AS_CDT_OP_MAP_PUT_ITEMS      = 33,
	// Op by key
	AS_CDT_OP_MAP_GET            = 34,
	AS_CDT_OP_MAP_GET_MATCHING   = 35,
	AS_CDT_OP_MAP_REMOVE         = 36,
	AS_CDT_OP_MAP_REMOVE_ITEMS   = 37,
	AS_CDT_OP_MAP_CONTAINS_KEY   = 38,
	AS_CDT_OP_MAP_INCREMENT_BY   = 39,
	AS_CDT_OP_MAP_CONTAINS_VALUE = 40,
	// Misc
	AS_CDT_OP_MAP_GET_ITEMS      = 41,
	AS_CDT_OP_MAP_KEYS           = 42,
	AS_CDT_OP_MAP_VALUES         = 43,
	AS_CDT_OP_MAP_CLEAR          = 44,
	AS_CDT_OP_MAP_SIZE           = 45,
} as_cdt_optype;


typedef enum as_operator_e {
	

	AS_OPERATOR_READ       = 1,
	

	AS_OPERATOR_WRITE      = 2,
	AS_OPERATOR_CDT_READ   = 3,
	AS_OPERATOR_CDT_MODIFY = 4,
	

	AS_OPERATOR_INCR       = 5,
	

	AS_OPERATOR_APPEND     = 9,
	

	AS_OPERATOR_PREPEND    = 10,
	

	AS_OPERATOR_TOUCH      = 11
} as_operator;


typedef struct as_binop_s {
	

	as_operator op;
	

	as_bin bin;
} as_binop;


typedef struct as_binops_s {
	

	bool _free;
	

	uint16_t capacity;
	

	uint16_t size;
	

	as_binop * entries;
} as_binops;


typedef struct as_operations_s {
	

	bool _free;
	

	uint16_t gen;
	

	uint32_t ttl;
	

	as_binops binops;
} as_operations;




#define as_operations_inita(__ops, __nops) \
	(__ops)->_free = false;\
	(__ops)->gen = 0;\
	(__ops)->ttl = 0;\
	(__ops)->binops._free = false;\
	(__ops)->binops.capacity = __nops;\
	(__ops)->binops.size = 0;\
	(__ops)->binops.entries = (as_binop *) alloca(sizeof(as_binop) * __nops);




as_operations * as_operations_init(as_operations * ops, uint16_t nops);


as_operations * as_operations_new(uint16_t nops);


void as_operations_destroy(as_operations * ops);


bool as_operations_add_write(as_operations * ops, const as_bin_name name, as_bin_value * value);


bool as_operations_add_write_int64(as_operations * ops, const as_bin_name name, int64_t value);


bool as_operations_add_write_double(as_operations * ops, const as_bin_name name, double value);


bool as_operations_add_write_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);


static inline bool as_operations_add_write_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_write_strp(ops, name, value, false);
}


bool as_operations_add_write_geojson_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);


static inline bool as_operations_add_write_geojson_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_write_geojson_strp(ops, name, value, false);
}


bool as_operations_add_write_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);


static inline bool as_operations_add_write_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_write_rawp(ops, name, value, size, false);
}


bool as_operations_add_read(as_operations * ops, const as_bin_name name);


bool as_operations_add_incr(as_operations * ops, const as_bin_name name, int64_t value);


bool as_operations_add_incr_double(as_operations * ops, const as_bin_name name, double value);


bool as_operations_add_prepend_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);


static inline bool as_operations_add_prepend_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_prepend_strp(ops, name, value, false);
}


bool as_operations_add_prepend_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);


static inline bool as_operations_add_prepend_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_prepend_rawp(ops, name, value, size, false);
}


bool as_operations_add_append_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);


static inline bool as_operations_add_append_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_append_strp(ops, name, value, false);
}


bool as_operations_add_append_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);


static inline bool as_operations_add_append_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_append_rawp(ops, name, value, size, false);
}


bool as_operations_add_touch(as_operations * ops);




bool as_operations_add_list_append(as_operations *ops, const as_bin_name name, as_val *val);


bool as_operations_add_list_append_int64(as_operations *ops, const as_bin_name name, int64_t value);


bool as_operations_add_list_append_double(as_operations *ops, const as_bin_name name, double value);


bool as_operations_add_list_append_strp(as_operations *ops, const as_bin_name name, const char *value, bool free);


static inline bool as_operations_add_list_append_str(as_operations *ops, const as_bin_name name, const char *value)
{
	return as_operations_add_list_append_strp(ops, name, value, false);
}


bool as_operations_add_list_append_rawp(as_operations *ops, const as_bin_name name, const uint8_t *value, uint32_t size, bool free);


static inline bool as_operations_add_list_append_raw(as_operations *ops, const as_bin_name name, const uint8_t *value, uint32_t size)
{
	return as_operations_add_list_append_rawp(ops, name, value, size, false);
}


bool as_operations_add_list_append_items(as_operations *ops, const as_bin_name name, as_list *list);


bool as_operations_add_list_insert(as_operations *ops, const as_bin_name name, int64_t index, as_val *val);


bool as_operations_add_list_insert_int64(as_operations *ops, const as_bin_name name, int64_t index, int64_t value);


bool as_operations_add_list_insert_double(as_operations *ops, const as_bin_name name, int64_t index, double value);


bool as_operations_add_list_insert_strp(as_operations *ops, const as_bin_name name, int64_t index, const char *value, bool free);


static inline bool as_operations_add_list_insert_str(as_operations *ops, const as_bin_name name, int64_t index, const char *value)
{
	return as_operations_add_list_insert_strp(ops, name, index, value, false);
}


bool as_operations_add_list_insert_rawp(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size, bool free);


static inline bool as_operations_add_list_insert_raw(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size)
{
	return as_operations_add_list_insert_rawp(ops, name, index, value, size, false);
}


bool as_operations_add_list_insert_items(as_operations *ops, const as_bin_name name, int64_t index, as_list *list);


bool as_operations_add_list_pop(as_operations *ops, const as_bin_name name, int64_t index);


bool as_operations_add_list_pop_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);


bool as_operations_add_list_pop_range_from(as_operations *ops, const as_bin_name name, int64_t index);


bool as_operations_add_list_remove(as_operations *ops, const as_bin_name name, int64_t index);


bool as_operations_add_list_remove_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);


bool as_operations_add_list_remove_range_from(as_operations *ops, const as_bin_name name, int64_t index);
//-----------------------------------------------------------------------------
// Other list modifies


bool as_operations_add_list_clear(as_operations *ops, const as_bin_name name);


bool as_operations_add_list_set(as_operations *ops, const as_bin_name name, int64_t index, as_val *val);


bool as_operations_add_list_set_int64(as_operations *ops, const as_bin_name name, int64_t index, int64_t value);


bool as_operations_add_list_set_double(as_operations *ops, const as_bin_name name, int64_t index, double value);


bool as_operations_add_list_set_strp(as_operations *ops, const as_bin_name name, int64_t index, const char *value, bool free);


static inline bool as_operations_add_list_set_str(as_operations *ops, const as_bin_name name, int64_t index, const char *value)
{
	return as_operations_add_list_set_strp(ops, name, index, value, false);
}


bool as_operations_add_list_set_rawp(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size, bool free);


static inline bool as_operations_add_list_set_raw(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size)
{
	return as_operations_add_list_set_rawp(ops, name, index, value, size, false);
}


bool as_operations_add_list_trim(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);
//-----------------------------------------------------------------------------
// Read operations


bool as_operations_add_list_get(as_operations *ops, const as_bin_name name, int64_t index);


bool as_operations_add_list_get_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);


bool as_operations_add_list_get_range_from(as_operations *ops, const as_bin_name name, int64_t index);


bool as_operations_add_list_size(as_operations *ops, const as_bin_name name);
} // end extern "C"


extern "C" {




#define AS_MAX_NAMESPACE_SIZE 32




typedef struct as_partition_s {
	

	as_node* master;
	

	as_node* prole;
} as_partition;


typedef struct as_partition_table_s {
	

	char ns[AS_MAX_NAMESPACE_SIZE];
	

	uint32_t size;
	

	as_partition partitions[];
} as_partition_table;


typedef struct as_partition_tables_s {
	

	uint32_t ref_count;
	

	uint32_t size;
	

	as_partition_table* array[];
} as_partition_tables;




as_partition_tables*
as_partition_tables_create(uint32_t capacity);


void
as_partition_table_destroy(as_partition_table* table);


as_partition_table*
as_partition_tables_get(as_partition_tables* tables, const char* ns);


bool
as_partition_tables_find_node(as_partition_tables* tables, as_node* node);


static inline uint32_t
as_partition_getid(const uint8_t* digest, uint32_t n_partitions)
{
	return (*(uint16_t*)digest) & (n_partitions - 1);
}
} // end extern "C"


#include <citrusleaf/alloc.h>
#include <citrusleaf/cf_ll.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
typedef struct as_pipe_connection {
	as_event_connection base;
	as_event_command* writer;
	cf_ll readers;
	bool canceled;
	bool in_pool;
} as_pipe_connection;
extern int
as_pipe_get_send_buffer_size();
extern int
as_pipe_get_recv_buffer_size();
extern as_connection_status
as_pipe_get_connection(as_event_command* cmd);
extern void
as_pipe_socket_error(as_event_command* cmd, as_error* err);
extern void
as_pipe_timeout(as_event_command* cmd);
extern void
as_pipe_response_error(as_event_command* cmd, as_error* err);
extern void
as_pipe_response_complete(as_event_command* cmd);
extern void
as_pipe_write_start(as_event_command* cmd);
extern void
as_pipe_read_start(as_event_command* cmd);
static inline as_event_command*
as_pipe_link_to_command(cf_ll_element* link)
{
	return (as_event_command*)((uint8_t*)link - offsetof(as_event_command, pipe_link));
}




#include <stdbool.h>
#include <stdint.h>
extern "C" {




#define AS_POLICY_TIMEOUT_DEFAULT 1000


#define AS_POLICY_RETRY_DEFAULT 1


#define AS_POLICY_COMPRESSION_THRESHOLD_DEFAULT 0


#define AS_POLICY_GEN_DEFAULT AS_POLICY_GEN_IGNORE


#define AS_POLICY_KEY_DEFAULT AS_POLICY_KEY_DIGEST


#define AS_POLICY_EXISTS_DEFAULT AS_POLICY_EXISTS_IGNORE


#define AS_POLICY_REPLICA_DEFAULT AS_POLICY_REPLICA_MASTER


#define AS_POLICY_CONSISTENCY_LEVEL_DEFAULT AS_POLICY_CONSISTENCY_LEVEL_ONE


#define AS_POLICY_COMMIT_LEVEL_DEFAULT AS_POLICY_COMMIT_LEVEL_ALL




typedef enum as_policy_retry_e {
	

	AS_POLICY_RETRY_NONE, 
	

	AS_POLICY_RETRY_ONCE, 
} as_policy_retry;


typedef enum as_policy_gen_e {
	

	AS_POLICY_GEN_IGNORE,
	

	AS_POLICY_GEN_EQ,
	

	AS_POLICY_GEN_GT
} as_policy_gen;


typedef enum as_policy_key_e {
	

	AS_POLICY_KEY_DIGEST,
	

	AS_POLICY_KEY_SEND,
} as_policy_key;


typedef enum as_policy_exists_e {
	

	AS_POLICY_EXISTS_IGNORE,
	

	AS_POLICY_EXISTS_CREATE,
	

	AS_POLICY_EXISTS_UPDATE,
	

	AS_POLICY_EXISTS_REPLACE,
	

	AS_POLICY_EXISTS_CREATE_OR_REPLACE
} as_policy_exists;


typedef enum as_policy_replica_e {
	

	AS_POLICY_REPLICA_MASTER,
	

	AS_POLICY_REPLICA_ANY
} as_policy_replica;


typedef enum as_policy_consistency_level_e {
	

	AS_POLICY_CONSISTENCY_LEVEL_ONE,
	

	AS_POLICY_CONSISTENCY_LEVEL_ALL,
} as_policy_consistency_level;


typedef enum as_policy_commit_level_e {
	

	AS_POLICY_COMMIT_LEVEL_ALL,
	

	AS_POLICY_COMMIT_LEVEL_MASTER,
} as_policy_commit_level;


typedef struct as_policy_write_s {
	

	uint32_t timeout;
	

	uint32_t retry;
	

	uint32_t compression_threshold;
	

	as_policy_key key;
	

	as_policy_gen gen;
	

	as_policy_exists exists;
	

	as_policy_commit_level commit_level;
} as_policy_write;


typedef struct as_policy_read_s {
	

	uint32_t timeout;
	

	uint32_t retry;
	

	as_policy_key key;
	

	as_policy_replica replica;
	

	as_policy_consistency_level consistency_level;
	

	bool deserialize;
} as_policy_read;


typedef struct as_policy_apply_s {
	

	uint32_t timeout;
	

	as_policy_key key;
	

	as_policy_commit_level commit_level;
	

	uint32_t ttl;
} as_policy_apply;


typedef struct as_policy_operate_s {
	

	uint32_t timeout;
	

	uint32_t retry;
	

	as_policy_key key;
	

	as_policy_gen gen;
	

	as_policy_replica replica;
	

	as_policy_consistency_level consistency_level;
	

	as_policy_commit_level commit_level;
	

	bool deserialize;
} as_policy_operate;


typedef struct as_policy_remove_s {
	

	uint32_t timeout;
	

	uint16_t generation;
	

	uint32_t retry;
	

	as_policy_key key;
	

	as_policy_gen gen;
	

	as_policy_commit_level commit_level;
} as_policy_remove;


typedef struct as_policy_query_s {
	

	uint32_t timeout;
	

	bool deserialize;
} as_policy_query;


typedef struct as_policy_scan_s {
	

	uint32_t timeout;
	

	bool fail_on_cluster_change;
} as_policy_scan;


typedef struct as_policy_info_s {
	

	uint32_t timeout;
	

	bool send_as_is;
	

	bool check_bounds;
} as_policy_info;


typedef struct as_policy_batch_s {
	

	uint32_t timeout;
	

	bool concurrent;
	

	bool use_batch_direct;
	

	bool allow_inline;
	

	bool send_set_name;
	

	bool deserialize;
} as_policy_batch;


typedef struct as_policy_admin_s {
	

	uint32_t timeout;
} as_policy_admin;


typedef struct as_policies_s {
	

	

	uint32_t timeout;
	

	uint32_t retry;
	

	as_policy_key key;
	

	as_policy_gen gen;
	

	as_policy_exists exists;
	

	as_policy_replica replica;
	

	as_policy_consistency_level consistency_level;
	

	as_policy_commit_level commit_level;
	

	

	as_policy_read read;
	

	as_policy_write write;
	

	as_policy_operate operate;
	

	as_policy_remove remove;
	

	as_policy_apply apply;
	

	as_policy_query query;
	

	as_policy_scan scan;
	

	as_policy_info info;
	

	as_policy_batch batch;
	

	as_policy_admin admin;
} as_policies;




static inline as_policy_read*
as_policy_read_init(as_policy_read* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->retry = AS_POLICY_RETRY_DEFAULT;
	p->key = AS_POLICY_KEY_DEFAULT;
	p->replica = AS_POLICY_REPLICA_DEFAULT;
	p->consistency_level = AS_POLICY_CONSISTENCY_LEVEL_DEFAULT;
	p->deserialize = true;
	return p;
}


static inline void
as_policy_read_copy(as_policy_read* src, as_policy_read* trg)
{
	trg->timeout = src->timeout;
	trg->retry = src->retry;
	trg->key = src->key;
	trg->replica = src->replica;
	trg->consistency_level = src->consistency_level;
	trg->deserialize = src->deserialize;
}


static inline as_policy_write*
as_policy_write_init(as_policy_write* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->retry = AS_POLICY_RETRY_DEFAULT;
	p->compression_threshold = AS_POLICY_COMPRESSION_THRESHOLD_DEFAULT;
	p->key = AS_POLICY_KEY_DEFAULT;
	p->gen = AS_POLICY_GEN_DEFAULT;
	p->exists = AS_POLICY_EXISTS_DEFAULT;
	p->commit_level = AS_POLICY_COMMIT_LEVEL_DEFAULT;
	return p;
}


static inline void
as_policy_write_copy(as_policy_write* src, as_policy_write* trg)
{
	trg->timeout = src->timeout;
	trg->retry = src->retry;
	trg->compression_threshold = src->compression_threshold;
	trg->key = src->key;
	trg->gen = src->gen;
	trg->exists = src->exists;
	trg->commit_level = src->commit_level;
}


static inline as_policy_operate*
as_policy_operate_init(as_policy_operate* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->retry = AS_POLICY_RETRY_DEFAULT;
	p->key = AS_POLICY_KEY_DEFAULT;
	p->gen = AS_POLICY_GEN_DEFAULT;
	p->replica = AS_POLICY_REPLICA_DEFAULT;
	p->consistency_level = AS_POLICY_CONSISTENCY_LEVEL_DEFAULT;
	p->commit_level = AS_POLICY_COMMIT_LEVEL_DEFAULT;
	p->deserialize = true;
	return p;
}


static inline void
as_policy_operate_copy(as_policy_operate* src, as_policy_operate* trg)
{
	trg->timeout = src->timeout;
	trg->retry = src->retry;
	trg->key = src->key;
	trg->gen = src->gen;
	trg->replica = src->replica;
	trg->consistency_level = src->consistency_level;
	trg->commit_level = src->commit_level;
	trg->deserialize = src->deserialize;
}


static inline as_policy_remove*
as_policy_remove_init(as_policy_remove* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->retry = AS_POLICY_RETRY_DEFAULT;
	p->key = AS_POLICY_KEY_DEFAULT;
	p->gen = AS_POLICY_GEN_DEFAULT;
	p->generation = 0;
	p->commit_level = AS_POLICY_COMMIT_LEVEL_DEFAULT;
	return p;
}


static inline void
as_policy_remove_copy(as_policy_remove* src, as_policy_remove* trg)
{
	trg->timeout = src->timeout;
	trg->retry = src->retry;
	trg->key = src->key;
	trg->gen = src->gen;
	trg->generation = src->generation;
	trg->commit_level = src->commit_level;
}


static inline as_policy_apply*
as_policy_apply_init(as_policy_apply* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->key = AS_POLICY_KEY_DEFAULT;
	p->commit_level = AS_POLICY_COMMIT_LEVEL_DEFAULT;
	p->ttl = 0; // AS_RECORD_DEFAULT_TTL
	return p;
}


static inline void
as_policy_apply_copy(as_policy_apply* src, as_policy_apply* trg)
{
	trg->timeout = src->timeout;
	trg->key = src->key;
	trg->commit_level = src->commit_level;
	trg->ttl = src->ttl;
}


static inline as_policy_info*
as_policy_info_init(as_policy_info* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->send_as_is = true;
	p->check_bounds	= true;
	return p;
}


static inline void
as_policy_info_copy(as_policy_info* src, as_policy_info* trg)
{
	trg->timeout = src->timeout;
	trg->send_as_is = src->send_as_is;
	trg->check_bounds = src->check_bounds;
}


static inline as_policy_batch*
as_policy_batch_init(as_policy_batch* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->concurrent = false;
	p->use_batch_direct = false;
	p->allow_inline = true;
	p->send_set_name = false;
	p->deserialize = true;
	return p;
}


static inline void
as_policy_batch_copy(as_policy_batch* src, as_policy_batch* trg)
{
	trg->timeout = src->timeout;
	trg->concurrent = src->concurrent;
	trg->use_batch_direct = src->use_batch_direct;
	trg->allow_inline = src->allow_inline;
	trg->send_set_name = src->send_set_name;
	trg->deserialize = src->deserialize;
}


static inline as_policy_admin*
as_policy_admin_init(as_policy_admin* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	return p;
}


static inline void
as_policy_admin_copy(as_policy_admin* src, as_policy_admin* trg)
{
	trg->timeout = src->timeout;
}


static inline as_policy_scan*
as_policy_scan_init(as_policy_scan* p)
{
	p->timeout = 0;
	p->fail_on_cluster_change = false;
	return p;
}


static inline void
as_policy_scan_copy(as_policy_scan* src, as_policy_scan* trg)
{
	trg->timeout = src->timeout;
	trg->fail_on_cluster_change = src->fail_on_cluster_change;
}


static inline as_policy_query*
as_policy_query_init(as_policy_query* p)
{
	p->timeout = 0;
	p->deserialize = true;
	return p;
}


static inline void
as_policy_query_copy(as_policy_query* src, as_policy_query* trg)
{
	trg->timeout = src->timeout;
	trg->deserialize = src->deserialize;
}


as_policies*
as_policies_init(as_policies* p);


void
as_policies_resolve(as_policies* p);
} // end extern "C"


#include <stddef.h>
#include <stdint.h>
extern "C" {


#if defined(__APPLE__) || defined(CF_WINDOWS)
typedef struct as_proto_s {
	uint64_t	version	:8;
	uint64_t	type	:8;
	uint64_t	sz		:48;
} as_proto;
typedef struct as_compressed_proto_s {
	as_proto	proto;
	uint64_t	uncompressed_sz;
} as_compressed_proto;
typedef struct as_msg_s {

	uint8_t		header_sz;			// number of uint8_ts in this header

	uint8_t		info1;				// bitfield about this request

	uint8_t		info2;

	uint8_t		info3;

	uint8_t		unused;

	uint8_t		result_code;

	uint32_t	generation;

	uint32_t	record_ttl;

	uint32_t	transaction_ttl;

	uint16_t	n_fields;			// size in uint8_ts

	uint16_t	n_ops;				// number of operations

	uint8_t		data[0];			// data contains first the fields, then the ops
} as_msg;
typedef struct as_proto_msg_s {
	as_proto  	proto;
	as_msg		m;
} as_proto_msg;
#else
typedef struct as_proto_s {
	uint8_t		version;
	uint8_t		type;
	uint64_t	sz:48;
	uint8_t		data[];
} __attribute__ ((__packed__)) as_proto;
typedef struct as_compressed_proto_s {
	as_proto	proto;
	uint64_t	uncompressed_sz;
	uint8_t		data[];					// compressed bytes
} __attribute__((__packed__)) as_compressed_proto;
typedef struct as_msg_s {

	uint8_t		header_sz;			// number of uint8_ts in this header

	uint8_t		info1;				// bitfield about this request

	uint8_t		info2;

	uint8_t		info3;

	uint8_t		unused;

	uint8_t		result_code;

	uint32_t	generation;

	uint32_t	record_ttl;

	uint32_t	transaction_ttl;

	uint16_t	n_fields;			// size in uint8_ts

	uint16_t	n_ops;				// number of operations

	uint8_t		data[];				// data contains first the fields, then the ops
} __attribute__((__packed__)) as_msg;
typedef struct as_proto_msg_s {
	as_proto  	proto;
	as_msg		m;
} __attribute__((__packed__)) as_proto_msg;


void as_proto_swap_to_be(as_proto *m);
void as_proto_swap_from_be(as_proto *m);
void as_msg_swap_header_from_be(as_msg *m);
} // end extern "C"


#include <stdarg.h>
extern "C" {




#define as_string_equals(__val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_DEFAULT, AS_INDEX_STRING, __val


#define as_integer_equals(__val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_DEFAULT, AS_INDEX_NUMERIC, (int64_t)__val


#define as_integer_range(__min, __max) AS_PREDICATE_RANGE, AS_INDEX_TYPE_DEFAULT, AS_INDEX_NUMERIC, (int64_t)__min, (int64_t)__max


#define as_range(indextype, datatype, __min, __max) AS_PREDICATE_RANGE, AS_INDEX_TYPE_ ##indextype, AS_INDEX_ ##datatype, __min, __max


#define as_contains(indextype, datatype, __val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_ ##indextype, AS_INDEX_ ##datatype, __val


#define as_equals(datatype, __val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_DEFAULT, AS_INDEX_ ##datatype, __val
#define as_geo_within(__val) AS_PREDICATE_RANGE, AS_INDEX_TYPE_DEFAULT, AS_INDEX_GEO2DSPHERE, __val
#define as_geo_contains(__val) AS_PREDICATE_RANGE, AS_INDEX_TYPE_DEFAULT, AS_INDEX_GEO2DSPHERE, __val




typedef union as_predicate_value_u {
	

	char * string;
	

	int64_t integer;
	

	struct {
		

		int64_t min;
		

		int64_t max;
	} integer_range;
} as_predicate_value;


typedef enum as_predicate_type_e {
	

	AS_PREDICATE_EQUAL,
	AS_PREDICATE_RANGE
} as_predicate_type;


typedef struct as_predicate_s {
	

	as_bin_name bin;
	

	as_predicate_type type;
	

	as_predicate_value value;
	

	as_index_datatype dtype;
	

	as_index_type itype;
} as_predicate;


typedef enum as_order_e {
	

	AS_ORDER_ASCENDING = 0,
	

	AS_ORDER_DESCENDING = 1
} as_order;


typedef struct as_ordering_s {
	

	as_bin_name bin;
	

	as_order order;
} as_ordering;


typedef struct as_query_bins_s {
	

	bool _free;
	

	uint16_t capacity;
	

	uint16_t size;
	

	as_bin_name * entries;
} as_query_bins;


typedef struct as_query_predicates_s {
	

	bool _free;
	

	uint16_t capacity;
	

	uint16_t size;
	

	as_predicate * 	entries;
} as_query_predicates;


typedef struct as_query_sort_s {
	

	bool _free;
	

	uint16_t capacity;
	

	uint16_t size;
	

	as_ordering * entries;
} as_query_ordering;


typedef struct as_query_s {
	

	bool _free;
	

	as_namespace ns;
	

	as_set set;
	

	as_query_bins select;
	

	as_query_predicates where;
	

	as_query_ordering orderby;
	

	as_udf_call apply;
} as_query;




as_query * as_query_init(as_query * query, const as_namespace ns, const as_set set);


as_query * as_query_new(const as_namespace ns, const as_set set);


void as_query_destroy(as_query * query);




#define as_query_select_inita(__query, __n) \
	if ( (__query) != NULL && (__query)->select.entries == NULL ) {\
		(__query)->select.entries = (as_bin_name *) alloca(__n * sizeof(as_bin_name));\
		if ( (__query)->select.entries ) { \
			(__query)->select._free = false;\
			(__query)->select.capacity = __n;\
			(__query)->select.size = 0;\
		}\
 	}


bool as_query_select_init(as_query * query, uint16_t n);


bool as_query_select(as_query * query, const char * bin);




#define as_query_where_inita(__query, __n) \
	if ( (__query)  != NULL && (__query)->where.entries == NULL ) {\
		(__query)->where.entries = (as_predicate *) alloca(__n * sizeof(as_predicate));\
		if ( (__query)->where.entries ) { \
			(__query)->where._free = false;\
			(__query)->where.capacity = __n;\
			(__query)->where.size = 0;\
		}\
 	}


bool as_query_where_init(as_query * query, uint16_t n);


bool as_query_where(as_query * query, const char * bin, as_predicate_type type, as_index_type itype, as_index_datatype dtype, ... );




#define as_query_orderby_inita(__query, __n) \
	if ( (__query) != NULL && (__query)->orderby.entries == NULL  ) {\
		(__query)->orderby.entries = (as_ordering *) alloca(__n * sizeof(as_ordering));\
		if ( (__query)->orderby.entries ) { \
			(__query)->orderby._free = false;\
			(__query)->orderby.capacity = __n;\
			(__query)->orderby.size = 0;\
		}\
 	}


bool as_query_orderby_init(as_query * query, uint16_t n);


bool as_query_orderby(as_query * query, const char * bin, as_order order);




bool as_query_apply(as_query * query, const char * module, const char * function, const as_list * arglist);
} // end extern "C"


#include <stdbool.h>
#include <stdint.h>
extern "C" {




typedef struct as_record_s {
	

	as_rec _;
	

	as_key key;
	

	uint16_t gen;
	

	uint32_t ttl;
	

	as_bins bins;
} as_record;


#define AS_RECORD_DEFAULT_TTL 0


#define AS_RECORD_NO_EXPIRE_TTL 0xFFFFFFFF




#define as_record_inita(__rec, __nbins) \
	as_record_init(__rec, 0);\
	(__rec)->bins._free = false;\
	(__rec)->bins.capacity = __nbins;\
	(__rec)->bins.size = 0;\
	(__rec)->bins.entries = (as_bin *) alloca(sizeof(as_bin) * __nbins);




as_record * as_record_new(uint16_t nbins);


as_record * as_record_init(as_record * rec, uint16_t nbins);


void as_record_destroy(as_record * rec);


uint16_t as_record_numbins(const as_record * rec);


bool as_record_set(as_record * rec, const as_bin_name name, as_bin_value * value);


bool as_record_set_int64(as_record * rec, const as_bin_name name, int64_t value);


bool as_record_set_double(as_record * rec, const as_bin_name name, double value);


bool as_record_set_strp(as_record * rec, const as_bin_name name, const char * value, bool free);


static inline bool as_record_set_str(as_record * rec, const as_bin_name name, const char * value)
{
	return as_record_set_strp(rec, name, value, false);
}


bool as_record_set_geojson_strp(as_record * rec, const as_bin_name name, const char * value, bool free);


static inline bool as_record_set_geojson_str(as_record * rec, const as_bin_name name, const char * value)
{
	return as_record_set_geojson_strp(rec, name, value, false);
}


bool as_record_set_rawp(as_record * rec, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);


bool as_record_set_raw_typep(as_record * rec, const as_bin_name name, const uint8_t * value, uint32_t size, as_bytes_type type, bool free);


static inline bool as_record_set_raw(as_record * rec, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_record_set_rawp(rec, name, value, size, false);
}


bool as_record_set_integer(as_record * rec, const as_bin_name name, as_integer * value);


bool as_record_set_as_double(as_record * rec, const as_bin_name name, as_double * value);


bool as_record_set_string(as_record * rec, const as_bin_name name, as_string * value);


bool as_record_set_geojson(as_record * rec, const as_bin_name name, as_geojson * value);


bool as_record_set_bytes(as_record * rec, const as_bin_name name, as_bytes * value);


bool as_record_set_list(as_record * rec, const as_bin_name name, as_list * value);


bool as_record_set_map(as_record * rec, const as_bin_name name, as_map * value);


bool as_record_set_nil(as_record * rec, const as_bin_name name);


as_bin_value * as_record_get(const as_record * rec, const as_bin_name name);


int64_t as_record_get_int64(const as_record * rec, const as_bin_name name, int64_t fallback);


double as_record_get_double(const as_record * rec, const as_bin_name name, double fallback);


char * as_record_get_str(const as_record * rec, const as_bin_name name);


char * as_record_get_geojson_str(const as_record * rec, const as_bin_name name);


as_integer * as_record_get_integer(const as_record * rec, const as_bin_name name);


as_double * as_record_get_as_double(const as_record * rec, const as_bin_name name);


as_string * as_record_get_string(const as_record * rec, const as_bin_name name);


as_geojson * as_record_get_geojson(const as_record * rec, const as_bin_name name);


as_bytes * as_record_get_bytes(const as_record * rec, const as_bin_name name);


as_list * as_record_get_list(const as_record * rec, const as_bin_name name);


as_map * as_record_get_map(const as_record * rec, const as_bin_name name);




bool as_record_foreach(const as_record * rec, as_rec_foreach_callback callback, void * udata);




static inline as_val * as_record_toval(const as_record * rec) 
{
	return (as_val *) rec;
}


static inline as_record * as_record_fromval(const as_val * v) 
{
	return (as_record *) as_util_fromval(v, AS_REC, as_rec);
}
} // end extern "C"


#include <stdbool.h>
#include <stdint.h>
extern "C" {




typedef struct as_record_iterator_s {
	

	bool _free;
	

	const as_record * record;
	

	uint32_t pos;
} as_record_iterator;




as_record_iterator * as_record_iterator_new(const as_record * record);


as_record_iterator * as_record_iterator_init(as_record_iterator * iterator, const as_record * record);


void as_record_iterator_destroy(as_record_iterator * iterator);


bool as_record_iterator_has_next(const as_record_iterator * iterator);


as_bin * as_record_iterator_next(as_record_iterator * iterator);
} // end extern "C"


extern "C" {




#define AS_SCAN_PRIORITY_DEFAULT AS_SCAN_PRIORITY_AUTO


#define AS_SCAN_PERCENT_DEFAULT 100


#define AS_SCAN_NOBINS_DEFAULT false


#define AS_SCAN_CONCURRENT_DEFAULT false


#define AS_SCAN_INCLUDE_LDT_DEFAULT false


#define AS_SCAN_DESERIALIZE_DEFAULT true




typedef enum as_scan_priority_e { 
	

	AS_SCAN_PRIORITY_AUTO, 
	

	AS_SCAN_PRIORITY_LOW,
	
 
	AS_SCAN_PRIORITY_MEDIUM,
	
 
	AS_SCAN_PRIORITY_HIGH
} as_scan_priority;


typedef enum as_scan_status_e {
	

	AS_SCAN_STATUS_UNDEF,
	

	AS_SCAN_STATUS_INPROGRESS,
	

	AS_SCAN_STATUS_ABORTED,
	

	AS_SCAN_STATUS_COMPLETED,
} as_scan_status;


typedef struct as_scan_info_s {
	

	as_scan_status status;
	

	uint32_t progress_pct;
	

	uint32_t records_scanned;
} as_scan_info;


typedef struct as_scan_bins_s {
	

	bool _free;
	

	uint16_t capacity;
	

	uint16_t size;
	

	as_bin_name * entries;
} as_scan_bins;


typedef struct as_scan_s {
	

	bool _free;
	

	as_scan_priority priority;
	

	uint8_t percent;
	

	bool no_bins;
	

	bool concurrent;
	

	bool include_ldt;
	

	bool deserialize_list_map;
	

	as_namespace ns;
	

	as_set set;
	

	as_scan_bins select;
	

	as_udf_call apply_each;
} as_scan;




as_scan * as_scan_init(as_scan * scan, const as_namespace ns, const as_set set);


as_scan * as_scan_new(const as_namespace ns, const as_set set);


void as_scan_destroy(as_scan * scan);




#define as_scan_select_inita(__scan, __n) \
	if ( (__scan) != NULL && (__scan)->select.entries == NULL ) {\
		(__scan)->select.entries = (as_bin_name *) alloca(__n * sizeof(as_bin_name));\
		if ( (__scan)->select.entries ) { \
			(__scan)->select._free = false;\
			(__scan)->select.capacity = __n;\
			(__scan)->select.size = 0;\
		}\
 	}


bool as_scan_select_init(as_scan * scan, uint16_t n);


bool as_scan_select(as_scan * scan, const char * bin);




bool as_scan_set_percent(as_scan * scan, uint8_t percent);


bool as_scan_set_priority(as_scan * scan, as_scan_priority priority);


bool as_scan_set_nobins(as_scan * scan, bool nobins);


bool as_scan_set_concurrent(as_scan * scan, bool concurrent);


bool as_scan_apply_each(as_scan * scan, const char * module, const char * function, as_list * arglist);
} // end extern "C"


#include <citrusleaf/cf_queue.h>
extern "C" {
// Concurrency kit needs to be under extern "C" when compiling C++.




typedef struct as_node_shm_s {
	

	char name[AS_NODE_NAME_SIZE];
	

	ck_swlock_t lock;
	

	struct sockaddr_in addr;
	

	uint8_t active;
	

	uint8_t has_batch_index;
	

	uint8_t has_replicas_all;
	

	uint8_t has_double;
	

	uint8_t has_geo;
	

	char pad[3];
} as_node_shm;


typedef struct as_partition_shm_s {
	

	uint32_t master;
	

	uint32_t prole;
} as_partition_shm;


typedef struct as_partition_table_shm_s {
	

	char ns[AS_MAX_NAMESPACE_SIZE];
	

	as_partition_shm partitions[];
} as_partition_table_shm;


typedef struct as_cluster_shm_s {
	

	uint64_t timestamp;
	

	uint32_t owner_pid;
	

	uint32_t nodes_size;
	

	uint32_t nodes_capacity;
	

	uint32_t nodes_gen;
	

	uint32_t n_partitions;
	

	uint32_t partition_tables_size;
	

	uint32_t partition_tables_capacity;
	

	uint32_t partition_tables_offset;
	

	uint32_t partition_table_byte_size;
	

	ck_spinlock_t take_over_lock;
	

	uint8_t lock;
	

	uint8_t ready;
	

	char pad[6];
	

	as_node_shm nodes[];
	// This is where the dynamically allocated partition tables are located.
} as_cluster_shm;


typedef struct as_shm_info_s {
	

	as_cluster_shm* cluster_shm;
	

	as_node** local_nodes;
	

	int shm_id;
	

	uint32_t takeover_threshold_ms;
	

	volatile bool is_tend_master;
} as_shm_info;




as_status
as_shm_create(struct as_cluster_s* cluster, as_error* err, as_config* config);


void
as_shm_destroy(struct as_cluster_s* cluster);


void
as_shm_add_nodes(struct as_cluster_s* cluster, as_vector* 
 nodes_to_add);


void
as_shm_remove_nodes(struct as_cluster_s* cluster, as_vector* 
 nodes_to_remove);


void
as_shm_update_partitions(as_shm_info* shm_info, const char* ns, char* bitmap_b64, int64_t len, as_node* node, bool master);


as_node*
as_shm_node_get(struct as_cluster_s* cluster, const char* ns, const uint8_t* digest, bool write, as_policy_replica replica);


static inline as_partition_table_shm*
as_shm_get_partition_tables(as_cluster_shm* cluster_shm)
{
	return (as_partition_table_shm*) ((char*)cluster_shm + cluster_shm->partition_tables_offset);
}


static inline as_partition_table_shm*
as_shm_get_partition_table(as_cluster_shm* cluster_shm, as_partition_table_shm* tables, uint32_t index)
{
	return (as_partition_table_shm*) ((char*)tables + (cluster_shm->partition_table_byte_size * index));
}


static inline as_partition_table_shm*
as_shm_next_partition_table(as_cluster_shm* cluster_shm, as_partition_table_shm* table)
{
	return (as_partition_table_shm*) ((char*)table + cluster_shm->partition_table_byte_size);
}
} // end extern "C"


#include <citrusleaf/cf_clock.h>
#include <stddef.h>
#include <stdint.h>
#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
// Windows send() and recv() parameter types are different.
#define as_socket_data_t void
#define as_socket_size_t size_t
#define as_close(fd) (close(fd))
#if defined(__APPLE__)
#define SOL_TCP IPPROTO_TCP
#define MSG_NOSIGNAL 0
#if defined(CF_WINDOWS)
#include <WinSock2.h>
#include <Ws2tcpip.h>
#define as_socket_data_t char
#define as_socket_size_t int
#define as_close(fd) (closesocket(fd))
#define MSG_DONTWAIT	0
#define MSG_NOSIGNAL	0
#define SHUT_RDWR		SD_BOTH
extern "C" {


int
as_socket_create_nb();


as_status
as_socket_start_connect_nb(as_error* err, int fd, struct sockaddr_in *sa);


as_status
as_socket_create_and_connect_nb(as_error* err, struct sockaddr_in *sa, int* fd);


int
as_socket_validate(int fd);
#if defined(__linux__) || defined(__APPLE__)


static inline uint64_t
as_socket_deadline(uint32_t timeout_ms)
{
	return (timeout_ms && timeout_ms <= INT32_MAX)? cf_getms() + timeout_ms : 0;
}


as_status
as_socket_write_forever(as_error* err, int fd, uint8_t *buf, size_t buf_len);


as_status
as_socket_write_limit(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint64_t deadline);


static inline as_status
as_socket_write_deadline(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint64_t deadline)
{
	if (deadline) {
		return as_socket_write_limit(err, fd, buf, buf_len, deadline);
	}
	else {
		return as_socket_write_forever(err, fd, buf, buf_len);
	}
}


static inline as_status
as_socket_write_timeout(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint32_t timeout_ms)
{
	if (timeout_ms && timeout_ms <= INT32_MAX) {
		return as_socket_write_limit(err, fd, buf, buf_len, cf_getms() + timeout_ms);
	}
	else {
		return as_socket_write_forever(err, fd, buf, buf_len);
	}
}


as_status
as_socket_read_forever(as_error* err, int fd, uint8_t *buf, size_t buf_len);


as_status
as_socket_read_limit(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint64_t deadline);


static inline as_status
as_socket_read_deadline(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint64_t deadline)
{
	if (deadline) {
		return as_socket_read_limit(err, fd, buf, buf_len, deadline);
	}
	else {
		return as_socket_read_forever(err, fd, buf, buf_len);
	}
}


static inline as_status
as_socket_read_timeout(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint32_t timeout_ms)
{
	if (timeout_ms && timeout_ms <= INT32_MAX) {
		return as_socket_read_limit(err, fd, buf, buf_len, cf_getms() + timeout_ms);
	}
	else {
		return as_socket_read_forever(err, fd, buf, buf_len);
	}
}


static inline void
as_socket_address_name(struct sockaddr_in* address, char* name)
{
	inet_ntop(AF_INET, &(address->sin_addr), name, INET_ADDRSTRLEN);
}
} // end extern "C"


extern "C" {




typedef enum as_status_e {
	AEROSPIKE_ERR_INVALID_NODE = -8,
	AEROSPIKE_ERR_NO_MORE_CONNECTIONS = -7,
	AEROSPIKE_ERR_ASYNC_CONNECTION = -6,
	AEROSPIKE_ERR_CLIENT_ABORT = -5,
	AEROSPIKE_ERR_INVALID_HOST = -4,
	AEROSPIKE_NO_MORE_RECORDS = -3,
	AEROSPIKE_ERR_PARAM	= -2,
	AEROSPIKE_ERR_CLIENT = -1,
	AEROSPIKE_ERR = -1,
	AEROSPIKE_OK = 0,
	AEROSPIKE_ERR_SERVER = 1,
	AEROSPIKE_ERR_RECORD_NOT_FOUND = 2,
	AEROSPIKE_ERR_RECORD_GENERATION = 3,
	AEROSPIKE_ERR_REQUEST_INVALID = 4,
	AEROSPIKE_ERR_RECORD_EXISTS = 5,
	AEROSPIKE_ERR_BIN_EXISTS = 6,
	AEROSPIKE_ERR_CLUSTER_CHANGE = 7,
	AEROSPIKE_ERR_SERVER_FULL = 8,
	AEROSPIKE_ERR_TIMEOUT = 9,
	AEROSPIKE_ERR_NO_XDR = 10,
	AEROSPIKE_ERR_CLUSTER = 11,
	AEROSPIKE_ERR_BIN_INCOMPATIBLE_TYPE = 12,
	AEROSPIKE_ERR_RECORD_TOO_BIG = 13,
	AEROSPIKE_ERR_RECORD_BUSY = 14,
	AEROSPIKE_ERR_SCAN_ABORTED = 15,
	AEROSPIKE_ERR_UNSUPPORTED_FEATURE = 16,
    AEROSPIKE_ERR_BIN_NOT_FOUND = 17,
	AEROSPIKE_ERR_DEVICE_OVERLOAD = 18,
	AEROSPIKE_ERR_RECORD_KEY_MISMATCH = 19,
	AEROSPIKE_ERR_NAMESPACE_NOT_FOUND = 20,
	AEROSPIKE_ERR_BIN_NAME = 21,
	AEROSPIKE_ERR_FAIL_FORBIDDEN = 22,
	AEROSPIKE_QUERY_END = 50,
	AEROSPIKE_SECURITY_NOT_SUPPORTED = 51,
	AEROSPIKE_SECURITY_NOT_ENABLED = 52,
	AEROSPIKE_SECURITY_SCHEME_NOT_SUPPORTED = 53,
	AEROSPIKE_INVALID_COMMAND = 54,
	AEROSPIKE_INVALID_FIELD = 55,
	AEROSPIKE_ILLEGAL_STATE = 56,
	AEROSPIKE_INVALID_USER = 60,
	AEROSPIKE_USER_ALREADY_EXISTS = 61,
	AEROSPIKE_INVALID_PASSWORD = 62,
	AEROSPIKE_EXPIRED_PASSWORD = 63,
	AEROSPIKE_FORBIDDEN_PASSWORD = 64,
	AEROSPIKE_INVALID_CREDENTIAL = 65,
	AEROSPIKE_INVALID_ROLE = 70,
	AEROSPIKE_ROLE_ALREADY_EXISTS = 71,
	AEROSPIKE_INVALID_PRIVILEGE = 72,
	AEROSPIKE_NOT_AUTHENTICATED = 80,
	AEROSPIKE_ROLE_VIOLATION = 81,
	AEROSPIKE_ERR_UDF = 100,
	AEROSPIKE_ERR_LARGE_ITEM_NOT_FOUND = 125,
	AEROSPIKE_ERR_BATCH_DISABLED = 150,
	AEROSPIKE_ERR_BATCH_MAX_REQUESTS_EXCEEDED = 151,
	AEROSPIKE_ERR_BATCH_QUEUES_FULL = 152,
	AEROSPIKE_ERR_GEO_INVALID_GEOJSON = 160,
	AEROSPIKE_ERR_INDEX_FOUND = 200,
	

	AEROSPIKE_ERR_INDEX_NOT_FOUND = 201,
	

	AEROSPIKE_ERR_INDEX_OOM = 202,
	

	AEROSPIKE_ERR_INDEX_NOT_READABLE = 203,
	

	AEROSPIKE_ERR_INDEX = 204,
	

	AEROSPIKE_ERR_INDEX_NAME_MAXLEN = 205,
	

	AEROSPIKE_ERR_INDEX_MAXCOUNT = 206,
	

	AEROSPIKE_ERR_QUERY_ABORTED = 210,
	

	AEROSPIKE_ERR_QUERY_QUEUE_FULL = 211,
	

	AEROSPIKE_ERR_QUERY_TIMEOUT = 212,
	

	AEROSPIKE_ERR_QUERY = 213,
	

	

	AEROSPIKE_ERR_UDF_NOT_FOUND				= 1301,
	

	AEROSPIKE_ERR_LUA_FILE_NOT_FOUND		= 1302,
	

	
	AEROSPIKE_ERR_LDT_INTERNAL                    = 1400,
	
	AEROSPIKE_ERR_LDT_NOT_FOUND                   = 1401,
	
	AEROSPIKE_ERR_LDT_UNIQUE_KEY                  = 1402,
	
	AEROSPIKE_ERR_LDT_INSERT                      = 1403,
	
	AEROSPIKE_ERR_LDT_SEARCH                      = 1404,
	
	AEROSPIKE_ERR_LDT_DELETE                      = 1405,
	
	AEROSPIKE_ERR_LDT_INPUT_PARM                  = 1409,
    // -------------------------------------------------
	
	AEROSPIKE_ERR_LDT_TYPE_MISMATCH               = 1410,
	
	AEROSPIKE_ERR_LDT_NULL_BIN_NAME               = 1411,
	
	AEROSPIKE_ERR_LDT_BIN_NAME_NOT_STRING         = 1412,
	
	AEROSPIKE_ERR_LDT_BIN_NAME_TOO_LONG           = 1413,
	
	AEROSPIKE_ERR_LDT_TOO_MANY_OPEN_SUBRECS       = 1414,
	
	AEROSPIKE_ERR_LDT_TOP_REC_NOT_FOUND           = 1415,
	
	AEROSPIKE_ERR_LDT_SUB_REC_NOT_FOUND           = 1416,
	
	AEROSPIKE_ERR_LDT_BIN_DOES_NOT_EXIST          = 1417,
	
	AEROSPIKE_ERR_LDT_BIN_ALREADY_EXISTS          = 1418,
	
	AEROSPIKE_ERR_LDT_BIN_DAMAGED                 = 1419,
    // -------------------------------------------------
	
	AEROSPIKE_ERR_LDT_SUBREC_POOL_DAMAGED         = 1420,
	
	AEROSPIKE_ERR_LDT_SUBREC_DAMAGED              = 1421,
	
	AEROSPIKE_ERR_LDT_SUBREC_OPEN                 = 1422,
	
	AEROSPIKE_ERR_LDT_SUBREC_UPDATE               = 1423,
	
	AEROSPIKE_ERR_LDT_SUBREC_CREATE               = 1424,
	
	AEROSPIKE_ERR_LDT_SUBREC_DELETE               = 1425, 
	
	AEROSPIKE_ERR_LDT_SUBREC_CLOSE                = 1426,
	
	AEROSPIKE_ERR_LDT_TOPREC_UPDATE               = 1427,
	
	AEROSPIKE_ERR_LDT_TOPREC_CREATE               = 1428,
    // -------------------------------------------------
	
	AEROSPIKE_ERR_LDT_FILTER_FUNCTION_BAD         = 1430,
	
	AEROSPIKE_ERR_LDT_FILTER_FUNCTION_NOT_FOUND   = 1431,
	
	AEROSPIKE_ERR_LDT_KEY_FUNCTION_BAD            = 1432,
	
	AEROSPIKE_ERR_LDT_KEY_FUNCTION_NOT_FOUND      = 1433,
	
	AEROSPIKE_ERR_LDT_TRANS_FUNCTION_BAD          = 1434,
	
	AEROSPIKE_ERR_LDT_TRANS_FUNCTION_NOT_FOUND    = 1435,
	
	AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_BAD        = 1436,
	
	AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_NOT_FOUND  = 1437,
	
	AEROSPIKE_ERR_LDT_USER_MODULE_BAD             = 1438,
	
	AEROSPIKE_ERR_LDT_USER_MODULE_NOT_FOUND       = 1439
} as_status;

#define AS_UDF_MODULE_MAX_SIZE 64
#define AS_UDF_MODULE_MAX_LEN (AS_UDF_MODULE_MAX_SIZE - 1)
#define AS_UDF_FUNCTION_MAX_SIZE 64
#define AS_UDF_FUNCTION_MAX_LEN (AS_UDF_FUNCTION_MAX_SIZE - 1)
#define AS_UDF_FILE_NAME_SIZE 128
#define AS_UDF_FILE_NAME_SIZE 128
#define AS_UDF_FILE_NAME_LEN AS_UDF_FILE_NAME_SIZE - 1
#define AS_UDF_FILE_HASH_SIZE (20 * 2)
typedef char as_udf_module_name[AS_UDF_MODULE_MAX_SIZE];
typedef char as_udf_function_name[AS_UDF_FUNCTION_MAX_SIZE];
typedef struct as_udf_call_s {
	bool _free;
	as_udf_module_name module;
	as_udf_function_name function;
	as_list * arglist;
} as_udf_call;
typedef enum as_udf_type_e {
	AS_UDF_TYPE_LUA
} as_udf_type;
typedef struct as_udf_file_s {
	bool _free;
	char name[AS_UDF_FILE_NAME_SIZE];
	uint8_t hash[AS_UDF_FILE_HASH_SIZE+1];
	as_udf_type type;
	struct {
		bool _free;
		uint32_t capacity;
		uint32_t size;
		uint8_t * bytes;
	} content;
} as_udf_file;
typedef struct as_udf_files_s {
	bool _free;
	uint32_t capacity;
	uint32_t size;
	as_udf_file * entries;
} as_udf_files;
as_udf_call * as_udf_call_init(as_udf_call * call, const as_udf_module_name module, const as_udf_function_name function, as_list * arglist);
as_udf_call * as_udf_call_new(const as_udf_module_name module, const as_udf_function_name function, as_list * arglist);
void as_udf_call_destroy(as_udf_call * call);
as_udf_file * as_udf_file_init(as_udf_file * file);
as_udf_file * as_udf_file_new();
void as_udf_file_destroy(as_udf_file * file);
as_udf_files * as_udf_files_init(as_udf_files * files, uint32_t capacity);
as_udf_files * as_udf_files_new(uint32_t capacity);
void as_udf_files_destroy(as_udf_files * files);
