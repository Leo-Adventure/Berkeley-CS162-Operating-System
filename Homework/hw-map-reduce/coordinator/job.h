/**
 * Logic for job and task management.
 *
 * You are not required to modify this file.
 */

#ifndef JOB_H__
#define JOB_H__

/* You may add definitions here */
#include <stdbool.h>
#include <time.h>
#include "../lib/lib.h"

struct job {
  int job_id;           /* A unique job id. */
  time_t* map_time;     /* An array tracking each map task's assigning time. */
  int n_reduce;         /* The number of map tasks, which equals file number. */
  char** files;         /* Input file data. */
  
  int map_finished;     /* The number of finished map tasks. */
  bool* map_success;    /* An array tracking each map task's success status. */
  int reduce_finished;  /* The number of finished reduce tasks. */
  bool* reduce_success; /* An array tracking each reduce task's success status. */
  time_t* reduce_time;  /* An array tracking each reduce task's assigning time. */
  char* args;           /* Auxiliary arguments. */
char* output_dir;     /* Output directory. */
  char* app;            /* Application used to deal with data. */
  int n_map;           /* The number of map tasks, which equals file number. */
  bool done;            /* Track job finish information. */
  bool failed;          /* Track job success information. */
};
#endif
