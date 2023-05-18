/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"

typedef struct count_words_p
{
  word_count_list_t *wclist;
  char *filename;
} count_words_p;

void *threadfun(void *wc)
{
  count_words_p *count = (count_words_p *)wc;
  char *filename = count->filename;
  // printf("The file name is %s\n", filename);
  word_count_list_t *wclist = count->wclist;
  FILE *infile = fopen(filename, "r");
  count_words(wclist, infile);
  fclose(infile);
  pthread_exit(NULL);
}

/*
 * main - handle command line, spawning one thread per file.
 */

int main(int argc, char *argv[])
{
  /* Create the empty data structure. */
  word_count_list_t word_counts;
  init_words(&word_counts);

  if (argc <= 1)
  {
    /* Process stdin in a single thread. */
    count_words(&word_counts, stdin);
  }
  else
  {
    /* TODO */
    int rc;
    int i, j;

    int nthreads = argc - 1;
    // printf("nthreads = %d\n", nthreads);
    pthread_t threads[nthreads];

    for (i = 0; i < nthreads; i++)
    {
      count_words_p *wc = (count_words_p *)malloc(sizeof(count_words_p));
      if (wc == NULL)
      {
        printf("ERROR: wc = NULL in malloc()\n");
      }
      wc->filename = argv[i + 1];
      wc->wclist = &word_counts;
      rc = pthread_create(&threads[i], NULL, threadfun, (void *)wc);
      if (rc)
      {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
      // pthread_join(threads[i], NULL);
    }
    for (j = 0; j < nthreads; j++)
    {
      pthread_join(threads[j], NULL);
    }
    //
  }

  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  pthread_exit(NULL);
  return 0;
}
