/**
 * Server binary.
 */

#include "kv_store.h"
#include <glib.h>
#include <memory.h>
#include <netinet/in.h>
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#ifndef SIG_PF
#define SIG_PF void (*)(int)
#endif

/* TODO: Add global state. */
GHashTable *ht;

void init(){
  ht = g_hash_table_new(g_bytes_hash, g_bytes_equal);
}

void add(buf key, buf val){
  GBytes *g_key = g_bytes_new(key.buf_val, key.buf_len);
  GBytes *g_value = g_bytes_new(val.buf_val, val.buf_len);
  g_hash_table_insert(ht, g_key, g_value);
}

buf lookup(GetArgs *key){
  GBytes *g_key = g_bytes_new(key->key.buf_val, key->key.buf_len);
  GBytes *g_value = g_hash_table_lookup(ht, g_key);
  g_bytes_unref(g_key);
  buf result;
  if (g_value != NULL) {
    gsize value_len;
    const char *data_ptr = (const char *)g_bytes_get_data(g_value, &value_len);
    result.buf_val = (char *)malloc(value_len + 1); // Allocate one extra byte for the null terminator
    if (result.buf_val == NULL) {
      result.buf_len = 0;
      return result;
    }
    memcpy(result.buf_val, data_ptr, value_len);
    result.buf_val[value_len] = '\0'; // Add null terminator
    result.buf_len = value_len;
  } else {
    result.buf_val = NULL;
    result.buf_len = 0;
  }

  return result;
}


extern void kvstore_1(struct svc_req *, struct SVCXPRT *);

/* Set up and run RPC server. */
int main(int argc, char **argv) {
  register SVCXPRT *transp;

  pmap_unset(KVSTORE, KVSTORE_V1);

  transp = svcudp_create(RPC_ANYSOCK);
  if (transp == NULL) {
    fprintf(stderr, "%s", "cannot create udp service.");
    exit(1);
  }
  if (!svc_register(transp, KVSTORE, KVSTORE_V1, kvstore_1, IPPROTO_UDP)) {
    fprintf(stderr, "%s", "unable to register (KVSTORE, KVSTORE_V1, udp).");
    exit(1);
  }

  transp = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (transp == NULL) {
    fprintf(stderr, "%s", "cannot create tcp service.");
    exit(1);
  }
  if (!svc_register(transp, KVSTORE, KVSTORE_V1, kvstore_1, IPPROTO_TCP)) {
    fprintf(stderr, "%s", "unable to register (KVSTORE, KVSTORE_V1, tcp).");
    exit(1);
  }

  /* TODO: Initialize state. */
  init();

  svc_run();
  fprintf(stderr, "%s", "svc_run returned");
  exit(1);
  /* NOTREACHED */
}

/* Example server-side RPC stub. */
int *example_1_svc(int *argp, struct svc_req *rqstp) {
  static int result;

  result = *argp + 1;

  return &result;
}

/* TODO: Add additional RPC stubs. */

char** echo_1_svc(char** argp, struct svc_req *rqstp) {
  static char* result;
  result = *argp;
  if(result == NULL){
    return NULL;
  }
  return &result;
}


void *put_1_svc(PutArgs *argp, struct svc_req *rqstp){
  static void* result;
  add(argp->key, argp->value);
  return &result;
}



buf *get_1_svc(GetArgs *argp, struct svc_req *rqstp){
  static buf result;

  if (result.buf_val != NULL) {
    free(result.buf_val);
  }

  result = lookup(argp);

  return &result;
}




