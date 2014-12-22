/*
 * Copyright (c) 2009 Mellanox Technologies Ltd.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following						    			    
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: Ido Shamay <idos@dev.mellanox.co.il>
 *
 * Description :
 *
 *  This API gathres the Socket interface methods for all perftest benchmarks 
 *  and can be used for any benchmark for IB.
 *  It passes messages between 2 end points through sockets interface methods,
 *  while passing the rellevant information for the IB entities. 
 *  
 * Methods :
 *
 *  ctx_get_local_lid  - Receives the Local id from the subnet manager.
 *  ctx_client_connect - Connects the client through sockets interface.
 *  ctx_server_connect - Connects the Server to client through sockets.
 *  ctx_hand_shake     - Passes the data between 2 end points machines.
 *  ctx_print_pingpong_data - Prints the data that was passed. 
 *  ctx_close_connection    - Closing the sockets interface.
 */

#ifndef PERFTEST_RESOURCES_H
#define PERFTEST_RESOURCES_H


// Files included for work.
#include <infiniband/verbs.h>

// Connection types availible.
#define RC  0
#define UC  1 
#define UD  2
// #define XRC 3

#define CYCLE_BUFFER        4096
#define CACHE_LINE_SIZE     64
#define MAX_INLINE	    	400

// Outstanding reads for "read" verb only.
#define MAX_OUT_READ_HERMON 16
#define MAX_OUT_READ        4
#define MAX_SEND_SGE        1
#define MAX_RECV_SGE        1
#define DEF_WC_SIZE         1
#define CQ_MODERATION       50
#define MTU_FIX				7

// Space for GRH when we scatter the packet in UD.
#define UD_ADDITION         40
#define PINGPONG_SEND_WRID  60
#define PINGPONG_RDMA_WRID	3
#define PINGPONG_READ_WRID	1
#define DEF_QKEY            0x11111111
#define ALL 		        1

#define KEY_MSG_SIZE 	    50     // Message size without gid.
#define KEY_MSG_SIZE_GID    98 // Message size with gid (MGID as well).

// The Format of the message we pass through sockets , without passing Gid.
#define KEY_PRINT_FMT "%04x:%04x:%06x:%06x:%08x:%016Lx"

// The Format of the message we pass through sockets (With Gid).
#define KEY_PRINT_FMT_GID "%04x:%04x:%06x:%06x:%08x:%016Lx:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"

// The Basic print format for all verbs.
#define BASIC_ADDR_FMT " %s address: LID %#04x QPN %#06x PSN %#06x"

// Addition format string for READ - the outstanding reads.
#define READ_FMT       " OUT %#04x"

// The print format of the pingpong_dest element for RDMA verbs.
#define RDMA_FMT       " RKey %#08x VAddr %#016Lx"

// The print format of a global address or a multicast address.
#define GID_FMT " %s: %02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d\n"

// End of Test
#define RESULT_LINE "------------------------------------------------------------------\n"

// The format of the results
#define RESULT_FMT     " #bytes     #iterations    BW peak[MB/sec]    BW average[MB/sec]\n"

#define RESULT_FMT_LAT " #bytes #iterations    t_min[usec]    t_max[usec]  t_typical[usec]\n"

// Result print format
#define REPORT_FMT     " %-7d    %d           %-7.2f            %-7.2f\n"

// Result print format for latency tests.
#define REPORT_FMT_LAT " %-7d %d          %-7.2f        %-7.2f      %-7.2f\n"

// Macro for allocating.
#define ALLOCATE(var,type,size)                                  \
    { if((var = (type*)malloc(sizeof(type)*(size))) == NULL)     \
        { fprintf(stderr," Cannot Allocate\n"); exit(1);}}

#define NOTIFY_COMP_ERROR_SEND(wc,scnt,ccnt)                     											\
	{ fprintf(stderr," Completion with error at client\n");      											\
	  fprintf(stderr," Failed status %d: wr_id %d syndrom 0x%x\n",wc.status,(int) wc.wr_id,wc.vendor_err);	\
	  fprintf(stderr, "scnt=%d, ccnt=%d\n",scnt, ccnt);	return 1;} 

#define NOTIFY_COMP_ERROR_RECV(wc,rcnt)                     											    \
	{ fprintf(stderr," Completion with error at server\n");      											\
	  fprintf(stderr," Failed status %d: wr_id %d syndrom 0x%x\n",wc.status,(int) wc.wr_id,wc.vendor_err);	\
	  fprintf(stderr, "rcnt=%d\n",rcnt); return 1;} 

// Macro to determine packet size in case of UD. 
// The UD addition is for the GRH .
#define SIZE(type,size) ((type == UD) ? (size + UD_ADDITION) : (size))

// Macro to define the buffer size (according to "Nahalem" chip set).
// for small message size (under 4K) , we allocate 4K buffer , and the RDMA write
// verb will write in cycle on the buffer. this improves the BW in "Nahalem" systems.
#define BUFF_SIZE(size) ((size < CYCLE_BUFFER) ? (CYCLE_BUFFER) : (size))

// UD addition to the buffer.
#define IF_UD_ADD(type) ((type == UD) ? (CACHE_LINE_SIZE) : (0))

// Macro that defines the adress where we write in RDMA.
// If message size is smaller then CACHE_LINE size then we write in CACHE_LINE jumps.
#define INC(size) ((size > CACHE_LINE_SIZE) ? ((size%CACHE_LINE_SIZE == 0) ?  \
	       (size) : (CACHE_LINE_SIZE*(size/CACHE_LINE_SIZE+1))) : (CACHE_LINE_SIZE))

#define MTU_SIZE(mtu_ind) ((1 << (MTU_FIX + mtu_ind)))

// The Verb of the benchmark.
typedef enum { SEND , WRITE , READ } VerbType;

// The type of the machine ( server or client actually).
typedef enum { SERVER , CLIENT } MachineType;

// The type of the machine ( server or client actually).
typedef enum { LOCAL , REMOTE } PrintDataSide;

/******************************************************************************
 * Perftest resources Structures and data types.
 ******************************************************************************/

struct perftest_parameters {
	int connection_type;
	int tx_depth;
	int rx_depth;
	int inline_size;
	int qp_timeout;
	int gid_index;
	int port;
	int ib_port;
	int use_event;
    int use_mcg;
	int sockfd;
	int signal_comp;
	int num_of_qps;
	int iters;
	int out_reads;
	int duplex;
	int sl;
	int mtu;
	uint8_t link_type;
	enum ibv_mtu curr_mtu;
    MachineType machine;
    PrintDataSide side;
	VerbType verb;
};

struct pingpong_dest {
	int lid;
	int out_reads;
	int qpn;
	int psn;  
	unsigned rkey;
	unsigned long long vaddr;
	union ibv_gid gid;
};

/****************************************************************************** 
 * Perftest resources Methods and interface utilitizes.
 ******************************************************************************/
const char *link_layer_str(uint8_t link_layer);
/* 
 *
 * Description : Determines the link layer type (IB or ETH).
 *
 * Parameters : 
 *
 * Value : 0 upon success. -1 if it fails.
 */
struct ibv_device* ctx_find_dev(const char *ib_devname);

int ctx_set_mtu(struct ibv_context *context,struct perftest_parameters *params);

/* ctx_set_link_layer.
 *
 * Description : Determines the link layer type (IB or ETH).
 *
 * Parameters : 
 *
 *  context - The context of the HCA device.
 *  params  - The perftest parameters of the device.
 *
 * Return Value : 0 upon success. -1 if it fails.
 */
int ctx_set_link_layer(struct ibv_context *context,struct perftest_parameters *params);

/* ctx_set_link_layer.
 *
 * Description : Determines the link layer type (IB or ETH).
 *
 * Parameters : 
 *
 *  context - The context of the HCA device.
 *  params  - The perftest parameters of the device.
 *
 * Return Value : 0 upon success. -1 if it fails.
 */
int ctx_set_link_layer(struct ibv_context *context,struct perftest_parameters *params);

/* ctx_cq_create.
 *
 * Description : 
 *
 *	Creates a QP , according to the attributes given in param.
 *	The relevent attributes are tx_depth,rx_depth,inline_size and connection_type.
 *
 * Parameters :
 *
 *	pd      - The Protection domain , each the qp will be assigned to.
 *	send_cq - The CQ that will produce send CQE.
 *	recv_qp - The CQ that will produce recv CQE.
 *	param   - The parameters for the QP.
 *
 * Return Value : Adress of the new QP.
 */
struct ibv_cq* ctx_cq_create(struct ibv_context *context, 
							 struct ibv_comp_channel *channel,
							 struct perftest_parameters *param);

/* ctx_qp_create.
 *
 * Description : 
 *
 *	Creates a QP , according to the attributes given in param.
 *	The relevent attributes are tx_depth,rx_depth,inline_size and connection_type.
 *
 * Parameters :
 *
 *	pd      - The Protection domain , each the qp will be assigned to.
 *	send_cq - The CQ that will produce send CQE.
 *	recv_qp - The CQ that will produce recv CQE.
 *	param   - The parameters for the QP.
 *
 * Return Value : Adress of the new QP.
 */
struct ibv_qp* ctx_qp_create(struct ibv_pd *pd,
							 struct ibv_cq *send_cq,
							 struct ibv_cq *recv_cq,
							 struct perftest_parameters *param);

/* ctx_modify_qp_to_init.
 *
 * Description :
 *
 *	Modifies the given QP to INIT state , according to attributes in param.
 *  The relevent attributes are ib_port, connection_type and verb. 
 *
 * Parameters :
 *
 *	qp     - The QP that will be moved to INIT.
 *	param  - The parameters for the QP.
 *
 * Return Value : 0 if success , 1 otherwise.
 *
 */
int ctx_modify_qp_to_init(struct ibv_qp *qp,struct perftest_parameters *param);

/* ctx_get_local_lid .
 *
 * Description :
 *
 *  This method find and returns the local Id in IB subnet manager of
 *  the selected port and HCA given.The lid identifies the port.
 *
 * Parameters : 
 *
 *  context - the context of the HCA device.
 *  ib_port - The port of the HCA (1 or 2).
 *
 * Return Value : The Lid itself. (No error values).
 */
uint16_t ctx_get_local_lid(struct ibv_context *context,int ib_port);

/* ctx_set_out_reads.
 *
 * Description :
 *
 *  This Method is used in READ verb.
 *  it sets the outstanding reads number according to the HCA gen.
 *
 * Parameters : 
 *
 *  context        - the context of the HCA device.
 *  num_user_reads - The num of outstanding reads the user requested 
 *					 ( 0 if he/she didn't requested ) and then MAX is selected. 
 *
 * Return Value : The number of outstanding reads , -1 if query device failed.
 */
int ctx_set_out_reads(struct ibv_context *context,int num_user_reads);

/* ctx_client_connect .
 *
 * Description :
 *
 *  Connect the client the a well known server to a requested port.
 *  It assumes the Server is waiting for request on the port.
 *
 * Parameters : 
 *
 *  servername - The server name (according to DNS) or IP.
 *  port       - The port that the server is listening to.
 *
 * Return Value : The file descriptor selected in the PDT for the socket.
 */
int ctx_client_connect(const char *servername, int port);

/* ctx_server_connect .
 *
 * Description :
 *
 *  Instructs a machine to listen on a requested port.
 *  when running this command the machine will wait for 1 client to
 *  contant it , on the selected port , with the ctx_client_connect method.
 *
 * Parameters : 
 *
 *  port - The port which the machine will listen.
 *
 * Return Value : The new file descriptor selected in the PDT for the socket.
 */
int ctx_server_connect(int port);

/* ctx_hand_shake .
 *
 * Description :
 *
 *  Exchanging the data , represented in struct pingpong_dest , between 
 *  a server and client that performed the ctx_server/clinet_connect.
 *  The method fills in rem_dest the remote machine data , and passed the data
 *  in my_dest to other machine.  
 *
 * Parameters : 
 *
 *  params   - The parameters needed for this method. Are mentioned above ,and
 *             contains standard IB info. (exists on perftest).
 *  my_dest  - Contains the data you want to pass to the other side.
 *  rem_dest - The other side data.
 *
 * Return Value : 0 upon success. -1 if it fails.
 */
int ctx_hand_shake(struct perftest_parameters *params,
				   struct pingpong_dest *my_dest,
				   struct pingpong_dest *rem_dest);


/* ctx_print_pingpong_data.
 *
 * Description :
 *
 *  Prints the data stored in the struct pingpong_dest.
 *
 * Parameters : 
 *
 *  params  - The parameters of the machine.
 *  element - The element to print.           
 */
void ctx_print_pingpong_data(struct pingpong_dest *element, 
							 struct perftest_parameters *params);

/* 
 * Description :
 *
 *  
 *
 * Parameters : 
 *
 *         
 */
inline int ctx_notify_events(struct ibv_cq *cq,struct ibv_comp_channel *channel);

/* increase_rem_addr.
 *
 * Description : 
 *	Increases the remote address in RDMA verbs by INC , 
 *  (at least 64 CACHE_LINE size) , so that the system 
 *
 */
inline void increase_rem_addr(struct ibv_send_wr *wr,int size,int scnt,uint64_t prim_addr);

/* increase_loc_addr.
 *
 * Description : 
 *	 
 *
 */
inline void increase_loc_addr(struct ibv_sge *sg,int size,int rcnt,
							  uint64_t prim_addr,int server_is_ud);

/* ctx_close_connection .
 *
 * Description :
 *
 *  Close the connection between the 2 machines.
 *  It performs an handshake to ensure the 2 sides are there.
 *
 * Parameters : 
 *
 *  params            - The parameters of the machine
 *  my_dest ,rem_dest - The 2 sides that ends the connection.
 *
 * Return Value : 0 upon success. -1 if it fails.
 */
int ctx_close_connection(struct perftest_parameters  *params,
				         struct pingpong_dest *my_dest,
				         struct pingpong_dest *rem_dest);


#endif /* PERFTEST_RESOURCES_H */
