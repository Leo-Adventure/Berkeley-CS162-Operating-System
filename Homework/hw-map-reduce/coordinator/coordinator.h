/**
 * The MapReduce coordinator.
 */

#ifndef H1_H__
#define H1_H__
#include "../rpc/rpc.h"
#include "../lib/lib.h"
#include "../app/app.h"
#include "job.h"
#include <glib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
typedef struct {
  int next_id;          /* Initialized to be 0. */
  GList* waiting_queue; /* Initialized to be NULL, store unfinished job id. */
} coordinator;

void coordinator_init(coordinator** coord_ptr);
void init_task_reply(get_task_reply* reply, struct job* job, int task, bool reduce);
void free_job_memory(struct job* job);
#endif
