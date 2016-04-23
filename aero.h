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

/**
 *	Wait for asynchronous task to complete using given polling interval.
 *
 *	@param err			The as_error to be populated if an error occurs.
 *	@param task			The task data used to poll for completion.
 *	@param interval_ms	The polling interval in milliseconds. If zero, 1000 ms is used.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup index_operations
 */
as_status aerospike_index_create_wait(as_error * err, as_index_task * task, uint32_t interval_ms);

/**
 *	Removes (drops) a secondary index.
 *
 *	~~~~~~~~~~{.c}
 *	if ( aerospike_index_remove(&as, &err, NULL, "test", idx_test_demo_bin1") != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param ns			The namespace containing the index to be removed.
 *	@param name			The name of the index to be removed.
 *
 *	@return AEROSPIKE_OK if successful or index does not exist. Otherwise an error.
 *
 *	@ingroup index_operations
 */
as_status aerospike_index_remove(
	aerospike * as, as_error * err, const as_policy_info * policy,
	const as_namespace ns, const char * name);

/******************************************************************************
 *	DEPRECATED FUNCTIONS
 *****************************************************************************/

/**
 *	Create a new secondary index on an integer bin.
 *
 *	@deprecated Use aerospike_index_create() instead.
 *
 *	@ingroup index_operations
 */
static inline as_status aerospike_index_integer_create(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const as_namespace ns, const as_set set, const as_bin_name bin, const char * name)
{
	return aerospike_index_create_complex(as, err, 0, policy, ns, set, bin, name, AS_INDEX_TYPE_DEFAULT, AS_INDEX_NUMERIC);
}

/**
 *	Create a new secondary index on a string bin.
 *
 *	@deprecated Use aerospike_index_create() instead.
 *
 *	@ingroup index_operations
 */
static inline as_status aerospike_index_string_create(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const as_namespace ns, const as_set set, const as_bin_name bin, const char * name)
{
	return aerospike_index_create_complex(as, err, 0, policy, ns, set, bin, name, AS_INDEX_TYPE_DEFAULT, AS_INDEX_STRING);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 *	@defgroup info_operations Info Operations
 *	@ingroup client_operations
 *
 *	The Info API provides the ability to query an Aerospike cluster for 
 *	information. 
 *
 *	The following API are provided:
 *	- aerospike_info_host() - Query a single host in the cluster.
 *	- aerospike_info_foreach() - Query every host in the cluster.
 *
 */

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_scan.h>
#include <aerospike/as_error.h>
#include <aerospike/as_node.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Callback for aerospike_info_foreach()
 *	
 *	@param err			The status and possible error information for the info request.
 *	@param node			The node which provided the response.
 *	@param res			The response to the info request.
 *	@param udata		The udata provided to the aerospike_info_foreach()
 *
 *	@return TRUE to continue to the next info response. FALSE to stop processing.
 *
 *	@ingroup info_operations
 */
typedef bool (* aerospike_info_foreach_callback)(const as_error * err, const as_node * node, const char * req, char * res, void * udata);

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Send an info request to a specific host. The response must be freed by the caller on success.
 *	
 *	~~~~~~~~~~{.c}
 *	char * res = NULL;
 *	if ( aerospike_info_host(&as, &err, NULL, "127.0.0.1", 3000, "info", &res) != AEROSPIKE_OK ) {
 *		// handle error
 *	}
 *	else {
 *		// handle response
 *		free(res);
 *		res = NULL;
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param addr			The IP address or hostname to send the request to.
 *	@param port			The port to send the request to.
 *	@param req			The info request to send.
 *	@param res			The response from the node. The response will be a NULL terminated string, allocated by the function, and must be freed by the caller.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error.
 *
 *	@ingroup info_operations
 */
as_status aerospike_info_host(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * addr, uint16_t port, const char * req, 
	char ** res
	);

/**
 *	Send an info request to a specific socket address. The response must be freed by the caller on success.
 *
 *	~~~~~~~~~~{.c}
 *	char * res = NULL;
 *	if ( aerospike_info_socket_address(&as, &err, NULL, &sa_in, "info", &res) != AEROSPIKE_OK ) {
 *		// handle error
 *	}
 *	else {
 *		// handle response
 *		free(res);
 *		res = NULL;
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param sa_in		The IP address and port to send the request to.
 *	@param req			The info request to send.
 *	@param res			The response from the node. The response will be a NULL terminated string, allocated by the function, and must be freed by the caller.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error.
 *
 *	@ingroup info_operations
 */
as_status aerospike_info_socket_address(
	aerospike * as, as_error * err, const as_policy_info * policy,
	struct sockaddr_in* sa_in, const char * req,
	char ** res
	);

/**
 *	Send an info request to a node in the cluster.  If node request fails, send request to the next
 *	node in the cluster.  Repeat until the node request succeeds. The response must be freed by
 *	the caller on success.
 *
 *	~~~~~~~~~~{.c}
 *	char * res = NULL;
 *	if ( aerospike_info_any(&as, &err, NULL, "info", &res) != AEROSPIKE_OK ) {
 *		// handle error
 *	}
 *	else {
 *		// handle response
 *		free(res);
 *		res = NULL;
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param req			The info request to send.
 *	@param res			The response from the node. The response will be a NULL terminated string, allocated by the function, and must be freed by the caller.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error.
 *
 *	@ingroup info_operations
 */
as_status aerospike_info_any(
	aerospike * as, as_error * err, const as_policy_info * policy,
	const char * req, char ** res
	);

/**
 *	Send an info request to the entire cluster.
 *
 *	~~~~~~~~~~{.c}
 *	if ( aerospike_info_foreach(&as, &err, NULL, "info", callback, NULL) != AEROSPIKE_OK ) {
 *		// handle error
 *	}
 *	~~~~~~~~~~
 *
 *	The callback takes a response string. The caller should not free this string.
 *
 *	~~~~~~~~~~{.c}
 *	bool callback(const as_error * err, const as_node * node, const char * req, char * res, void * udata) {
 *		// handle response
 *	}
 *	~~~~~~~~~~
 *
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param req			The info request to send.
 *	@param callback		The function to call when a response is received.
 *	@param udata		User-data to send to the callback.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error.
 *
 *	@ingroup info_operations
 */
as_status aerospike_info_foreach(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * req, 
	aerospike_info_foreach_callback callback, void * udata
	);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 *	@defgroup key_operations Key Operations
 *	@ingroup client_operations
 *
 *	Aerospike provides a key based API to access and modify data into the 
 *	cluster. 
 *
 *	The Key API is a collection of APIs that use as_key as for looking up
 *	records for accessing and modifying in the cluster. 
 *	
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_listener.h>
#include <aerospike/as_error.h>
#include <aerospike/as_key.h>
#include <aerospike/as_list.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_record.h>
#include <aerospike/as_status.h>
#include <aerospike/as_val.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Look up a record by key and return all bins.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *	
 *	as_record* rec = NULL;
 *	if (aerospike_key_get(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		as_record_destroy(rec);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param rec 			The record to be populated with the data from request.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_get(
	aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, as_record** rec
	);

/**
 *	Asynchronously look up a record by key and return all bins.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		// Process record bins
 *		// Do not call as_record_destroy() because the calling function will do that for you.
 *	}
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_status status = aerospike_key_get_async(&as, &err, NULL, &key, my_listener, NULL, NULL, NULL);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param listener			User function to be called with command results.
 *	@param udata 			User data to be forwarded to user callback.
 *	@param event_loop 		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_get_async(
	aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key,
	as_async_record_listener listener, void* udata, as_event_loop* event_loop,
	as_pipe_listener pipe_listener
	);

/**
 *	Lookup a record by key, then return specified bins.
 *
 *	~~~~~~~~~~{.c}
 *	char* select[] = {"bin1", "bin2", "bin3", NULL};
 *	
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *	
 *	as_record* rec = NULL;
 *	if (aerospike_key_select(&as, &err, NULL, &key, select, &rec) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		as_record_destroy(rec);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param bins			The bins to select. A NULL terminated array of NULL terminated strings.
 *	@param rec 			The record to be populated with the data from request.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_select(
	aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key,
	const char* bins[], as_record** rec
	);

/**
 *	Asynchronously lookup a record by key, then return specified bins.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		// Process record bins
 *		// Do not call as_record_destroy() because the calling function will do that for you.
 *	}
 *
 *	char* select[] = {"bin1", "bin2", "bin3", NULL};
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_status status = aerospike_key_select_async(&as, &err, NULL, &key, select, my_listener, NULL, NULL, NULL);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param bins				The bins to select. A NULL terminated array of NULL terminated strings.
 *	@param listener			User function to be called with command results.
 *	@param udata			User data to be forwarded to user callback.
 *	@param event_loop		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_select_async(
	aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, const char* bins[],
	as_async_record_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener
	);

/**
 *	Check if a record exists in the cluster via its key. The record's metadata 
 * 	will be populated if the record exists.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *	
 *	as_record* rec = NULL;
 *	if (aerospike_key_exists(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		if (rec) {
 *			printf("Record exists.");
 *			as_record_destroy(rec);
 *		}
 *		else {
 *			printf("Record doesn't exist.");
 *		}
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param rec    		The metadata will be populated if the record exists.
 *
 *	@return AEROSPIKE_OK if successful. AEROSPIKE_ERR_RECORD_NOT_FOUND if not found. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_exists(
	aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key, as_record** rec
	);

/**
 *	Asynchronously check if a record exists in the cluster via its key. The record's metadata
 * 	will be populated if the record exists.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		if (record) {
 *			printf("Record exists.");
 *			// Do not call as_record_destroy() because the calling function will do that for you.
 *		}
 *		else {
 *			printf("Record doesn't exist.");
 *		}
 *	}
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_status status = aerospike_key_exists_async(&as, &err, NULL, &key, my_listener, NULL, NULL, NULL);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param listener			User function to be called with command results.
 *	@param udata			User data to be forwarded to user callback.
 *	@param event_loop		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_exists_async(
	aerospike* as, as_error* err, const as_policy_read* policy, const as_key* key,
	as_async_record_listener listener, void* udata, as_event_loop* event_loop,
	as_pipe_listener pipe_listener
	);

/**
 *	Store a record in the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_record rec;
 *	as_record_init(&rec, 2);
 *	as_record_set_str(&rec, "bin1", "abc");
 *	as_record_set_int64(&rec, "bin2", 123);
 *	
 *	if (aerospike_key_put(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	as_record_destroy(&rec);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param rec 			The record containing the data to be written.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_put(
	aerospike* as, as_error* err, const as_policy_write* policy, const as_key* key, as_record* rec
	);

/**
 *	Asynchronously store a record in the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		printf("Command succeeded\n");
 *	}
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_record rec;
 *	as_record_init(&rec, 2);
 *	as_record_set_str(&rec, "bin1", "abc");
 *	as_record_set_int64(&rec, "bin2", 123);
 *
 *	as_status status = aerospike_key_put_async(&as, &err, NULL, &key, &rec, my_listener, NULL, NULL, NULL);
 *	as_record_destroy(&rec);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param rec				The record containing the data to be written.
 *	@param listener			User function to be called with command results.
 *	@param udata			User data to be forwarded to user callback.
 *	@param event_loop		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_put_async(
	aerospike* as, as_error* err, const as_policy_write* policy, const as_key* key, as_record* rec,
	as_async_write_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener
	);

/**
 *	Remove a record from the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	if (aerospike_key_remove(&as, &err, NULL, &key) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *
 *	@return AEROSPIKE_OK if successful and AEROSPIKE_ERR_RECORD_NOT_FOUND if the record was not found. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_remove(
	aerospike* as, as_error* err, const as_policy_remove* policy, const as_key* key
	);

/**
 *	Asynchronously remove a record from the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		printf("Command succeeded\n");
 *	}
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_status status = aerospike_key_remove(&as, &err, NULL, &key, my_listener, NULL, NULL, NULL);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param listener			User function to be called with command results.
 *	@param udata			User data to be forwarded to user callback.
 *	@param event_loop		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_remove_async(
	aerospike* as, as_error* err, const as_policy_remove* policy, const as_key* key,
	as_async_write_listener listener, void* udata, as_event_loop* event_loop,
	as_pipe_listener pipe_listener
	);

/**
 *	Lookup a record by key, then perform specified operations.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_operations ops;
 *	as_operations_inita(&ops,3);
 *	as_operations_add_incr(&ops, "bin1", 456);
 *	as_operations_add_append_str(&ops, "bin2", "def");
 *	as_operations_add_read(&ops, "bin1")
 *
 *	as_record * rec = NULL;
 *
 *	if (aerospike_key_operate(&as, &err, NULL, &key, &ops, &rec) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		as_record_destroy(rec);
 *	}
 *	as_operations_destroy(&ops);
 *	~~~~~~~~~~
 *	
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ops			The operations to perform on the record.
 *	@param rec			The record to be populated with the data from AS_OPERATOR_READ operations.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_operate(
	aerospike* as, as_error* err, const as_policy_operate* policy, const as_key* key,
	const as_operations* ops, as_record** rec
	);

/**
 *	Asynchronously lookup a record by key, then perform specified operations.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		// Process record bins
 *		// Do not call as_record_destroy() because the calling function will do that for you.
 *	}
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_operations ops;
 *	as_operations_inita(&ops,3);
 *	as_operations_add_incr(&ops, "bin1", 456);
 *	as_operations_add_append_str(&ops, "bin2", "def");
 *	as_operations_add_read(&ops, "bin1")
 *
 *	as_status status = aerospike_key_operate(&as, &err, NULL, &key, &ops, my_listener, NULL, NULL, NULL);
 *	as_operations_destroy(&ops);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param ops				The operations to perform on the record.
 *	@param listener			User function to be called with command results.
 *	@param udata			User data to be forwarded to user callback.
 *	@param event_loop		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_operate_async(
	aerospike* as, as_error* err, const as_policy_operate* policy, const as_key* key, const as_operations* ops,
	as_async_record_listener listener, void* udata, as_event_loop* event_loop, as_pipe_listener pipe_listener
	);

/**
 *	Lookup a record by key, then apply the UDF.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_arraylist args;
 *	as_arraylist_inita(&args, 2);
 *	as_arraylist_append_int64(&args, 1);
 *	as_arraylist_append_int64(&args, 2);
 *	
 *	as_val * res = NULL;
 *	
 *	if (aerospike_key_apply(&as, &err, NULL, &key, "math", "add", &args, &res) != AEROSPIKE_OK) {
 *		printf("error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		as_val_destroy(res);
 *	}
 *	
 *	as_arraylist_destroy(&args);
 *	~~~~~~~~~~
 *
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param module		The module containing the function to execute.
 *	@param function 	The function to execute.
 *	@param arglist 		The arguments for the function.
 *	@param result 		The return value from the function.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status aerospike_key_apply(
	aerospike * as, as_error * err, const as_policy_apply * policy, 
	const as_key * key,
	const char * module, const char * function, as_list * arglist, 
	as_val ** result
	);

/**
 *	Asynchronously lookup a record by key, then apply the UDF.
 *
 *	~~~~~~~~~~{.c}
 *	void my_listener(as_error* err, as_val* val, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Command failed: %d %s\n", err->code, err->message);
 *			return;
 *		}
 *		// Process value.  The calling function will call as_val_destroy().
 *		// If the value needs to be preserved, bump up the reference count using as_val_reserve()
 *		// and call as_val_destroy() when done with the value.
 *	}
 *
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *
 *	as_arraylist args;
 *	as_arraylist_inita(&args, 2);
 *	as_arraylist_append_int64(&args, 1);
 *	as_arraylist_append_int64(&args, 2);
 *
 *	as_status status = aerospike_key_apply(&as, &err, NULL, &key, "math", "add", &args, my_listener, NULL, NULL, NULL);
 *	as_arraylist_destroy(&args);
 *	~~~~~~~~~~
 *
 *	@param as				The aerospike instance to use for this operation.
 *	@param err				The as_error to be populated if an error occurs.
 *	@param policy			The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key				The key of the record.
 *	@param module			The module containing the function to execute.
 *	@param function			The function to execute.
 *	@param arglist			The arguments for the function.
 *	@param listener			User function to be called with command results.
 *	@param udata			User data to be forwarded to user callback.
 *	@param event_loop		Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *	@param pipe_listener	Enables command pipelining, if not NULL. The given callback is invoked after the current command
 *							has been sent to the server. This allows for issuing the next command even before receiving a
 *							result for the current command.
 *
 *	@return AEROSPIKE_OK if async command succesfully queued. Otherwise an error.
 *
 *	@ingroup key_operations
 */
as_status
aerospike_key_apply_async(
	aerospike* as, as_error* err, const as_policy_apply* policy, const as_key* key,
	const char* module, const char* function, as_list* arglist,
	as_async_value_listener listener, void* udata, as_event_loop* event_loop,
	as_pipe_listener pipe_listener
	);
	
/**
 *	Do the connected servers support the new floating point type.
 *	The cluster must already be connected (aerospike_connect()) prior to making this call.
 */
bool
aerospike_has_double(aerospike* as);

/**
 *	Do the connected servers support geospatial data and queries.
 *	The cluster must already be connected (aerospike_connect()) prior to making this call.
 */
bool
aerospike_has_geo(aerospike* as);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 * Functionality related to Large List Data Type
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_error.h>
#include <aerospike/as_ldt.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_status.h>
#include <aerospike/as_key.h>
#include <aerospike/as_val.h>
#include <aerospike/as_boolean.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Add a value into the llist.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *	
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_llist_add(&as, &err, NULL, &key, &llist, (as_val *) &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param val			The value to insert into the llist.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_add(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val);

/**
 *	Add / update a value into the llist. Existing value will be overwritten.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_llist_update(&as, &err, NULL, &key, &llist, (as_val *) &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param val			The value to update/insert into the llist.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_update(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val);



/**
 *	Add a list of values into the llist.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *
 *	as_arraylist vals;
 *	as_arraylist_inita(&vals, 2);
 *	as_string s;
 *	as_string_init(s,"a string",false);
 *	as_arraylist_append_string(&vals, s);
 *	as_arraylist_append_int64(&vals, 35);
 *
 *	if ( aerospike_llist_add_all(&as, &err, NULL, &key, &llist, (as_list *)vals) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param vals			The list of values to insert into the llist.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_add_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_list * vals);

/**
 *	Add / update a list of values into the llist.
 *	If the value in input list already exists it will be overwritten.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *
 *	as_arraylist vals;
 *	as_arraylist_inita(&vals, 2);
 *	as_string s;
 *	as_string_init(s,"a string",false);
 *	as_arraylist_append_string(&vals, s);
 *	as_arraylist_append_int64(&vals, 35);
 *
 *	if ( aerospike_llist_update_all(&as, &err, NULL, &key, &llist, (as_list *)vals) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param vals			The list of values to update/insert into the llist.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_update_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_list * vals);

/**
 *	Search for a value in the llist.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *	
 *	as_integer search_val;
 *	as_integer_init(&search_val, 42);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find(&as, &err, NULL, &key, &llist, &search_val, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// do logic because element exists
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param search_val 	The search value
 *	@param elements		The returned list of values
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *	
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * search_val,
	as_list ** elements );

/**
 *	Select values from the beginning of list up to a maximum count.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find_first(&as, &err, NULL, &key, &llist, 5, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param count		Maximum number of values to return.
 *	@param elements		The returned list of values.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find_first(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t count, as_list ** elements);
	
/**
 *	Select values from the beginning of list up to a maximum count after applying lua filter.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find_first_filter(&as, &err, NULL, &key, &llist, 5, "search_filter", NULL, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param count		Maximum number of values to return.
 *	@param filter		The name of the User-Defined-Function to use as a search filter.
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter.
 *	@param elements		The returned list of values.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find_first_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t count,
	const as_udf_function_name filter, const as_list *filter_args, as_list ** elements);

/**
 *	Select values from the end of list up to a maximum count.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find_last(&as, &err, NULL, &key, &llist, 5, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param count		Maximum number of values to return.
 *	@param elements		The returned list of values.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find_last(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t count, as_list ** elements);
	
/**
 *	Select values from the end of list up to a maximum count after applying lua filter.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find_last_filter(&as, &err, NULL, &key, &llist, 5, "search_filter", NULL, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param count		Maximum number of values to return.
 *	@param filter		The name of the User-Defined-Function to use as a search filter.
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter.
 *	@param elements		The returned list of values.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find_last_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t count,
	const as_udf_function_name filter, const as_list *filter_args, as_list ** elements);

/**
 *	Select values from the begin key up to a maximum count.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_integer from_val;
 *	as_integer_init(&from_val, 42);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find_from(&as, &err, NULL, &key, &llist, &from_val, 5, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param from_val		Value from which to start.
 *	@param count		Maximum number of values to return.
 *	@param elements		The returned list of values.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find_from(
	aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key,
	const as_ldt * ldt, const as_val * from_val, uint32_t count, as_list ** elements);

/**
 *	Select values from the begin key up to a maximum count after applying lua filter.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_integer from_val;
 *	as_integer_init(&from_val, 42);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_find_from_filter(&as, &err, NULL, &key, &llist, &from_val, 5, "search_filter", NULL, &result_list ) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to lookup from. If not an llist bin, will return error.
 *	@param from_val		Value from which to start.
 *	@param count		Maximum number of values to return.
 *	@param filter		The name of the User-Defined-Function to use as a search filter.
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter.
 *	@param elements		The returned list of values.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_find_from_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy, const as_key * key,
	const as_ldt * ldt, const as_val * from_val, uint32_t count,
	const as_udf_function_name filter, const as_list *filter_args, as_list ** elements);

/**
 *	Given an llist bin, return all values in the list.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_filter(&as, &err, NULL, &key, &llist,
 *			 &result_list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to search from. If not an llist bin, will return error.
 *	@param elements		The pointer to a list of elements returned from search function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_scan(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, as_list ** elements );

/**
 *	Given an llist bin, filter the collection of objects using the given
 *	filter function. If no filter function is specified, return all values.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_filter(&as, &err, NULL, &key, &llist,
 *			"search_filter", NULL, &result_list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to search from. If not an llist bin, will return error.
 *	@param filter		The name of the User-Defined-Function to use as a search filter.
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter.
 *	@param elements		The pointer to a list of elements returned from search function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );

/**
 *	Given an llist bin, return the key values from MIN to MAX, and then
 *	filter the returned collection of objects using the given
 *	filter function. If no filter function is specified, return all values.
 *	Limit returned values size to given count.  If count is zero, do not
 *	limit returned values.  Count > 0 are supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_integer min_value;
 *	as_integer_init(&min_value, 18);
 *
 *	as_integer max_value;
 *	as_integer_init(&max_value, 99);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_range_limit(&as, &err, NULL, &key, &llist, &min_value, &max_value, 20,
 *	    "search_filter", NULL, &result_list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to search from. If not an llist bin, will return error.
 *	@param min_value	The minimum range value (or null to be LEAST value)
 *	@param max_value	The maximum range value (or null to be the GREATEST value)
 *	@param count		The maximum number of values to return, pass in zero to obtain all values within range.
 *	@param filter		The name of the User-Defined-Function to use as a search filter (or null if no filter)
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter (or null)
 *	@param elements		The pointer to a list of elements returned from search function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_range_limit(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_val * min_value, const as_val * max_value, uint32_t count,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );
	
/**
 *	Given an llist bin, return the key values from MIN to MAX, and then
 *	filter the returned collection of objects using the given
 *	filter function. If no filter function is specified, return all values.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	as_integer min_value;
 *	as_integer_init(&min_value, 18);
 *
 *	as_integer max_value;
 *	as_integer_init(&max_value, 99);
 *
 *	as_list *result_list = NULL;
 *
 *	if ( aerospike_llist_range(&as, &err, NULL, &key, &llist, &min_value, &max_value,
 *	    "search_filter", NULL, &result_list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(result_list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to search from. If not an llist bin, will return error.
 *	@param min_value	The minimum range value (or null to be LEAST value)
 *	@param max_value	The maximum range value (or null to be the GREATEST value)
 *	@param filter		The name of the User-Defined-Function to use as a search filter (or null if no filter)
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter (or null)
 *	@param elements		The pointer to a list of elements returned from search function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
static inline as_status aerospike_llist_range(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_val * min_value, const as_val * max_value,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements )
{
	return aerospike_llist_range_limit(as, err, policy, key, ldt, min_value, max_value, 0, filter, filter_args, elements);
}

/**
 *	Look up a llist and find how many elements it contains
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *	uint32_t llist_size = 0;
 *
 *	if ( aerospike_llist_size(&as, &err, NULL, &key, &llist, &llist_size) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist to operate on. If not an llist bin, will return error.
 *	@param n			Return the number of elements in the llist.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);

/**
 *	Delete the given value from the llist
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "llist", AS_LDT_LLIST, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_llist_remove(&as, &err, NULL, &key, &llist, &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to delete from. If not an llist bin, will return error.
 *	@param element		The value to delete from the set.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_remove(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val *element
	);

/**
 *	Destroy the llist bin
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	if ( aerospike_llist_destroy(&as, &err, NULL, &key, &llist) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The llist bin to destroy. If not an llist bin, will return error.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);

/**
 *	SET the storage capacity for this LDT.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *	uint32_t ldt_capacity = 5000;
 *
 *	if ( aerospike_llist_set_capacity(&as, &err, NULL, &key, &llist, ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LLIST bin, will return error.
 *	@param ldt_capacity Set by function to 1 for true, 0 for false
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t ldt_capacity
	);

/**
 *	Check the storage capacity for this LDT.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_llist_get_capacity(&as, &err, NULL, &key, &llist, &ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LLIST bin, will return error.
 *	@param ldt_capacity Set by function to 1 for true, 0 for false
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);

/**
 *	Check to see if an LLIST object exists in this record bin.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *	uint32_t ldt_exists = 0;
 *
 *	if ( aerospike_llist_size(&as, &err, NULL, &key, &llist, &ldt_exists) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LLIST bin, will return error.
 *	@param ldt_exists	Ptr to as_boolean: Set to TRUE if ldt exists, otherwise false.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);

/**
 *	Set page size for LLIST bin.
 *	Supported by server versions >= 3.5.8.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt llist;
 *	as_ldt_init(&llist, "myllist", AS_LDT_LLIST, NULL);
 *
 *	if ( aerospike_llist_set_page_size(&as, &err, NULL, &key, &llist, 8192) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LLIST bin, will return error.
 *	@param page_size	The new llist page size.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_llist_set_page_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t page_size);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 * Functionality related to Large Map Data Type
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_error.h>
#include <aerospike/as_ldt.h>
#include <aerospike/as_list.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_status.h>
#include <aerospike/as_key.h>
#include <aerospike/as_val.h>
#include <aerospike/as_boolean.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Add a value into the lmap.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *	
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_lmap_put(&as, &err, NULL, &key, &lmap, (const as_val *) &ival, (as_val *) &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param mkey			The map-key.
 *	@param mval			The map-value associated with mkey.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_put(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_val * mkey, const as_val * mval
	);

/**
 *	Add multiple entries into the lmap.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *
 *
 *	as_arraylist vals;
 *	as_arraylist_inita(&vals, 2);
 *	as_string s;
 *	as_string_init(s,"a string",false);
 *	as_arraylist_append_string(&vals, s);
 *	as_arraylist_append_int64(&vals, 35);
 *
 *	if ( aerospike_lmap_put_all(&as, &err, NULL, &key, &lmap, (const as_map *)vals) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param vals			A map containing the entries to add to the lmap.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_put_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_map * vals
	);

/**
 *	Get the value of an entry in the Lmap, using the given map-key
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *  boolean exists = false;
 *	
 *	if ( aerospike_lmap_get(&as, &err, NULL, &key, &lmap, &ikey, &p_val) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// do logic because element exists
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lmap bin to lookup from. If not an lmap bin, will return error.
 *	@param mkey 		The map key.
 *	@param mval 		Returned map value.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *	
 *	@ingroup ldt_operations
 */

as_status aerospike_lmap_get(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * mkey,
	as_val ** mval
	);

/**
 *	Get all the entries in an lmap
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *
 *  as_map *p_map = NULL;
 *
 *	if ( aerospike_lmap_get_all(&as, &err, NULL, &key, &lmap, &p_map) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// do logic because element exists
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lmap bin to lookup from. If not an lmap bin, will return error.
 *	@param elements 	Returned pointer to the map of entries.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */

as_status aerospike_lmap_get_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_map ** elements
	);

/**
 *	Given an lmap bin, scan through all entries in the map, and apply the
 *	given filter function. If no filter function is specified, all values
 *	in the lmap will be returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *
 *	as_map *p_map = NULL;
 *
 *	if ( aerospike_lmap_filter(&as, &err, NULL, &key, &lmap,
 *			"counter_filter", NULL, (as_map *) &p_map) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_map_destroy(p_map);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lmap bin to operate on. If not an lmap bin, will return error.
 *	@param filter		The name of the User-Defined-Function to use as a read-filter.
 *						The UDF should either return the entry, or nil, if filtered out.
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter.
 *	@param elements		The pointer to a map of entries returned from the function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_udf_function_name filter, const as_list *filter_args,
	as_map ** elements
	);

/**
 *	Look up a lmap and find how many elements it contains
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *	uint32_t lmap_size = 0;
 *
 *	if ( aerospike_lmap_size(&as, &err, NULL, &key, &lmap, &lmap_size) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lmap to operate on. If not an lmap bin, will return error.
 *	@param n			Return the number of elements in the lmap.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);

/**
 *	Delete the given value from the lmap
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "lmap", AS_LDT_LMAP, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_lmap_remove(&as, &err, NULL, &key, &lmap, (const as_val*)&ikey) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lmap bin to delete from. If not an lmap bin, will return error.
 *	@param mkey			The key to delete from the set.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_remove(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val *mkey
	);

/**
 *	Destroy the lmap bin
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *
 *	if ( aerospike_lmap_destroy(&as, &err, NULL, &key, &lmap) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lmap bin to destroy. If not an lmap bin, will return error.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);

/**
 *	Change an LDT storage capacity (in number of elements)
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_lmap_set_capacity(&as, &err, NULL, &key, &lmap, ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to check
 *	@param ldt_capacity	The new capacity for this LDT, in terms of elements, not bytes
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t ldt_capacity
	);

/**
 *	Get an LDTs storage capacity (in number of elements)
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_lmap_get_capacity(&as, &err, NULL, &key, &lmap, &ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT bin to operate on
 *	@param ldt_capacity The LDT Capacity, in terms of elements, not bytes.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);

/**
 *	Check to see if an LDT object exists in this record bin.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lmap;
 *	as_ldt_init(&lmap, "mylmap", AS_LDT_LMAP, NULL);
 *	uint32_t ldt_exists = 0;
 *
 *	if ( aerospike_lmap_ldt_exists(&as, &err, NULL, &key, &lmap, &ldt_exists) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LMAP bin, will return error.
 *	@param ldt_exists	Ptr to as_boolean: Set to TRUE if ldt exists, otherwise false.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lmap_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);


#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 * Functionality related to Large Set Data Type
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_error.h>
#include <aerospike/as_ldt.h>
#include <aerospike/as_list.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_status.h>
#include <aerospike/as_key.h>
#include <aerospike/as_val.h>
#include <aerospike/as_boolean.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Add a value into the lset.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *	
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_lset_add(&as, &err, NULL, &key, &lset, (as_val *) &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param val			The value to insert into the lset.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_add(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val);

/**
 *	Add a list of values into the lset.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *
 *
 *	as_arraylist vals;
 *	as_arraylist_inita(&vals, 2);
 *	as_string s;
 *	as_string_init(s,"a string",false);
 *	as_arraylist_append_string(&vals, s);
 *	as_arraylist_append_int64(&vals, 35);
 *
 *	if ( aerospike_lset_add_all(&as, &err, NULL, &key, &lset, (as_list *)vals) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to insert values to.
 *	@param vals			The list of values to insert into the lset.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_add_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_list * vals);

/**
 *	See if a value exists in an lset
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *  boolean exists = false;
 *	
 *	if ( aerospike_lset_exists(&as, &err, NULL, &key, &lset, &ival, &exists) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// do logic because element exists
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset bin to lookup from. If not an lset bin, will return error.
 *	@param val          The value we're searching for.
 *	@param exists 		Returned boolean value to indicate value exists.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *	
 *	@ingroup ldt_operations
 */

as_status aerospike_lset_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val,
	as_boolean *exists);


/**
 *	Fetch (get) a value from the lset.
 *	Note that this is useful mainly in the case where the search criteria for
 *	an object is less than the entire object -- and that is when the standard
 *	defaults are overridden and the unique_identifier() function is employed
 *	to use only part of the object for search and compare. 
 *	The unique_identifier() function is defined on create -- and declared in
 *	the USER_MODULE.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *  as_val * p_return_val;
 *	
 *	if ( aerospike_lset_exists(&as, &err, NULL, &key, &lset, &ival, &p_return_value) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// do logic because element exists
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset bin to lookup from. If not an lset bin, will return error.
 *	@param val          The value we're searching for.
 *	@param pp_return_val   Returned value.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *	
 *	@ingroup ldt_operations
 */

as_status aerospike_lset_get(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val,
	as_val ** pp_return_val );

/**
 *	Given an lset bin, filter the set of objects using the given filter function.
 *	If no filter function is specified, all values in the set will be returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *
 *	as_list *list = NULL;
 *
 *	if ( aerospike_lset_filter(&as, &err, NULL, &key, &lset,
 *			"search_filter", NULL, (as_list *) &list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset bin to search from. If not an lset bin, will return error.
 *	@param filter		The name of the User-Defined-Function to use as a search filter.
 *	@param filter_args	The list of parameters passed in to the User-Defined-Function filter.
 *	@param elements		The pointer to a list of elements returned from search function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );

/**
 *	Given an lset bin, scan for all the values in the set
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *
 *	as_list *list = NULL;
 *
 *	if ( aerospike_lset_scan(&as, &err, NULL, &key, &lset,
 *			"search_filter", NULL, (as_list *) &list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned elements
 *		as_arraylist_destroy(list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset bin to search from. If not an lset bin, will return error.
 *	@param elements		The pointer to a list of elements returned from search function. Pointer should
 *						be NULL passed in.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_scan(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_list ** elements );

/**
 *	Look up a lset and find how many elements it contains
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *	uint32_t lset_size = 0;
 *
 *	if ( aerospike_lset_size(&as, &err, NULL, &key, &lset, &lset_size) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset to operate on. If not an lset bin, will return error.
 *	@param n			Return the number of elements in the lset.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);

/**
 *	Delete the given value from the lset
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "lset", AS_LDT_LSET, NULL);
 *
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_lset_remove(&as, &err, NULL, &key, &lset, &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset bin to delete from. If not an lset bin, will return error.
 *	@param element		The value to delete from the set.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_remove(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val *element
	);

/**
 *	Destroy the lset bin
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *
 *	if ( aerospike_lset_destroy(&as, &err, NULL, &key, &lset) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The lset bin to destroy. If not an lset bin, will return error.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);

/**
 *	Change an LDT storage capacity (in number of elements)
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LMAP, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_lset_set_capacity(&as, &err, NULL, &key, &lset, ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to check
 *	@param ldt_capacity	The number of elements cap for the LDT.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t ldt_capacity
	);

/**
 *	Get an LDTs storage capacity (in number of elements)
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LMAP, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_lset_get_capacity(&as, &err, NULL, &key, &lset, &ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT bin to operate on
 *	@param ldt_capacity The LDT Capacity, in terms of elements, not bytes.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);

/**
 *	Check to see if an LDT object exists in this record bin.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lset;
 *	as_ldt_init(&lset, "mylset", AS_LDT_LSET, NULL);
 *	uint32_t ldt_exists = 0;
 *
 *	if ( aerospike_lset_ldt_exists(&as, &err, NULL, &key, &lset, &ldt_exists) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LMAP bin, will return error.
 *	@param ldt_exists	Ptr to as_boolean: Set to TRUE if ldt exists, otherwise false.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lset_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 *	@defgroup ldt_operations Large Data Type Operations
 *	@ingroup client_operations
 *
 *	The ldt_operations module provides API to manipulate
 *	Large Data Types. Currently supported types include:
 *	   - lstack = large stack
 *	   - lset = large set
 *
 *	Forthcoming API:
 * 	   - llist = large list
 * 	   - lmap = large map
 *
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_error.h>
#include <aerospike/as_ldt.h>
#include <aerospike/as_list.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_status.h>
#include <aerospike/as_key.h>
#include <aerospike/as_val.h>
#include <aerospike/as_boolean.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Push a value onto the lstack.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *	
 *	as_integer ival;
 *	as_integer_init(&ival, 123);
 *
 *	if ( aerospike_lstack_push(&as, &err, NULL, &key, &stack, (as_val *) &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to push values to.
 *	@param val			The value to push on to the lstack.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_push(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_val * val);


/**
 *	Push a value onto the lstack.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *
 *	as_arraylist vals;
 *	as_arraylist_inita(&vals, 2);
 *	as_string s;
 *	as_string_init(s,"a string",false);
 *	as_arraylist_append_string(&vals, s);
 *	as_arraylist_append_int64(&vals, 35);
 *
 *	if ( aerospike_lstack_push_all(&as, &err, NULL, &key, &stack, (as_val *) &ival) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The ldt bin to push values to.
 *	@param vals			The list of values to push on to the lstack. list[0] is the first to push on the stack.
 *						list[n] is top of the stack.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_push_all(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, const as_list * vals);

/**
 *	Look up an lstack, then peek to get the top n values from the stack.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *
 *  uint32_t peek_count = 3;
 *
 *	as_arraylist list = as_arraylist_init(&list, peek_count, 0);
 *	
 *	if ( aerospike_lstack_peek(&as, &err, NULL, &key, &stack, peek_count, (as_list *) &list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned stack elements
 *		as_arraylist_destroy(list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The stack bin to peek values from. If not a stack bin, will return error.
 *	@param peek_count	The number of elements to peek from the lstack.
 *	@param elements		Pointer to a list of elements peeked from the lstack.
 *						Pointer should be NULL passed in.
 *						If stack_size shorter than n, only stack_size is returned.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *	
 *	@ingroup ldt_operations
 */

as_status aerospike_lstack_peek(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t peek_count,
	as_list ** elements );

/**
 *	Look up a record by key, then peek into a stack bin, and do UDF post processing
 *	to filter for only the desired values.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *
 *  uint32_t peek_count = 3;
 *
 *	as_arraylist list = as_arraylist_init(&list, peek_count, 0);
 *
 *	if ( aerospike_lstack_peek_with_filter(&as, &err, NULL, &key, &stack, peek_count,
 *			"myfilter", NULL, (as_list *) &list) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		// process the returned stack elements
 *		as_arraylist_destroy(list);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The stack bin to peek values from. If not a stack bin, will return error.
 *	@param peek_count	The number of elements to peek from the lstack.
 *	@param filter		The name of the User-Defined-Function to use as a stack element filter.
 *	@param filter_args	The list of parameters to the User-Defined-Function filter.
 *	@param elements		Pointer to list of elements peeked from the lstack.
 *						Pointer should be initialized to NULL when passed in;
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_filter(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t peek_count,
	const as_udf_function_name filter, const as_list *filter_args,
	as_list ** elements );

/**
 *	Destroys an existing lstack
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *	uint32_t cap_elements = 0;
 *
 *	if ( aerospike_lstack_destroy(&as, &err, NULL, &key, &stack) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The stack bin to peek values from. If not a stack bin, will return error.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */

as_status aerospike_lstack_destroy(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt
	);

/**
 *	Find how many elements are on the lstack
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *	uint32_t stack_size = 0;
 *
 *	if ( aerospike_lstack_size(&as, &err, NULL, &key, &stack, &stack_size) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The stack bin to peek values from. If not a stack bin, will return error.
 *	@param n			Return the number of elements on the lstack.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_size(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *n
	);

/**
 *	Change an LDT storage capacity (in number of elements)
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_lstack_set_capacity(&as, &err, NULL, &key, &stack, ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The stack bin to peek values from. If not a stack bin, will return error.
 *	@param ldt_capacity	The number of elements cap for the lstack.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_set_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt, uint32_t ldt_capacity
	);

/**
 *	Get an lstack's  storage capacity (in number of elements)
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt stack;
 *	as_ldt_init(&stack, "mystack", AS_LDT_LSTACK, NULL);
 *	uint32_t ldt_capacity = 0;
 *
 *	if ( aerospike_lstack_get_capacity(&as, &err, NULL, &key, &stack, &ldt_capacity) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The stack bin to peek values from. If not a stack bin, will return error.
 *	@param ldt_capacity The LDT Capacity, in terms of elements, not bytes.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_get_capacity(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	uint32_t *ldt_capacity
	);

/**
 *	Check to see if an LSTACK object exists in this record bin.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "myns", "myset", "mykey");
 *
 *	as_ldt lstack;
 *	as_ldt_init(&lstack, "mylstack", AS_LDT_LSTACK, NULL);
 *	uint32_t ldt_exists = 0;
 *
 *	if ( aerospike_lstack_size(&as, &err, NULL, &key, &lstack, &ldt_exists) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param key			The key of the record.
 *	@param ldt 			The LDT to operate on. If not an LSTACK bin, will return error.
 *	@param ldt_exists	Ptr to as_boolean: Set to TRUE if ldt exists, otherwise false.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error.
 *
 *	@ingroup ldt_operations
 */
as_status aerospike_lstack_ldt_exists(
	aerospike * as, as_error * err, const as_policy_apply * policy,
	const as_key * key, const as_ldt * ldt,
	as_boolean *ldt_exists
	);


#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/** 
 *	@defgroup query_operations Query Operations
 *	@ingroup client_operations
 *
 *	The Aerospike Query Operations provide the ability to query data in the 
 *	Aerospike database. The queries can only be performed on secondary indexes, 
 *	which have been created in the database. To scan all the records in the
 *	database, then you must use the @ref scan_operations.
 *	
 *	## Usage
 *
 *	Before you can execute a query, you first need to build a query using 
 *	as_query. See as_query for details on building queries.
 *
 *	Once you have a query defined, then you can execute the query :
 *
 *	-	aerospike_query_foreach() -	Executes a query and invokes a callback
 *		function for each result returned.
 *	
 *	When aerospike_query_foreach() is executed, it will process the results
 *	and create records on the stack. Because the records are on the stack, 
 *	they will only be available within the context of the callback function.
 *
 *
 *	## Walk-through
 *	
 *	First, we define a query using as_query. The query will be for the "test"
 *	namespace and "demo" set. We will add a where predicate on "bin2", on which
 *	we have already created a secondary index. 
 *	
 *	~~~~~~~~~~{.c}
 *	as_query query;
 *	as_query_init(&query, "test", "demo");
 *
 *	as_query_where_init(&query, 1);
 *	as_query_where(&query, "bin2", as_integer_equals(100));
 *	~~~~~~~~~~
 *
 *	Now that we have a query defined, we want to execute it using 
 *	aerospike_query_foreach().
 *	
 *	~~~~~~~~~~{.c}
 *	if ( aerospike_query_foreach(&as, &err, NULL, &query, callback, NULL) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	The callback provided to the function above is implemented as:
 *	
 *	~~~~~~~~~~{.c}
 *	bool callback(const as_val * val, void * udata) {
 *		as_record * rec = as_record_fromval(val);
 *		if ( !rec ) return false;
 *		fprintf("record contains %d bins", as_record_numbins(rec));
 *		return true;
 *	}
 *	~~~~~~~~~~
 *
 *	An as_query is simply a query definition, so it does not contain any state,
 *	allowing it to be reused for multiple query operations. 
 *	
 *	When you are finished with the query, you should destroy the resources 
 *	allocated to it:
 *
 *	~~~~~~~~~~{.c}
 *	as_query_destroy(&query);
 *	~~~~~~~~~~
 *
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_error.h>
#include <aerospike/as_event.h>
#include <aerospike/as_job.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_query.h>
#include <aerospike/as_record.h>
#include <aerospike/as_status.h>
#include <aerospike/as_stream.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	This callback will be called for each value or record returned from a query.
 *	Multiple threads will likely be calling this callback in parallel.  Therefore,
 *	your callback implementation should be thread safe.
 *
 *	The aerospike_query_foreach() function accepts this callback.
 *
 *	~~~~~~~~~~{.c}
 *	bool my_callback(as_val * val, void * udata) {
 *		return true;
 *	}
 *	~~~~~~~~~~
 *
 *	@param val 			The value received from the query.
 *	@param udata 		User-data provided to the calling function.
 *
 *	@return `true` to continue to the next value. Otherwise, iteration will end.
 *
 *	@ingroup query_operations
 */
typedef bool (*aerospike_query_foreach_callback)(const as_val* val, void* udata);

/**
 *	Asynchronous query user callback.  This function is called for each record returned.
 *	This function is also called once when the query completes or an error has occurred.
 *
 *	@param err			This error structure is only populated when the command fails. Null on success.
 *	@param record 		Returned record.  Use as_val_reserve() on record to prevent calling function from destroying.
 *						The record will be null on final query completion or query error.
 *	@param udata 		User data that is forwarded from asynchronous command function.
 *	@param event_loop 	Event loop that this command was executed on.  Use this event loop when running
 *						nested asynchronous commands when single threaded behavior is desired for the
 *						group of commands.
 *
 *	@return `true` to continue to the next value. Otherwise, the query will end.
 *
 *	@ingroup query_operations
 */
typedef bool (*as_async_query_record_listener)(as_error* err, as_record* record, void* udata, as_event_loop* event_loop);

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Execute a query and call the callback function for each result item.
 *	Multiple threads will likely be calling the callback in parallel.  Therefore,
 *	your callback implementation should be thread safe.
 *
 *	~~~~~~~~~~{.c}
 *	as_query query;
 *	as_query_init(&query, "test", "demo");
 *	as_query_select(&query, "bin1");
 *	as_query_where(&query, "bin2", as_integer_equals(100));
 *	
 *	if ( aerospike_query_foreach(&as, &err, NULL, &query, callback, NULL) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	
 *	as_query_destroy(&query);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param query		The query to execute against the cluster.
 *	@param callback		The callback function to call for each result value.
 *	@param udata		User-data to be passed to the callback.
 *
 *	@return AEROSPIKE_OK on success, otherwise an error.
 *
 *	@ingroup query_operations
 */
as_status
aerospike_query_foreach(
	aerospike* as, as_error* err, const as_policy_query* policy, const as_query* query,
	aerospike_query_foreach_callback callback, void* udata
	);

/**
 *	Asynchronously execute a query and call the listener function for each result item.
 *	Standard secondary index queries are supported, but aggregation queries are not supported
 *	in async mode.
 *
 *	~~~~~~~~~~{.c}
 *	bool my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Query failed: %d %s\n", err->code, err->message);
 *			return false;
 *		}
 *
 *		if (! record) {
 *			printf("Query ended\n");
 *			return false;
 *		}
 *
 *		// Process record
 *		// Do not call as_record_destroy() because the calling function will do that for you.
 *		return true;
 *	}
 *	as_query query;
 *	as_query_init(&query, "test", "demo");
 *	as_query_select(&query, "bin1");
 *	as_query_where(&query, "bin2", as_integer_equals(100));
 *
 *	if ( aerospike_query_foreach(&as, &err, NULL, &query, callback, NULL) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	as_query_destroy(&query);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param query		The query to execute against the cluster.
 *	@param listener		The function to be called for each returned value.
 *	@param udata		User-data to be passed to the callback.
 *	@param event_loop 	Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *
 *	@return AEROSPIKE_OK if async query succesfully queued. Otherwise an error.
 *
 *	@ingroup query_operations
 */
as_status
aerospike_query_async(
	aerospike* as, as_error* err, const as_policy_query* policy, const as_query* query,
	as_async_query_record_listener listener, void* udata, as_event_loop* event_loop
	);
	
/**
 *	Apply user defined function on records that match the query filter.
 *	Records are not returned to the client.
 *	This asynchronous server call will return before command is complete.
 *	The user can optionally wait for command completion.
 *
 *	~~~~~~~~~~{.c}
 *	as_query query;
 *	as_query_init(&query, "test", "demo");
 *	as_query_select(&query, "bin1");
 *	as_query_where(&query, "bin2", as_integer_equals(100));
 *	as_query_apply(&query, "my_lua.lua", "my_lua_function", NULL);
 *	uint64_t query_id = 0;
 *
 *	if (aerospike_query_background(&as, &err, NULL, &query, &query_id) == AEROSPIKE_OK) {
 *		aerospike_query_wait(as, &err, NULL, &query, query_id, 0);
 *	}
 *	else {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	as_query_destroy(&query);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param query		The query to execute against the cluster.
 *	@param query_id		The id for the query job, which can be used for querying the status of the query.
 *
 *	@return AEROSPIKE_OK on success, otherwise an error.
 *
 *	@ingroup query_operations
 */
as_status
aerospike_query_background(
	aerospike* as, as_error* err, const as_policy_write* policy,
	const as_query* query, uint64_t* query_id
	);

/**
 *	Wait for a background query to be completed by servers.
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The info policy to use for this operation. If NULL, then the default policy will be used.
 *	@param query		The query that was executed against the cluster.
 *	@param query_id		The id for the query job, which can be used for querying the status of the query.
 *	@param interval_ms	Polling interval in milliseconds. If zero, 1000 ms is used.
 *
 *	@return AEROSPIKE_OK on success, otherwise an error.
 *
 *	@ingroup query_operations
 */
static inline as_status
aerospike_query_wait(
   aerospike* as, as_error* err, const as_policy_info* policy,
   const as_query* query, uint64_t query_id, uint32_t interval_ms
   )
{
	const char* module = (query->where.size > 0)? "query" : "scan";
	return aerospike_job_wait(as, err, policy, module, query_id, interval_ms);
}

/**
 *	Check the progress of a background query running on the database.
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The info policy to use for this operation. If NULL, then the default policy will be used.
 *	@param query		The query that was executed against the cluster.
 *	@param query_id		The id for the query job, which can be used for querying the status of the query.
 *	@param info			Information about this background query, to be populated by this operation.
 *
 *	@return AEROSPIKE_OK on success, otherwise an error.
 *
 *	@ingroup query_operations
 */
static inline as_status
aerospike_query_info(
	aerospike* as, as_error* err, const as_policy_info* policy,
	const as_query* query, uint64_t query_id, as_job_info* info
	)
{
	const char* module = (query->where.size > 0)? "query" : "scan";
	return aerospike_job_info(as, err, policy, module, query_id, false, info);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/** 
 *	@defgroup scan_operations Scan Operations
 *	@ingroup client_operations
 *
 *	Aerospike Scan Operations provide the ability to scan all record of a 
 *	namespace and set in an Aerospike database. 
 *
 *	## Usage
 *
 *	Before you can execute a scan, you first need to define a scan using 
 *	as_scan. See as_scan for details on defining scans.
 *
 *	Once you have a scan defined, then you can execute the scan
 *	using either:
 *
 *	- aerospike_scan_foreach() — Execute a scan on the database, then process 
 *		the results.
 *	- aerospike_scan_background() — Send a scan to the database, and not wait 
 *		for completed. The scan is given an id, which can be used to query the
 *		scan status.
 *
 *	When aerospike_scan_foreach() is executed, it will process the results
 *	and create records on the stack. Because the records are on the stack, 
 *	they will only be available within the context of the callback function.
 *
 *	When aerospike_scan_background() is executed, the client will not wait for 
 *	results from the database. Instead, the client will be given a scan_id, 
 *	which can be used to query the scan status on the database via 
 *	aerospike_scan_info().
 *
 *	## Walk-through
 *	
 *	First, we build a scan using as_scan. The scan will be on the "test"
 *	namespace and "demo" set. We will select only bins "a" and "b" to be returned 
 *	for each record.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *
 *	as_scan_select_inita(&scan, 2);
 *	as_scan_select(&scan, "a");
 *	as_scan_select(&scan, "B");
 *	~~~~~~~~~~
 *	
 *	Now that we have a scan defined, we want to execute it using 
 *	aerospike_scan_foreach().
 *	
 *	~~~~~~~~~~{.c}
 *	if ( aerospike_scan_foreach(&as, &err, NULL, &scan, callback, NULL) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *	
 *	The callback provided to the function above is implemented as:
 *	
 *	~~~~~~~~~~{.c}
 *	bool callback(const as_val * val, void * udata) {
 *		as_record * rec = as_record_fromval(val);
 *		if ( !rec ) return false;
 *		fprintf("record contains %d bins", as_record_numbins(rec));
 *		return true;
 *	}
 *	~~~~~~~~~~
 *	
 *	An as_scan is simply a scan definition, so it does not contain any state,
 *	allowing it to be reused for multiple scan operations. 
 *	
 *	When you are finished with the scan, you should destroy the resources 
 *	allocated to it:
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_destroy(&scan);
 *	~~~~~~~~~~
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_listener.h>
#include <aerospike/as_error.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_record.h>
#include <aerospike/as_scan.h>
#include <aerospike/as_status.h>
#include <aerospike/as_val.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	This callback will be called for each value or record returned from a synchronous scan.
 *	Multiple threads will likely be calling this callback in parallel.  Therefore,
 *	your callback implementation should be thread safe.
 *
 *	The following functions accept the callback:
 *	-	aerospike_scan_foreach()
 *	-	aerospike_scan_node()
 *	
 *	~~~~~~~~~~{.c}
 *	bool my_callback(const as_val * val, void * udata) {
 *		return true;
 *	}
 *	~~~~~~~~~~
 *
 *	@param val 			The value received from the query.
 *	@param udata 		User-data provided to the calling function.
 *
 *	@return `true` to continue to the next value. Otherwise, the scan will end.
 *
 *	@ingroup scan_operations
 */
typedef bool (*aerospike_scan_foreach_callback)(const as_val* val, void* udata);

/**
 *	Asynchronous scan user callback.  This function is called for each record returned.
 *	This function is also called once when the scan completes or an error has occurred.
 *
 *	@param err			This error structure is only populated when the command fails. Null on success.
 *	@param record 		Returned record.  Use as_val_reserve() on record to prevent calling function from destroying.
 *						The record will be null on final scan completion or scan error.
 *	@param udata 		User data that is forwarded from asynchronous command function.
 *	@param event_loop 	Event loop that this command was executed on.  Use this event loop when running
 *						nested asynchronous commands when single threaded behavior is desired for the
 *						group of commands.
 *
 *	@return `true` to continue to the next value. Otherwise, the scan will end.
 *
 *	@ingroup scan_operations
 */
typedef bool (*as_async_scan_listener)(as_error* err, as_record* record, void* udata, as_event_loop* event_loop);

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Scan the records in the specified namespace and set in the cluster.
 *
 *	Scan will be run in the background by a thread on client side.
 *	No callback will be called in this case.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *	
 *	uint64_t scanid = 0;
 *	
 *	if (aerospike_scan_background(&as, &err, NULL, &scan, &scanid) != AEROSPIKE_OK) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		printf("Running background scan job: %ll", scanid);
 *	}
 *	as_scan_destroy(&scan);
 *	~~~~~~~~~~
 *
 *	The scanid can be used to query the status of the scan running in the 
 *	database via aerospike_scan_info().
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan 		The scan to execute against the cluster.
 *	@param scan_id		The id for the scan job, which can be used for querying the status of the scan.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 *
 *	@ingroup scan_operations
 */
as_status
aerospike_scan_background(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan,
	uint64_t* scan_id
	);

/**
 *	Wait for a background scan to be completed by servers.
 *
 *	~~~~~~~~~~{.c}
 *	uint64_t scan_id = 1234;
 *	aerospike_scan_wait(&as, &err, NULL, scan_id, 0);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan_id		The id for the scan job.
 *	@param interval_ms	The polling interval in milliseconds. If zero, 1000 ms is used.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 */
as_status
aerospike_scan_wait(
	aerospike* as, as_error* err, const as_policy_info* policy, uint64_t scan_id,
	uint32_t interval_ms
	);

/**
 *	Check the progress of a background scan running on the database. The status
 *	of the scan running on the datatabse will be populated into an as_scan_info.
 *	
 *	~~~~~~~~~~{.c}
 *	uint64_t scan_id = 1234;
 *	as_scan_info scan_info;
 *	
 *	if (aerospike_scan_info(&as, &err, NULL, &scan, scan_id, &scan_info) != AEROSPIKE_OK) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		printf("Scan id=%ll, status=%d percent=%d", scan_id, scan_info.status, scan_info.progress_pct);
 *	}
 *	~~~~~~~~~~
 *	
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan_id		The id for the scan job to check the status of.
 *	@param info			Information about this scan, to be populated by this operation.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 *
 *	@ingroup scan_operations
 */
as_status
aerospike_scan_info(
	aerospike* as, as_error* err, const as_policy_info* policy, uint64_t scan_id, as_scan_info* info
	);

/**
 *	Scan the records in the specified namespace and set in the cluster.
 *
 *	Call the callback function for each record scanned. When all records have 
 *	been scanned, then callback will be called with a NULL value for the record.
 *
 *	Multiple threads will likely be calling the callback in parallel.  Therefore,
 *	your callback implementation should be thread safe.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *	
 *	if (aerospike_scan_foreach(&as, &err, NULL, &scan, callback, NULL) != AEROSPIKE_OK) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	as_scan_destroy(&scan);
 *	~~~~~~~~~~
 *	
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan			The scan to execute against the cluster.
 *	@param callback		The function to be called for each record scanned.
 *	@param udata		User-data to be passed to the callback.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 *
 *	@ingroup scan_operations
 */
as_status
aerospike_scan_foreach(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan,
	aerospike_scan_foreach_callback callback, void* udata
	);

/**
 *	Scan the records in the specified namespace and set for a single node.
 *
 *	The callback function will be called for each record scanned. When all records have
 *	been scanned, then callback will be called with a NULL value for the record.
 *
 *	~~~~~~~~~~{.c}
 *	char* node_names = NULL;
 *	int n_nodes = 0;
 *	as_cluster_get_node_names(as->cluster, &n_nodes, &node_names);
 *
 *	if (n_nodes <= 0)
 *		return <error>;
 *
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *
 *	if (aerospike_scan_node(&as, &err, NULL, &scan, node_names[0], callback, NULL) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *
 *	free(node_names);
 *	as_scan_destroy(&scan);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan			The scan to execute against the cluster.
 *	@param node_name	The node name to scan.
 *	@param callback		The function to be called for each record scanned.
 *	@param udata		User-data to be passed to the callback.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 *
 *	@ingroup scan_operations
 */
as_status
aerospike_scan_node(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan,
	const char* node_name, aerospike_scan_foreach_callback callback, void* udata
	);

/**
 *	Asynchronously scan the records in the specified namespace and set in the cluster.
 *
 *	Call the listener function for each record scanned. When all records have
 *	been scanned, then listener will be called with a NULL value for the record.
 *
 *	Scans of each node will be run on the same event loop, so the listener's implementation does
 *	not need to be thread safe.
 *
 *	~~~~~~~~~~{.c}
 *	bool my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Scan failed: %d %s\n", err->code, err->message);
 *			return false;
 *		}
 *
 *		if (! record) {
 *			printf("Scan ended\n");
 *			return false;
 *		}
 *
 *		// Process record
 *		// Do not call as_record_destroy() because the calling function will do that for you.
 *		return true;
 *	}
 *
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *
 *	as_status status = aerospike_scan_async(&as, &err, NULL, &scan, NULL, my_listener, NULL, NULL);
 *	as_scan_destroy(&scan);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan			The scan to execute against the cluster.
 *	@param scan_id		The id for the scan job.  Use NULL if the scan_id will not be used.
 *	@param listener		The function to be called for each record scanned.
 *	@param udata		User-data to be passed to the callback.
 *	@param event_loop 	Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *
 *	@return AEROSPIKE_OK if async scan succesfully queued. Otherwise an error.
 *
 *	@ingroup scan_operations
 */
as_status
aerospike_scan_async(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan, uint64_t* scan_id,
	as_async_scan_listener listener, void* udata, as_event_loop* event_loop
	);
	
/**
 *	Asynchronously scan the records in the specified namespace and set for a single node.
 *
 *	The listener function will be called for each record scanned. When all records have
 *	been scanned, then callback will be called with a NULL value for the record.
 *
 *	~~~~~~~~~~{.c}
 *	bool my_listener(as_error* err, as_record* record, void* udata, as_event_loop* event_loop)
 *	{
 *		if (err) {
 *			printf("Scan failed: %d %s\n", err->code, err->message);
 *			return false;
 *		}
 *
 *		if (! record) {
 *			printf("Scan ended\n");
 *			return false;
 *		}
 *
 *		// Process record
 *		// Do not call as_record_destroy() because the calling function will do that for you.
 *		return true;
 *	}
 *
 *	char* node_names = NULL;
 *	int n_nodes = 0;
 *	as_cluster_get_node_names(as->cluster, &n_nodes, &node_names);
 *
 *	if (n_nodes <= 0)
 *		return <error>;
 *
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *
 *	as_status status = aerospike_scan_node_async(&as, &err, NULL, &scan, NULL, node_names[0], my_listener, NULL, NULL);
 *
 *	free(node_names);
 *	as_scan_destroy(&scan);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param scan			The scan to execute against the cluster.
 *	@param scan_id		The id for the scan job.  Use NULL if the scan_id will not be used.
 *	@param node_name	The node name to scan.
 *	@param listener		The function to be called for each record scanned.
 *	@param udata		User-data to be passed to the callback.
 *	@param event_loop 	Event loop assigned to run this command. If NULL, an event loop will be choosen by round-robin.
 *
 *	@return AEROSPIKE_OK if async scan succesfully queued. Otherwise an error.
 *
 *	@ingroup scan_operations
 */
as_status
aerospike_scan_node_async(
	aerospike* as, as_error* err, const as_policy_scan* policy, const as_scan* scan, uint64_t* scan_id,
	const char* node_name, as_async_scan_listener listener, void* udata, as_event_loop* event_loop
	);
	
#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/** 
 *	@defgroup udf_operations UDF Operations
 *	@ingroup client_operations
 *
 *	The UDF API provides the ability to manage UDFs in the cluster.
 *
 *	Management capabilities include:
 *	- aerospike_udf_list() - 	List the UDF modules in the cluster.
 *	- aerospike_udf_get() -		Download a UDF module.
 *	- aerospike_udf_put() -		Upload a UDF module.
 *	- aerospike_udf_remove() -	Remove a UDF module.
 *
 */

#include <aerospike/aerospike.h>
#include <aerospike/as_error.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_status.h>
#include <aerospike/as_udf.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	List the UDF files in the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	as_udf_files files;
 *	as_udf_files_init(&files, 0);
 *	
 *	if ( aerospike_udf_list(&as, &err, NULL, &files) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		printf("files[%d]:\n", files.size);
 *		for( int i = 0; i < files.size; i++ ) {
 *			as_udf_file * file = &files.entries[i];
 *			printf("  - %s (%d) [%s]\n", file->name, file->type, file->hash);
 *		}
 *	}
 *	
 *	as_udf_files_destroy(&files);
 *	~~~~~~~~~~
 *
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param files 		The list to populate with the results from the request.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error occurred.
 *
 *	@ingroup udf_operations
 */
as_status aerospike_udf_list(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	as_udf_files * files
	);


/**
 *	Get specified UDF file from the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	as_udf_file file;
 *	as_udf_file_init(&file);
 *	
 *	if ( aerospike_udf_get(&as, &err, NULL, "my.lua", AS_UDF_TYPE_LUA, &file) != AEROSPIKE_OK ) {
 *	    fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *	    printf("%s type=%d hash=%s size=%d:\n", file.name, file.type. file.hash, file.content.size);
 *	    if ( file.type == AS_UDF_TYPE_UDF ) {
 *	        printf("%s", file.content.bytes)
 *	    }
 *	}
 *	
 *	as_udf_file_destroy(&file);
 *	~~~~~~~~~~
 *
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param filename		The name of the UDF file.
 *	@param type			The type of UDF file.
 *	@param file			The file from the cluster.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error occurred.
 *
 *	@ingroup udf_operations
 */
as_status aerospike_udf_get(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * filename, as_udf_type type, as_udf_file * file
	);

/**
 *	Put a UDF file into the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	as_bytes content;
 *	as_bytes_init(&content);
 *	...
 *	
 *	if ( aerospike_udf_put(&as, &err, NULL, "my.lua", AS_UDF_TYPE_LUA, &content) != AEROSPIKE_OK ) {
 *	    fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	
 *	as_bytes_destroy(&content);
 *	~~~~~~~~~~
 *
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param filename		The name of the UDF file.
 *	@param type			The type of UDF file.
 *	@param content		The file of the UDF file.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error occurred.
 *
 *	@ingroup udf_operations
 */
as_status aerospike_udf_put(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * filename, as_udf_type type, as_bytes * content
	);

/**
 *	Wait for asynchronous udf put to complete using given polling interval.
 *
 *	~~~~~~~~~~{.c}
 *	as_bytes content;
 *	as_bytes_init(&content);
 *
 *	if (aerospike_udf_put(&as, &err, NULL, "my.lua", AS_UDF_TYPE_LUA, &content) == AEROSPIKE_OK ) {
 *	    aerospike_udf_put_wait(&as, &err, NULL, "my.lua", 0);
 *	}
 *	as_bytes_destroy(&content);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param filename		The name of the UDF file.
 *	@param interval_ms	The polling interval in milliseconds. If zero, 1000 ms is used.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error occurred.
 *
 *	@ingroup udf_operations
 */
as_status aerospike_udf_put_wait(
	aerospike * as, as_error * err, const as_policy_info * policy,
	const char * filename, uint32_t interval_ms);

/**
 *	Remove a UDF file from the cluster.
 *
 *	~~~~~~~~~~{.c}
 *	if ( aerospike_udf_remove(&as, &err, NULL, "my.lua") != AEROSPIKE_OK ) {
 *	    fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param filename 		The name of the UDF file.
 *
 *	@return AEROSPIKE_OK if successful. Otherwise an error occurred.
 *
 *	@ingroup udf_operations
 */
as_status aerospike_udf_remove(
	aerospike * as, as_error * err, const as_policy_info * policy, 
	const char * filename
	);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/aerospike.h>
#include <aerospike/as_config.h>
#include <aerospike/as_key.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Maximum size of role string including null byte.
 */
#define AS_ROLE_SIZE 32

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Permission codes define the type of permission granted for a user's role.
 */
typedef enum as_privilege_code_e {
	/**
	 *	User can edit/remove other users.  Global scope only.
	 */
	AS_PRIVILEGE_USER_ADMIN = 0,
	
	/**
	 *	User can perform systems administration functions on a database that do not involve user
	 *	administration.  Examples include setting dynamic server configuration.
	 *	Global scope only.
	 */
	AS_PRIVILEGE_SYS_ADMIN = 1,
	
	/**
	 *	User can perform data administration functions on a database that do not involve user
	 *	administration.  Examples include create/drop index and user defined functions. 
	 *	Global scope only.
	 */
	AS_PRIVILEGE_DATA_ADMIN = 2,

	/**
	 *	User can read data only.
	 */
	AS_PRIVILEGE_READ = 10,
	
	/**
	 *	User can read and write data.
	 */
	AS_PRIVILEGE_READ_WRITE = 11,
	
	/**
	 *	User can read and write data through user defined functions.
	 */
	AS_PRIVILEGE_READ_WRITE_UDF = 12
} as_privilege_code;

/**
 *	User privilege.
 */
typedef struct as_privilege_s {
	/**
	 *	Namespace scope.  Apply permission to this null terminated namespace only.
	 *	If string length is zero, the privilege applies to all namespaces.
	 */
	as_namespace ns;
	
	/**
	 *	Set name scope.  Apply permission to this null terminated set within namespace only.
	 *	If string length is zero, the privilege applies to all sets within namespace.
	 */
	as_set set;
	
	/**
	 *	Privilege code.
	 */
	as_privilege_code code;
} as_privilege;

/**
 *	Role definition.
 */
typedef struct as_role_s {
	/**
	 *	Role name.
	 */
	char name[AS_ROLE_SIZE];
	
	/**
	 *	Length of privileges array.
	 */
	int privileges_size;
	
	/**
	 *	Array of assigned privileges.
	 */
	as_privilege privileges[];
} as_role;

/**
 *	User and assigned roles.
 */
typedef struct as_user_s {
	/**
	 *	User name.
	 */
	char name[AS_USER_SIZE];
	
	/**
	 *	Length of roles array.
	 */
	int roles_size;

	/**
	 *	Array of assigned role names.
	 */
	char roles[][AS_ROLE_SIZE];
} as_user;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	Create user with password and roles.  Clear-text password will be hashed using bcrypt before 
 *	sending to server.
 */
as_status
aerospike_create_user(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char* password, const char** roles, int roles_size);

/**
 *	Remove user from cluster.
 */
as_status
aerospike_drop_user(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name);

/**
 *	Set user's password by user administrator.  Clear-text password will be hashed using bcrypt before sending to server.
 */
as_status
aerospike_set_password(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char* password);

/**
 *	Change user's password by user.  Clear-text password will be hashed using bcrypt before sending to server.
 */
as_status
aerospike_change_password(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char* password);

/**
 *	Add role to user's list of roles.
 */
as_status
aerospike_grant_roles(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char** roles, int roles_size);

/**
 *	Remove role from user's list of roles.
 */
as_status
aerospike_revoke_roles(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, const char** roles, int roles_size);

/**
 *	Create user defined role.
 */
as_status
aerospike_create_role(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role, as_privilege** privileges, int privileges_size);

/**
 *	Delete user defined role.
 */
as_status
aerospike_drop_role(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role);

/**
 *	Add specified privileges to user.
 */
as_status
aerospike_grant_privileges(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role, as_privilege** privileges, int privileges_size);

/**
 *	Remove specified privileges from user.
 */
as_status
aerospike_revoke_privileges(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role, as_privilege** privileges, int privileges_size);

/**
 *	Retrieve roles for a given user.
 *	When successful, as_user_destroy() must be called to free resources.
 */
as_status
aerospike_query_user(aerospike* as, as_error* err, const as_policy_admin* policy, const char* user_name, as_user** user);

/**
 *	Release as_user_roles memory.
 */
void
as_user_destroy(as_user* user);

/**
 *	Retrieve all users and their roles.
 *	When successful, as_users_destroy() must be called to free resources.
 */
as_status
aerospike_query_users(aerospike* as, as_error* err, const as_policy_admin* policy, as_user*** users, int* users_size);

/**
 *	Release memory for as_user_roles array.
 */
void
as_users_destroy(as_user** users, int users_size);

/**
 *	Retrieve role definition for a given role name.
 *	When successful, as_role_destroy() must be called to free resources.
 */
as_status
aerospike_query_role(aerospike* as, as_error* err, const as_policy_admin* policy, const char* role_name, as_role** role);

/**
 *	Release as_role memory.
 */
void
as_role_destroy(as_role* role);

/**
 *	Retrieve all roles and their privileges.
 *	When successful, as_roles_destroy() must be called to free resources.
 */
as_status
aerospike_query_roles(aerospike* as, as_error* err, const as_policy_admin* policy, as_role*** roles, int* roles_size);

/**
 *	Release memory for as_role array.
 */
void
as_roles_destroy(as_role** roles, int roles_size);

/**
 *	@private
 *	Authenticate user with a server node.  This is done automatically after socket open.
 *  Do not use this method directly.
 */
as_status
as_authenticate(as_error* err, int fd, const char* user, const char* credential, uint64_t deadline_ms);
	
/**
 *	@private
 *	Write authentication command to buffer.  Return buffer length.
 */
uint32_t
as_authenticate_set(const char* user, const char* credential, uint8_t* buffer);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_async_proto.h>
#include <aerospike/as_cluster.h>
#include <aerospike/as_event_internal.h>
#include <aerospike/as_listener.h>
#include <citrusleaf/alloc.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * TYPES
 *****************************************************************************/

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

/******************************************************************************
 * FUNCTIONS
 *****************************************************************************/

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
	
#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_cluster.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * FUNCTIONS
 *****************************************************************************/

uint32_t
as_async_get_pending(as_cluster* cluster);

void
as_async_get_connections(as_cluster* cluster, uint32_t* async_conn, uint32_t* async_conn_pool);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#pragma GCC diagnostic ignored "-Waddress"

#include <aerospike/as_bin.h>
#include <aerospike/as_key.h>
#include <aerospike/as_record.h>
#include <aerospike/as_status.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *	STRUCTURES
 *****************************************************************************/

/**
 *	A collection of keys to be batch processed.
 */
typedef struct as_batch_s {

	/**
	 *	If true, then this structure will be freed when as_batch_destroy() 
	 *	is called.
	 */
	bool _free;
	
	/**
	 *	Sequence of keys in the batch.
	 */
	struct {

		/**
		 *	If true, then this structure will be freed when as_batch_destroy()
		 *	is called.
		 */
		bool _free;

		/**
		 *	The number of keys this structure contains.
		 */
		uint32_t size;

		/**
		 *	The keys contained by this batch.
		 */
		as_key * entries;

	} keys;

} as_batch;

/**
 *	The (key, result, record) for an entry in a batch read.
 *	The result is AEROSPIKE_OK if the record is found,
 *	AEROSPIKE_ERR_RECORD_NOT_FOUND if the transaction succeeds but the record is
 *	not found, or another error code if the transaction fails.
 *	The record is NULL if either the transaction failed or the record does not
 *	exist. For aerospike_batch_exists() calls the record will never contain bins
 *	but will contain metadata (generation and expiration).
 */
typedef struct as_batch_read_s {

	/**
	 *	The key requested.
	 */
	const as_key * key;

	/**
	 *	The result of the transaction to read this key.
	 */
	as_status result;

	/**
	 *	The record for the key requested, NULL if the key was not found.
	 */
	as_record record;

} as_batch_read;


/*********************************************************************************
 *	INSTANCE MACROS
 *********************************************************************************/


/** 
 *	Initializes `as_batch` with specified capacity using alloca().
 *
 *	For heap allocation, use `as_batch_new()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_batch batch;
 *	as_batch_inita(&batch, 2);
 *	as_key_init(as_batch_get(&batch, 0), "ns", "set", "key1");
 *	as_key_init(as_batch_get(&batch, 1), "ns", "set", "key2");
 *	~~~~~~~~~~
 *
 *	When the batch is no longer needed, then use as_batch_destroy() to
 *	release the batch and associated resources.
 *	
 *	@param __batch		The query to initialize.
 *	@param __size		The number of keys to allocate.
 *
 *	@relates as_batch
 *	@ingroup batch_object
 */
#define as_batch_inita(__batch, __size) \
	if ( (__batch) != NULL ) {\
		(__batch)->_free = false;\
		(__batch)->keys.entries = (as_key *) alloca(sizeof(as_key) * __size);\
		if ( (__batch)->keys.entries ) { \
			(__batch)->keys._free = false;\
			(__batch)->keys.size = __size;\
		}\
 	}

/*********************************************************************************
 *	INSTANCE FUNCTIONS
 *********************************************************************************/

/**
 *	Create and initialize a heap allocated as_batch capable of storing
 *	`capacity` keys.
 *
 *	~~~~~~~~~~{.c}
 *	as_batch * batch = as_batch_new(2);
 *	as_key_init(as_batch_get(batch, 0), "ns", "set", "key1");
 *	as_key_init(as_batch_get(batch, 1), "ns", "set", "key2");
 *	~~~~~~~~~~
 *
 *	When the batch is no longer needed, then use as_batch_destroy() to
 *	release the batch and associated resources.
 *	
 *	@param size			The number of keys to allocate.
 *
 *	@relates as_batch
 *	@ingroup batch_object
 */
as_batch * as_batch_new(uint32_t size);

/**
 *	Initialize a stack allocated as_batch capable of storing `capacity` keys.
 *
 *	~~~~~~~~~~{.c}
 *	as_batch batch;
 *	as_batch_init(&batch, 2);
 *	as_key_init(as_batch_get(&batch, 0), "ns", "set", "key1");
 *	as_key_init(as_batch_get(&batch, 1), "ns", "set", "key2");
 *	~~~~~~~~~~
 *
 *	When the batch is no longer needed, then use as_batch_destroy() to
 *	release the batch and associated resources.
 *	
 *	@param batch		The batch to initialize.
 *	@param size			The number of keys to allocate.
 *	
 *	@relates as_batch
 *	@ingroup batch_object
 */
as_batch * as_batch_init(as_batch * batch, uint32_t size);

/**
 *	Destroy the batch of keys.
 *
 *	~~~~~~~~~~{.c}
 *	as_batch_destroy(batch);
 *	~~~~~~~~~~
 *
 *	@param batch 	The batch to release.
 *
 *	@relates as_batch
 *	@ingroup batch_object
 */
void as_batch_destroy(as_batch * batch);

/**
 *	Get the key at given position of the batch. If the position is not
 *	within the allocated capacity for the batchm then NULL is returned.
 *
 *	@param batch 	The batch to get the key from.
 *	@param i		The position of the key.
 *
 *	@return On success, the key at specified position. If position is invalid, then NULL.
 *
 *	@relates as_batch
 *	@ingroup batch_object
 */
static inline as_key * as_batch_keyat(const as_batch * batch, uint32_t i)
{
	return (batch != NULL && batch->keys.entries != NULL && batch->keys.size > i) ? &batch->keys.entries[i] : NULL;
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_integer.h>
#include <aerospike/as_string.h>
#include <aerospike/as_geojson.h>
#include <aerospike/as_bytes.h>
#include <aerospike/as_list.h>
#include <aerospike/as_map.h>
#include <aerospike/as_val.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Maximum bin name size
 */
#define AS_BIN_NAME_MAX_SIZE 15

/**
 *	Maximum bin name length
 */
#define AS_BIN_NAME_MAX_LEN (AS_BIN_NAME_MAX_SIZE - 1)

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Bin Name
 */
typedef char as_bin_name[AS_BIN_NAME_MAX_SIZE];

/**
 *	Bin Value
 */
typedef union as_bin_value_s {
	as_val nil;
	as_integer integer;
	as_double dbl;
	as_string string;
	as_bytes bytes;
	as_list list;
	as_map map;
} as_bin_value;

/**
 *	Represents a bin of a record. Each bin is a (name,value) pair. 
 *
 *	Bins of a record should never be directly accessed. The bins should only
 *	be modified via as_record functions. The only time an as_bin is directly 
 *	accessible is during iteration via as_record_iterator, but the 
 *	as_bin functions should be used to read the values.
 *
 *	@ingroup client_objects
 */
typedef struct as_bin_s {

	/**
	 *	Bin name.
	 */
	as_bin_name name;

	/**
	 *	Bin value.
	 */
	as_bin_value value;

	/**
	 *	Bin value pointer.
	 *	If NULL, then there is no value.
	 *	It can point to as_bin.value or a different value.
	 */
	as_bin_value * valuep;
	
} as_bin;

/**
 * Sequence of bins.
 */
typedef struct as_bins_s {

	/**
	 *	@private
	 *	If true, then as_record_destroy() will free data
	 */
	bool _free;

	/**
	 *	Number of entries allocated to data.
	 */
	uint16_t capacity;

	/**
	 *	Number of entries currently holding data.
	 */
	uint16_t size;

	/**
	 *	Storage for bins
	 */
	as_bin * entries;

} as_bins;

/******************************************************************************
 *	INLINE FUNCTIONS
 *****************************************************************************/

/**
 *	Get the name of the bin.
 *
 *	~~~~~~~~~~{.c}
 *	char * name = as_bin_get_name(bin);
 *	~~~~~~~~~~
 *
 *	@param bin 	The bin to get the name of.
 *
 *	@return The name of the bin.
 *
 *	@relates as_bin
 */
static inline char * as_bin_get_name(const as_bin * bin) {
	return (char *) bin->name;
}


/**
 *	Get the value of the bin.
 *
 *	~~~~~~~~~~{.c}
 *	as_bin_value val = as_bin_get_value(bin);
 *	~~~~~~~~~~
 *
 *	@param bin 	The bin to get the value of.
 *
 *	@return The value of the bin.
 *
 *	@relates as_bin
 */
static inline as_bin_value * as_bin_get_value(const as_bin * bin) {
	return bin->valuep;
}


/**
 *	Get the type for the value of the bin.
 *
 *	~~~~~~~~~~{.c}
 *	as_val_t type = as_bin_get_type(bin);
 *	~~~~~~~~~~
 *
 *	@param bin 	The bin to get value's type.
 *
 *	@return The type of the bin's value
 *
 *	@relates as_bin
 */
static inline as_val_t as_bin_get_type(const as_bin * bin) {
	return as_val_type(bin->valuep);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_config.h>
#include <aerospike/as_node.h>
#include <aerospike/as_partition.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_thread_pool.h>

#ifdef __cplusplus
extern "C" {
#endif
	
// Concurrency kit needs to be under extern "C" when compiling C++.
#include <aerospike/ck/ck_pr.h>

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 * Seed host.
 */
typedef struct as_seed_s {
	/**
	 * Host name.
	 */
	char* name;
	
	/**
	 * Host port.
	 */
	in_port_t port;
} as_seed;

/**
 *	@private
 *	Reference counted array of seed hosts.
 */
typedef struct as_seeds_s {
	/**
	 *	@private
	 *	Reference count.
	 */
	uint32_t ref_count;

	/*
	 *	@private
	 *	Length of seed array.
	 */
	uint32_t size;

	/**
	 *	@private
	 *	Seed array.
	 */
	as_seed array[];
} as_seeds;

/**
 *	@private
 *  Reference counted array of server node pointers.
 */
typedef struct as_nodes_s {
	/**
	 *	@private
	 *  Reference count of node array.
	 */
	uint32_t ref_count;
	
	/**
	 *	@private
	 *  Length of node array.
	 */
	uint32_t size;
	
	/**
	 *	@private
	 *  Server node array.
	 */
	as_node* array[];
} as_nodes;


/**
 *	@private
 *	Reference counted array of address maps.
 */
typedef struct as_addr_maps_s {
	/**
	 *	@private
	 *	Reference count.
	 */
	uint32_t ref_count;

	/*
	 *	@private
	 *	Length of address map array.
	 */
	uint32_t size;

	/**
	 *	@private
	 *	tAddress map array.
	 */
	as_addr_map array[];
} as_addr_maps;

/**
 *	@private
 *	Reference counted release function definition.
 */
typedef void (*as_release_fn) (void* value);

/**
 *	@private
 *  Reference counted data to be garbage collected.
 */
typedef struct as_gc_item_s {
	/**
	 *	@private
	 *  Reference counted data to be garbage collected.
	 */
	void* data;
	
	/**
	 *	@private
	 *  Release function.
	 */
	as_release_fn release_fn;
} as_gc_item;

/**
 *	Cluster of server nodes.
 */
typedef struct as_cluster_s {
	/**
	 *	@private
	 *	Active nodes in cluster.
	 */
	as_nodes* nodes;
	
	/**
	 *	@private
	 *	Hints for best node for a partition.
	 */
	as_partition_tables* partition_tables;
		
	/**
	 *	@private
	 *	Nodes to be garbage collected.
	 */
	as_vector* /* <as_gc_item> */ gc;
	
	/**
	 *	@private
	 *	Shared memory implementation of cluster.
	 */
	struct as_shm_info_s* shm_info;
	
	/**
	 *	@private
	 *	User name in UTF-8 encoded bytes.
	 */
	char* user;
	
	/**
	 *	@private
	 *	Password in hashed format in bytes.
	 */
	char* password;
	
	/**
	 *	@private
	 *	Initial seed nodes specified by user.
	 */
	as_seeds* seeds;
	
	/**
	 *	@private
	 *	Configuration version.  Incremented, when the configuration is changed.
	 */
	uint32_t version;
	
	/**
	 *	@private
	 *	A IP translation table is used in cases where different clients use different server
	 *	IP addresses.  This may be necessary when using clients from both inside and outside
	 *	a local area network.  Default is no translation.
	 *
	 *	The key is the IP address returned from friend info requests to other servers.  The
	 *	value is the real IP address used to connect to the server.
	 */
	as_addr_maps* ip_map;
	
	/**
	 *	@private
	 *	Pool of threads used to query server nodes in parallel for batch, scan and query.
	 */
	as_thread_pool thread_pool;
		
	/**
	 *	@private
	 *	Cluster tend thread.
	 */
	pthread_t tend_thread;
	
	/**
	 *	@private
	 *	Lock for the tend thread to wait on with the tend interval as timeout.
	 *	Normally locked, resulting in waiting a full interval between
	 *	tend iterations.  Upon cluster shutdown, unlocked by the main
	 *	thread, allowing a fast termination of the tend thread.
	 */
	pthread_mutex_t tend_lock;
	
	/**
	 *	@private
	 *	Tend thread identifier to be used with tend_lock.
	 */
	pthread_cond_t tend_cond;
	
	/**
	 *	@private
	 *	Milliseconds between cluster tends.
	 */
	uint32_t tend_interval;

	/**
	 *	@private
	 *	Size of node's synchronous connection pool.
	 */
	uint32_t conn_queue_size;
	
	/**
	 *	@private
	 *	Maximum number of asynchronous (non-pipeline) connections allowed for each node.
	 *	Async transactions will be rejected if the maximum async node connections would be exceeded.
	 *	This variable is ignored if asynchronous event loops are not created.
	 */
	uint32_t async_max_conns_per_node;
	
	/**
	 *	@private
	 *	Maximum number of pipeline connections allowed for each node.
	 *	Pipeline transactions will be rejected if the maximum pipeline node connections would be exceeded.
	 *	This variable is ignored if asynchronous event loops are not created.
	 */
	uint32_t pipe_max_conns_per_node;
	
	/**
	 *	@private
	 *	Number of pending async commands (i.e., commands with an outstanding reply).
	 */
	uint32_t async_pending;

	/**
	 *	@private
	 *	Number of active async pipeline and non-pipeline connections combined.
	 */
	uint32_t async_conn_count;

	/**
	 *	@private
	 *	Number of async connections in the pools.
	 */
	uint32_t async_conn_pool;

	/**
	 *	@private
	 *	Initial connection timeout in milliseconds.
	 */
	uint32_t conn_timeout_ms;
	
	/**
	 *	@private
	 *	Maximum socket idle in seconds.
	 */
	uint32_t max_socket_idle;
	
	/**
	 *	@private
	 *	Random node index.
	 */
	uint32_t node_index;
	
	/**
	 *	@private
	 *	Total number of data partitions used by cluster.
	 */
	uint16_t n_partitions;
	
	/**
	 *	@private
	 *	If "services-alternate" should be used instead of "services"
	 */
	bool use_services_alternate;
	
	/**
	 *	@private
	 *	Should continue to tend cluster.
	 */
	volatile bool valid;
} as_cluster;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	Create and initialize cluster.
 */
as_status
as_cluster_create(as_config* config, as_error* err, as_cluster** cluster);

/**
 *	Close all connections and release memory associated with cluster.
 */
void
as_cluster_destroy(as_cluster* cluster);

/**
 *	Is cluster connected to any server nodes.
 */
bool
as_cluster_is_connected(as_cluster* cluster);

/**
 *	Get all node names in cluster.
 */
void
as_cluster_get_node_names(as_cluster* cluster, int* n_nodes, char** node_names);

/**
 *	Reserve reference counted access to cluster nodes.
 */
static inline as_nodes*
as_nodes_reserve(as_cluster* cluster)
{
	as_nodes* nodes = (as_nodes *)ck_pr_load_ptr(&cluster->nodes);
	//ck_pr_fence_acquire();
	ck_pr_inc_32(&nodes->ref_count);
	return nodes;
}

/**
 *	Release reference counted access to cluster nodes.
 */
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

/**
 *	Reserve reference counted access to seeds.
 */
static inline as_seeds*
as_seeds_reserve(as_cluster* cluster)
{
	as_seeds* seeds = (as_seeds *)ck_pr_load_ptr(&cluster->seeds);
	ck_pr_inc_32(&seeds->ref_count);
	return seeds;
}

/**
 *	Release reference counted access to seeds.
 */
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

/**
 *	Add seeds to the cluster.
 */
void
as_seeds_add(as_cluster* cluster, as_seed* seed_list, uint32_t size);

/**
 *	Replace the seeds of the cluster.
 */
void
as_seeds_update(as_cluster* cluster, as_seed* seed_list, uint32_t size);

/**
 *	Reserve reference counted access to IP map.
 */
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

/**
 *	Release reference counted access to IP map.
 */
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

/**
 *	Replace the IP address map of the cluster.
 */
void
as_ip_map_update(as_cluster* cluster, as_addr_map* ip_map_list, uint32_t size);

/**
 * 	Change maximum async connections per node.
 */
void
as_cluster_set_async_max_conns_per_node(as_cluster* cluster, uint32_t async_size, uint32_t pipe_size);

/**
 *	@private
 *	Change user and password that is used to authenticate with cluster servers.
 */
void
as_cluster_change_password(as_cluster* cluster, const char* user, const char* password);

/**
 *	@private
 *	Get random node in the cluster.
 *	as_nodes_release() must be called when done with node.
 */
as_node*
as_node_get_random(as_cluster* cluster);

/**
 *	@private
 *	Get node given node name.
 *	as_nodes_release() must be called when done with node.
 */
as_node*
as_node_get_by_name(as_cluster* cluster, const char* name);

/**
 *	@private
 *	Reserve reference counted access to partition tables.
 *	as_partition_tables_release() must be called when done with tables.
 */
static inline as_partition_tables*
as_partition_tables_reserve(as_cluster* cluster)
{
	as_partition_tables* tables = (as_partition_tables *)ck_pr_load_ptr(&cluster->partition_tables);
	ck_pr_inc_32(&tables->ref_count);
	return tables;
}

/**
 *	@private
 *	Release reference counted access to partition tables.
 */
static inline void
as_partition_tables_release(as_partition_tables* tables)
{
	bool destroy;
	ck_pr_dec_32_zero(&tables->ref_count, &destroy);
	
	if (destroy) {
		cf_free(tables);
	}
}

/**
 *	@private
 *	Get partition table given namespace.
 */
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

/**
 *	@private
 *	Get mapped node given digest key and partition table.  If there is no mapped node, a random
 *	node is used instead.
 *	as_nodes_release() must be called when done with node.
 */
as_node*
as_partition_table_get_node(as_cluster* cluster, as_partition_table* table, const uint8_t* digest, bool write, as_policy_replica replica);

/**
 *	@private
 *	Get shared memory mapped node given digest key.  If there is no mapped node, a random node is used instead.
 *	as_nodes_release() must be called when done with node.
 */
as_node*
as_shm_node_get(as_cluster* cluster, const char* ns, const uint8_t* digest, bool write, as_policy_replica replica);

/**
 *	@private
 *	Get mapped node given digest key.  If there is no mapped node, a random node is used instead.
 *	as_nodes_release() must be called when done with node.
 */
static inline as_node*
as_node_get(as_cluster* cluster, const char* ns, const uint8_t* digest, bool write, as_policy_replica replica)
{
#ifdef AS_TEST_PROXY
	return as_node_get_random(cluster);
#else
	if (cluster->shm_info) {
		return as_shm_node_get(cluster, ns, digest, write, replica);
	}
	else {
		as_partition_table* table = as_cluster_get_partition_table(cluster, ns);
		return as_partition_table_get_node(cluster, table, digest, write, replica);
	}
#endif
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_bin.h>
#include <aerospike/as_buffer.h>
#include <aerospike/as_cluster.h>
#include <aerospike/as_key.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_proto.h>
#include <aerospike/as_record.h>
#include <citrusleaf/cf_byte_order.h>
#include <citrusleaf/cf_digest.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

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

/**
 *	@private
 *	Macros use these stand-ins for cf_malloc() / cf_free(), so that
 *	instrumentation properly substitutes them.
 */

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

/**
 *	@private
 *	Allocate command buffer on stack or heap depending on given size.
 */
#define as_command_init(_sz) (_sz > AS_STACK_BUF_SIZE) ? (uint8_t*)local_malloc(_sz) : (uint8_t*)alloca(_sz)

/**
 *	@private
 *	Free command buffer.
 */
#define as_command_free(_buf, _sz) if (_sz > AS_STACK_BUF_SIZE) {local_free(_buf);}

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	@private
 *	Node map data used in as_command_execute().
 */
typedef struct as_command_node_s {
	as_node* node;
	const char* ns;
	const uint8_t* digest;
	as_policy_replica replica;
	bool write;
} as_command_node;

/**
 *	@private
 *	Data used in as_command_parse_result().
 */
typedef struct as_command_parse_result_data_s {
	as_record** record;
	bool deserialize;
} as_command_parse_result_data;

/**
 *	@private
 *	Parse results callback used in as_command_execute().
 */
typedef as_status (*as_parse_results_fn) (as_error* err, int fd, uint64_t deadline_ms, void* user_data);

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Calculate size of command header plus key fields.
 */
size_t
as_command_key_size(as_policy_key policy, const as_key* key, uint16_t* n_fields);

/**
 *	@private
 *	Calculate size of string field.
 */
static inline size_t
as_command_string_field_size(const char* value)
{
	return strlen(value) + AS_FIELD_HEADER_SIZE;
}

/**
 *	@private
 *	Calculate size of field structure given field value size.
 */
static inline size_t
as_command_field_size(size_t size)
{
	return size + AS_FIELD_HEADER_SIZE;
}

/**
 *	@private
 *	Calculate size of as_val field.
 */
size_t
as_command_value_size(as_val* val, as_buffer* buffer);

/**
 *	@private
 *	Calculate size of bin name and value combined.
 */
static inline size_t
as_command_bin_size(const as_bin* bin, as_buffer* buffer)
{
	return strlen(bin->name) + as_command_value_size((as_val*)bin->valuep, buffer) + 8;
}

/**
 *	@private
 *	Calculate size of bin name.  Return error is bin name greater than 14 characters.
 */
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

/**
 *	@private
 *	Calculate size of string operation.
 */
static inline size_t
as_command_string_operation_size(const char* value)
{
	return strlen(value) + AS_OPERATION_HEADER_SIZE;
}

/**
 *	@private
 *	Write command header for all commands.
 */
uint8_t*
as_command_write_header(uint8_t* cmd, uint8_t read_attr, uint8_t write_attr,
	as_policy_commit_level commit_level, as_policy_consistency_level consistency,
	as_policy_exists exists, as_policy_gen gen_policy, uint32_t gen, uint32_t ttl,
	uint32_t timeout_ms, uint16_t n_fields, uint16_t n_bins);

/**
 *	@private
 *	Write command header for read commands only.
 */
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

/**
 *	@private
 *	Write field header.
 */
static inline uint8_t*
as_command_write_field_header(uint8_t* p, uint8_t id, uint32_t size)
{
	*(uint32_t*)p = cf_swap_to_be32(size+1);
	p += 4;
	*p++ = id;
	return p;
}

/**
 *	@private
 *	Write string field.
 */
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

/**
 *	@private
 *	Write uint64_t field.
 */
static inline uint8_t*
as_command_write_field_uint64(uint8_t* p, uint8_t id, uint64_t val)
{
	p = as_command_write_field_header(p, id, sizeof(uint64_t));
	*(uint64_t*)p = cf_swap_to_be64(val);
	return p + sizeof(uint64_t);
}

/**
 *	@private
 *	Write as_buffer field.
 */
static inline uint8_t*
as_command_write_field_buffer(uint8_t* p, uint8_t id, as_buffer* buffer)
{
	p = as_command_write_field_header(p, id, buffer->size);
	memcpy(p, buffer->data, buffer->size);
	return p + buffer->size;
}

/**
 *	@private
 *	Write digest field.
 */
static inline uint8_t*
as_command_write_field_digest(uint8_t* p, const as_digest* val)
{
	p = as_command_write_field_header(p, AS_FIELD_DIGEST, AS_DIGEST_VALUE_SIZE);
	memcpy(p, val->value, AS_DIGEST_VALUE_SIZE);
	return p + AS_DIGEST_VALUE_SIZE;
}

/**
 *	@private
 *	Write key structure.
 */
uint8_t*
as_command_write_key(uint8_t* p, as_policy_key policy, const as_key* key);

/**
 *	@private
 *	Write bin header and bin name.
 */
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

/**
 *	@private
 *	Write bin.
 */
uint8_t*
as_command_write_bin(uint8_t* begin, uint8_t operation_type, const as_bin* bin, as_buffer* buffer);

/**
 *	@private
 *	Finish writing command.
 */
static inline size_t
as_command_write_end(uint8_t* begin, uint8_t* end)
{
	uint64_t len = end - begin;
	uint64_t proto = (len - 8) | (AS_MESSAGE_VERSION << 56) | (AS_MESSAGE_TYPE << 48);
	*(uint64_t*)begin = cf_swap_to_be64(proto);
	return len;
}

/**
 *	@private
 *	Finish writing compressed command.
 */
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

/**
 *	@private
 *	Calculate max size the compressed command buffer.
 */
size_t
as_command_compress_max_size(size_t cmd_sz);

/**
 *	@private
 *	Compress command buffer.
 */
as_status
as_command_compress(as_error* err, uint8_t* cmd, size_t cmd_sz, uint8_t* compressed_cmd, size_t* compressed_size);

/**
 *	@private
 *	Send command to the server.
 */
as_status
as_command_execute(as_cluster* cluster, as_error* err, as_command_node* cn, uint8_t* command, size_t command_len,
   uint32_t timeout_ms, uint32_t retry,
   as_parse_results_fn parse_results_fn, void* parse_results_data);

/**
 *	@private
 *	Parse header of server response.
 */
as_status
as_command_parse_header(as_error* err, int fd, uint64_t deadline_ms, void* user_data);

/**
 *	@private
 *	Parse server record.  Used for reads.
 */
as_status
as_command_parse_result(as_error* err, int fd, uint64_t deadline_ms, void* user_data);

/**
 *	@private
 *	Parse server success or failure result.
 */
as_status
as_command_parse_success_failure(as_error* err, int fd, uint64_t deadline_ms, void* user_data);

/**
 *	@private
 *	Parse server success or failure bins.
 */
as_status
as_command_parse_success_failure_bins(uint8_t** pp, as_error* err, as_msg* msg, as_val** value);

/**
 *	@private
 *	Parse bins received from the server.
 */
uint8_t*
as_command_parse_bins(as_record* rec, uint8_t* buf, uint32_t n_bins, bool deserialize);

/**
 *	@private
 *	Parse user defined function error.
 */
as_status
as_command_parse_udf_failure(uint8_t* p, as_error* err, as_msg* msg, as_status status);

/**
 *	@private
 *	Skip over fields section in returned data.
 */
uint8_t*
as_command_ignore_fields(uint8_t* p, uint32_t n_fields);

/**
 *	@private
 *	Skip over bins in returned data.
 */
uint8_t*
as_command_ignore_bins(uint8_t* p, uint32_t n_bins);

/**
 *	@private
 *	Parse key fields received from server.  Used for reads.
 */
uint8_t*
as_command_parse_key(uint8_t* p, uint32_t n_fields, as_key* key);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_error.h>
#include <aerospike/as_policy.h>
#include <aerospike/as_password.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

#ifdef __linux__
/**
 * Default path to the system UDF files.
 */
#define AS_CONFIG_LUA_SYSTEM_PATH "/opt/aerospike/client/sys/udf/lua"

/**
 * Default path to the user UDF files.
 */
#define AS_CONFIG_LUA_USER_PATH "/opt/aerospike/client/usr/udf/lua"
#endif

#ifdef __APPLE__
/**
 * Default path to the system UDF files.
 */
#define AS_CONFIG_LUA_SYSTEM_PATH "/usr/local/aerospike/client/sys/udf/lua"

/**
 * Default path to the user UDF files.
 */
#define AS_CONFIG_LUA_USER_PATH "/usr/local/aerospike/client/usr/udf/lua"
#endif

/**
 * The size of path strings
 */
#define AS_CONFIG_PATH_MAX_SIZE 256

/**
 * The maximum string length of path strings
 */
#define AS_CONFIG_PATH_MAX_LEN 	(AS_CONFIG_PATH_MAX_SIZE - 1)

/**
 * The size of as_config.hosts
 */
#define AS_CONFIG_HOSTS_SIZE 256

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Host Information
 *
 *	@ingroup as_config_object
 */
typedef struct as_config_host_s {
	
	/**
	 *	Host address
	 */
	const char * addr;
	
	/**
	 *	Host port
	 */
	uint16_t port;

} as_config_host;

/**
 *	IP translation table.
 *
 *	@ingroup as_config_object
 */
typedef struct as_addr_map_s {
	
	/**
	 *	Original hostname or IP address in string format.
	 */
    char * orig;
	
	/**
	 *	Use this IP address instead.
	 */
    char * alt;
	
} as_addr_map;

/**
 *	lua module config
 *
 *	@ingroup as_config_object
 */
typedef struct as_config_lua_s {

	/**
	 *	Enable caching of UDF files in the client
	 *	application.
	 */
	bool cache_enabled;

	/**
	 *	The path to the system UDF files. These UDF files 
	 *	are installed with the aerospike client library.
	 *	Default location defined in: AS_CONFIG_LUA_SYSTEM_PATH
	 */
	char system_path[AS_CONFIG_PATH_MAX_SIZE];

	/**
	 *	The path to user's UDF files.
	 *	Default location defined in: AS_CONFIG_LUA_USER_PATH
	 */
	char user_path[AS_CONFIG_PATH_MAX_SIZE];

} as_config_lua;

/**
 *	The `as_config` contains the settings for the `aerospike` client. Including
 *	default policies, seed hosts in the cluster and other settings.
 *
 *	## Initialization
 *
 *	Before using as_config, you must first initialize it. This will setup the 
 *	default values.
 *
 *	~~~~~~~~~~{.c}
 *	as_config config;
 *	as_config_init(&config);
 *	~~~~~~~~~~
 *
 *	Once initialized, you can populate the values.
 *
 *	## Seed Hosts
 *	
 *	The client will require at least one seed host defined in the 
 *	configuration. The seed host is defined in `as_config.hosts`. 
 *
 *	~~~~~~~~~~{.c}
 *	as_config_add_host(&config, "127.0.0.1", 3000);
 *	~~~~~~~~~~
 *
 *	You can define up to 256 hosts for the seed. The client will iterate over
 *	the list until it connects with one of the hosts. 
 *
 *	## Policies
 *
 *	The configuration also defines default policies for the application. The 
 *	`as_config_init()` function already presets default values for the policies.
 *	
 *	Policies define the behavior of the client, which can be global across
 *	operations, global to a single operation, or local to a single use of an
 *	operation.
 *	
 *	Each database operation accepts a policy for that operation as an a argument.
 *	This is considered a local policy, and is a single use policy. This policy
 *	supersedes any global policy defined.
 *	
 *	If a value of the policy is not defined, then the rule is to fallback to the
 *	global policy for that operation. If the global policy for that operation is
 *	undefined, then the global default value will be used.
 *
 *	If you find that you have behavior that you want every use of an operation
 *	to utilize, then you can specify the default policy in as_config.policies.
 *
 *	For example, the `aerospike_key_put()` operation takes an `as_policy_write`
 *	policy. If you find yourself setting the `key` policy value for every call 
 *	to `aerospike_key_put()`, then you may find it beneficial to set the global
 *	`as_policy_write` in `as_policies.write`, which all write operations will use.
 *
 *	~~~~~~~~~~{.c}
 *	config.policies.write.key = AS_POLICY_KEY_SEND;
 *	~~~~~~~~~~
 *
 *	If you find that you want to use a policy value across all operations, then 
 *	you may find it beneficial to set the default policy value for that policy 
 *	value.
 *
 *	For example, if you keep setting the key policy value to 
 *	`AS_POLICY_KEY_SEND`, then you may want to just set `as_policies.key`. This
 *	will set the global default value for the policy value. So, if an global
 *  operation policy or a local operation policy does not define a value, then
 *	this value will be used.
 *
 *	~~~~~~~~~~{.c}
 *	config.policies.key = AS_POLICY_KEY_SEND;
 *	~~~~~~~~~~
 *
 *	Global default policy values:
 *	-	as_policies.timeout
 *	-	as_policies.retry
 *	-	as_policies.key
 *	-	as_policies.gen
 *	-	as_policies.exists
 *
 *	Global operation policies:
 *	-	as_policies.read
 *	-	as_policies.write
 *	-	as_policies.operate
 *	-	as_policies.remove
 *	-	as_policies.query
 *	-	as_policies.scan
 *	-	as_policies.info
 *
 *
 *	## User-Defined Function Settings
 *	
 *	If you are using using user-defined functions (UDF) for processing query 
 *	results (i.e aggregations), then you will find it useful to set the 
 *	`mod_lua` settings. Of particular importance is the `mod_lua.user_path`, 
 *	which allows you to define a path to where the client library will look for
 *	Lua files for processing.
 *	
 *	~~~~~~~~~~{.c}
 *	strcpy(config.mod_lua.user_path, "/home/me/lua");
 *	~~~~~~~~~~
 *
 *	@ingroup client_objects
 */
typedef struct as_config_s {

	/**
	 *	User authentication to cluster.  Leave empty for clusters running without restricted access.
	 */
	char user[AS_USER_SIZE];
	
	/**
	 *	Password authentication to cluster.  The hashed value of password will be stored by the client 
	 *	and sent to server in same format.  Leave empty for clusters running without restricted access.
	 */
	char password[AS_PASSWORD_HASH_SIZE];
	
	/**
	 *	A IP translation table is used in cases where different clients use different server
	 *	IP addresses.  This may be necessary when using clients from both inside and outside
	 *	a local area network.  Default is no translation.
	 *
	 *	The key is the IP address returned from friend info requests to other servers.  The
	 *	value is the real IP address used to connect to the server.
	 *
	 *	A deep copy of ip_map is performed in aerospike_connect().  The caller is
	 *  responsible for memory deallocation of the original data structure.
	 */
	as_addr_map * ip_map;
	
	/**
	 *	Length of ip_map array.
	 *  Default: 0
	 */
	uint32_t ip_map_size;
	
	/**
	 * Maximum number of synchronous connections allowed per server node.  Synchronous transactions
	 * will go through retry logic and potentially fail with error code "AEROSPIKE_ERR_NO_MORE_CONNECTIONS"
	 * if the maximum number of connections would be exceeded.
	 * 
	 * The number of connections used per node depends on how many concurrent threads issue
	 * database commands plus sub-threads used for parallel multi-node commands (batch, scan,
	 * and query). One connection will be used for each thread.
	 *
	 * Default: 300
	 */
	uint32_t max_conns_per_node;
	
	/**
	 *	Maximum number of asynchronous (non-pipeline) connections allowed for each node.
	 *	This limit will be enforced at the node/event loop level.  If the value is 100 and 2 event
	 *	loops are created, then each node/event loop asynchronous (non-pipeline) connection pool 
	 *	will have a limit of 50. Async transactions will be rejected if the limit would be exceeded.
	 *	This variable is ignored if asynchronous event loops are not created.
	 *	Default: 300
	 */
	uint32_t async_max_conns_per_node;

	/**
	 *	Maximum number of pipeline connections allowed for each node.
	 *	This limit will be enforced at the node/event loop level.  If the value is 100 and 2 event
	 *	loops are created, then each node/event loop pipeline connection pool will have a limit of 50. 
	 *	Async transactions will be rejected if the limit would be exceeded.
	 *	This variable is ignored if asynchronous event loops are not created.
	 *	Default: 64
	 */
	uint32_t pipe_max_conns_per_node;
	
	/**
	 *	Initial host connection timeout in milliseconds.  The timeout when opening a connection
	 *	to the server host for the first time.
	 *	Default: 1000
	 */
	uint32_t conn_timeout_ms;

	/**
	 *	Polling interval in milliseconds for cluster tender
	 *	Default: 1000
	 */
	uint32_t tender_interval;

	/**
	 *	Number of threads stored in underlying thread pool that is used in batch/scan/query commands.
	 *	These commands are often sent to multiple server nodes in parallel threads.  A thread pool 
	 *	improves performance because threads do not have to be created/destroyed for each command.
	 *	Calculate your value using the following formula:
	 *
	 *	thread_pool_size = (concurrent batch/scan/query commands) * (server nodes)
	 *
	 *	Default: 16
	 */
	uint32_t thread_pool_size;

	/**
	 *	Count of entries in hosts array.
	 */
	uint32_t hosts_size;
	
	/**
	 *	(seed) hosts
	 *	Populate with one or more hosts in the cluster
	 *	that you intend to connect with.
	 */
	as_config_host hosts[AS_CONFIG_HOSTS_SIZE];

	/**
	 *	Client policies
	 */
	as_policies policies;

	/**
	 *	lua config.  This is a global config even though it's located here in cluster config.
	 *	This config has been left here to avoid breaking the API.
	 *
	 *	The global lua config will only be changed once on first cluster initialization.
	 *	A better method for initializing lua configuration is to leave this field alone and
	 *	instead call aerospike_init_lua():
	 *
	 *	~~~~~~~~~~{.c}
	 *	// Get default global lua configuration.
	 *	as_config_lua lua;
	 *	as_config_lua_init(&lua);
	 *
	 *	// Optionally modify lua defaults.
	 *	lua.cache_enabled = <enable lua cache>;
	 *	strcpy(lua.system_path, <lua system directory>);
	 *	strcpy(lua.user_path, <lua user directory>);
	 *
	 *	// Initialize global lua configuration.
	 *	aerospike_init_lua(&lua);
	 *	~~~~~~~~~~
	 */
	as_config_lua lua;

	/**
	 *	Action to perform if client fails to connect to seed hosts.
	 *
	 *	If fail_if_not_connected is true (default), the cluster creation will fail
	 *	when all seed hosts are not reachable.
	 *
	 *	If fail_if_not_connected is false, an empty cluster will be created and the 
	 *	client will automatically connect when Aerospike server becomes available.
	 */
	bool fail_if_not_connected;
	
	/**
	 *	Flag to signify if "services-alternate" should be used instead of "services"
	 *	Default : false
	 */
	bool use_services_alternate;

	/**
	 *	Indicates if shared memory should be used for cluster tending.  Shared memory
	 *	is useful when operating in single threaded mode with multiple client processes.
	 *	This model is used by wrapper languages such as PHP and Python.  When enabled, 
	 *	the data partition maps are maintained by only one process and all other processes 
	 *	use these shared memory maps.
	 *
	 *	Shared memory should not be enabled for multi-threaded programs.
	 *	Default: false
	 */
	bool use_shm;

	/**
	 *	Shared memory identifier.  This identifier should be the same for all applications
	 *	that use the Aerospike C client. 
	 *	Default: 0xA5000000
	 */
	int shm_key;
	
	/**
	 *	Shared memory maximum number of server nodes allowed.  This value is used to size
	 *	the fixed shared memory segment.  Leave a cushion between actual server node
	 *	count and shm_max_nodes so new nodes can be added without having to reboot the client.
	 *	Default: 16
	 */
	uint32_t shm_max_nodes;
	
	/**
	 *	Shared memory maximum number of namespaces allowed.  This value is used to size
	 *	the fixed shared memory segment.  Leave a cushion between actual namespaces
	 *	and shm_max_namespaces so new namespaces can be added without having to reboot the client.
	 *	Default: 8
	 */
	uint32_t shm_max_namespaces;
	
	/**
	 *	Take over shared memory cluster tending if the cluster hasn't been tended by this
	 *	threshold in seconds.
	 *	Default: 30
	 */
	uint32_t shm_takeover_threshold_sec;
} as_config;

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Initialize the configuration to default values.
 *
 *	You should do this to ensure the configuration has valid values, before 
 *	populating it with custom options.
 *
 *	~~~~~~~~~~{.c}
 *		as_config config;
 *		as_config_init(&config);
 *		as_config_add_host(&config, "127.0.0.1", 3000);
 *	~~~~~~~~~~
 *	
 *	@param c The configuration to initialize.
 *	
 *	@return The initialized configuration on success. Otherwise NULL.
 *
 *	@relates as_config
 */
as_config * as_config_init(as_config * c);

/**
 *	Add host to seed the cluster.
 *
 *	~~~~~~~~~~{.c}
 *		as_config config;
 *		as_config_init(&config);
 *		as_config_add_host(&config, "127.0.0.1", 3000);
 *	~~~~~~~~~~
 *
 *	@relates as_config
 */
static inline void
as_config_add_host(as_config* config, const char* addr, uint16_t port)
{
	as_config_host* host = &config->hosts[config->hosts_size++];
	host->addr = addr;
	host->port = port;
}

/**
 *	User authentication for servers with restricted access.  The password will be stored by the
 *	client and sent to server in hashed format.
 *
 *	~~~~~~~~~~{.c}
 *		as_config config;
 *		as_config_init(&config);
 *		as_config_set_user(&config, "charlie", "mypassword");
 *	~~~~~~~~~~
 *
 *	@relates as_config
 */
bool
as_config_set_user(as_config* config, const char* user, const char* password);
	
/**
 *	Initialize global lua configuration to defaults.
 */
static inline void
as_config_lua_init(as_config_lua* lua)
{
	lua->cache_enabled = false;
	strcpy(lua->system_path, AS_CONFIG_LUA_SYSTEM_PATH);
	strcpy(lua->user_path, AS_CONFIG_LUA_USER_PATH);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_status.h>
#include <aerospike/as_string.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 * The size of as_error.message
 *
 *	@ingroup as_error_object
 */
#define AS_ERROR_MESSAGE_MAX_SIZE 	1024

/**
 * The maximum string length of as_error.message
 *
 *	@ingroup as_error_object
 */
#define AS_ERROR_MESSAGE_MAX_LEN 	(AS_ERROR_MESSAGE_MAX_SIZE - 1)

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	All operations that interact with the Aerospike cluster accept an as_error
 *	argument and return an as_status value. The as_error argument is populated
 *	with information about the error that occurred. The as_status return value
 *	is the as_error.code value.
 *
 *	When an operation succeeds, the as_error.code value is usually set to 
 *	`AEROSPIKE_OK`. There are some operations which may have other success 
 *	status codes, so please review each operation for information on status 
 *	codes.
 *
 *	When as_error.code is not a success value (`AEROSPIKE_OK`), then you can 
 *	expect the other fields of as_error.code to be populated.
 *
 *	Example usage:
 *	~~~~~~~~~~{.c}
 *	as_error err;
 *
 *	if ( aerospike_key_get(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "(%d) %s at %s[%s:%d]\n", error.code, err.message, err.func, err.file. err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	You can reuse an as_error with multiple operations. Each operation 
 *	internally resets the error. So, if an error occurred in one operation,
 *	and you did not check it, then the error will be lost with subsequent 
 *	operations.
 *
 *	Example usage:
 *
 *	~~~~~~~~~~{.c}
 *	as_error err;
 *
 *	if ( aerospike_key_put(&as, &err, NULL, &key, rec) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "(%d) %s at %s[%s:%d]\n", error.code, err.message, err.func, err.file. err.line);
 *	}
 *
 *	if ( aerospike_key_get(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK ) {
 *		fprintf(stderr, "(%d) %s at %s[%s:%d]\n", error.code, err.message, err.func, err.file. err.line);
 *	}
 *	~~~~~~~~~~
 *
 *	@ingroup client_objects
 */
typedef struct as_error_s {

	/**
	 *	Numeric error code
	 */
	as_status code;

	/**
	 *	NULL-terminated error message
	 */
	char message[AS_ERROR_MESSAGE_MAX_SIZE];

	/**
	 *	Name of the function where the error occurred.
	 */
	const char * func;

	/**
	 *	Name of the file where the error occurred.
	 */
	const char * file;

	/**
	 *	Line in the file where the error occurred.
	 */
	uint32_t line;

} as_error;

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	as_error_update(&as->error, AEROSPIKE_OK, "%s %d", "a", 1);
 *
 *	@ingroup as_error_object
 */
#define as_error_update(__err, __code, __fmt, ...) \
	as_error_setallv( __err, __code, __func__, __FILE__, __LINE__, __fmt, ##__VA_ARGS__ );

/**
 *	as_error_set_message(&as->error, AEROSPIKE_ERR, "error message");
 *
 *	@ingroup as_error_object
 */
#define as_error_set_message(__err, __code, __msg) \
	as_error_setall( __err, __code, __msg, __func__, __FILE__, __LINE__ );

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Initialize the error to default (empty) values, returning the error.
 *
 *	@param err The error to initialize.
 *
 *	@returns The initialized err.
 *
 *	@relates as_error
 *	@ingroup as_error_object
 */
static inline as_error * as_error_init(as_error * err) {
	err->code = AEROSPIKE_OK;
	err->message[0] = '\0';
	err->func = NULL;
	err->file = NULL;
	err->line = 0;
	return err;
}

/**
 *	Resets the error to default (empty) values, returning the status code.
 *
 *	@param err The error to reset.
 *
 *	@returns AEROSPIKE_OK.
 *
 *	@relates as_error
 *	@ingroup as_error_object
 */
static inline as_status as_error_reset(as_error * err) {
	err->code = AEROSPIKE_OK;
	err->message[0] = '\0';
	err->func = NULL;
	err->file = NULL;
	err->line = 0;
	return err->code;
}

/**
 *	Sets the error.
 *
 *	@return The status code set for the error.
 *
 *	@relates as_error
 */
static inline as_status as_error_setall(as_error * err, as_status code, const char * message, const char * func, const char * file, uint32_t line) {
	err->code = code;
	as_strncpy(err->message, message, AS_ERROR_MESSAGE_MAX_SIZE);
	err->func = func;
	err->file = file;
	err->line = line;
	return err->code;
}

/**
 *	Sets the error.
 *
 *	@return The status code set for the error.
 *
 *	@relates as_error
 */
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

/**
 *	Sets the error message
 *
 *	@relates as_error
 */
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

/**
 *	Copy error from source to target.
 *
 *	@relates as_error
 */
static inline void as_error_copy(as_error * trg, const as_error * src) {
	trg->code = src->code;
	strcpy(trg->message, src->message);
	trg->func = src->func;
	trg->file = src->file;
	trg->line = src->line;
}

/**
 *	Return string representation of error code.  Result should not be freed.
 *
 *	@relates as_error
 */
char*
as_error_string(as_status status);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_queue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/**
 *	@defgroup async_events Asynchronous Event Abstraction
 *
 *  Generic asynchronous events abstraction.  Designed to support multiple event libraries
 *	such as libev and libuv.  Only one library can be supported per build.
 */
#if defined(AS_USE_LIBEV)
#include <ev.h>
#elif defined(AS_USE_LIBUV)
#include <uv.h>
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * TYPES
 *****************************************************************************/
	
/**
 *	Generic asynchronous event loop abstraction.  There is one event loop per thread.
 *	Event loops can be created by the client, or be referenced to externally created event loops.
 *
 *	@ingroup async_events
 */
typedef struct {
#if defined(AS_USE_LIBEV)
	struct ev_loop* loop;
	struct ev_async wakeup;
#elif defined(AS_USE_LIBUV)
	uv_loop_t* loop;
	uv_async_t* wakeup;
#else
	void* loop;
#endif
		
	pthread_mutex_t lock;
	as_queue queue;
	as_queue pipe_cb_queue;
	pthread_t thread;
	uint32_t index;
	bool pipe_cb_calling;
} as_event_loop;

/******************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

extern as_event_loop* as_event_loops;
extern uint32_t as_event_loop_size;
extern uint32_t as_event_loop_current;

/******************************************************************************
 * PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 *	Create new event loops. This method should only be called when asynchronous client commands 
 *	will be used and the calling program itself is not asynchronous.  If this method is used,
 *	it must be called before aerospike_connect().
 *
 *	@param capacity	Number of event loops to create.
 *
 *	@ingroup async_events
 */
as_event_loop*
as_event_create_loops(uint32_t capacity);

/**
 *	Set the number of externally created event loops.  This method should be called when the 
 *	calling program wants to share event loops with the client.  This reduces resource usage and
 *	can increase performance.
 *
 *	This method is used in conjunction with as_event_set_external_loop() to fully define the
 *	the external loop to the client and obtain a reference the client's event loop abstraction.
 *
 *	~~~~~~~~~~{.c}
 *	struct {
 *		pthread_t thread;
 *		struct ev_loop* loop;
 *		as_event_loop* as_loop;
 *	} my_loop;
 *
 *  static void* my_loop_worker_thread(void* udata) {
 *		struct my_loop* myloop = udata;
 *		myloop->loop = ev_loop_new(EVFLAG_AUTO);
 *		myloop->as_loop = as_event_set_external_loop(myloop->loop);
 *		ev_loop(myloop->loop, 0);
 *		ev_loop_destroy(myloop->loop);
 *		return NULL;
 *	}
 *
 *	int capacity = 8;
 *	struct my_loop* loops = malloc(sizeof(struct my_loop) * capacity);
 *	as_event_set_external_loop_capacity(capacity);
 *
 *	for (int i = 0; i < capacity; i++) {
 *		struct my_loop* myloop = &loops[i];
 *		return pthread_create(&myloop->thread, NULL, my_loop_worker_thread, myloop) == 0;
 *	}
 *	~~~~~~~~~~
 *
 *	@param capacity	Number of externally created event loops.
 *
 *	@ingroup async_events
 */
bool
as_event_set_external_loop_capacity(uint32_t capacity);

/**
 *	Register an external event loop with the client. This method should be called when the 
 *	calling program wants to share event loops with the client.  This reduces resource usage and
 *	can increase performance.
 *
 *	This method must be called in the same thread as the event loop that is being registered.
 *
 *	This method is used in conjunction with as_event_set_external_loop_capacity() to fully define
 *	the external loop to the client and obtain a reference the client's event loop abstraction.
 *
 *	~~~~~~~~~~{.c}
 *	struct {
 *		pthread_t thread;
 *		struct ev_loop* loop;
 *		as_event_loop* as_loop;
 *	} my_loop;
 *
 *  static void* my_loop_worker_thread(void* udata) {
 *		struct my_loop* myloop = udata;
 *		myloop->loop = ev_loop_new(EVFLAG_AUTO);
 *		myloop->as_loop = as_event_set_external_loop(myloop->loop);
 *		ev_loop(myloop->loop, 0);
 *		ev_loop_destroy(myloop->loop);
 *		return NULL;
 *	}
 *
 *	int capacity = 8;
 *	struct my_loop* loops = malloc(sizeof(struct my_loop) * capacity);
 *	as_event_set_external_loop_capacity(capacity);
 *
 *	for (int i = 0; i < capacity; i++) {
 *		struct my_loop* myloop = &loops[i];
 *		return pthread_create(&myloop->thread, NULL, my_loop_worker_thread, myloop) == 0;
 *	}
 *	~~~~~~~~~~
 *
 *	@param loop		External event loop.
 *	@return			Client's generic event loop abstraction that is used in client async commands.
 *					Returns NULL if external loop capacity would be exceeded.
 *
 *	@ingroup async_events
 */
as_event_loop*
as_event_set_external_loop(void* loop);

/**
 *	Find client's event loop abstraction given the external event loop.
 *
 *	@param loop		External event loop.
 *	@return			Client's generic event loop abstraction that is used in client async commands.
 *					Returns NULL if loop not found.
 *
 *	@ingroup async_events
 */
as_event_loop*
as_event_loop_find(void* loop);

/**
 *	Retrieve event loop by array index.
 *
 *	@param index	Event loop array index.
 *	@return			Client's generic event loop abstraction that is used in client async commands.
 *
 *	@ingroup async_events
 */
static inline as_event_loop*
as_event_loop_get_by_index(uint32_t index)
{
	return index < as_event_loop_size ? &as_event_loops[index] : NULL;
}

/**
 *	Retrieve a random event loop using round robin distribution.
 *
 *	@return			Client's generic event loop abstraction that is used in client async commands.
 *
 *	@ingroup async_events
 */
static inline as_event_loop*
as_event_loop_get()
{
	// Increment is not atomic because it doesn't need to be exactly accurate.
	uint32_t current = as_event_loop_current++;
	return &as_event_loops[current % as_event_loop_size];
}

/**
 *	Close internally created event loops and release memory for event loop abstraction.
 *	This method should be called once on program shutdown if as_event_create_loops() or
 *	as_event_set_external_loop_capacity() was called.
 *
 *	@ingroup async_events
 */
void
as_event_close_loops();

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_admin.h>
#include <aerospike/as_cluster.h>
#include <aerospike/as_listener.h>
#include <aerospike/as_queue.h>
#include <aerospike/as_proto.h>
#include <aerospike/as_socket.h>
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
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * TYPES
 *****************************************************************************/
	
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
#endif
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
#endif
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
	
/******************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

extern as_event_loop* as_event_loops;
extern uint32_t as_event_loop_size;
extern uint32_t as_event_loop_current;
	
/******************************************************************************
 * COMMON FUNCTIONS
 *****************************************************************************/

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
	
/******************************************************************************
 * IMPLEMENTATION SPECIFIC FUNCTIONS
 *****************************************************************************/

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

/******************************************************************************
 * LIBEV INLINE FUNCTIONS
 *****************************************************************************/

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

/******************************************************************************
 * LIBUV INLINE FUNCTIONS
 *****************************************************************************/

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

/******************************************************************************
 * EVENT_LIB NOT DEFINED INLINE FUNCTIONS
 *****************************************************************************/

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

#endif
	
/******************************************************************************
 * COMMON INLINE FUNCTIONS
 *****************************************************************************/

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

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_cluster.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	@private
 *	Name value pair.
 */
typedef struct as_name_value_s {
	char* name;
	char* value;
} as_name_value;

struct sockaddr_in;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Send info command to specific node. The values must be freed by the caller on success.
 */
as_status
as_info_command_node(as_error* err, as_node* node, char* command, bool send_asis, uint64_t deadline_ms, char** response);

/**
 *	@private
 *	Send info command to specific host. The values must be freed by the caller on success.
 */
as_status
as_info_command_host(as_cluster* cluster, as_error* err, struct sockaddr_in* sa_in, char* command,
	 bool send_asis, uint64_t deadline_ms, char** response);

/**
 *	@private
 *	Send info command to specific socket. The values must be freed by the caller on success.
 *	Set max_response_length to zero if response size should not be bounded.
 */
as_status
as_info_command(as_error* err, int fd, char* names, bool send_asis, uint64_t deadline_ms,
				uint64_t max_response_length, char** values);

/**
 *	@private
 *	Create and authenticate socket for info requests.
 */
as_status
as_info_create_socket(as_cluster* cluster, as_error* err, struct sockaddr_in* sa_in,
					  uint64_t deadline_ms, int* fd_out);

/**
 *	@private
 *	Return the single command's info response buffer value.
 *	The original buffer will be modified with the null termination character.
 */
as_status
as_info_parse_single_response(char *values, char **value);

/**
 *	@private
 *	Parse info response buffer into name/value pairs, one for each command.
 *	The original buffer will be modified with null termination characters to
 *	delimit each command name and value referenced by the name/value pairs.
 */
void
as_info_parse_multi_response(char* buf, as_vector* /* <as_name_value> */ values);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/aerospike.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	The status of a particular background scan.
 */
typedef enum as_job_status_e {
	/**
	 *	The job status is undefined.
	 *	This is likely due to the status not being properly checked.
	 */
	AS_JOB_STATUS_UNDEF,

	/**
	 *	The job is currently running.
	 */
	AS_JOB_STATUS_INPROGRESS,

	/**
	 *	The job completed successfully.
	 */
	AS_JOB_STATUS_COMPLETED,
} as_job_status;

/**
 *	Information about a particular background job.
 */
typedef struct as_job_info_s {
	/**
	 *	Status of the job.
	 */
	as_job_status status;

	/**
	 *	Progress estimate for the job, as percentage.
	 */
	uint32_t progress_pct;

	/**
	 *	How many records have been scanned.
	 */
	uint32_t records_read;
} as_job_info;

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Wait for a background job to be completed by servers.
 *
 *	~~~~~~~~~~{.c}
 *	uint64_t job_id = 1234;
 *	aerospike_job_wait(&as, &err, NULL, "scan", job_id, 0);
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param module		Background module. Values: scan | query
 *	@param job_id		Job ID.
 *	@param interval_ms	Polling interval in milliseconds. If zero, 1000 ms is used.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 */
as_status
aerospike_job_wait(
	aerospike* as, as_error* err, const as_policy_info* policy, const char* module, uint64_t job_id,
	uint32_t interval_ms
	);
	
/**
 *	Check the progress of a background job running on the database. The status
 *	of the job running on the datatabse will be populated in as_job_info.
 *
 *	~~~~~~~~~~{.c}
 *	uint64_t job_id = 1234;
 *	as_job_info job_info;
 *
 *	if (aerospike_scan_info(&as, &err, NULL, "scan", job_id, &job_info) != AEROSPIKE_OK) {
 *		fprintf(stderr, "error(%d) %s at [%s:%d]", err.code, err.message, err.file, err.line);
 *	}
 *	else {
 *		printf("Scan id=%ll, status=%d percent=%d", job_id, job_info.status, job_info.progress_pct);
 *	}
 *	~~~~~~~~~~
 *
 *	@param as			The aerospike instance to use for this operation.
 *	@param err			The as_error to be populated if an error occurs.
 *	@param policy		The policy to use for this operation. If NULL, then the default policy will be used.
 *	@param module		Background module. Values: scan | query
 *	@param job_id		Job ID.
 *	@param stop_if_in_progress	Stop querying nodes if background job is still running.
 *	@param info			Information about this background job, to be populated by this operation.
 *
 *	@return AEROSPIKE_OK on success. Otherwise an error occurred.
 */
as_status
aerospike_job_info(
	aerospike* as, as_error* err, const as_policy_info* policy, const char* module, uint64_t job_id,
	bool stop_if_in_progress, as_job_info * info
	);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_bytes.h>
#include <aerospike/as_integer.h>
#include <aerospike/as_error.h>
#include <aerospike/as_string.h>
#include <aerospike/as_status.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	The size of as_digest.value
 *
 *	@ingroup as_key_object
 */
#define AS_DIGEST_VALUE_SIZE 20

/**
 *	The maxium size of as_namespace.
 *
 *	@ingroup as_key_object
 */
#define AS_NAMESPACE_MAX_SIZE 32

/**
 *	The maxium size of as_set.
 *
 *	@ingroup as_key_object
 */
#define AS_SET_MAX_SIZE 64

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Namespace Name
 *
 *	@ingroup as_key_object
 */
typedef char as_namespace[AS_NAMESPACE_MAX_SIZE];

/**
 *	Set Name
 *
 *	@ingroup as_key_object
 */
typedef char as_set[AS_SET_MAX_SIZE];

/**
 *	Digest value
 *
 *	@ingroup as_key_object
 */
typedef uint8_t as_digest_value[AS_DIGEST_VALUE_SIZE];

/**
 *	The digest is the value used to locate a record based on the
 *	set and digest of the record. The digest is calculated using RIPEMD-160.
 *	Keys for digests can be either a string or integer.
 *
 *	@ingroup as_key_object
 */
typedef struct as_digest_s {

	/**
	 *	Indicates whether the digest was calculated.
	 */
	bool init;

	/**
	 *	The digest value.
	 */
	as_digest_value value;

} as_digest;

/**
 *	Key value
 *
 *	@ingroup as_key_object
 */
typedef union as_key_value_u {

	/**
	 *	Integer value.
	 */
	as_integer integer;

	/**
	 * String value.
	 */
	as_string string;

	/**
	 * Raw value.
	 */
	as_bytes bytes;

} as_key_value;


/** 
 *	A key is used for locating records in the database.
 *
 *	## Initialization
 *
 *	A key can either be stack or heap allocated. Use one of the following 
 *	functions to properly initialize an as_key.
 * 
 * 	Each function requires a namespace, set and key value. The set can be 
 *	and empty string.
 *	
 *	For stack allocated as_key, you should you the following functions to
 *	initialize the value:
 *
 *	- as_key_init()	- Initialize the key with a string value.
 *	- as_key_init_int64() - Initialize the key with an int64_t value.
 *	- as_key_init_str() - Same as as_key_init(). 
 *	- as_key_init_raw() - Initialize the key with byte array.
 *	- as_key_init_value() - Initialize the key with an as_key_value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *	~~~~~~~~~~
 *	
 *	For heap allocated as_key, you should use the following functions
 *	to allocate and initialize the value on the heap.
 *
 *	- as_key_new() 	- Initialize the key with a string value.
 *	- as_key_new_int64() - Initialize the key with an int64_t value.
 *	- as_key_new_str() - Same as as_key_new(). 
 *	- as_key_new_raw() - Initialize the key with byte array.
 *	- as_key_new_value() - Initialize the key with an as_key_value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key * key = as_key_new("ns", "set", "key");
 *	~~~~~~~~~~
 *
 *	## Destruction
 *
 *	When you no longer require an instance of as_key, you should release the
 *	key and associated resources via as_key_destroy().
 *
 *	~~~~~~~~~~{.c}
 *	as_key_destroy(key);
 *	~~~~~~~~~~
 *
 *	This function should be used on both stack and heap allocated keys.
 *
 *	## Operations
 *
 *	The following are operations which require a key.
 *
 *	- aerospike_key_get()
 *	- aerospike_key_select()
 *	- aerospike_key_exists()
 *	- aerospike_key_put()
 *	- aerospike_key_operate()
 *	- aerospike_key_remove()
 *	- aerospike_key_apply()
 *
 *	## Digest
 *
 *	Each operation that requires a key,  internally generates a digest for the 
 *	key. The digest is a hash value used to locate a record in the cluster. Once
 *	calculated, the digest will be reused.
 *
 *	To get the digest value of a key, use as_key_digest().
 *
 *	@ingroup client_objects
 */
typedef struct as_key_s {

	/**
	 *	@private
	 *	If true, then as_key_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	The namespace the key belongs to.
	 */
	as_namespace ns;

	/**
	 *	The set the key belongs to.
	 */
	as_set set;

	/**
	 *	The key value.
	 */
	as_key_value value;

	/**
	 *	The key value pointer.
	 *	If NULL, then there is no value.
	 *	It can point to as_key.value or a different value.
	 */
	as_key_value * valuep;

	/**
	 * Digest for the key.
	 */
	as_digest digest;

} as_key;

/******************************************************************************
 *	as_key FUNCTIONS
 *****************************************************************************/

/**
 *	Initialize a stack allocated as_key to a NULL-terminated string value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init(&key, "ns", "set", "key");
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *	
 *	@param key		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_init(as_key * key, const as_namespace ns, const as_set set, const char * value);

/**
 *	Initialize a stack allocated as_key to a int64_t value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init_int64(&key, "ns", "set", 123);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *
 *	@param key		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_init_int64(as_key * key, const as_namespace ns, const as_set set, int64_t value);

/**
 *	Initialize a stack allocated as_key to a NULL-terminated string value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init_strp(&key, "ns", "set", stdup("key"), true);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *
 *	@param key		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *	@param free		If true, then the key's value can be freed when the key is destroyed.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_init_strp(as_key * key, const as_namespace ns, const as_set set, const char * value, bool free);

/**
 *	Initialize a stack allocated as_key to a NULL-terminated string value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key key;
 *	as_key_init_str(&key, "ns", "set", "key");
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *
 *	@param key		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value. Must last for the lifetime of the key.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
static inline as_key * as_key_init_str(as_key * key, const as_namespace ns, const as_set set, const char * value)
{
	return as_key_init_strp(key, ns, set, value, false);
}

/**
 *	Initialize a stack allocated as_key to bytes array.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t * rgb = (uint8_t *) malloc(3);
 *	rgb[0] = 255;
 *	rgb[1] = 255;
 *	rgb[3] = 255;
 *	
 *	as_key key;
 *	as_key_init_rawp(&key, "ns", "set", rgb, 3, true);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *
 *	@param key		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *	@param size		The number of bytes in value.
 *	@param free		If true, then the key's value can be freed when the key is destroyed.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_init_rawp(as_key * key, const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size, bool free);

/**
 *	Initialize a stack allocated as_key to bytes array.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t rgb[3] = {254,254,120};
 *	
 *	as_key key;
 *	as_key_init_raw(&key, "ns", "set", rgb, 3);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *
 *	@param key		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *	@param size		The number of bytes in value. Must last for the lifetime of the key.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
static inline as_key * as_key_init_raw(as_key * key, const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size)
{
	return as_key_init_rawp(key, ns, set, value, size, false);
}

/**
 *	Initialize a stack allocated as_key with a digest.
 *
 *	~~~~~~~~~~{.c}
 *	as_digest_value digest = {0};
 *	
 *	as_key key;
 *	as_key_init_digest(&key, "ns", "set", digest);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *	
 *	@param key 		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param digest	The digest for the key.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_init_digest(as_key * key, const as_namespace ns, const as_set set, const as_digest_value digest);

/**
 *	Initialize a stack allocated as_key to an as_key_value.
 *
 *	~~~~~~~~~~{.c}
 *	as_string str;
 *	as_string_init(&str, "abc", false);
 *	
 *	as_key key;
 *	as_key_init_value(&key, "ns", "set", (as_key_value *) str);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key.
 *
 *	@param key 		The key to initialize.
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_init_value(as_key * key, const as_namespace ns, const as_set set, const as_key_value * value);


/**
 *	Creates and initializes a heap allocated as_key to a NULL-terminated string value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key * key = as_key_new("ns", "set", "key");
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_new(const as_namespace ns, const as_set set, const char * value);

/**
 *	Initialize a stack allocated as_key to a int64_t value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key * key = as_key_new_int64("ns", "set", 123);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_new_int64(const as_namespace ns, const as_set set, int64_t value);

/**
 *	Creates and initializes a heap allocated as_key to a NULL-terminated string value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key * key = as_key_new_strp("ns", "set", strdup("key"), true);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *	@param free		If true, then the key's value can be freed when the key is destroyed.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_new_strp(const as_namespace ns, const as_set set, const char * value, bool free);

/**
 *	Creates and initializes a heap allocated as_key to a NULL-terminated string value.
 *
 *	~~~~~~~~~~{.c}
 *	as_key * key = as_key_new_str("ns", "set", "key");
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value. Must last for the lifetime of the key.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
static inline as_key * as_key_new_str(const as_namespace ns, const as_set set, const char * value)
{
	return as_key_new_strp(ns, set, value, false);
}

/**
 *	Initialize a stack allocated as_key to a byte array.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t * rgb = (uint8_t *) malloc(3);
 *	rgb[0] = 255;
 *	rgb[1] = 255;
 *	rgb[3] = 255;
 *	
 *	as_key * key = as_key_new_rawp("ns", "set", rgb, 3, true);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *	@param size		The number of bytes in the value.
 *	@param free		If true, then the key's value can be freed when the key is destroyed.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_new_rawp(const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size, bool free);

/**
 *	Initialize a stack allocated as_key to a byte array.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t rgb[3] = {254,254,120};
 *	
 *	as_key * key = as_key_new_raw("ns", "set", rgb, 3);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value. Must last for the lifetime of the key.
 *	@param size		The number of bytes in the value.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
static inline as_key * as_key_new_raw(const as_namespace ns, const as_set set, const uint8_t * value, uint32_t size)
{
	return as_key_new_rawp(ns, set, value, size, false);
}

/**
 *	Initialize a stack allocated as_key with a digest.
 *
 *	~~~~~~~~~~{.c}
 *	as_digest_value digest = {0};
 *	
 *	as_key * key = as_key_new_digest("ns", "set", digest);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param digest	The key's digest.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_new_digest(const as_namespace ns, const as_set set, const as_digest_value digest);

/**
 *	Initialize a stack allocated as_key to a an as_key_value.
 *
 *	~~~~~~~~~~{.c}
 *	as_string str;
 *	as_string_init(&str, "abc", false);
 *	
 *	as_key * key = as_key_new_value("ns", "set", (as_key_value *) str);
 *	~~~~~~~~~~
 *
 *	Use as_key_destroy() to release resources allocated to as_key via
 *	this function.
 *
 *	@param ns 		The namespace for the key.
 *	@param set		The set for the key.
 *	@param value	The key's value.
 *
 *	@return A new as_key on success. Otherwise NULL.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_key * as_key_new_value(const as_namespace ns, const as_set set, const as_key_value * value);

/**
 *	Destory the as_key, releasing resources.
 *
 *	~~~~~~~~~~{.c}
 *	as_key_destroy(key);
 *	~~~~~~~~~~
 *
 *	@param key The as_key to destroy.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
void as_key_destroy(as_key * key);

/**
 *	Get the digest for the given key. 
 *
 *	The digest is computed the first time function is called. Subsequent calls
 *	will return the previously calculated value.
 *
 *	~~~~~~~~~~{.c}
 *	as_digest * digest = as_key_digest(key);
 *	~~~~~~~~~~
 *
 *	@param key The key to get the digest for.
 *
 *	@return The digest for the key.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_digest * as_key_digest(as_key * key);

/**
 *	Set the digest value in the key structure.  Keys must be integer, string or blob.
 *	Otherwise, an error is returned.
 *
 *	@param err Error message that is populated on error.
 *	@param key The key to get the digest for.
 *
 *	@return Status code.
 *
 *	@relates as_key
 *	@ingroup as_key_object
 */
as_status
as_key_set_digest(as_error* err, as_key* key);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_bin.h>
#include <aerospike/as_map.h>
#include <aerospike/as_udf.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	CONSTANTS
 *****************************************************************************/
/**
 *	Enumeration of Large Data Types
 */
typedef enum as_ldt_type_e {

	AS_LDT_LLIST,
	AS_LDT_LMAP,
	AS_LDT_LSET,
	AS_LDT_LSTACK

} as_ldt_type;


/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Represents a bin containing an LDT value. 
 *
 *	@ingroup client_objects
 */
typedef struct as_ldt_s {

	/**
	 *	@private
	 *	If true, then as_ldt_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Bin name.
	 */
	as_bin_name name;

	/**
	 *	LDT Type.
	 */
	as_ldt_type type;

	/**
	 *	LDT UDF Module
	 */
	as_udf_module_name module;
	
} as_ldt;

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Creates and initializes a heap allocated as_ldt.
 *
 *	~~~~~~~~~~{.c}
 *	as_ldt * ldt = as_ldt_new("mystack", AS_LDT_LSTACK, NULL);
 *	~~~~~~~~~~
 *
 *	Use as_ldt_destroy() to release resources allocated to as_ldt via
 *	this function.
 *	
 *	@param name		The name of the bin to contain the ldt.
 *	@param type		The type of ldt data to store in the bin.
 *	@param module	The name of ldt customization module to use for this initialization.
 *
 *	@return The initialized as_key on success. Otherwise NULL.
 *
 *	@relates as_ldt
 *	@ingroup as_ldt_object
 */
as_ldt * as_ldt_new(const as_bin_name name, const as_ldt_type type, const as_udf_module_name module);


/**
 *	Initialize a stack allocated as_ldt.
 *
 *	~~~~~~~~~~{.c}
 *	as_ldt ldt;
 *	as_ldt_init(&ldt, "mystack", AS_LDT_LSTACK, NULL);
 *	~~~~~~~~~~
 *
 *	Use as_ldt_destroy() to release resources allocated to as_ldt via
 *	this function.
 *	
 *	@param ldt		The ldt to initialize.
 *	@param name		The name of the bin to contain the ldt.
 *	@param type		The type of ldt data to store in the bin.
 *	@param module	The name of ldt customization module to use for this initialization.
 *
 *	@return The initialized as_ldt on success. Otherwise NULL.
 *
 *	@relates as_ldt
 *	@ingroup as_ldt_object
 */
as_ldt * as_ldt_init(as_ldt * ldt, const as_bin_name name, const as_ldt_type type, const as_udf_module_name module);

/**
 *	Destroy the as_ldt, releasing resources.
 *
 *	~~~~~~~~~~{.c}
 *	as_ldt_destroy(ldt);
 *	~~~~~~~~~~
 *
 *	@param ldt The as_ldt to destroy.
 *
 *	@relates as_ldt
 *	@ingroup as_ldt_object
 */
void as_ldt_destroy(as_ldt * ldt);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_error.h>
#include <aerospike/as_event.h>
#include <aerospike/as_record.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	User callback when an asynchronous write completes.
 *
 *	@param err			This error structure is only populated when the command fails. Null on success.
 *	@param udata 		User data that is forwarded from asynchronous command function.
 *	@param event_loop 	Event loop that this command was executed on.  Use this event loop when running
 *						nested asynchronous commands when single threaded behavior is desired for the
 *						group of commands.
 */
typedef void (*as_async_write_listener) (as_error* err, void* udata, as_event_loop* event_loop);
	
/**
 *	User callback when an asynchronous read completes with a record result.
 *
 *	@param err			This error structure is only populated when the command fails. Null on success.
 *	@param record 		The return value from the asynchronous command. This value will need to be cast
 *						to the structure that corresponds to the asynchronous command.  Null on error.
 *	@param udata 		User data that is forwarded from asynchronous command function.
 *	@param event_loop 	Event loop that this command was executed on.  Use this event loop when running
 *						nested asynchronous commands when single threaded behavior is desired for the
 *						group of commands.
 */
typedef void (*as_async_record_listener) (as_error* err, as_record* record, void* udata, as_event_loop* event_loop);

/**
 *	User callback when asynchronous read completes with an as_val result.
 *
 *	@param err			This error structure is only populated when the command fails. Null on success.
 *	@param val			The return value from the asynchronous command. This value will need to be cast
 *						to the structure that corresponds to the asynchronous command.  Null on error.
 *	@param udata 		User data that is forwarded from asynchronous command function.
 *	@param event_loop 	Event loop that this command was executed on.  Use this event loop when running
 *						nested asynchronous commands when single threaded behavior is desired for the
 *						group of commands.
 */
typedef void (*as_async_value_listener) (as_error* err, as_val* val, void* udata, as_event_loop* event_loop);

/**
 *	User callback when pipelined command has been sent, i.e., when the connection is ready for sending
 *	the next command.
 *
 *	@param udata 		User data that is forwarded from asynchronous command function.
 *	@param event_loop 	Event loop that this command was executed on.  Use this event loop when running
 *						nested asynchronous commands when single threaded behavior is desired for the
 *						group of commands.
 */
typedef void (*as_pipe_listener) (void* udata, as_event_loop* event_loop);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_cluster.h>
#include <aerospike/as_vector.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Lookup address(es) given hostname. Addresses are returned in the sockaddr_in vector
 *	if it's not null.  The addition to the vector will be done via a unique add just in
 *	case there are duplicates. Return zero on success.
 */
as_status
as_lookup(as_error* err, char* hostname, uint16_t port, as_vector* /*<struct sockaddr_in>*/ addresses);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_error.h>
#include <aerospike/as_event.h>
#include <aerospike/as_queue.h>
#include <aerospike/as_vector.h>
#include <citrusleaf/cf_queue.h>
#include <netinet/in.h>
#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Concurrency kit needs to be under extern "C" when compiling C++.
#include <aerospike/ck/ck_pr.h>
	
/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Maximum size (including NULL byte) of a hostname.
 */
#define AS_HOSTNAME_SIZE 256

/**
 *	Maximum size of node name
 */
#define AS_NODE_NAME_SIZE 20

// Leave this is in for backwards compatibility.
#define AS_NODE_NAME_MAX_SIZE AS_NODE_NAME_SIZE

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Socket address information.
 */
typedef struct as_address_s {
	/**
	 *	Socket IP address.
	 */
	struct sockaddr_in addr;
	
	/**
	 *	Socket IP address string representation (xxx.xxx.xxx.xxx).
	 */
	char name[INET_ADDRSTRLEN];
} as_address;
	
struct as_cluster_s;

/**
 *	Server node representation.
 */
typedef struct as_node_s {
	/**
	 *	@private
	 *  Reference count of node.
	 */
	uint32_t ref_count;
	
	/**
	 *	@private
	 *	Server's generation count for partition management.
	 */
	uint32_t partition_generation;
	
	/**
	 *	The name of the node.
	 */
	char name[AS_NODE_NAME_SIZE];
	
	/**
	 *	@private
	 *	Primary host address index into addresses array.
	 */
	uint32_t address_index;
	
	/**
	 *	@private
	 *	Vector of sockaddr_in which the host is currently known by.
	 *	Only used by tend thread. Not thread-safe.
	 */
	as_vector /* <as_address> */ addresses;
	
	/**
	 *	@private
	 *	Vector of aliases which the host is currently known by.
	 *	Only used by tend thread. Not thread-safe.
	 */
	as_vector /* <as_host> */ aliases;

	struct as_cluster_s* cluster;
	
	/**
	 *	@private
	 *	Pool of current, cached FDs.
	 */
	cf_queue* conn_q;
	
	/**
	 *	@private
	 *	Array of connection pools used in async commands.  There is one pool per node/event loop.
	 *	Only used by event loop threads. Not thread-safe.
	 */
	as_queue* async_conn_qs;
	
	/**
	 * 	@private
	 * 	Pool of connections used in pipelined async commands.  Also not thread-safe.
	 */
	as_queue* pipe_conn_qs;

	/**
	 *	@private
	 *	Socket used exclusively for cluster tend thread info requests.
	 */
	int info_fd;
		
	/**
	 *	@private
	 *	Number of other nodes that consider this node a member of the cluster.
	 */
	uint32_t conn_count;

	/**
	 *	@private
	 *	Number of other nodes that consider this node a member of the cluster.
	 */
	uint32_t friends;
	
	/**
	 *	@private
	 *	Number of consecutive info request failures.
	 */
	uint32_t failures;

	/**
	 *	@private
	 *	Shared memory node array index.
	 */
	uint32_t index;
	
	/**
	 *	@private
	 *	Is node currently active.
	 */
	uint8_t active;
	
	/**
	 *	@private
	 *	Does node support batch-index protocol?
	 */
	uint8_t has_batch_index;
	
	/**
	 *	@private
	 *	Does node support replicas-all info protocol?
	 */
	uint8_t has_replicas_all;
	
	/**
	 *	@private
	 *	Does node support floating point type?
	 */
	uint8_t has_double;
	
	/**
	 *	@private
	 *	Does node support geospatial queries?
	 */
	uint8_t has_geo;
	
} as_node;

/**
 *	@private
 *	Node discovery information.
 */
typedef struct as_node_info_s {
	/**
	 *	@private
	 *	Node name.
	 */
	char name[AS_NODE_NAME_SIZE];

	/**
	 *	@private
	 *	Validated socket.
	 */
	int fd;

	/**
	 *	@private
	 *	Does node support batch-index protocol?
	 */
	uint8_t has_batch_index;
	
	/**
	 *	@private
	 *	Does node support replicas-all info protocol?
	 */
	uint8_t has_replicas_all;
	
	/**
	 *	@private
	 *	Does node support floating point type?
	 */
	uint8_t has_double;
	
	/**
	 *	@private
	 *	Does node support geospatial queries?
	 */
	uint8_t has_geo;
	
} as_node_info;

/**
 *	@private
 *	Friend host address information.
 */
typedef struct as_host_s {
	/**
	 *	@private
	 *	Hostname or IP address string representation (xxx.xxx.xxx.xxx).
	 */
	char name[AS_HOSTNAME_SIZE];
	
	/**
	 *	@private
	 *	Socket IP port.
	 */
	in_port_t port;
	
} as_host;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Create new cluster node.
 */
as_node*
as_node_create(struct as_cluster_s* cluster, as_host* host, struct sockaddr_in* addr, as_node_info* node_info);

/**
 *	@private
 *	Close all connections in pool and free resources.
 */
void
as_node_destroy(as_node* node);

/**
 *	@private
 *	Set node to inactive.
 */
static inline void
as_node_deactivate(as_node* node)
{
	// Make volatile write so changes are reflected in other threads.
	ck_pr_store_8(&node->active, false);
}

/**
 *	@private
 *	Reserve existing cluster node.
 */
static inline void
as_node_reserve(as_node* node)
{
	//ck_pr_fence_acquire();
	ck_pr_inc_32(&node->ref_count);
}

/**
 *	@private
 *	Release existing cluster node.
 */
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

/**
 *	@private
 *	Add socket address to node addresses.
 */
void
as_node_add_address(as_node* node, as_host* host, struct sockaddr_in* addr);

/**
 *	@private
 *	Get socket address and name.
 */
static inline struct sockaddr_in*
as_node_get_address(as_node* node)
{
	as_address* address = (as_address *)as_vector_get(&node->addresses, node->address_index);
	return &address->addr;
}

/**
 *	Get socket address and name.
 */
static inline as_address*
as_node_get_address_full(as_node* node)
{
	return (as_address *)as_vector_get(&node->addresses, node->address_index);
}

/**
 *	@private
 *	Get a connection to the given node from pool and validate.  Return 0 on success.
 */
as_status
as_node_get_connection(as_error* err, as_node* node, uint64_t deadline_ms, int* fd);

/**
 *	@private
 *	Close a node's connection and do not put back into pool.
 */
static inline void
as_node_close_connection(as_node* node, int fd) {
	close(fd);
	ck_pr_dec_32(&node->conn_count);
}

/**
 *	@private
 *	Put connection back into pool.
 */
static inline void
as_node_put_connection(as_node* node, int fd)
{
	if (cf_queue_push(node->conn_q, &fd) != CF_QUEUE_OK) {
		as_node_close_connection(node, fd);
	}
}

/**
 *	@private
 *	Are hosts equal.
 */
static inline bool
as_host_equals(as_host* h1, as_host* h2)
{
	return strcmp(h1->name, h2->name) == 0 && h1->port == h2->port;
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <aerospike/as_bin.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

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

/**
 *	Operation Identifiers
 */
typedef enum as_operator_e {

	/**
	 *	Return the bin from the cluster.
	 */
	AS_OPERATOR_READ       = 1,
	
	/**
	 *	Update the bin.
	 */
	AS_OPERATOR_WRITE      = 2,

	AS_OPERATOR_CDT_READ   = 3,
	AS_OPERATOR_CDT_MODIFY = 4,

	/**
	 *	Increment a bin containing an
	 *	integer value.
	 */
	AS_OPERATOR_INCR       = 5,

	/**
	 *	Append bytes to the bin containing
	 *	either a string or blob.
	 */
	AS_OPERATOR_APPEND     = 9,
	
	/**
	 *	Prepend bytes to the bin containing
	 *	either a string or blob.
	 */
	AS_OPERATOR_PREPEND    = 10,

	/**
	 *	Touch the record's ttl.
	 */
	AS_OPERATOR_TOUCH      = 11

} as_operator;

/**
 *	Operation on a bin.
 *	The value for the bin will be applied according to the operation.
 */
typedef struct as_binop_s {

	/**
	 *	The operation to be performed on the bin.
	 */
	as_operator op;

	/**
	 *	The bin the operation will be performed on.
	 */
	as_bin bin;

} as_binop;

/**
 *	Sequence of operations.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 *	as_operations_inita(&ops, 2);
 *	as_operations_add_incr(&ops, "bin1", 123);
 *	as_operations_add_append_str(&ops, "bin2", "abc");
 *	...
 *	as_operations_destroy(&ops);
 *	~~~~~~~~~~
 *
 */
typedef struct as_binops_s {

	/**
	 *	@private
	 *	If true, then as_binops_destroy() will free the entries.
	 */
	bool _free;

	/**
	 *	Number of entries allocated
	 */
	uint16_t capacity;

	/**
	 *	Number of entries used
	 */
	uint16_t size;

	/**
	 *	Sequence of entries
	 */
	as_binop * entries;

} as_binops;

/**
 *	The `aerospike_key_operate()` function provides the ability to execute
 *	multiple operations on a record in the database as a single atomic 
 *	transaction.
 *
 *	The `as_operations` object is used to define the operations to be performed
 *	on the record.
 *
 *	## Initialization
 *
 *	Before using as_operations, you must first initialize it via either:
 *	- as_operations_inita()
 *	- as_operations_init()
 *	- as_operations_new()
 *
 *	as_operations_inita() is a macro that initializes a stack allocated 
 *	as_operations and allocates an internal array of operations. The macro
 *	accepts a pointer to the stack allocated as_operations and the number of
 *	operations to be added.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 *	as_operations_inita(&ops, 2);
 *	~~~~~~~~~~
 *
 *	as_operations_init() is a function that initializes a stack allocated 
 *	as_operations. It differes from as_operations_inita() in that it allocates
 *	the internal array of operations on the heap. It accepts a pointer to the 
 *	stack allocated as_operations and the number of operations to be added.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 *	as_operations_init(&ops, 2);
 *	~~~~~~~~~~
 *	
 *	as_operations_new() is a function that will allocate a new as_operations
 *	on the heap. It will also allocate the internal array of operation on the 
 *	heap.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations * ops = as_operations_new(2);
 *	~~~~~~~~~~
 *
 *	When you no longer needthe as_operations, you can release the resources 
 *	allocated to it via as_operations_destroy().
 *
 *	## Destruction
 *	
 *	When you no longer require an as_operations, you should call 
 *	`as_operations_destroy()` to release it and associated resources.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_destroy(ops);
 *	~~~~~~~~~~
 *
 *	## Usage
 *
 *	as_operations is a sequence of operations to be applied to a record.
 *	
 *	Each of the following operations is added to the end of the sequence
 *	of operations.
 *
 *	When you have compiled the sequence of operations you want to execute,
 *	then you will send it to aerospike_key_operate().
 *
 *
 *	### Modifying a String
 *
 *	Aerospike allows you to append a string to a bin containing
 *	a string.
 *
 *	The following appends a "abc" to bin "bin1".
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_append_str(ops, "bin1", "abc");
 *	~~~~~~~~~~
 *	
 *	There is also a prepend operation, which will add the string
 *	to the beginning of the bin's current value.
 *	
 *	~~~~~~~~~~{.c}
 *	as_operations_add_prepend_str(ops, "bin1", "abc");
 *	~~~~~~~~~~
 *
 *	### Modifying a Byte Array
 *
 *	Aerospike allows you to append a byte array to a bin containing
 *	a byte array.
 *
 *	The following appends a 4 byte sequence to bin "bin1".
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t raw[4] = { 1, 2, 3, 4 };
 *	as_operations_add_append_raw(ops, "bin1", raw, 4);
 *	~~~~~~~~~~
 *	
 *	There is also a prepend operation, which will add the bytes
 *	to the beginning of the bin's current value.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t raw[4] = { 1, 2, 3, 4 };
 *	as_operations_add_prepend_raw(ops, "bin1", raw, 4);
 *	~~~~~~~~~~
 *
 *	### Increment an Integer
 *
 *	Aerospike allows you to increment the value of a bin 
 *
 *	The following increments the value in bin "bin1" by 4.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_incr(ops, "bin1", 4);
 *	~~~~~~~~~~
 *	
 *	### Write a Value
 *
 *	Write a value into a bin. Overwriting previous value.
 *
 *	The following writes a string "xyz" to "bin1".
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_write_str(ops, "bin1", "xyz");
 *	~~~~~~~~~~
 *	
 *	### Read a Value
 *
 *	Read a value from a bin. This is ideal, if you performed an 
 *	operation on a bin, and want to read the new value.
 *
 *	The following reads the value of "bin1"
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_read(ops, "bin1", "xyz");
 *	~~~~~~~~~~
 *
 *	### Touch a Record
 *
 *	Touching a record will refresh its ttl and increment the generation
 *	of the record.
 *
 *	The following touches a record.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_touch(ops);
 *	~~~~~~~~~~
 *
 *	@ingroup client_objects
 */
typedef struct as_operations_s {

	/**
	 *	@private
	 *	If true, then as_operations_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	The generation of the record.
	 */
	uint16_t gen;

	/**
	 *	The time-to-live (expiration) of the record in seconds.
	 */
	uint32_t ttl;

	/**
	 * Operations to be performed on the bins of a record.
	 */
	as_binops binops;

} as_operations;

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Initializes a stack allocated `as_operations` (as_operations) and allocates
 *	`__nops` number of entries on the stack.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 * 	as_operations_inita(&ops, 2);
 *	as_operations_add_incr(&ops, "bin1", 123);
 *	as_operations_add_append_str(&ops, "bin2", "abc");
 *	~~~~~~~~~~
 *
 *	@param __ops		The `as_operations *` to initialize.
 *	@param __nops		The number of `as_binops.entries` to allocate on the
 *						stack.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
#define as_operations_inita(__ops, __nops) \
	(__ops)->_free = false;\
	(__ops)->gen = 0;\
	(__ops)->ttl = 0;\
	(__ops)->binops._free = false;\
	(__ops)->binops.capacity = __nops;\
	(__ops)->binops.size = 0;\
	(__ops)->binops.entries = (as_binop *) alloca(sizeof(as_binop) * __nops);

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Intializes a stack allocated `as_operations`.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 * 	as_operations_init(&ops, 2);
 *	as_operations_add_incr(&ops, "bin1", 123);
 *	as_operations_add_append_str(&ops, "bin2", "abc");
 *	~~~~~~~~~~
 *
 *	Use `as_operations_destroy()` to free the resources allocated to the
 *	`as_operations`.
 *
 *	@param ops 		The `as_operations` to initialize.
 *	@param nops		The number of `as_operations.binops.entries` to allocate on the heap.
 *
 *	@return The initialized `as_operations` on success. Otherwise NULL.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
as_operations * as_operations_init(as_operations * ops, uint16_t nops);

/**
 *	Create and initialize a heap allocated `as_operations`.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops = as_operations_new(2);
 *	as_operations_add_incr(ops, "bin1", 123);
 *	as_operations_add_append_str(ops, "bin2", "abc");
 *	~~~~~~~~~~
 *
 *	Use `as_operations_destroy()` to free the resources allocated to the
 *	`as_operations`.
 *
 *	@param nops		The number of `as_operations.binops.entries` to allocate on the heap.
 *
 *	@return The new `as_operations` on success. Otherwise NULL.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
as_operations * as_operations_new(uint16_t nops);

/**
 *	Destroy an `as_operations` and release associated resources.
 *
 *	~~~~~~~~~~{.c}
 * 	as_operations_destroy(binops);
 *	~~~~~~~~~~
 *
 *	@param ops 	The `as_operations` to destroy.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
void as_operations_destroy(as_operations * ops);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write(as_operations * ops, const as_bin_name name, as_bin_value * value);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with an int64_t value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_int64(as_operations * ops, const as_bin_name name, int64_t value);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a double value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_double(as_operations * ops, const as_bin_name name, double value);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_write_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_write_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated GeoJSON string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name			The name of the bin to perform the operation on.
 *	@param value		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_geojson_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated GeoJSON string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name			The name of the bin to perform the operation on.
 *	@param value		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_write_geojson_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_write_geojson_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_write_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_write_rawp(ops, name, value, size, false);
}

/**
 *	Add a `AS_OPERATOR_READ` bin operation.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_read(as_operations * ops, const as_bin_name name);

/**
 *	Add a `AS_OPERATOR_INCR` bin operation with (required) int64_t value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_incr(as_operations * ops, const as_bin_name name, int64_t value);

/**
 *	Add a `AS_OPERATOR_INCR` bin operation with double value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_incr_double(as_operations * ops, const as_bin_name name, double value);

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_prepend_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_prepend_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_prepend_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_prepend_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *	@param size 		The size of the value.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_prepend_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_prepend_rawp(ops, name, value, size, false);
}

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_append_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_append_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_append_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_append_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *	@param size 		The size of the value.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_append_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_append_rawp(ops, name, value, size, false);
}

/**
 *	Add a `AS_OPERATOR_TOUCH` record operation.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_touch(as_operations * ops);

/******************************************************************************
 *	CDT FUNCTIONS
 *****************************************************************************/

/**
 *  Add an as_val element to end of list.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param val			Value to append. Consumes a reference of this as_val.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_append(as_operations *ops, const as_bin_name name, as_val *val);

/**
 *  Add an integer to end of list. Convenience function of as_operations_add_list_append()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value		An integer value.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_append_int64(as_operations *ops, const as_bin_name name, int64_t value);

/**
 *  Add a double to end of list. Convenience function of as_operations_add_list_append()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value		A double value.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_append_double(as_operations *ops, const as_bin_name name, double value);

/**
 *  Add a string to end of list. Convenience function of as_operations_add_list_append()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value		A c-string.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_append_strp(as_operations *ops, const as_bin_name name, const char *value, bool free);

/**
 *  Add a string to end of list. Convenience function of as_operations_add_list_append()
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value		A c-string.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_list_append_str(as_operations *ops, const as_bin_name name, const char *value)
{
	return as_operations_add_list_append_strp(ops, name, value, false);
}

/**
 *  Add a blob to end of list. Convenience function of as_operations_add_list_append()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value		A blob.
 *	@param size 		Size of the blob.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_append_rawp(as_operations *ops, const as_bin_name name, const uint8_t *value, uint32_t size, bool free);

/**
 *  Add a blob to end of list. Convenience function of as_operations_add_list_append()
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value		A blob.
 *	@param size 		Size of the blob.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_list_append_raw(as_operations *ops, const as_bin_name name, const uint8_t *value, uint32_t size)
{
	return as_operations_add_list_append_rawp(ops, name, value, size, false);
}

/**
 *  Add multiple values to end of list. 
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param list			List of values to append. Consumes a reference of this as_list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */

bool as_operations_add_list_append_items(as_operations *ops, const as_bin_name name, as_list *list);

/**
 *  Insert an as_val element to list at index position.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the as_val will be inserted at. Negative index counts from end of list.
 *	@param val			Value to insert. Consumes a reference of this as_list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_insert(as_operations *ops, const as_bin_name name, int64_t index, as_val *val);

/**
 *  Insert integer to list at index position. Convenience function of as_operations_add_list_insert()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the integer will be inserted at. Negative index counts from end of list.
 *	@param value 		An integer value.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_insert_int64(as_operations *ops, const as_bin_name name, int64_t index, int64_t value);

/**
 *  Insert double to list at index position. Convenience function of as_operations_add_list_insert()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the double will be inserted at. Negative index counts from end of list.
 *	@param value 		A double value.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_insert_double(as_operations *ops, const as_bin_name name, int64_t index, double value);

/**
 *  Insert string to list at index position. Convenience function of as_operations_add_list_insert()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the string will be inserted at. Negative index counts from end of list.
 *	@param value 		A c-string.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_insert_strp(as_operations *ops, const as_bin_name name, int64_t index, const char *value, bool free);

/**
 *  Insert string to list at index position. Convenience function of as_operations_add_list_insert()
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the string will be inserted at. Negative index counts from end of list.
 *	@param value 		A c-string.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_list_insert_str(as_operations *ops, const as_bin_name name, int64_t index, const char *value)
{
	return as_operations_add_list_insert_strp(ops, name, index, value, false);
}

/**
 *  Insert blob to list at index position. Convenience function of as_operations_add_list_insert()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the blob will be inserted at. Negative index counts from end of list.
 *	@param value 		A blob.
 *	@param size 		Size of the blob.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_insert_rawp(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size, bool free);

/**
 *  Insert blob to list at index position. Convenience function of as_operations_add_list_insert()
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the blob will be inserted at. Negative index counts from end of list.
 *	@param value 		A blob.
 *	@param size 		Size of the blob.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_list_insert_raw(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size)
{
	return as_operations_add_list_insert_rawp(ops, name, index, value, size, false);
}

/**
 *  Insert multiple values to list at index position.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position which the blob will be inserted at. Negative index counts from end of list.
 *	@param list 		List of values to insert. Consumes reference of list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_insert_items(as_operations *ops, const as_bin_name name, int64_t index, as_list *list);

/**
 *  Remove and return a value at index.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which the value will be removed and returned.  Negative index counts from end of list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_pop(as_operations *ops, const as_bin_name name, int64_t index);

/**
 *  Remove and return N values from index.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start the removal. Negative index counts from end of list.
 *  @param count 		Number of values to remove. If not enough values in list, will remove to list end.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_pop_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);

/**
 *  Remove and return all values from index to the end of list.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start the removal. Negative index counts from end of list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_pop_range_from(as_operations *ops, const as_bin_name name, int64_t index);

/**
 *  Remove value at index.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start the removal. Negative index counts from end of list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_remove(as_operations *ops, const as_bin_name name, int64_t index);

/**
 *  Remove N values from index.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start the removal. Negative index counts from end of list.
 *  @param count 		Number of values to remove. If not enough values in list, will remove to list end.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_remove_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);

/**
 *  Remove all values from index until end of list.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start the removal. Negative index counts from end of list.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_remove_range_from(as_operations *ops, const as_bin_name name, int64_t index);

//-----------------------------------------------------------------------------
// Other list modifies

/**
 *  Remove all values. Will leave empty list in bin.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_clear(as_operations *ops, const as_bin_name name);

/**
 *  Set an as_val element of the list at the index position.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param val			Consumes a reference of this as_val.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_set(as_operations *ops, const as_bin_name name, int64_t index, as_val *val);

/**
 *  Set value at index as integer. Convenience function of as_operations_add_list_set()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param value		An integer value.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_set_int64(as_operations *ops, const as_bin_name name, int64_t index, int64_t value);

/**
 *  Set value at index as double. Convenience function of as_operations_add_list_set()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param value		A double value.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_set_double(as_operations *ops, const as_bin_name name, int64_t index, double value);

/**
 *  Set value at index as string. Convenience function of as_operations_add_list_set()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param value		A c-string.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_set_strp(as_operations *ops, const as_bin_name name, int64_t index, const char *value, bool free);

/**
 *  Set value at index as string. Convenience function of as_operations_add_list_set()
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param value		A c-string.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_list_set_str(as_operations *ops, const as_bin_name name, int64_t index, const char *value)
{
	return as_operations_add_list_set_strp(ops, name, index, value, false);
}

/**
 *  Set value at index as blob. Convenience function of as_operations_add_list_set()
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param value		A blob.
 *	@param size 		Size of the blob.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_set_rawp(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size, bool free);

/**
 *  Set value at index as blob. Convenience function of as_operations_add_list_set()
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to set. Negative index counts from end of list.
 *  @param value		A blob.
 *	@param size 		Size of the blob.
 *
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_list_set_raw(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size)
{
	return as_operations_add_list_set_rawp(ops, name, index, value, size, false);
}

/**
 * 	Remove values NOT within range(index, count).
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Values from 0-index position are removed. Negative index counts from end of list.
 *  @param count 		Number of values to keep. All other values beyond count are removed.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_trim(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);

//-----------------------------------------------------------------------------
// Read operations

/**
 *  Get value of list at index.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position to get. Negative index counts from end of list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_get(as_operations *ops, const as_bin_name name, int64_t index);
/**
 *  Get multiple values of list starting at index.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start. Negative index counts from end of list.
 * 	@param count 		Number of values to get. If not enough in list, will return all remaining.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_get_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);

/**
 * Get multiple values of list starting at index until end of list.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *  @param index 		Index position at which to start. Negative index counts from end of list.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_get_range_from(as_operations *ops, const as_bin_name name, int64_t index);

/**
 * Get number of values in list.
 *  
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	
 *  @return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_list_size(as_operations *ops, const as_bin_name name);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_node.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Maximum namespace size including null byte.  Effective maximum length is 31.
 */
#define AS_MAX_NAMESPACE_SIZE 32

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	@private
 *  Map of namespace data partitions to nodes.
 */
typedef struct as_partition_s {
	/**
	 *	@private
	 *  Master node for this partition.
	 */
	as_node* master;
	
	/**
	 *	@private
	 *  Prole node for this partition.
	 *  TODO - not ideal for replication factor > 2.
	 */
	as_node* prole;
} as_partition;

/**
 *	@private
 *  Map of namespace to data partitions.
 */
typedef struct as_partition_table_s {
	/**
	 *	@private
	 *	Namespace
	 */
	char ns[AS_MAX_NAMESPACE_SIZE];
	
	/**
	 *	@private
	 *  Fixed length of partition array.
	 */
	uint32_t size;

	/**
	 *	@private
	 *	Array of partitions for a given namespace.
	 */
	as_partition partitions[];
} as_partition_table;

/**
 *	@private
 *  Reference counted array of partition table pointers.
 */
typedef struct as_partition_tables_s {
	/**
	 *	@private
	 *  Reference count of partition table array.
	 */
	uint32_t ref_count;
	
	/**
	 *	@private
	 *  Length of partition table array.
	 */
	uint32_t size;

	/**
	 *	@private
	 *  Partition table array.
	 */
	as_partition_table* array[];
} as_partition_tables;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Create reference counted structure containing partition tables.
 */
as_partition_tables*
as_partition_tables_create(uint32_t capacity);

/**
 *	@private
 *	Destroy and release memory for partition table.
 */
void
as_partition_table_destroy(as_partition_table* table);

/**
 *	@private
 *	Get partition table given namespace.
 */
as_partition_table*
as_partition_tables_get(as_partition_tables* tables, const char* ns);

/**
 *	@private
 *	Is node referenced in any partition table.
 */
bool
as_partition_tables_find_node(as_partition_tables* tables, as_node* node);
	
/**
 *	@private
 *	Return partition ID given digest.
 */
static inline uint32_t
as_partition_getid(const uint8_t* digest, uint32_t n_partitions)
{
	return (*(uint16_t*)digest) & (n_partitions - 1);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2015 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#pragma once

#include <aerospike/as_async.h>
#include <aerospike/as_log.h>
#include <aerospike/as_log_macros.h>
#include <aerospike/as_node.h>
#include <aerospike/as_socket.h>

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
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

/**
 *	@defgroup client_policies Client Policies
 *	
 *  Policies define the behavior of database operations. 
 *
 *  Policies fall into two groups: policy values and operation policies.
 *  A policy value is a single value which defines how the client behaves. An
 *  operation policy is a group of policy values which affect an operation.
 *
 *  ## Policy Values
 *
 *  The following are the policy values. For details, please see the documentation
 *  for each policy value
 *
 *  - as_policy_key
 *  - as_policy_gen
 *  - as_policy_exists
 *  - as_policy_replica
 *  - as_policy_consistency_level
 *  - as_policy_commit_level
 *
 *  ## Operation Policies
 *
 *  The following are the operation policies. Operation policies are groups of
 *  policy values for a type of operation.
 *
 *  - as_policy_batch
 *  - as_policy_info
 *  - as_policy_operate
 *  - as_policy_read
 *  - as_policy_remove
 *  - as_policy_query
 *  - as_policy_scan
 *  - as_policy_write
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Default timeout value
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_TIMEOUT_DEFAULT 1000

/**
 *	Default number of retries when a transaction fails due to a network error.
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_RETRY_DEFAULT 1

/**
 *	Default value for compression threshold
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_COMPRESSION_THRESHOLD_DEFAULT 0

/**
 *	Default as_policy_gen value
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_GEN_DEFAULT AS_POLICY_GEN_IGNORE

/**
 *	Default as_policy_key value
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_KEY_DEFAULT AS_POLICY_KEY_DIGEST

/**
 *	Default as_policy_exists value
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_EXISTS_DEFAULT AS_POLICY_EXISTS_IGNORE

/**
 *	Default as_policy_replica value
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_REPLICA_DEFAULT AS_POLICY_REPLICA_MASTER

/**
 *	Default as_policy_consistency_level value for read
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_CONSISTENCY_LEVEL_DEFAULT AS_POLICY_CONSISTENCY_LEVEL_ONE

/**
 *	Default as_policy_commit_level value for write
 *
 *	@ingroup client_policies
 */
#define AS_POLICY_COMMIT_LEVEL_DEFAULT AS_POLICY_COMMIT_LEVEL_ALL

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Retry Policy
 *
 *	Specifies the behavior of failed operations. 
 *
 *	@ingroup client_policies
 */
typedef enum as_policy_retry_e {

	/**
	 *	Only attempt an operation once.
	 */
	AS_POLICY_RETRY_NONE, 

	/**
	 *	If an operation fails, attempt the operation
	 *	one more time.
	 */
	AS_POLICY_RETRY_ONCE, 

} as_policy_retry;

/**
 *	Generation Policy
 *
 *	Specifies the behavior of record modifications with regard to the 
 *	generation value.
 *
 *	@ingroup client_policies
 */
typedef enum as_policy_gen_e {

	/**
	 *	Write a record, regardless of generation.
	 */
	AS_POLICY_GEN_IGNORE,

	/**
	 *	Write a record, ONLY if generations are equal
	 */
	AS_POLICY_GEN_EQ,

	/**
	 *	Write a record, ONLY if local generation is 
	 *	greater-than remote generation
	 */
	AS_POLICY_GEN_GT

} as_policy_gen;

/**
 *	Key Policy
 *
 *	Specifies the behavior for whether keys or digests
 *	should be sent to the cluster.
 *
 *	@ingroup client_policies
 */
typedef enum as_policy_key_e {

	/**
	 *	Send the digest value of the key.
	 *
	 *	This is the recommended mode of operation. This calculates the digest
	 *	and send the digest to the server. The digest is only calculated on
	 *	the client, and not on the server.
	 */
	AS_POLICY_KEY_DIGEST,

	/**
	 *	Send the key, in addition to the digest value.
	 *
	 *	If you want keys to be returned when scanning or querying, the keys must
	 *	be stored on the server. This policy causes a write operation to store
	 *	the key. Once a key is stored, the server will keep it - there is no
	 *	need to use this policy on subsequent updates of the record.
	 *
	 *	If this policy is used on read or delete operations, or on subsequent
	 *	updates of a record with a stored key, the key sent will be compared
	 *	with the key stored on the server. A mismatch will cause
	 *	AEROSPIKE_ERR_RECORD_KEY_MISMATCH to be returned.
	 */
	AS_POLICY_KEY_SEND,

} as_policy_key;

/**
 *	Existence Policy
 *	
 *	Specifies the behavior for writing the record
 *	depending whether or not it exists.
 *
 *	@ingroup client_policies
 */
typedef enum as_policy_exists_e {

	/**
	 *	Write the record, regardless of existence. (i.e. create or update.)
	 */
	AS_POLICY_EXISTS_IGNORE,

	/**
	 *	Create a record, ONLY if it doesn't exist.
	 */
	AS_POLICY_EXISTS_CREATE,

	/**
	 *	Update a record, ONLY if it exists.
	 */
	AS_POLICY_EXISTS_UPDATE,

	/**
	 *	Completely replace a record, ONLY if it exists.
	 */
	AS_POLICY_EXISTS_REPLACE,

	/**
	 *	Completely replace a record if it exists, otherwise create it.
	 */
	AS_POLICY_EXISTS_CREATE_OR_REPLACE

} as_policy_exists;

/**
 *  Replica Policy
 *
 *  Specifies which partition replica to read from.
 *
 *  @ingroup client_policies
 */
typedef enum as_policy_replica_e {

	/**
	 *  Read from the partition master replica node.
	 */
	AS_POLICY_REPLICA_MASTER,

	/**
	 *  Read from an unspecified replica node.
	 */
	AS_POLICY_REPLICA_ANY

} as_policy_replica;

/**
 *  Consistency Level
 *
 *  Specifies the number of replicas to be consulted
 *  in a read operation to provide the desired
 *  consistency guarantee.
 *
 *  @ingroup client_policies
 */
typedef enum as_policy_consistency_level_e {

	/**
	 *  Involve a single replica in the operation.
	 */
	AS_POLICY_CONSISTENCY_LEVEL_ONE,

	/**
	 *  Involve all replicas in the operation.
	 */
	AS_POLICY_CONSISTENCY_LEVEL_ALL,

} as_policy_consistency_level;

/**
 *  Commit Level
 *
 *  Specifies the number of replicas required to be successfully
 *  committed before returning success in a write operation
 *  to provide the desired consistency guarantee.
 *
 *  @ingroup client_policies
 */
typedef enum as_policy_commit_level_e {

	/**
	 *  Return succcess only after successfully committing all replicas.
	 */
	AS_POLICY_COMMIT_LEVEL_ALL,

	/**
	 *  Return succcess after successfully committing the master replica.
	 */
	AS_POLICY_COMMIT_LEVEL_MASTER,

} as_policy_commit_level;

/**
 *	Write Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_write_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	Maximum number of retries when a transaction fails due to a network error.
	 */
	uint32_t retry;

	/**
	 *	Minimum record size beyond which it is compressed and sent to the server.
	 */
	uint32_t compression_threshold;

	/**
	 *	Specifies the behavior for the key.
	 */
	as_policy_key key;

	/**
	 *	Specifies the behavior for the generation
	 *	value.
	 */
	as_policy_gen gen;

	/**
	 *	Specifies the behavior for the existence 
	 *	of the record.
	 */
	as_policy_exists exists;

	/**
	 *  Specifies the number of replicas required
	 *  to be committed successfully when writing
	 *  before returning transaction succeeded.
	 */
	as_policy_commit_level commit_level;

} as_policy_write;

/**
 *	Read Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_read_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	Maximum number of retries when a transaction fails due to a network error.
	 */
	uint32_t retry;

	/**
	 *	Specifies the behavior for the key.
	 */
	as_policy_key key;

	/**
	 *  Specifies the replica to be consulted for the read.
	 */
	as_policy_replica replica;

	/**
	 *  Specifies the number of replicas consulted
	 *  when reading for the desired consistency guarantee.
	 */
	as_policy_consistency_level consistency_level;
	
	/**
	 *	Should raw bytes representing a list or map be deserialized to as_list or as_map. 
	 *	Set to false for backup programs that just need access to raw bytes.
	 *	Default: true
	 */
	bool deserialize;

} as_policy_read;

/**
 *	Key Apply Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_apply_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	Specifies the behavior for the key.
	 */
	as_policy_key key;

	/**
	 *  Specifies the number of replicas required
	 *  to be committed successfully when writing
	 *  before returning transaction succeeded.
	 */
	as_policy_commit_level commit_level;

	/**
	*	The time-to-live (expiration) of the record in seconds.
	*	There are two special values that can be set in the record TTL:
	*	(*) ZERO (defined as AS_RECORD_DEFAULT_TTL), which means that the
	*	   record will adopt the default TTL value from the namespace.
	*	(*) 0xFFFFFFFF (also, -1 in a signed 32 bit int)
	*	   (defined as AS_RECORD_NO_EXPIRE_TTL), which means that the record
	*	   will get an internal "void_time" of zero, and thus will never expire.
	*
	*	Note that the TTL value will be employed ONLY on write/update calls.
	*/
	uint32_t ttl;

} as_policy_apply;

/**
 *	Operate Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_operate_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	Maximum number of retries when a transaction fails due to a network error.
	 */
	uint32_t retry;
	
	/**
	 *	Specifies the behavior for the key.
	 */
	as_policy_key key;

	/**
	 *	Specifies the behavior for the generation
	 *	value.
	 */
	as_policy_gen gen;

	/**
	 *  Specifies the replica to be consulted for the read.
	 */
	as_policy_replica replica;

	/**
	 *  Specifies the number of replicas consulted
	 *  when reading for the desired consistency guarantee.
	 */
	as_policy_consistency_level consistency_level;

	/**
	 *  Specifies the number of replicas required
	 *  to be committed successfully when writing
	 *  before returning transaction succeeded.
	 */
	as_policy_commit_level commit_level;

	/**
	 *	Should raw bytes representing a list or map be deserialized to as_list or as_map.
	 *	Set to false for backup programs that just need access to raw bytes.
	 *	Default: true
	 */
	bool deserialize;

} as_policy_operate;

/**
 *	Remove Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_remove_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	The generation of the record.
	 */
	uint16_t generation;

	/**
	 *	Maximum number of retries when a transaction fails due to a network error.
	 */
	uint32_t retry;
	
	/**
	 *	Specifies the behavior for the key.
	 */
	as_policy_key key;

	/**
	 *	Specifies the behavior for the generation
	 *	value.
	 */
	as_policy_gen gen;

	/**
	 *  Specifies the number of replicas required
	 *  to be committed successfully when writing
	 *  before returning transaction succeeded.
	 */
	as_policy_commit_level commit_level;

} as_policy_remove;

/**
 *	Query Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_query_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 *
	 *	The default (0) means do not timeout.
	 */
	uint32_t timeout;

	/**
	 *	Should raw bytes representing a list or map be deserialized to as_list or as_map.
	 *	Set to false for backup programs that just need access to raw bytes.
	 *	Default: true
	 */
	bool deserialize;
	
} as_policy_query;

/**
 *	Scan Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_scan_s {

	/**
	 *	Maximum time in milliseconds to wait for the operation to complete.
	 *
	 *	The default (0) means do not timeout.
	 */
	uint32_t timeout;

	/**
	 *	Abort the scan if the cluster is not in a 
	 *	stable state.
	 */
	bool fail_on_cluster_change;

} as_policy_scan;

/**
 *	Info Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_info_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	Send request without any further processing.
	 */
	bool send_as_is;

	/**
	 *	Ensure the request is within allowable size limits.
	 */
	bool check_bounds;

} as_policy_info;

/**
 *	Batch Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_batch_s {

	/**
	 *	Maximum time in milliseconds to wait for 
	 *	the operation to complete.
	 */
	uint32_t timeout;

	/**
	 *	Determine if batch commands to each server are run in parallel threads.
	 *	<p>
	 *	Values:
	 *	<ul>
	 *	<li>
	 *	false: Issue batch commands sequentially.  This mode has a performance advantage for small
	 *	to medium sized batch sizes because commands can be issued in the main transaction thread.
	 *	This is the default.
	 *	</li>
	 *	<li>
	 *	true: Issue batch commands in parallel threads.  This mode has a performance
	 *	advantage for large batch sizes because each node can process the command immediately.
	 *	The downside is extra threads will need to be created (or taken from
	 *	a thread pool).
	 *	</li>
	 *	</ul>
	 */
	bool concurrent;
	
	/**
	 *	Use old batch direct protocol where batch reads are handled by direct low-level batch server
	 *	database routines.  The batch direct protocol can be faster when there is a single namespace,
	 *	but there is one important drawback.  The batch direct protocol will not proxy to a different
	 *	server node when the mapped node has migrated a record to another node (resulting in not
	 *	found record).
	 *	<p>
	 *	This can happen after a node has been added/removed from the cluster and there is a lag
	 *	between records being migrated and client partition map update (once per second).
	 *	<p>
	 *	The new batch index protocol will perform this record proxy when necessary.
	 *	Default: false (use new batch index protocol if server supports it)
	 */
	bool use_batch_direct;

	/**
	 *	Allow batch to be processed immediately in the server's receiving thread when the server
	 *	deems it to be appropriate.  If false, the batch will always be processed in separate
	 *	transaction threads.  This field is only relevant for the new batch index protocol.
	 *	<p>
	 *	For batch exists or batch reads of smaller sized records (<= 1K per record), inline
	 *	processing will be significantly faster on "in memory" namespaces.  The server disables
	 *	inline processing on disk based namespaces regardless of this policy field.
	 *	<p>
	 *	Inline processing can introduce the possibility of unfairness because the server
	 *	can process the entire batch before moving onto the next command.
	 *	Default: true
	 */
	bool allow_inline;
	
	/**
	 * Send set name field to server for every key in the batch for batch index protocol.
	 * This is only necessary when authentication is enabled and security roles are defined
	 * on a per set basis.
	 * Default: false
	 */
	bool send_set_name;

	/**
	 *	Should raw bytes be deserialized to as_list or as_map. Set to false for backup programs that
	 *	just need access to raw bytes.
	 *	Default: true
	 */
	bool deserialize;

} as_policy_batch;

/**
 *	Administration Policy
 *
 *	@ingroup client_policies
 */
typedef struct as_policy_admin_s {
	
	/**
	 *	Maximum time in milliseconds to wait for
	 *	the operation to complete.
	 */
	uint32_t timeout;
	
} as_policy_admin;

/**
 *	Struct of all policy values and operation policies. 
 *	
 *	This is utilizes by as_config, to define global and default values
 *	for policies.
 *
 *	@ingroup as_config_t
 */
typedef struct as_policies_s {

	/***************************************************************************
	 *	DEFAULT VALUES, IF SPECIFIC POLICY IS UNDEFINED
	 **************************************************************************/

	/**
	 *	Default timeout in milliseconds.
	 *
	 *	Will be used if specific policies have a timeout of 0 (zero).
	 *
	 *	The default value is `AS_POLICY_TIMEOUT_DEFAULT`.
	 */
	uint32_t timeout;

	/**
	 *	Maximum number of retries when a transaction fails due to a network error.
	 *
	 *	The default value is `AS_POLICY_RETRY_DEFAULT`.
	 */
	uint32_t retry;
	
	/**
	 *	Specifies the behavior for the key.
	 *	
	 *	The default value is `AS_POLICY_KEY_DEFAULT`.
	 */
	as_policy_key key;

	/**
	 *	Specifies the behavior for the generation
	 *	value.
	 *	
	 *	The default value is `AS_POLICY_GEN_DEFAULT`.
	 */
	as_policy_gen gen;

	/**
	 *	Specifies the behavior for the existence 
	 *	of the record.
	 *	
	 *	The default value is `AS_POLICY_EXISTS_DEFAULT`.
	 */
	as_policy_exists exists;

	/**
	 *	Specifies which replica to read.
	 *
	 *	The default value is `AS_POLICY_REPLICA_MASTER`.
	 */
	as_policy_replica replica;

	/**
	 *	Specifies the consistency level for reading.
	 *
	 *	The default value is `AS_POLICY_CONSISTENCY_LEVEL_ONE`.
	 */
	as_policy_consistency_level consistency_level;

	/**
	 *	Specifies the commit level for writing.
	 *
	 *	The default value is `AS_POLICY_COMMIT_LEVEL_ALL`.
	 */
	as_policy_commit_level commit_level;

	/***************************************************************************
	 *	SPECIFIC POLICIES
	 **************************************************************************/

	/**
	 *	The default read policy.
	 */
	as_policy_read read;

	/**
	 *	The default write policy.
	 */
	as_policy_write write;

	/**
	 *	The default operate policy.
	 */
	as_policy_operate operate;

	/**
	 *	The default remove policy.
	 */
	as_policy_remove remove;

	/**
	 *	The default apply policy.
	 */
	as_policy_apply apply;

	/**
	 *	The default query policy.
	 */
	as_policy_query query;

	/**
	 *	The default scan policy.
	 */
	as_policy_scan scan;

	/**
	 *	The default info policy.
	 */
	as_policy_info info;

	/**
	 *	The default batch policy.
	 */
	as_policy_batch batch;
	
	/**
	 *	The default administration policy.
	 */
	as_policy_admin admin;

} as_policies;

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Initialize as_policy_read to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_read
 */
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

/**
 *	Copy as_policy_read values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_read
 */
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

/**
 *	Initialize as_policy_write to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_write
 */
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

/**
 *	Copy as_policy_write values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_write
 */
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

/**
 *	Initialize as_policy_operate to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_operate
 */
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

/**
 *	Copy as_policy_operate values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_operate
 */
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

/**
 *	Initialize as_policy_remove to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_remove
 */
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

/**
 *	Copy as_policy_remove values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_remove
 */
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

/**
 *	Initialize as_policy_apply to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_apply
 */
static inline as_policy_apply*
as_policy_apply_init(as_policy_apply* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->key = AS_POLICY_KEY_DEFAULT;
	p->commit_level = AS_POLICY_COMMIT_LEVEL_DEFAULT;
	p->ttl = 0; // AS_RECORD_DEFAULT_TTL
	return p;
}

/**
 *	Copy as_policy_apply values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_apply
 */
static inline void
as_policy_apply_copy(as_policy_apply* src, as_policy_apply* trg)
{
	trg->timeout = src->timeout;
	trg->key = src->key;
	trg->commit_level = src->commit_level;
	trg->ttl = src->ttl;
}

/**
 *	Initialize as_policy_info to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_info
 */
static inline as_policy_info*
as_policy_info_init(as_policy_info* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	p->send_as_is = true;
	p->check_bounds	= true;
	return p;
}

/**
 *	Copy as_policy_info values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_info
 */
static inline void
as_policy_info_copy(as_policy_info* src, as_policy_info* trg)
{
	trg->timeout = src->timeout;
	trg->send_as_is = src->send_as_is;
	trg->check_bounds = src->check_bounds;
}

/**
 *	Initialize as_policy_batch to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_batch
 */
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

/**
 *	Copy as_policy_batch values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_batch
 */
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

/**
 *	Initialize as_policy_admin to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_admin
 */
static inline as_policy_admin*
as_policy_admin_init(as_policy_admin* p)
{
	p->timeout = AS_POLICY_TIMEOUT_DEFAULT;
	return p;
}

/**
 *	Copy as_policy_admin values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_admin
 */
static inline void
as_policy_admin_copy(as_policy_admin* src, as_policy_admin* trg)
{
	trg->timeout = src->timeout;
}

/**
 *	Initialize as_policy_scan to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_scan
 */
static inline as_policy_scan*
as_policy_scan_init(as_policy_scan* p)
{
	p->timeout = 0;
	p->fail_on_cluster_change = false;
	return p;
}

/**
 *	Copy as_policy_scan values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_scan
 */
static inline void
as_policy_scan_copy(as_policy_scan* src, as_policy_scan* trg)
{
	trg->timeout = src->timeout;
	trg->fail_on_cluster_change = src->fail_on_cluster_change;
}

/**
 *	Initialize as_policy_query to default values.
 *
 *	@param p	The policy to initialize.
 *	@return		The initialized policy.
 *
 *	@relates as_policy_query
 */
static inline as_policy_query*
as_policy_query_init(as_policy_query* p)
{
	p->timeout = 0;
	p->deserialize = true;
	return p;
}

/**
 *	Copy as_policy_query values.
 *
 *	@param src	The source policy.
 *	@param trg	The target policy.
 *
 *	@relates as_policy_query
 */
static inline void
as_policy_query_copy(as_policy_query* src, as_policy_query* trg)
{
	trg->timeout = src->timeout;
	trg->deserialize = src->deserialize;
}

/**
 *	Initialize as_policies to undefined values.
 *  as_policies_resolve() will later be called resolve undefined values to global defaults.
 *
 *	@param p	The policies to undefine
 *	@return		The undefined policies.
 *
 *	@relates as_policies
 */
as_policies*
as_policies_init(as_policies* p);

/**
 *	Resolve global policies (like timeout) with operational policies (like as_policy_read).
 *
 *	@param p	The policies to resolve
 *
 *	@relates as_policies
 */
void
as_policies_resolve(as_policies* p);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

#if defined(__APPLE__) || defined(CF_WINDOWS)

#pragma pack(push, 1) // packing is now 1
typedef struct as_proto_s {
	uint64_t	version	:8;
	uint64_t	type	:8;
	uint64_t	sz		:48;
} as_proto;
#pragma pack(pop) // packing is back to what it was

#pragma pack(push, 1) // packing is now 1
typedef struct as_compressed_proto_s {
	as_proto	proto;
	uint64_t	uncompressed_sz;
} as_compressed_proto;
#pragma pack(pop) // packing is back to what it was

#pragma pack(push, 1) // packing is now 1
typedef struct as_msg_s {
/*00*/	uint8_t		header_sz;			// number of uint8_ts in this header
/*01*/	uint8_t		info1;				// bitfield about this request
/*02*/	uint8_t		info2;
/*03*/	uint8_t		info3;
/*04*/	uint8_t		unused;
/*05*/	uint8_t		result_code;
/*06*/	uint32_t	generation;
/*10*/	uint32_t	record_ttl;
/*14*/	uint32_t	transaction_ttl;
/*18*/	uint16_t	n_fields;			// size in uint8_ts
/*20*/	uint16_t	n_ops;				// number of operations
/*22*/	uint8_t		data[0];			// data contains first the fields, then the ops
} as_msg;
#pragma pack(pop) // packing is back to what it was

#pragma pack(push, 1) // packing is now 1
typedef struct as_proto_msg_s {
	as_proto  	proto;
	as_msg		m;
} as_proto_msg;
#pragma pack(pop) // packing is back to what it was

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
/*00*/	uint8_t		header_sz;			// number of uint8_ts in this header
/*01*/	uint8_t		info1;				// bitfield about this request
/*02*/	uint8_t		info2;
/*03*/	uint8_t		info3;
/*04*/	uint8_t		unused;
/*05*/	uint8_t		result_code;
/*06*/	uint32_t	generation;
/*10*/	uint32_t	record_ttl;
/*14*/	uint32_t	transaction_ttl;
/*18*/	uint16_t	n_fields;			// size in uint8_ts
/*20*/	uint16_t	n_ops;				// number of operations
/*22*/	uint8_t		data[];				// data contains first the fields, then the ops
} __attribute__((__packed__)) as_msg;

typedef struct as_proto_msg_s {
	as_proto  	proto;
	as_msg		m;
} __attribute__((__packed__)) as_proto_msg;

#endif

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

void as_proto_swap_to_be(as_proto *m);
void as_proto_swap_from_be(as_proto *m);
void as_msg_swap_header_from_be(as_msg *m);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once
#pragma GCC diagnostic ignored "-Waddress"

#include <aerospike/aerospike_index.h>
#include <aerospike/as_bin.h>
#include <aerospike/as_key.h>
#include <aerospike/as_list.h>
#include <aerospike/as_udf.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Macro for setting setting the STRING_EQUAL predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_string_equals("abc"));
 *	~~~~~~~~~~
 *
 *	@relates as_query
 */
#define as_string_equals(__val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_DEFAULT, AS_INDEX_STRING, __val

/**
 *	Macro for setting setting the INTEGER_EQUAL predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_integer_equals(123));
 *	~~~~~~~~~~
 *
 *	@relates as_query
 */
#define as_integer_equals(__val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_DEFAULT, AS_INDEX_NUMERIC, (int64_t)__val

/**
 *	Macro for setting setting the INTEGER_RANGE predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_integer_range(1,100));
 *	~~~~~~~~~~
 *	
 *	@relates as_query
 *	@ingroup query_object
 */
#define as_integer_range(__min, __max) AS_PREDICATE_RANGE, AS_INDEX_TYPE_DEFAULT, AS_INDEX_NUMERIC, (int64_t)__min, (int64_t)__max

/**
 *	Macro for setting setting the RANGE predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_range(LIST,NUMERIC,1,100));
 *	~~~~~~~~~~
 *	
 *	@relates as_query
 *	@ingroup query_object
 */
#define as_range(indextype, datatype, __min, __max) AS_PREDICATE_RANGE, AS_INDEX_TYPE_ ##indextype, AS_INDEX_ ##datatype, __min, __max

/**
 *	Macro for setting setting the CONTAINS predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_contains(LIST,STRING,"val"));
 *	~~~~~~~~~~
 *	
 *	@relates as_query
 *	@ingroup query_object
 */
#define as_contains(indextype, datatype, __val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_ ##indextype, AS_INDEX_ ##datatype, __val

/**
 *	Macro for setting setting the EQUALS predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_equals(NUMERIC,5));
 *	~~~~~~~~~~
 *	
 *	@relates as_query
 *	@ingroup query_object
 */
#define as_equals(datatype, __val) AS_PREDICATE_EQUAL, AS_INDEX_TYPE_DEFAULT, AS_INDEX_ ##datatype, __val

#define as_geo_within(__val) AS_PREDICATE_RANGE, AS_INDEX_TYPE_DEFAULT, AS_INDEX_GEO2DSPHERE, __val

#define as_geo_contains(__val) AS_PREDICATE_RANGE, AS_INDEX_TYPE_DEFAULT, AS_INDEX_GEO2DSPHERE, __val


/******************************************************************************
 *	TYPES 	
 *****************************************************************************/

/**
 *	Union of supported predicates
 */
typedef union as_predicate_value_u {
	
	/**
	 *	String Value
	 */
	char * string;

	/**
	 *	Integer Value
	 */
	int64_t integer;

	/**
	 *	Integer Range Value
	 */
	struct {

		/**
		 *	Minimum value
		 */
		int64_t min;

		/**
		 *	Maximum value
		 */
		int64_t max;

	} integer_range;

} as_predicate_value;

/**
 *	The types of predicates supported.
 */
typedef enum as_predicate_type_e {

	/**
	 *	String Equality Predicate. 
	 *	Requires as_predicate_value.string to be set.
	 */
	AS_PREDICATE_EQUAL,

	AS_PREDICATE_RANGE
} as_predicate_type;

/**
 *	Defines a predicate, including the bin, type of predicate and the value
 *	for the predicate.
 */
typedef struct as_predicate_s {

	/**
	 *	Bin to apply the predicate to
	 */
	as_bin_name bin;

	/**
	 *	The predicate type, dictates which values to use from the union
	 */
	as_predicate_type type;

	/**
	 *	The value for the predicate.
	 */
	as_predicate_value value;

	/*
	 * The type of data user wants to query
	 */

	as_index_datatype dtype;

	/*
	 * The type of index predicate is on
	 */
	as_index_type itype;
} as_predicate;

/**
 *	Enumerations defining the direction of an ordering.
 */
typedef enum as_order_e {

	/**
	 *	Ascending order
	 */
	AS_ORDER_ASCENDING = 0,

	/**
	 *	bin should be in ascending order
	 */
	AS_ORDER_DESCENDING = 1

} as_order;


/**
 *	Defines the direction a bin should be ordered by.
 */
typedef struct as_ordering_s {

	/**
	 *	Name of the bin to sort by
	 */
	as_bin_name bin;

	/**
	 *	Direction of the sort
	 */
	as_order order;

} as_ordering;

/**
 *	Sequence of bins which should be selected during a query.
 *
 *	Entries can either be initialized on the stack or on the heap.
 *
 *	Initialization should be performed via a query object, using:
 *	-	as_query_select_init()
 *	-	as_query_select_inita()
 */
typedef struct as_query_bins_s {

	/**
	 *	@private
	 *	If true, then as_query_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Number of entries allocated
	 */
	uint16_t capacity;

	/**
	 *	Number of entries used
	 */
	uint16_t size;

	/**
	 *	Sequence of entries
	 */
	as_bin_name * entries;

} as_query_bins;

/**
 *	Sequence of predicates to be applied to a query.
 *
 *	Entries can either be initialized on the stack or on the heap.
 *
 *	Initialization should be performed via a query object, using:
 *	-	as_query_where_init()
 *	-	as_query_where_inita()
 */
typedef struct as_query_predicates_s {

	/**
	 *	@private
	 *	If true, then as_query_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Number of entries allocated
	 */
	uint16_t capacity;

	/**
	 *	Number of entries used
	 */
	uint16_t size;

	/**
	 *	Sequence of entries
	 */
	as_predicate * 	entries;

} as_query_predicates;

/**
 *	Sequence of ordering to be applied to a query results.
 *
 *	Entries can either be initialized on the stack or on the heap.
 *	
 *	Initialization should be performed via a query object, using:
 *	-	as_query_orderby_init()
 *	-	as_query_orderby_inita()
 */
typedef struct as_query_sort_s {

	/**
	 *	@private
	 *	If true, then as_query_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Number of entries allocated
	 */
	uint16_t capacity;

	/**
	 *	Number of entries used
	 */
	uint16_t size;

	/**
	 *	Sequence of entries
	 */
	as_ordering * entries;

} as_query_ordering;


/** 
 *	The as_query object is used define a query to be executed in the datasbase.
 *
 *	## Initialization
 *	
 *	Before using an as_query, it must be initialized via either: 
 *	- as_query_init()
 *	- as_query_new()
 *	
 *	as_query_init() should be used on a stack allocated as_query. It will
 *	initialize the as_query with the given namespace and set. On success,
 *	it will return a pointer to the initialized as_query. Otherwise, NULL 
 *	is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_query query;
 *	as_query_init(&query, "namespace", "set");
 *	~~~~~~~~~~
 *
 *	as_query_new() should be used to allocate and initialize a heap allocated
 *	as_query. It will allocate the as_query, then initialized it with the 
 *	given namespace and set. On success, it will return a pointer to the 
 *	initialized as_query. Otherwise, NULL is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_query * query = as_query_new("namespace", "set");
 *	~~~~~~~~~~
 *
 *	## Destruction
 *
 *	When you are finished with the as_query, you can destroy it and associated
 *	resources:
 *
 *	~~~~~~~~~~{.c}
 *	as_query_destroy(query);
 *	~~~~~~~~~~
 *
 *	## Usage
 *
 *	The following explains how to use an as_query to build a query.
 *
 *	### Selecting Bins
 *
 *	as_query_select() is used to specify the bins to be selected by the query.
 *	
 *	~~~~~~~~~~{.c}
 *	as_query_select(query, "bin1");
 *	as_query_select(query, "bin2");
 *	~~~~~~~~~~
 *
 *	Before adding bins to select, the select structure must be initialized via
 *	either:
 *	- as_query_select_inita() - Initializes the structure on the stack.
 *	- as_query_select_init() - Initializes the structure on the heap.
 *
 *	Both functions are given the number of bins to be selected.
 *
 *	A complete example using as_query_select_inita()
 *
 *	~~~~~~~~~~{.c}
 *	as_query_select_inita(query, 2);
 *	as_query_select(query, "bin1");
 *	as_query_select(query, "bin2");
 *	~~~~~~~~~~
 *
 *
 *	### Predicates on Bins
 *
 *	as_query_where() is used to specify predicates to be added to the the query.
 *
 *	**Note:** Currently, a single where predicate is supported. To do more advanced filtering,
 *	you will want to use a UDF to process the result set on the server.
 *	
 *	~~~~~~~~~~{.c}
 *	as_query_where(query, "bin1", as_string_equals("abc"));
 *	~~~~~~~~~~
 *
 *	The predicates that you can apply to a bin include:
 *	- as_string_equals() - Test for string equality.
 *	- as_integer_equals() - Test for integer equality.
 *	- as_integer_range() - Test for integer within a range.
 *
 *	Before adding predicates, the where structure must be initialized. To
 *	initialize the where structure, you can choose to use one of the following:
 *	- as_query_where_inita() - Initializes the structure on the stack.
 *	- as_query_where_init() - Initializes the structure on the heap.
 *	
 *	Both functions are given the number of predicates to be added.
 *
 *	A complete example using as_query_where_inita():
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where_inita(query, 1);
 *	as_query_where(query, "bin1", as_string_equals("abc"));
 *	~~~~~~~~~~
 *
 *
 *	### Sorting Results
 *
 *	as_query_orderby() is used to specify ordering of results of a query.
 *	
 *	~~~~~~~~~~{.c}
 *	as_query_orderby(query, "bin1", AS_ORDER_ASCENDING);
 *	~~~~~~~~~~
 *
 *	The sort order can be:
 *	- `AS_ORDER_ASCENDING`
 *	- `AS_ORDER_DESCENDING`
 *
 *	Before adding ordering, the orderby structure must be initialized via 
 *	either:
 *	- as_query_orderby_inita() - Initializes the structure on the stack.
 *	- as_query_orderby_init() - Initializes the structure on the heap.
 *	
 *	Both functions are given the number of orderings to be added.
 *
 *	A complete example using as_query_orderby_inita():
 *	
 *	~~~~~~~~~~{.c}
 *	as_query_orderby_inita(query, 2);
 *	as_query_orderby(query, "bin1", AS_ORDER_ASCENDING);
 *	as_query_orderby(query, "bin2", AS_ORDER_ASCENDING);
 *	~~~~~~~~~~
 *
 *	### Applying a UDF to Query Results
 *
 *	A UDF can be applied to the results of a query.
 *
 *	To define the UDF for the query, use as_query_apply().
 *
 *	~~~~~~~~~~{.c}
 *	as_query_apply(query, "udf_module", "udf_function", arglist);
 *	~~~~~~~~~~
 *
 *	@ingroup client_objects
 */
typedef struct as_query_s {

	/**
	 *	@private
	 *	If true, then as_query_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Namespace to be queried.
	 *
	 *	Should be initialized via either:
	 *	-	as_query_init() -	To initialize a stack allocated query.
	 *	-	as_query_new() -	To heap allocate and initialize a query.
	 */
	as_namespace ns;

	/**
	 *	Set to be queried.
	 *
	 *	Should be initialized via either:
	 *	-	as_query_init() -	To initialize a stack allocated query.
	 *	-	as_query_new() -	To heap allocate and initialize a query.
	 */
	as_set set;

	/**
	 *	Name of bins to select.
	 *	
	 *	Use either of the following function to initialize:
	 *	-	as_query_select_init() -	To initialize on the heap.
	 *	-	as_query_select_inita() -	To initialize on the stack.
	 *
	 *	Use as_query_select() to populate.
	 */
	as_query_bins select;

	/**
	 *	Predicates for filtering.
	 *	
	 *	Use either of the following function to initialize:
	 *	-	as_query_where_init() -		To initialize on the heap.
	 *	-	as_query_where_inita() -	To initialize on the stack.
	 *
	 *	Use as_query_where() to populate.
	 */
	as_query_predicates where;

	/**
	 *	Bins to order by.
	 *	
	 *	Use either of the following function to initialize:
	 *	-	as_query_orderby_init() -	To initialize on the heap.
	 *	-	as_query_orderby_inita() -	To initialize on the stack.
	 *
	 *	Use as_query_orderby() to populate.
	 */
	as_query_ordering orderby;

	/**
	 *	UDF to apply to results of the query
	 *
	 *	Should be set via `as_query_apply()`.
	 */
	as_udf_call apply;

} as_query;

/******************************************************************************
 *	INSTANCE FUNCTIONS
 *****************************************************************************/

/**
 *	Initialize a stack allocated as_query.
 *
 *	~~~~~~~~~~{.c}
 *	as_query query;
 *	as_query_init(&query, "test", "demo");
 *	~~~~~~~~~~
 *
 *	@param query 	The query to initialize.
 *	@param ns 		The namespace to query.
 *	@param set 		The set to query.
 *
 *	@return On success, the initialized query. Otherwise NULL.
 *
 *	@relates as_query
 */
as_query * as_query_init(as_query * query, const as_namespace ns, const as_set set);

/**
 *	Create and initialize a new heap allocated as_query.
 *
 *	~~~~~~~~~~{.c}
 *	as_query * query = as_query_new("test", "demo");
 *	~~~~~~~~~~
 *	
 *	@param ns 		The namespace to query.
 *	@param set 		The set to query.
 *
 *	@return On success, the new query. Otherwise NULL.
 *
 *	@relates as_query
 *	@ingroup query_object
 */
as_query * as_query_new(const as_namespace ns, const as_set set);

/**
 *	Destroy the query and associated resources.
 *	
 *	~~~~~~~~~~{.c}
 *	as_query_destroy(scan);
 *	~~~~~~~~~~
 *
 *	@param query 	The query to destroy.
 *
 *	@relates as_query
 */
void as_query_destroy(as_query * query);

/******************************************************************************
 *	SELECT FUNCTIONS
 *****************************************************************************/

/** 
 *	Initializes `as_query.select` with a capacity of `n` using `alloca`
 *
 *	For heap allocation, use `as_query_select_init()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_select_inita(&query, 2);
 *	as_query_select(&query, "bin1");
 *	as_query_select(&query, "bin2");
 *	~~~~~~~~~~
 *	
 *	@param __query	The query to initialize.
 *	@param __n		The number of bins to allocate.
 *
 *	@relates as_query
 *	@ingroup query_object
 */
#define as_query_select_inita(__query, __n) \
	if ( (__query) != NULL && (__query)->select.entries == NULL ) {\
		(__query)->select.entries = (as_bin_name *) alloca(__n * sizeof(as_bin_name));\
		if ( (__query)->select.entries ) { \
			(__query)->select._free = false;\
			(__query)->select.capacity = __n;\
			(__query)->select.size = 0;\
		}\
 	}

/** 
 *	Initializes `as_query.select` with a capacity of `n` using `malloc()`.
 *	
 *	For stack allocation, use `as_query_select_inita()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_select_init(&query, 2);
 *	as_query_select(&query, "bin1");
 *	as_query_select(&query, "bin2");
 *	~~~~~~~~~~
 *
 *	@param query	The query to initialize.
 *	@param n		The number of bins to allocate.
 *
 *	@return On success, the initialized. Otherwise an error occurred.
 *
 *	@relates as_query
 *	@ingroup query_object
 */
bool as_query_select_init(as_query * query, uint16_t n);

/**
 *	Select bins to be projected from matching records.
 *
 *	You have to ensure as_query.select has sufficient capacity, prior to 
 *	adding a bin. If capacity is sufficient then false is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_select_init(&query, 2);
 *	as_query_select(&query, "bin1");
 *	as_query_select(&query, "bin2");
 *	~~~~~~~~~~
 *
 *	@param query 		The query to modify.
 *	@param bin 			The name of the bin to select.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 *	@ingroup query_object
 */
bool as_query_select(as_query * query, const char * bin);

/******************************************************************************
 *	WHERE FUNCTIONS
 *****************************************************************************/

/** 
 *	Initializes `as_query.where` with a capacity of `n` using `alloca()`.
 *
 *	For heap allocation, use `as_query_where_init()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where_inita(&query, 3);
 *	as_query_where(&query, "bin1", as_string_equals("abc"));
 *	as_query_where(&query, "bin2", as_integer_equals(123));
 *	as_query_where(&query, "bin3", as_integer_range(0,123));
 *	~~~~~~~~~~
 *
 *	@param __query	The query to initialize.
 *	@param __n		The number of as_predicate to allocate.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
#define as_query_where_inita(__query, __n) \
	if ( (__query)  != NULL && (__query)->where.entries == NULL ) {\
		(__query)->where.entries = (as_predicate *) alloca(__n * sizeof(as_predicate));\
		if ( (__query)->where.entries ) { \
			(__query)->where._free = false;\
			(__query)->where.capacity = __n;\
			(__query)->where.size = 0;\
		}\
 	}

/** 
 *	Initializes `as_query.where` with a capacity of `n` using `malloc()`.
 *
 *	For stack allocation, use `as_query_where_inita()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where_init(&query, 3);
 *	as_query_where(&query, "bin1", as_string_equals("abc"));
 *	as_query_where(&query, "bin1", as_integer_equals(123));
 *	as_query_where(&query, "bin1", as_integer_range(0,123));
 *	~~~~~~~~~~
 *
 *	@param query	The query to initialize.
 *	@param n		The number of as_predicate to allocate.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
bool as_query_where_init(as_query * query, uint16_t n);

/**
 *	Add a predicate to the query.
 *
 *	You have to ensure as_query.where has sufficient capacity, prior to
 *	adding a predicate. If capacity is insufficient then false is returned.
 *
 *	String predicates are not owned by as_query.  If the string is allocated
 *	on the heap, the caller is responsible for freeing the string after the query
 *	has been executed.  as_query_destroy() will not free this string predicate.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_where_init(&query, 3);
 *	as_query_where(&query, "bin1", as_string_equals("abc"));
 *	as_query_where(&query, "bin1", as_integer_equals(123));
 *	as_query_where(&query, "bin1", as_integer_range(0,123));
 *	~~~~~~~~~~
 *
 *	@param query		The query add the predicate to.
 *	@param bin			The name of the bin the predicate will apply to.
 *	@param type			The type of predicate.
 *	@param itype		The type of index.
 *	@param dtype		The underlying data type that the index is based on.
 *	@param ... 			The values for the predicate.
 *	
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
bool as_query_where(as_query * query, const char * bin, as_predicate_type type, as_index_type itype, as_index_datatype dtype, ... );

/******************************************************************************
 *	ORDERBY FUNCTIONS
 *****************************************************************************/

/** 
 *	Initializes `as_query.where` with a capacity of `n` using `alloca()`.
 *
 *	For heap allocation, use `as_query_where_init()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_orderby_inita(&query, 1);
 *	as_query_orderby(&query, "bin1", AS_ORDER_ASCENDING);
 *	~~~~~~~~~~
 *
 *	@param __query	The query to initialize.
 *	@param __n		The number of as_orders to allocate.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
#define as_query_orderby_inita(__query, __n) \
	if ( (__query) != NULL && (__query)->orderby.entries == NULL  ) {\
		(__query)->orderby.entries = (as_ordering *) alloca(__n * sizeof(as_ordering));\
		if ( (__query)->orderby.entries ) { \
			(__query)->orderby._free = false;\
			(__query)->orderby.capacity = __n;\
			(__query)->orderby.size = 0;\
		}\
 	}

/** 
 *	Initializes `as_query.orderby` with a capacity of `n` using `malloc()`.
 *	
 *	For stack allocation, use `as_query_orderby_inita()`.
 *	
 *	~~~~~~~~~~{.c}
 *	as_query_orderby_init(&query, 1);
 *	as_query_orderby(&query, "bin1", AS_ORDER_ASCENDING);
 *	~~~~~~~~~~
 *
 *	@param query	The query to initialize.
 *	@param n		The number of as_orders to allocate.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
bool as_query_orderby_init(as_query * query, uint16_t n);

/**
 *	Add a bin to sort by to the query.
 *	
 *	You have to ensure as_query.orderby has sufficient capacity, prior to 
 *	adding an ordering. If capacity is insufficient then false is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_orderby_init(&query, 1);
 *	as_query_orderby(&query, "bin1", AS_ORDER_ASCENDING);
 *	~~~~~~~~~~
 *
 *	@param query	The query to modify.
 *	@param bin		The name of the bin to sort by.
 *	@param order	The sort order: `AS_ORDER_ASCENDING` or `AS_ORDER_DESCENDING`.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
bool as_query_orderby(as_query * query, const char * bin, as_order order);

/******************************************************************************
 *	QUERY MODIFIER FUNCTIONS
 *****************************************************************************/

/**
 *	Apply a function to the results of the query.
 *
 *	~~~~~~~~~~{.c}
 *	as_query_apply(&query, "my_module", "my_function", NULL);
 *	~~~~~~~~~~
 *
 *	@param query		The query to apply the function to.
 *	@param module		The module containing the function to invoke.
 *	@param function		The function in the module to invoke.
 *	@param arglist		The arguments to use when calling the function.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_query
 */
bool as_query_apply(as_query * query, const char * module, const char * function, const as_list * arglist);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_bin.h>
#include <aerospike/as_bytes.h>
#include <aerospike/as_integer.h>
#include <aerospike/as_key.h>
#include <aerospike/as_list.h>
#include <aerospike/as_map.h>
#include <aerospike/as_rec.h>
#include <aerospike/as_string.h>
#include <aerospike/as_geojson.h>
#include <aerospike/as_util.h>
#include <aerospike/as_val.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Records in Aerospike are collections of named bins. 
 *
 *	The bins in a record are analogous to columns in relational databases. 
 *	However, unlike columns, the bins themselves are not typed. Instead, bins 
 *	contain values which are typed. So, it is possible to have multiple records 
 *	with bins of the same name but different types for values.
 *
 *	The bin's value can only be of the types defined in `as_bin_value`.
 *	
 *	## Initialization
 *	
 *	There are several ways to initialize an `as_record`. 
 *
 *	You can create the `as_record` on the stack:
 *	
 *	~~~~~~~~~~{.c}
 *	as_record rec;
 *	~~~~~~~~~~
 *	
 *	Then initialize it using either the `as_record_init()` function or 
 *	`as_record_inita()` macro.
 *
 *	The `as_record_init()` function will initialize the variable, then 
 *	allocate the specified number of bins using `malloc()`. The following
 *	initializes `rec` with 2 bins.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_init(&rec, 2);
 *	~~~~~~~~~~
 *
 *	The `as_record_inita()` macro will initialize the variable, then allocate
 *	the specified number of bins using `alloca()`. The following initializes 
 *	`rec` with 2 bins.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_inita(&rec, 2);
 *	~~~~~~~~~~
 *	
 *	The `as_record_new()` function will allocate an `as_record` on the heap
 *	using `malloc()` then allocate the specified number of bins using 
 *	`malloc()`. The following creates a new `as_record` with 2 bins.
 *
 *	~~~~~~~~~~{.c}
 *	as_record * rec = as_record_new(2);
 *	~~~~~~~~~~
 *	
 *	## Destruction
 *
 *	When you no longer require an as_record, you should call `as_record_destroy()`
 *	to release the record and associated resources.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_destroy(rec);
 *	~~~~~~~~~~
 *
 *	If the record has been ref-counted, then the ref-count will be decremented, 
 *	until it reaches 0 (zero), at which point, the record will be released.
 *
 *	## Setting Bin Values
 *
 *	The following are functions for setting values in bins of a record. Utilize 
 *	the appropriate setter for the data you want to store in a bin.
 *
 *   Function                    |  Description
 *	---------------------------- | ----------------------------------------------
 *	 `as_record_set_int64()`     | Set the bin value to a 64-bit integer.
 *	 `as_record_set_str()`       | Set the bin value to a NULL-terminated string.
 *	 `as_record_set_integer()`   | Set the bin value to an `as_integer`.
 *	 `as_record_set_double()`    | Set the bin value to an `as_double`.
 *	 `as_record_set_string()`    | Set the bin value to an `as_string`.
 *	 `as_record_set_geojson()`   | Set the bin value to an `as_geojson`.
 *	 `as_record_set_bytes()`     | Set the bin value to an `as_bytes`.
 *	 `as_record_set_list()`      | Set the bin value to an `as_list`.                    
 *	 `as_record_set_map()`       | Set the bin value to an `as_map`.
 *	 `as_record_set_nil()`       | Set the bin value to an `as_nil`.
 *	 `as_record_set()`           | Set the bin value to an `as_bin_value`.
 *
 *	## Getting Bin Values
 *
 *	The following are functions for getting values from bins of a record. 
 *	Utilize the appropriate getter for the data you want to read from a bin.
 *	
 *
 *   Function                    |  Description
 *	---------------------------- | ----------------------------------------------
 *	 `as_record_get_int64()`     | Get the bin as a 64-bit integer.
 *	 `as_record_get_str()`       | Get the bin as a NULL-terminated string.
 *	 `as_record_get_integer()`   | Get the bin as an `as_integer`.
 *	 `as_record_get_double()`    | Get the bin as an `as_double`.
 *	 `as_record_get_string()`    | Get the bin as an `as_string`.
 *	 `as_record_get_geojson()`   | Get the bin as an `as_geojson`.
 *	 `as_record_get_bytes()`     | Get the bin as an `as_bytes`.
 *	 `as_record_get_list()`      | Get the bin as an `as_list`. 
 *	 `as_record_get_map()`       | Get the bin as an `as_map`.
 *	 `as_record_get()`           | Get the bin as an `as_bin_value`.
 *
 *	If you are unsure of the type of data stored in the bin, then you should 
 *	use `as_record_get()`. You can then check the type of the value using
 *	`as_val_type()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_val * value = as_record_get(rec, "bin1");
 *	switch ( as_val_type(value) ) {
 *		case AS_NIL: break;
 *		case AS_INTEGER: break;
 *		case AS_DOUBLE: break;
 *		case AS_STRING: break;
 *		case AS_GEOJSON: break;
 *		case AS_BYTES: break;
 *		case AS_LIST: break;
 *		case AS_MAP: break;
 *		case AS_REC: break;
 *		case AS_UNDEF: break;
 *	}
 *	~~~~~~~~~~
 *
 *	## Traversing Bins
 *
 *	If you want to traverse the bins of a record, then you have two options:
 *	
 *	- as_record_foreach() — Calls a function for each bin traversed.
 *	- as_record_iterator — Uses an iterator pattern to traverse bins.
 *
 *	@extends as_rec
 *	@ingroup client_objects
 */
typedef struct as_record_s {

	/**
	 *	@private
	 *	as_record is "derived" from as_rec.
	 *	So you can actually type cast as_record to as_rec.
	 */
	as_rec _;

	/**
	 *	The key of the record.
	 *	This is populated when a record is read from the database.
	 *	This should not be set by the user.
	 */
	as_key key;

	/**
	 *	The generation of the record.
	 */
	uint16_t gen;

	/**
	 *	The time-to-live (expiration) of the record in seconds.
	 *	There are two special values that can be set in the record TTL:
	 *	(*) ZERO (defined as AS_RECORD_DEFAULT_TTL), which means that the
	 *	    record will adopt the default TTL value from the namespace.
	 *	(*) 0xFFFFFFFF (also, -1 in a signed 32 bit int)
	 *	    (defined as AS_RECORD_NO_EXPIRE_TTL), which means that the record
	 *	    will get an internal "void_time" of zero, and thus will never expire.
	 *
	 *	Note that the TTL value will be employed ONLY on write/update calls.
	 */
	uint32_t ttl;

	/**
	 *	The bins of the record.
	 */
	as_bins bins;

} as_record;

/**
 * When the record is given a TTL value of ZERO, it will adopt the TTL value
 * that is the default TTL value for the namespace (defined in the config file).
 */
#define AS_RECORD_DEFAULT_TTL 0

/**
 * When the record is given a TTL value of 0xFFFFFFFF, it will set the internal
 * void_time value (the absolute clock time value that shows when a record
 * will expire) to zero, which means the record will never expire
 */
#define AS_RECORD_NO_EXPIRE_TTL 0xFFFFFFFF

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 * Initialize a stack allocated `as_record` then allocate `__nbins` capacity 
 * for as_record.bins on the stack.
 *
 *	~~~~~~~~~~{.c}
 *	as_record record;
 *	as_record_inita(&record, 2);
 *	as_record_set_int64(&record, "bin1", 123);
 *	as_record_set_int64(&record, "bin2", 456);
 *	~~~~~~~~~~
 *
 *	When you are finished using the `as_record` instance, you should release the 
 *	resources allocated to it by calling `as_record_destroy()`.
 *
 *	@param __rec		The `as_record *` to initialize.
 *	@param __nbins		The number of `as_record.bins.entries` to allocate on the 
 *						stack.
 *	
 *	@relates as_record
 */
#define as_record_inita(__rec, __nbins) \
	as_record_init(__rec, 0);\
	(__rec)->bins._free = false;\
	(__rec)->bins.capacity = __nbins;\
	(__rec)->bins.size = 0;\
	(__rec)->bins.entries = (as_bin *) alloca(sizeof(as_bin) * __nbins);

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Create a new as_record on the heap.
 *
 *	~~~~~~~~~~{.c}
 *	as_record * r = as_record_new(2);
 *	as_record_set_int64(r, "bin1", 123);
 *	as_record_set_str(r, "bin1", "abc");
 *	~~~~~~~~~~
 *
 *	When you are finished using the `as_record` instance, you should release the 
 *	resources allocated to it by calling `as_record_destroy()`.
 *
 *	@param nbins 	The number of bins to initialize. Set to 0, if unknown.
 *
 *	@return a pointer to the new as_record if successful, otherwise NULL.
 *
 *	@relates as_record
 */
as_record * as_record_new(uint16_t nbins);

/**
 *	Initializes an as_record created on the stack.
 *
 *	~~~~~~~~~~{.c}
 *	as_record r;
 *	as_record_init(&r, 2);
 *	as_record_set_int64(&r, "bin1", 123);
 *	as_record_set_str(&r, "bin1", "abc");
 *	~~~~~~~~~~
 *
 *	When you are finished using the `as_record` instance, you should release the 
 *	resources allocated to it by calling `as_record_destroy()`.
 *
 *	@param rec		The record to initialize.
 *	@param nbins	The number of bins to initialize. Set to 0, if unknown.
 *
 *	@return a pointer to the initialized as_record if successful, otherwise NULL.
 *
 *	@relates as_record
 */
as_record * as_record_init(as_record * rec, uint16_t nbins);

/**
 *	Destroy the as_record and associated resources.
 *
 *	@param rec The record to destroy.
 *
 *	@relates as_record
 */
void as_record_destroy(as_record * rec);

/**
 *	Get the number of bins in the record.
 *
 *	@return the number of bins in the record.
 *
 *	@relates as_record
 */
uint16_t as_record_numbins(const as_record * rec);

/**
 *	Set specified bin's value to an as_bin_value.
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set(as_record * rec, const as_bin_name name, as_bin_value * value);

/**
 *	Set specified bin's value to an int64_t.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_int64(rec, "bin", 123);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_int64(as_record * rec, const as_bin_name name, int64_t value);

/**
 *	Set specified bin's value to a double.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_double(rec, "bin", 123.456);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_double(as_record * rec, const as_bin_name name, double value);

/**
 *	Set specified bin's value to an NULL terminated string.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_strp(rec, "bin", strdup("abc"), true);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *	@param free		If true, then the value will be freed when the record is destroyed.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_strp(as_record * rec, const as_bin_name name, const char * value, bool free);

/**
 *	Set specified bin's value to an NULL terminated string.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_str(rec, "bin", "abc");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin. Must last for the lifetime of the record.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
static inline bool as_record_set_str(as_record * rec, const as_bin_name name, const char * value)
{
	return as_record_set_strp(rec, name, value, false);
}

/**
 *	Set specified bin's value to an NULL terminated GeoJSON string.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_geojson_strp(rec, "bin", strdup("abc"), true);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *	@param free		If true, then the value will be freed when the record is destroyed.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_geojson_strp(as_record * rec, const as_bin_name name, const char * value, bool free);

/**
 *	Set specified bin's value to an NULL terminated GeoJSON string.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_geojson_str(rec, "bin", "abc");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin. Must last for the lifetime of the record.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
static inline bool as_record_set_geojson_str(as_record * rec, const as_bin_name name, const char * value)
{
	return as_record_set_geojson_strp(rec, name, value, false);
}

/**
 *	Set specified bin's value to an NULL terminated string.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t * bytes = (uint8_t *) malloc(3);
 *	bytes[0] = 1;
 *	bytes[1] = 2;
 *	bytes[3] = 3;
 *
 *	as_record_set_raw(rec, "bin", bytes, 3, true);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *	@param size		The size of the value.
 *	@param free		If true, then the value will be freed when the record is destroyed.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_rawp(as_record * rec, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Set specified bin's value to an as_bytes value of a specified type.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t * bytes = (uint8_t *) malloc(3);
 *	bytes[0] = 1;
 *	bytes[1] = 2;
 *	bytes[3] = 3;
 *
 *	as_record_set_raw(rec, "bin", bytes, 3, true);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *	@param size		The size of the value.
 *	@param type 	The as_bytes_type designation (AS_BYTES_*)
 *	@param free		If true, then the value will be freed when the record is destroyed.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_raw_typep(as_record * rec, const as_bin_name name, const uint8_t * value, uint32_t size, as_bytes_type type, bool free);

/**
 *	Set specified bin's value to an NULL terminated string.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t bytes[3] = {1,2,3};
 *	as_record_set_raw(rec, "bin", bytes, 3);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *	@param size		The size of the value. Must last for the lifetime of the record.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
static inline bool as_record_set_raw(as_record * rec, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_record_set_rawp(rec, name, value, size, false);
}

/**
 *	Set specified bin's value to an as_integer.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_integer(rec, "bin", as_integer_new(123));
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_integer(as_record * rec, const as_bin_name name, as_integer * value);

/**
 *	Set specified bin's value to an as_double.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_as_double(rec, "bin", as_double_new(123.456));
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_as_double(as_record * rec, const as_bin_name name, as_double * value);

/**
 *	Set specified bin's value to an as_string.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_string(rec, "bin", as_string_new("abc", false));
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_string(as_record * rec, const as_bin_name name, as_string * value);

/**
 *	Set specified bin's value to an as_geojson.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_geojson(rec, "bin", as_geojson_new("abc", false));
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_geojson(as_record * rec, const as_bin_name name, as_geojson * value);

/**
 *	Set specified bin's value to an as_bytes.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_integer(rec, "bin", bytes);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_bytes(as_record * rec, const as_bin_name name, as_bytes * value);

/**
 *	Set specified bin's value to an as_list.
 *
 *	~~~~~~~~~~{.c}
 *	as_arraylist list;
 *	as_arraylist_init(&list);
 *	as_arraylist_add_int64(&list, 1);
 *	as_arraylist_add_int64(&list, 2);
 *	as_arraylist_add_int64(&list, 3);
 *
 *	as_record_set_list(rec, "bin", &list);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_list(as_record * rec, const as_bin_name name, as_list * value);

/**
 *	Set specified bin's value to an as_map.
 *
 *	~~~~~~~~~~{.c}
 *	as_hashmap map;
 *	as_hashmap_init(&map, 32);
 *	as_stringmap_set_int64(&map, "a", 1);
 *	as_stringmap_set_int64(&map, "b", 2);
 *	as_stringmap_set_int64(&map, "c", 3);
 *
 *	as_record_set_map(rec, "bin", &map);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param value	The value of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_map(as_record * rec, const as_bin_name name, as_map * value);

/**
 *	Set specified bin's value to as_nil.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_set_nil(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return true on success, false on failure.
 *
 *	@relates as_record
 */
bool as_record_set_nil(as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value.
 *
 *	~~~~~~~~~~{.c}
 *	as_val * value = as_record_get(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_bin_value * as_record_get(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an int64_t.
 *
 *	~~~~~~~~~~{.c}
 *	int64_t value = as_record_get_int64(rec, "bin", INT64_MAX);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param fallback	The default value to use, if the bin doesn't exist or is not an integer.
 *
 *	@return the value if it exists, otherwise 0.
 *
 *	@relates as_record
 */
int64_t as_record_get_int64(const as_record * rec, const as_bin_name name, int64_t fallback);

/**
 *	Get specified bin's value as a double.
 *
 *	~~~~~~~~~~{.c}
 *	double value = as_record_get_double(rec, "bin", -1.0);
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *	@param fallback	The default value to use, if the bin doesn't exist or is not an integer.
 *
 *	@return the value if it exists, otherwise 0.
 *
 *	@relates as_record
 */
double as_record_get_double(const as_record * rec, const as_bin_name name, double fallback);

/**
 *	Get specified bin's value as an NULL terminated string.
 *
 *	~~~~~~~~~~{.c}
 *	char * value = as_record_get_str(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
char * as_record_get_str(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an NULL terminated GeoJSON string.
 *
 *	~~~~~~~~~~{.c}
 *	char * value = as_record_get_geojson_str(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
char * as_record_get_geojson_str(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_integer.
 *
 *	~~~~~~~~~~{.c}
 *	as_integer * value = as_record_get_integer(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_integer * as_record_get_integer(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_double.
 *
 *	~~~~~~~~~~{.c}
 *	as_double * value = as_record_get_as_double(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_double * as_record_get_as_double(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_string.
 *
 *	~~~~~~~~~~{.c}
 *	as_string * value = as_record_get_string(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_string * as_record_get_string(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_geojson.
 *
 *	~~~~~~~~~~{.c}
 *	as_geojson * value = as_record_get_geojson(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_geojson * as_record_get_geojson(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_bytes.
 *
 *	~~~~~~~~~~{.c}
 *	as_bytes * value = as_record_get_bytes(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_bytes * as_record_get_bytes(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_list.
 *
 *	~~~~~~~~~~{.c}
 *	as_list * value = as_record_get_list(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_list * as_record_get_list(const as_record * rec, const as_bin_name name);

/**
 *	Get specified bin's value as an as_map.
 *
 *	~~~~~~~~~~{.c}
 *	as_map * value = as_record_get_map(rec, "bin");
 *	~~~~~~~~~~
 *
 *	@param rec		The record containing the bin.
 *	@param name		The name of the bin.
 *
 *	@return the value if it exists, otherwise NULL.
 *
 *	@relates as_record
 */
as_map * as_record_get_map(const as_record * rec, const as_bin_name name);

/******************************************************************************
 *	ITERATION FUNCTIONS
 ******************************************************************************/

/**
 *	Iterate over each bin in the record and invoke the callback function.
 *	
 *	~~~~~~~~~~{.c}
 *	bool print_bin(const char * name, const as_val * value, void * udata) {
 *		char * sval = as_val_tostring(value);
 *		printf("bin: name=%s, value=%s\n", name, sval);
 *		free(sval);
 *		return true;
 *	}
 *
 *	as_record_foreach(rec, print_bin, NULL);
 *	~~~~~~~~~~
 *
 *	If the callback returns true, then iteration will continue to the next bin.
 *	Otherwise, the iteration will halt and `as_record_foreach()` will return
 *	false.
 *
 *	@param rec		The record containing the bins to iterate over.
 *	@param callback	The callback to invoke for each bin.
 *	@param udata	User-data provided for the callback.
 *
 *	@return true if iteration completes fully. false if iteration was aborted.
 *
 *	@relates as_record
 */
bool as_record_foreach(const as_record * rec, as_rec_foreach_callback callback, void * udata);

/******************************************************************************
 *	CONVERSION FUNCTIONS
 ******************************************************************************/

/**
 *	Convert to an as_val.
 *
 *	@relates as_record
 */
static inline as_val * as_record_toval(const as_record * rec) 
{
	return (as_val *) rec;
}

/**
 *	Convert from an as_val.
 *
 *	@relates as_record
 */
static inline as_record * as_record_fromval(const as_val * v) 
{
	return (as_record *) as_util_fromval(v, AS_REC, as_rec);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_bin.h>
#include <aerospike/as_bytes.h>
#include <aerospike/as_integer.h>
#include <aerospike/as_key.h>
#include <aerospike/as_list.h>
#include <aerospike/as_map.h>
#include <aerospike/as_rec.h>
#include <aerospike/as_record.h>
#include <aerospike/as_string.h>
#include <aerospike/as_util.h>
#include <aerospike/as_val.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Iterator over bins of a record.
 *
 *	## Initialization
 *
 *	The as_record_iterator can be initialized via:
 *
 *	- as_record_iterator_init() — initializes a stack allocated 
 *		as_record_iterator.
 *	- as_record_iterator_new() — allocated and initializes an 
 *		as_record_iterator on the heap.
 *
 *	Both of the function require the record on which it will iterate.
 *
 *	To initialize an as_record_iterator on the stack:
 *
 *	~~~~~~~~~~{.c}
 *	as_record_iterator it;
 *	as_record_iterator_init(&it, record);
 *	~~~~~~~~~~
 *
 *	To initialize an as_record_iterator on the heap:
 *
 *	~~~~~~~~~~{.c}
 *	as_record_iterator * it as_record_iterator_new(record);
 *	~~~~~~~~~~
 *
 *	## Destruction 
 *
 *	When you no longer require the iterator, you should release it and 
 *	associated resource via as_record_iterator_destroy():
 *
 *	~~~~~~~~~~{.c}
 *	as_record_iterator_destroy(it);
 *	~~~~~~~~~~
 *
 *	## Usage
 *
 *	With an initialized as_record_iterator, you can traverse the bins of
 *	a record. 
 *
 *	Traversal is usually performed by first checking to see if 
 *	the there are any bins available to traverse to via 
 *	as_record_iterator_has_next(), which returns true if there are more bins,
 *	or false if there are no more bins. 
 *
 *	~~~~~~~~~~{.c}
 *	as_record_iterator_has_next(&it);
 *	~~~~~~~~~~
 *
 *	When you are sure there are more bins, then you will use 
 *	as_record_iterator_next() to read the next bin. If there are no bins 
 *	available, then NULL is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_bin * bin = as_record_iterator_next(&it);
 *	~~~~~~~~~~
 *
 *	If  as_record_iterator_next() returns a bin, then you can use the following
 *	functions to get information about the bin:
 *
 *	- as_bin_get_name() — Get the bin's name.
 *	- as_bin_get_value() — Get the bin's value.
 *	- as_bin_get_type() — Get the bin's values' types.
 *
 *	Most often, a traversal is performed in a while loop. The following is a 
 *	simple example:
 *
 *	~~~~~~~~~~{.c}
 *	while ( as_record_iterator_has_next(&it) ) {
 *		as_bin * bin = as_record_iterator_next(&it);
 *		char *   name = as_bin_get_name(bin);
 *		as_val * value = (as_val *) as_bin_get_value(bin);
 *	}
 *	~~~~~~~~~~
 *
 *	@ingroup as_record_object
 */
typedef struct as_record_iterator_s {

	/**
	 *	@private
	 *	If true, then as_record_iterator_destroy() will free this object.
	 */
	bool _free;

	/**
	 *	The record being iterated over.
	 */
	const as_record * record;

	/**
	 *	Current position of the iterator
	 */
	uint32_t pos;

} as_record_iterator;

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Create and initialize a heap allocated as_record_iterator for the 
 *	specified record.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_iterator * it = as_record_iterator_new(rec);
 *
 *	while ( as_record_iterator_has_next(&it) ) {
 *		as_bin * bin = as_record_iterator_next(&it);
 *	}
 *
 *	as_record_iterator_destroy(&it);
 *	~~~~~~~~~~
 *	
 *	@param record 	The record to iterate over.
 *
 *	@return On success, a new as_record_iterator. Otherwise an error occurred.
 *
 *	@relates as_record_iterator
 *	@ingroup as_record_object
 */
as_record_iterator * as_record_iterator_new(const as_record * record);

/**
 *	Initializes a stack allocated as_record_iterator for the specified record.
 *
 *	~~~~~~~~~~{.c}
 *	as_record_iterator it;
 *	as_record_iterator_init(&it, rec);
 *	
 *	while ( as_record_iterator_has_next(&it) ) {
 *		as_bin * bin = as_record_iterator_next(&it);
 *	}
 *
 *	as_record_iterator_destroy(&it);
 *	~~~~~~~~~~
 *
 *	When you are finished using the `as_record` instance, you should release the 
 *	resources allocated to it by calling `as_record_destroy()`.
 *
 *	@param iterator		The iterator to initialize.
 *	@param record		The record to iterate over
 *
 *	@return On success, a new as_record_iterator. Otherwise an error occurred.
 *
 *	@relates as_record_iterator
 *	@ingroup as_record_object
 */
as_record_iterator * as_record_iterator_init(as_record_iterator * iterator, const as_record * record);

/**
 *	Destroy the as_record_iterator and associated resources.
 *
 *	@param iterator The iterator to destroy.
 *
 *	@relates as_record_iterator
 *	@ingroup as_record_object
 */
void as_record_iterator_destroy(as_record_iterator * iterator);

/**
 *	Test if there are more bins in the iterator.
 *
 *	@param iterator The iterator to test.
 *
 *	@return the number of bins in the record.
 *
 *	@relates as_record_iterator
 *	@ingroup as_record_object
 */
bool as_record_iterator_has_next(const as_record_iterator * iterator);

/**
 *	Read the next bin from the iterator. 
 *
 *	@param iterator		The iterator to read from.
 *
 *	@return The next bin from the iterator.
 *
 *	@relates as_record_iterator
 *	@ingroup as_record_object
 */
as_bin * as_record_iterator_next(as_record_iterator * iterator);


#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 
#pragma GCC diagnostic ignored "-Waddress"

#include <aerospike/as_bin.h>
#include <aerospike/as_key.h>
#include <aerospike/as_udf.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Default value for as_scan.priority
 */
#define AS_SCAN_PRIORITY_DEFAULT AS_SCAN_PRIORITY_AUTO

/**
 *	Default value for as_scan.percent
 */
#define AS_SCAN_PERCENT_DEFAULT 100

/**
 *	Default value for as_scan.no_bins
 */
#define AS_SCAN_NOBINS_DEFAULT false

/**
 *	Default value for as_scan.concurrent
 */
#define AS_SCAN_CONCURRENT_DEFAULT false

/**
 *	Default value for as_scan.include_ldt
 */
#define AS_SCAN_INCLUDE_LDT_DEFAULT false

/**
 *	Default value for as_scan.deserialize_list_map
 */
#define AS_SCAN_DESERIALIZE_DEFAULT true

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	Priority levels for a scan operation.
 */
typedef enum as_scan_priority_e { 

	/**
	 *	The cluster will auto adjust the scan priority.
	 */
	AS_SCAN_PRIORITY_AUTO, 

	/**
	 *	Low priority scan.
	 */
	AS_SCAN_PRIORITY_LOW,

	/**
	 *	Medium priority scan.
	 */ 
	AS_SCAN_PRIORITY_MEDIUM,

	/**
	 *	High priority scan.
	 */ 
	AS_SCAN_PRIORITY_HIGH
	
} as_scan_priority;

/**
 *	The status of a particular background scan.
 */
typedef enum as_scan_status_e {

	/**
	 *	The scan status is undefined.
	 *	This is likely due to the status not being properly checked.
	 */
	AS_SCAN_STATUS_UNDEF,

	/**
	 *	The scan is currently running.
	 */
	AS_SCAN_STATUS_INPROGRESS,

	/**
	 *	The scan was aborted. Due to failure or the user.
	 */
	AS_SCAN_STATUS_ABORTED,

	/**
	 *	The scan completed successfully.
	 */
	AS_SCAN_STATUS_COMPLETED,

} as_scan_status;

/**
 *	Information about a particular background scan.
 *
 *	@ingroup as_scan_object 
 */
typedef struct as_scan_info_s {

	/**
	 *	Status of the scan.
	 */
	as_scan_status status;

	/**
	 *	Progress estimate for the scan, as percentage.
	 */
	uint32_t progress_pct;

	/**
	 *	How many records have been scanned.
	 */
	uint32_t records_scanned;

} as_scan_info;

/**
 *	Sequence of bins which should be selected during a scan.
 *
 *	Entries can either be initialized on the stack or on the heap.
 *
 *	Initialization should be performed via a query object, using:
 *	- as_scan_select_init()
 *	- as_scan_select_inita()
 */
typedef struct as_scan_bins_s {

	/**
	 *	@private
	 *	If true, then as_scan_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Number of entries allocated
	 */
	uint16_t capacity;

	/**
	 *	Number of entries used
	 */
	uint16_t size;

	/**
	 *	Sequence of entries
	 */
	as_bin_name * entries;

} as_scan_bins;

/**
 *	In order to execute a scan using the Scan API, an as_scan object
 *	must be initialized and populated.
 *
 *	## Initialization
 *	
 *	Before using an as_scan, it must be initialized via either: 
 *	- as_scan_init()
 *	- as_scan_new()
 *	
 *	as_scan_init() should be used on a stack allocated as_scan. It will
 *	initialize the as_scan with the given namespace and set. On success,
 *	it will return a pointer to the initialized as_scan. Otherwise, NULL 
 *	is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan scan;
 *	as_scan_init(&scan, "namespace", "set");
 *	~~~~~~~~~~
 *
 *	as_scan_new() should be used to allocate and initialize a heap allocated
 *	as_scan. It will allocate the as_scan, then initialized it with the 
 *	given namespace and set. On success, it will return a pointer to the 
 *	initialized as_scan. Otherwise, NULL is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan * scan = as_scan_new("namespace", "set");
 *	~~~~~~~~~~
 *
 *	## Destruction
 *
 *	When you are finished with the as_scan, you can destroy it and associated
 *	resources:
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_destroy(scan);
 *	~~~~~~~~~~
 *
 *	## Usage
 *
 *	An initialized as_query can be populated with additional fields.
 *
 *	### Selecting Bins
 *
 *	as_scan_select() is used to specify the bins to be selected by the scan.
 *	If a scan specifies bins to be selected, then only those bins will be 
 *	returned. If no bins are selected, then all bins will be returned.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_select(query, "bin1");
 *	as_scan_select(query, "bin2");
 *	~~~~~~~~~~
 *
 *	Before adding bins to select, the select structure must be initialized via
 *	either:
 *	- as_scan_select_inita() - Initializes the structure on the stack.
 *	- as_scan_select_init() - Initializes the structure on the heap.
 *
 *	Both functions are given the number of bins to be selected.
 *
 *	A complete example using as_scan_select_inita()
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_select_inita(query, 2);
 *	as_scan_select(query, "bin1");
 *	as_scan_select(query, "bin2");
 *	~~~~~~~~~~
 *
 *	### Returning only meta data
 *
 *	A scan can return only record meta data, and exclude bins.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_set_nobins(scan, true);
 *	~~~~~~~~~~
 *
 *	### Scan nodes in parallel
 *
 *	A scan can be made to scan all the nodes in parallel
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_set_concurrent(scan, true);
 *	~~~~~~~~~~
 *
 *	### Scan a Percentage of Records
 *
 *	A scan can define the percentage of record in the cluster to be scaned.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_set_percent(scan, 100);
 *	~~~~~~~~~~
 *
 *	### Scan a Priority
 *
 *	To set the priority of the scan, the set as_scan.priority.
 *
 *	The priority of a scan can be defined as either:
 *	- `AS_SCAN_PRIORITY_AUTO`
 *	- `AS_SCAN_PRIORITY_LOW`
 *	- `AS_SCAN_PRIORITY_MEDIUM`
 * 	- `AS_SCAN_PRIORITY_HIGH`
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_set_priority(scan, AS_SCAN_PRIORITY_LOW);
 *	~~~~~~~~~~
 *
 *	### Applying a UDF to each Record Scanned
 *
 *	A UDF can be applied to each record scanned.
 *
 *	To define the UDF for the scan, use as_scan_apply_each().
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_apply_each(scan, "udf_module", "udf_function", arglist);
 *	~~~~~~~~~~
 *
 *	@ingroup client_objects
 */
typedef struct as_scan_s {

	/**
	 *	@private
	 *	If true, then as_scan_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	Priority of scan.
	 *
	 *	Default value is AS_SCAN_PRIORITY_DEFAULT.
	 */
	as_scan_priority priority;

	/**
	 *	Percentage of the data to scan.
	 *
	 *	Default value is AS_SCAN_PERCENT_DEFAULT.
	 */
	uint8_t percent;

	/**
	 *	Set to true if the scan should return only the metadata of the record.
	 *
	 *	Default value is AS_SCAN_NOBINS_DEFAULT.
	 */
	bool no_bins;

	/**
	 *	Set to true if the scan should scan all the nodes in parallel
	 *
	 *	Default value is AS_SCAN_CONCURRENT_DEFAULT.
	 */
	bool concurrent;

	/**
	 *	Include large data type bin values in addition to large data type bin names.
	 *	If false, LDT bin names will be returned, but LDT bin values will be empty.
	 *	If true,  LDT bin names and the entire LDT bin values will be returned.
	 *	This is useful for backups.
	 *	Default: false
	 */
	bool include_ldt;

	/**
	 *	Set to true if the scan should deserialize list and map raw bytes.
	 *	Set to false for backup programs that just need access to raw bytes.
	 *
	 *	Default value is AS_SCAN_DESERIALIZE_DEFAULT.
	 */
	bool deserialize_list_map;
	
	/**
	 * 	@memberof as_scan
	 *	Namespace to be scanned.
	 *
	 *	Should be initialized via either:
	 *	-	as_scan_init() -	To initialize a stack allocated scan.
	 *	-	as_scan_new() -		To heap allocate and initialize a scan.
	 *
	 */
	as_namespace ns;

	/**
	 *	Set to be scanned.
	 *
	 *	Should be initialized via either:
	 *	-	as_scan_init() -	To initialize a stack allocated scan.
	 *	-	as_scan_new() -		To heap allocate and initialize a scan.
	 *
	 */
	as_set set;

	/**
	 *	Name of bins to select.
	 *	
	 *	Use either of the following function to initialize:
	 *	- as_scan_select_init() -	To initialize on the heap.
	 *	- as_scan_select_inita() -	To initialize on the stack.
	 *
	 *	Use as_scan_select() to populate.
	 */
	as_scan_bins select;
	
	/**
	 *	Apply the UDF for each record scanned on the server.
	 *
	 *	Should be set via `as_scan_apply_each()`.
	 */
	as_udf_call apply_each;

} as_scan;

/******************************************************************************
 *	INSTANCE FUNCTIONS
 *****************************************************************************/

/**
 *	Initializes a scan.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan scan;
 *	as_scan_init(&scan, "test", "demo");
 *	~~~~~~~~~~
 *
 *	When you no longer require the scan, you should release the scan and 
 *	related resources via `as_scan_destroy()`.
 *
 *	@param scan		The scan to initialize.
 *	@param ns 		The namespace to scan.
 *	@param set 		The set to scan.
 *
 *	@returns On succes, the initialized scan. Otherwise NULL.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
as_scan * as_scan_init(as_scan * scan, const as_namespace ns, const as_set set);

/**
 *	Create and initializes a new scan on the heap.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan * scan = as_scan_new("test","demo");
 *	~~~~~~~~~~
 *
 *	When you no longer require the scan, you should release the scan and 
 *	related resources via `as_scan_destroy()`.
 *
 *	@param ns 		The namespace to scan.
 *	@param set 		The set to scan.
 *
 *	@returns On success, a new scan. Otherwise NULL.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
as_scan * as_scan_new(const as_namespace ns, const as_set set);

/**
 *	Releases all resources allocated to the scan.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_destroy(scan);
 *	~~~~~~~~~~
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
void as_scan_destroy(as_scan * scan);

/******************************************************************************
 *	SELECT FUNCTIONS
 *****************************************************************************/

/** 
 *	Initializes `as_scan.select` with a capacity of `n` using `alloca`
 *
 *	For heap allocation, use `as_scan_select_init()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_select_inita(&scan, 2);
 *	as_scan_select(&scan, "bin1");
 *	as_scan_select(&scan, "bin2");
 *	~~~~~~~~~~
 *	
 *	@param __scan	The scan to initialize.
 *	@param __n		The number of bins to allocate.
 *
 *	@ingroup as_scan_object
 */
#define as_scan_select_inita(__scan, __n) \
	if ( (__scan) != NULL && (__scan)->select.entries == NULL ) {\
		(__scan)->select.entries = (as_bin_name *) alloca(__n * sizeof(as_bin_name));\
		if ( (__scan)->select.entries ) { \
			(__scan)->select._free = false;\
			(__scan)->select.capacity = __n;\
			(__scan)->select.size = 0;\
		}\
 	}

/** 
 *	Initializes `as_scan.select` with a capacity of `n` using `malloc()`.
 *	
 *	For stack allocation, use `as_scan_select_inita()`.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_select_init(&scan, 2);
 *	as_scan_select(&scan, "bin1");
 *	as_scan_select(&scan, "bin2");
 *	~~~~~~~~~~
 *
 *	@param scan		The scan to initialize.
 *	@param n		The number of bins to allocate.
 *
 *	@return On success, the initialized. Otherwise an error occurred.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
bool as_scan_select_init(as_scan * scan, uint16_t n);

/**
 *	Select bins to be projected from matching records.
 *
 *	You have to ensure as_scan.select has sufficient capacity, prior to 
 *	adding a bin. If capacity is insufficient then false is returned.
 *
 *	~~~~~~~~~~{.c}
 *	as_scan_select_init(&scan, 2);
 *	as_scan_select(&scan, "bin1");
 *	as_scan_select(&scan, "bin2");
 *	~~~~~~~~~~
 *
 *	@param scan 		The scan to modify.
 *	@param bin 			The name of the bin to select.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
bool as_scan_select(as_scan * scan, const char * bin);

/******************************************************************************
 *	MODIFIER FUNCTIONS
 *****************************************************************************/

/**
 *	The percentage of data to scan.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_set_percent(&q, 100);
 *	~~~~~~~~~~
 *
 *	@param scan 		The scan to set the priority on.
 *	@param percent		The percent to scan.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
bool as_scan_set_percent(as_scan * scan, uint8_t percent);

/**
 *	Set the priority for the scan.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_set_priority(&q, AS_SCAN_PRIORITY_LOW);
 *	~~~~~~~~~~
 *
 *	@param scan 		The scan to set the priority on.
 *	@param priority		The priority for the scan.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
bool as_scan_set_priority(as_scan * scan, as_scan_priority priority);

/**
 *	Do not return bins. This will only return the metadata for the records.
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_set_nobins(&q, true);
 *	~~~~~~~~~~
 *
 *	@param scan 		The scan to set the priority on.
 *	@param nobins		If true, then do not return bins.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
bool as_scan_set_nobins(as_scan * scan, bool nobins);

/**
 *	Scan all the nodes in prallel
 *	
 *	~~~~~~~~~~{.c}
 *	as_scan_set_concurrent(&q, true);
 *	~~~~~~~~~~
 *
 *	@param scan 		The scan to set the concurrency on.
 *	@param concurrent	If true, scan all the nodes in parallel
 *
 *	@return On success, true. Otherwise an error occurred.
 */
bool as_scan_set_concurrent(as_scan * scan, bool concurrent);

/**
 *	Apply a UDF to each record scanned on the server.
 *	
 *	~~~~~~~~~~{.c}
 *	as_arraylist arglist;
 *	as_arraylist_init(&arglist, 2, 0);
 *	as_arraylist_append_int64(&arglist, 1);
 *	as_arraylist_append_int64(&arglist, 2);
 *	
 *	as_scan_apply_each(&q, "module", "func", (as_list *) &arglist);
 *
 *	as_arraylist_destroy(&arglist);
 *	~~~~~~~~~~
 *
 *	@param scan 		The scan to apply the UDF to.
 *	@param module 		The module containing the function to execute.
 *	@param function 	The function to execute.
 *	@param arglist 		The arguments for the function.
 *
 *	@return On success, true. Otherwise an error occurred.
 *
 *	@relates as_scan
 *	@ingroup as_scan_object
 */
bool as_scan_apply_each(as_scan * scan, const char * module, const char * function, as_list * arglist);

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_config.h>
#include <aerospike/as_partition.h>
#include <citrusleaf/cf_queue.h>

#ifdef __cplusplus
extern "C" {
#endif

// Concurrency kit needs to be under extern "C" when compiling C++.
#include <aerospike/ck/ck_spinlock.h>
#include <aerospike/ck/ck_swlock.h>

/******************************************************************************
 *	TYPES
 *****************************************************************************/

/**
 *	@private
 *	Shared memory representation of node. 48 bytes.
 */
typedef struct as_node_shm_s {
	/**
	 *	@private
	 *	Node name.
	 */
	char name[AS_NODE_NAME_SIZE];
		
	/**
	 *	@private
	 *	Lightweight node read/write lock.
	 */
	ck_swlock_t lock;
	
	/**
	 *	@private
	 *	Socket address.
	 */
	struct sockaddr_in addr;

	/**
	 *	@private
	 *	Is node currently active.
	 */
	uint8_t active;
	
	/**
	 *	@private
	 *	Does node support batch-index protocol?
	 */
	uint8_t has_batch_index;
	
	/**
	 *	@private
	 *	Does node support replicas-all info protocol?
	 */
	uint8_t has_replicas_all;

	/**
	 *	@private
	 *	Does node support floating point type?
	 */
	uint8_t has_double;

	/**
	 *	@private
	 *	Does node support geospatial?
	 */
	uint8_t has_geo;

	/**
	 *	@private
	 *	Pad to 8 byte boundary.
	 */
	char pad[3];
} as_node_shm;

/**
 *	@private
 *  Shared memory representation of map of namespace data partitions to nodes. 8 bytes.
 */
typedef struct as_partition_shm_s {
	/**
	 *	@private
	 *	Master node index offset.
	 */
	uint32_t master;

	/**
	 *	@private
	 *	Prole node index offset.
	 */
	uint32_t prole;
} as_partition_shm;

/**
 *	@private
 *	Shared memory representation of map of namespace to data partitions. 32 bytes + partitions size.
 */
typedef struct as_partition_table_shm_s {
	/**
	 *	@private
	 *	Namespace name.
	 */
	char ns[AS_MAX_NAMESPACE_SIZE];
	
	/**
	 *	@private
	 *	Array of partitions for a given namespace.
	 */
	as_partition_shm partitions[];
} as_partition_table_shm;

/**
 *	@private
 *	Shared memory cluster map. The map contains fixed arrays of nodes and partition tables.
 *	Each partition table contains a fixed array of partitions.  The shared memory segment will be 
 *	sized on startup and never change afterwards.  If the max nodes or max namespaces are reached, 
 *	the tender client will ignore additional nodes/namespaces and log an error message that the
 *	corresponding array is full.
 */
typedef struct as_cluster_shm_s {
	/**
	 *	@private
	 *	Last time cluster was tended in milliseconds since epoch.
	 */
	uint64_t timestamp;

	/**
	 *	@private
	 *	Cluster tend owner process id.
	 */
	uint32_t owner_pid;
	
	/**
	 *	@private
	 *	Current size of nodes array.
	 */
	uint32_t nodes_size;
	
	/**
	 *	@private
	 *	Maximum size of nodes array.
	 */
	uint32_t nodes_capacity;
	
	/**
	 *	@private
	 *	Nodes generation count.  Incremented whenever a node is added or removed from cluster.
	 */
	uint32_t nodes_gen;
	
	/**
	 *	@private
	 *	Total number of data partitions used by cluster.
	 */
	uint32_t n_partitions;

	/**
	 *	@private
	 *	Current size of partition tables array.
	 */
	uint32_t partition_tables_size;
	
	/**
	 *	@private
	 *	Maximum size of partition tables array.
	 */
	uint32_t partition_tables_capacity;

	/**
	 *	@private
	 *	Cluster offset to partition tables at the end of this structure.
	 */
	uint32_t partition_tables_offset;
	
	/**
	 *	@private
	 *	Bytes required to hold one partition_table.
	 */
	uint32_t partition_table_byte_size;

	/**
	 *	@private
	 *	Spin lock for taking over from a dead cluster tender.
	 */
	ck_spinlock_t take_over_lock;
	
	/**
	 *	@private
	 *	Shared memory master mutex lock.  Used to determine cluster tend owner.
	 */
	uint8_t lock;
	
	/**
	 *	@private
	 *	Has shared memory been fully initialized and populated.
	 */
	uint8_t ready;
	
	/**
	 *	@private
	 *	Pad to 8 byte boundary.
	 */
	char pad[6];

	/*
	 *	@private
	 *	Dynamically allocated node array.
	 */
	as_node_shm nodes[];
	
	// This is where the dynamically allocated partition tables are located.
} as_cluster_shm;

/**
 *	@private
 *	Local data related to shared memory implementation.
 */
typedef struct as_shm_info_s {
	/**
	 *	@private
	 *	Pointer to cluster shared memory.
	 */
	as_cluster_shm* cluster_shm;
	
	/**
	 *	@private
	 *	Array of pointers to local nodes.  
	 *	Array index offsets are synchronized with shared memory node offsets.
	 */
	as_node** local_nodes;
	
	/**
	 *	@private
	 *	Shared memory identifier.
	 */
	int shm_id;
	
	/**
	 *	@private
	 *	Take over shared memory cluster tending if the cluster hasn't been tended by this
	 *	millisecond threshold.
	 */
	uint32_t takeover_threshold_ms;
	
	/**
	 *	@private
	 *	Is this process responsible for performing cluster tending.
	 */
	volatile bool is_tend_master;
} as_shm_info;

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Create shared memory implementation of cluster.
 */
as_status
as_shm_create(struct as_cluster_s* cluster, as_error* err, as_config* config);

/**
 *	@private
 *	Destroy shared memory components.
 */
void
as_shm_destroy(struct as_cluster_s* cluster);

/**
 *	@private
 *	Add nodes to shared memory.
 */
void
as_shm_add_nodes(struct as_cluster_s* cluster, as_vector* /* <as_node*> */ nodes_to_add);

/**
 *	@private
 *	Remove nodes from shared memory.
 */
void
as_shm_remove_nodes(struct as_cluster_s* cluster, as_vector* /* <as_node*> */ nodes_to_remove);

/**
 *	@private
 *	Update shared memory partition tables for given namespace.
 */
void
as_shm_update_partitions(as_shm_info* shm_info, const char* ns, char* bitmap_b64, int64_t len, as_node* node, bool master);

/**
 *	@private
 *	Get shared memory mapped node given digest key.  If there is no mapped node, a random node is 
 *	used instead.  as_nodes_release() must be called when done with node.
 */
as_node*
as_shm_node_get(struct as_cluster_s* cluster, const char* ns, const uint8_t* digest, bool write, as_policy_replica replica);

/**
 *	@private
 *	Get shared memory partition tables array.
 */
static inline as_partition_table_shm*
as_shm_get_partition_tables(as_cluster_shm* cluster_shm)
{
	return (as_partition_table_shm*) ((char*)cluster_shm + cluster_shm->partition_tables_offset);
}

/**
 *	@private
 *	Get partition table identified by index.
 */
static inline as_partition_table_shm*
as_shm_get_partition_table(as_cluster_shm* cluster_shm, as_partition_table_shm* tables, uint32_t index)
{
	return (as_partition_table_shm*) ((char*)tables + (cluster_shm->partition_table_byte_size * index));
}

/**
 *	@private
 *	Get next partition table in array.
 */
static inline as_partition_table_shm*
as_shm_next_partition_table(as_cluster_shm* cluster_shm, as_partition_table_shm* table)
{
	return (as_partition_table_shm*) ((char*)table + cluster_shm->partition_table_byte_size);
}

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <aerospike/as_error.h>
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
#endif

#if defined(__APPLE__)
#define SOL_TCP IPPROTO_TCP
#define MSG_NOSIGNAL 0
#endif

#if defined(CF_WINDOWS)
#include <WinSock2.h>
#include <Ws2tcpip.h>

#define as_socket_data_t char
#define as_socket_size_t int
#define as_close(fd) (closesocket(fd))

#define MSG_DONTWAIT	0
#define MSG_NOSIGNAL	0

#define SHUT_RDWR		SD_BOTH
#endif // CF_WINDOWS

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	@private
 *	Create non-blocking socket.
 */
int
as_socket_create_nb();

/**
 *	@private
 *	Connect to non-blocking socket.
 */
as_status
as_socket_start_connect_nb(as_error* err, int fd, struct sockaddr_in *sa);

/**
 *	@private
 *	Create non-blocking socket and connect.
 */
as_status
as_socket_create_and_connect_nb(as_error* err, struct sockaddr_in *sa, int* fd);

/**
 *	@private
 *	Peek for socket connection status.
 *
 *	@return   0 : socket is connected, but no data available.
 *			> 0 : byte size of data available.
 *			< 0 : socket is invalid.
 */
int
as_socket_validate(int fd);

#if defined(__linux__) || defined(__APPLE__)

/**
 *	@private
 *	Calculate future deadline given timeout.
 */
static inline uint64_t
as_socket_deadline(uint32_t timeout_ms)
{
	return (timeout_ms && timeout_ms <= INT32_MAX)? cf_getms() + timeout_ms : 0;
}

/**
 *	@private
 *	Write socket data without timeouts.
 */
as_status
as_socket_write_forever(as_error* err, int fd, uint8_t *buf, size_t buf_len);

/**
 *	@private
 *	Write socket data with future deadline in milliseconds.
 *	Do not adjust for zero deadline.
 */
as_status
as_socket_write_limit(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint64_t deadline);

/**
 *	@private
 *	Write socket data with future deadline in milliseconds.
 *	If deadline is zero, do not set deadline.
 */
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

/**
 *	@private
 *	Write socket data with timeout in milliseconds.
 *	If timeout is zero or > MAXINT, do not set timeout.
 */
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

/**
 *	@private
 *	Read socket data without timeouts.
 */
as_status
as_socket_read_forever(as_error* err, int fd, uint8_t *buf, size_t buf_len);

/**
 *	@private
 *	Read socket data with future deadline in milliseconds.
 *	Do not adjust for zero deadline.
 */
as_status
as_socket_read_limit(as_error* err, int fd, uint8_t *buf, size_t buf_len, uint64_t deadline);

/**
 *	@private
 *	Read socket data with future deadline in milliseconds.
 *	If deadline is zero, do not set deadline.
 */
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

/**
 *	@private
 *	Read socket data with timeout in milliseconds.
 *	If timeout is zero or > MAXINT, do not set timeout.
 */
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

/**
 *	@private
 *	Convert socket address to a string.
 */
static inline void
as_socket_address_name(struct sockaddr_in* address, char* name)
{
	inet_ntop(AF_INET, &(address->sin_addr), name, INET_ADDRSTRLEN);
}

#endif

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *	TYPES
 ******************************************************************************/

/**
 *	Status codes used as return values as as_error.code values.
 */
typedef enum as_status_e {

	/***************************************************************************
	 *	Client Errors
	 **************************************************************************/
	/**
	 *	Node invalid or could not be found.
	 */
	AEROSPIKE_ERR_INVALID_NODE = -8,
	
	/**
	 *	Asynchronous connection error.
	 */
	AEROSPIKE_ERR_NO_MORE_CONNECTIONS = -7,

	/**
	 *	Asynchronous connection error.
	 */
	AEROSPIKE_ERR_ASYNC_CONNECTION = -6,
	
	/**
	 *	Query or scan was aborted in user's callback.
	 */
	AEROSPIKE_ERR_CLIENT_ABORT = -5,

	/**
	 *	Host name could not be found in DNS lookup.
	 */
	AEROSPIKE_ERR_INVALID_HOST = -4,

	/**
	 *	No more records available when parsing batch, scan or query records.
	 */
	AEROSPIKE_NO_MORE_RECORDS = -3,
	
	/**
	 *	Invalid client API parameter.
	 */
	AEROSPIKE_ERR_PARAM	= -2,
	
	/**
	 *	Generic client API usage error.
	 */
	AEROSPIKE_ERR_CLIENT = -1,
	
	/**
	 *	Deprecated.  Generic client error.  Keep for legacy reasons.
	 */
	AEROSPIKE_ERR = -1,

	/***************************************************************************
	 *	Success
	 **************************************************************************/

	/**
	 *	Generic success.
	 */
	AEROSPIKE_OK = 0,
	
	/***************************************************************************
	 *	Server Errors
	 **************************************************************************/
	
	/**
	 *	Generic error returned by server.
	 */
	AEROSPIKE_ERR_SERVER = 1,
	
	/**
	 *	Record does not exist in database. May be returned by read, or write
	 *	with policy AS_POLICY_EXISTS_UPDATE.
	 */
	AEROSPIKE_ERR_RECORD_NOT_FOUND = 2,
	
	/**
	 *	Generation of record in database does not satisfy write policy.
	 */
	AEROSPIKE_ERR_RECORD_GENERATION = 3,
	
	/**
	 *	Request protocol invalid, or invalid protocol field.
	 */
	AEROSPIKE_ERR_REQUEST_INVALID = 4,
	
	/**
	 *	Record already exists. May be returned by write with policy
	 *	AS_POLICY_EXISTS_CREATE.
	 */
	AEROSPIKE_ERR_RECORD_EXISTS = 5,
	
	/**
	 *	Bin already exists.
	 */
	AEROSPIKE_ERR_BIN_EXISTS = 6,

	/**
	 *	A cluster state change occurred during the request. This may also be
	 *	returned by scan operations with the fail_on_cluster_change flag set.
	 */
	AEROSPIKE_ERR_CLUSTER_CHANGE = 7,
	
	/**
	 *	The server node is running out of memory and/or storage device space
	 *	reserved for the specified namespace.
	 */
	AEROSPIKE_ERR_SERVER_FULL = 8,
	
	/**
	 *	Request timed out.  Can be triggered by client or server.
	 */
	AEROSPIKE_ERR_TIMEOUT = 9,
	
	/**
	 *	XDR is not available for the cluster.
	 */
	AEROSPIKE_ERR_NO_XDR = 10,
	
	/**
	 *	Generic cluster discovery & connection error.
	 */
	AEROSPIKE_ERR_CLUSTER = 11,
	
	/**
	 *	Bin modification operation can't be done on an existing bin due to its
	 *	value type.
	 */
	AEROSPIKE_ERR_BIN_INCOMPATIBLE_TYPE = 12,
	
	/**
	 *	Record being (re-)written can't fit in a storage write block.
	 */
	AEROSPIKE_ERR_RECORD_TOO_BIG = 13,
	
	/**
	 *	Too may concurrent requests for one record - a "hot-key" situation.
	 */
	AEROSPIKE_ERR_RECORD_BUSY = 14,
	
	/**
	 *	Scan aborted by user.
	 */
	AEROSPIKE_ERR_SCAN_ABORTED = 15,
	
	/**
	 *	Sometimes our doc, or our customers wishes, get ahead of us.  We may have
	 *	processed something that the server is not ready for (unsupported feature).
	 */
	AEROSPIKE_ERR_UNSUPPORTED_FEATURE = 16,
	
	/**
	 *	Bin-level replace-only supported on server but not on client.
	 */
    AEROSPIKE_ERR_BIN_NOT_FOUND = 17,
	
	/**
	 *	The server node's storage device(s) can't keep up with the write load.
	 */
	AEROSPIKE_ERR_DEVICE_OVERLOAD = 18,
	
	/**
	 *	Record key sent with transaction did not match key stored on server.
	 */
	AEROSPIKE_ERR_RECORD_KEY_MISMATCH = 19,
	
	/**
	 *	Namespace in request not found on server.
	 */
	AEROSPIKE_ERR_NAMESPACE_NOT_FOUND = 20,
	
	/**
	 *	Sent too-long bin name (should be impossible in this client) or exceeded
	 *	namespace's bin name quota.
	 */
	AEROSPIKE_ERR_BIN_NAME = 21,
	
	/**
	 *	Operation not allowed at this time.
	 */
	AEROSPIKE_ERR_FAIL_FORBIDDEN = 22,

	/**
	 *	There are no more records left for query.
	 */
	AEROSPIKE_QUERY_END = 50,
	
	/**
	 *	Security functionality not supported by connected server.
	 */
	AEROSPIKE_SECURITY_NOT_SUPPORTED = 51,
	
	/**
	 *	Security functionality not enabled by connected server.
	 */
	AEROSPIKE_SECURITY_NOT_ENABLED = 52,
		
	/**
	 *	Security type not supported by connected server.
	 */
	AEROSPIKE_SECURITY_SCHEME_NOT_SUPPORTED = 53,
	
	/**
	 *	Administration command is invalid.
	 */
	AEROSPIKE_INVALID_COMMAND = 54,
	
	/**
	 *	Administration field is invalid.
	 */
	AEROSPIKE_INVALID_FIELD = 55,
	
	/**
	 *	Security protocol not followed.
	 */
	AEROSPIKE_ILLEGAL_STATE = 56,
	
	/**
	 *	User name is invalid.
	 */
	AEROSPIKE_INVALID_USER = 60,
	
	/**
	 *	User was previously created.
	 */
	AEROSPIKE_USER_ALREADY_EXISTS = 61,
	
	/**
	 *	Password is invalid.
	 */
	AEROSPIKE_INVALID_PASSWORD = 62,
	
	/**
	 *	Password has expired.
	 */
	AEROSPIKE_EXPIRED_PASSWORD = 63,
	
	/**
	 *	Forbidden password (e.g. recently used)
	 */
	AEROSPIKE_FORBIDDEN_PASSWORD = 64,
	
	/**
	 *	Security credential is invalid.
	 */
	AEROSPIKE_INVALID_CREDENTIAL = 65,

	/**
	 *	Role name is invalid.
	 */
	AEROSPIKE_INVALID_ROLE = 70,
	
	/**
	 *	Role already exists.
	 */
	AEROSPIKE_ROLE_ALREADY_EXISTS = 71,

	/**
	 *	Privilege is invalid.
	 */
	AEROSPIKE_INVALID_PRIVILEGE = 72,
	
	/**
	 *	User must be authentication before performing database operations.
	 */
	AEROSPIKE_NOT_AUTHENTICATED = 80,
	
	/**
	 *	User does not possess the required role to perform the database operation.
	 */
	AEROSPIKE_ROLE_VIOLATION = 81,
	
	/**
	 *	Generic UDF error.
	 */
	AEROSPIKE_ERR_UDF = 100,
	
	/**
	 *	The requested item in a large collection was not found.
	 */
	AEROSPIKE_ERR_LARGE_ITEM_NOT_FOUND = 125,

	/**
	 * Batch functionality has been disabled.
	 */
	AEROSPIKE_ERR_BATCH_DISABLED = 150,
	
	/**
	 * Batch max requests have been exceeded.
	 */
	AEROSPIKE_ERR_BATCH_MAX_REQUESTS_EXCEEDED = 151,
	
	/**
	 * All batch queues are full.
	 */
	AEROSPIKE_ERR_BATCH_QUEUES_FULL = 152,

	/**
	 * Invalid/Unsupported GeoJSON
	 */
	AEROSPIKE_ERR_GEO_INVALID_GEOJSON = 160,

	/**
	 *	Index found.
	 */
	AEROSPIKE_ERR_INDEX_FOUND = 200,
	
	/**
	 *	Index not found
	 */
	AEROSPIKE_ERR_INDEX_NOT_FOUND = 201,
	
	/**
	 *	Index is out of memory
	 */
	AEROSPIKE_ERR_INDEX_OOM = 202,
	
	/**
	 *	Unable to read the index.
	 */
	AEROSPIKE_ERR_INDEX_NOT_READABLE = 203,
	
	/**
	 *	Generic secondary index error.
	 */
	AEROSPIKE_ERR_INDEX = 204,
	
	/**
	 *	Index name is too long.
	 */
	AEROSPIKE_ERR_INDEX_NAME_MAXLEN = 205,
	
	/**
	 *	System already has maximum allowed indices.
	 */
	AEROSPIKE_ERR_INDEX_MAXCOUNT = 206,
	
	/**
	 *	Query was aborted.
	 */
	AEROSPIKE_ERR_QUERY_ABORTED = 210,
	
	/**
	 *	Query processing queue is full.
	 */
	AEROSPIKE_ERR_QUERY_QUEUE_FULL = 211,

	/**
	 *	Secondary index query timed out on server.
	 */
	AEROSPIKE_ERR_QUERY_TIMEOUT = 212,

	/**
	 *	Generic query error.
	 */
	AEROSPIKE_ERR_QUERY = 213,
	
	/***************************************************************************
	 *	UDF OPERATIONS
	 **************************************************************************/

	/**
	 *	UDF does not exist.
	 */
	AEROSPIKE_ERR_UDF_NOT_FOUND				= 1301,
	/**
	 *	LUA file does not exist.
	 */
	AEROSPIKE_ERR_LUA_FILE_NOT_FOUND		= 1302,

	/***************************************************************************
	 *	Large Data Type (LDT) OPERATIONS
	 **************************************************************************/

	/** Internal LDT error. */
	AEROSPIKE_ERR_LDT_INTERNAL                    = 1400,

	/** LDT item not found */
	AEROSPIKE_ERR_LDT_NOT_FOUND                   = 1401,

	/** Unique key violation: Duplicated item inserted when 'unique key" was set.*/
	AEROSPIKE_ERR_LDT_UNIQUE_KEY                  = 1402,

	/** General error during insert operation. */
	AEROSPIKE_ERR_LDT_INSERT                      = 1403,

	/** General error during search operation. */
	AEROSPIKE_ERR_LDT_SEARCH                      = 1404,

	/** General error during delete operation. */
	AEROSPIKE_ERR_LDT_DELETE                      = 1405,


	/** General input parameter error. */
	AEROSPIKE_ERR_LDT_INPUT_PARM                  = 1409,

    // -------------------------------------------------

	/** LDT Type mismatch for this bin.  */
	AEROSPIKE_ERR_LDT_TYPE_MISMATCH               = 1410,

	/** The supplied LDT bin name is null. */
	AEROSPIKE_ERR_LDT_NULL_BIN_NAME               = 1411,

	/** The supplied LDT bin name must be a string. */
	AEROSPIKE_ERR_LDT_BIN_NAME_NOT_STRING         = 1412,

	/** The supplied LDT bin name exceeded the 14 char limit. */
	AEROSPIKE_ERR_LDT_BIN_NAME_TOO_LONG           = 1413,

	/** Internal Error: too many open records at one time. */
	AEROSPIKE_ERR_LDT_TOO_MANY_OPEN_SUBRECS       = 1414,

	/** Internal Error: Top Record not found.  */
	AEROSPIKE_ERR_LDT_TOP_REC_NOT_FOUND           = 1415,

	/** Internal Error: Sub Record not found. */
	AEROSPIKE_ERR_LDT_SUB_REC_NOT_FOUND           = 1416,

	/** LDT Bin does not exist. */
	AEROSPIKE_ERR_LDT_BIN_DOES_NOT_EXIST          = 1417,

	/** Collision: LDT Bin already exists. */
	AEROSPIKE_ERR_LDT_BIN_ALREADY_EXISTS          = 1418,

	/** LDT control structures in the Top Record are damaged. Cannot proceed. */
	AEROSPIKE_ERR_LDT_BIN_DAMAGED                 = 1419,

    // -------------------------------------------------

	/** Internal Error: LDT Subrecord pool is damaged. */
	AEROSPIKE_ERR_LDT_SUBREC_POOL_DAMAGED         = 1420,

	/** LDT control structures in the Sub Record are damaged. Cannot proceed. */
	AEROSPIKE_ERR_LDT_SUBREC_DAMAGED              = 1421,

	/** Error encountered while opening a Sub Record. */
	AEROSPIKE_ERR_LDT_SUBREC_OPEN                 = 1422,

	/** Error encountered while updating a Sub Record. */
	AEROSPIKE_ERR_LDT_SUBREC_UPDATE               = 1423,

	/** Error encountered while creating a Sub Record. */
	AEROSPIKE_ERR_LDT_SUBREC_CREATE               = 1424,

	/** Error encountered while deleting a Sub Record. */
	AEROSPIKE_ERR_LDT_SUBREC_DELETE               = 1425, 

	/** Error encountered while closing a Sub Record. */
	AEROSPIKE_ERR_LDT_SUBREC_CLOSE                = 1426,

	/** Error encountered while updating a TOP Record. */
	AEROSPIKE_ERR_LDT_TOPREC_UPDATE               = 1427,

	/** Error encountered while creating a TOP Record. */
	AEROSPIKE_ERR_LDT_TOPREC_CREATE               = 1428,

    // -------------------------------------------------

	/** The filter function name was invalid. */
	AEROSPIKE_ERR_LDT_FILTER_FUNCTION_BAD         = 1430,

	/** The filter function was not found. */
	AEROSPIKE_ERR_LDT_FILTER_FUNCTION_NOT_FOUND   = 1431,

	/** The function to extract the Unique Value from a complex object was invalid. */
	AEROSPIKE_ERR_LDT_KEY_FUNCTION_BAD            = 1432,

	/** The function to extract the Unique Value from a complex object was not found. */
	AEROSPIKE_ERR_LDT_KEY_FUNCTION_NOT_FOUND      = 1433,

	/** The function to transform an object into a binary form was invalid. */
	AEROSPIKE_ERR_LDT_TRANS_FUNCTION_BAD          = 1434,

	/** The function to transform an object into a binary form was not found. */
	AEROSPIKE_ERR_LDT_TRANS_FUNCTION_NOT_FOUND    = 1435,

	/** The function to untransform an object from binary form to live form was invalid. */
	AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_BAD        = 1436,

	/** The function to untransform an object from binary form to live form not found. */
	AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_NOT_FOUND  = 1437,

	/** The UDF user module name for LDT Overrides was invalid */
	AEROSPIKE_ERR_LDT_USER_MODULE_BAD             = 1438,

	/** The UDF user module name for LDT Overrides was not found */
	AEROSPIKE_ERR_LDT_USER_MODULE_NOT_FOUND       = 1439

} as_status;

#ifdef __cplusplus
} // end extern "C"
#endif
/*
 * Copyright 2008-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <aerospike/as_bytes.h>
#include <aerospike/as_list.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Maximum number of bytes in UDF module name.
 */
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
