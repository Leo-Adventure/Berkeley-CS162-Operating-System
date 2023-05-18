/*

Copyright © 2019 University of California, Berkeley

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

word_count provides lists of words and associated count

Functional methods take the head of a list as first arg.
Mutators take a reference to a list as first arg.
*/

#include "word_count.h"

/* Basic utilities */
// copy str
char *new_string(char *str) {
  char *new_str = (char *) malloc(strlen(str) + 1);
  if (new_str == NULL) {
    return NULL;
  }
  return strcpy(new_str, str);
}


int init_words(WordCount **wclist) {
  /* Initialize word count.
     Returns 0 if no errors are encountered
     in the body of this function; 1 otherwise.
  */
  // wclist = (WordCount**)malloc(sizeof(WordCount*));
  // memset(wclist, 0, sizeof(WordCount*));
  // if(wclist == NULL){
  //   return 1;
  // }
  // printf("Out init_words\n");
  *wclist = NULL;
  return 0;
}

ssize_t len_words(WordCount *wchead) {
  /* Return -1 if any errors are
     encountered in the body of
     this function.
  */

  size_t len = 0;
  if(wchead == NULL){
    return len;
  }

  WordCount *wc =wchead;
  while(wc){
    len++;
    wc = wc->next;
  }
  return len;
}

WordCount *find_word(WordCount *wchead, char *word) {
  /* Return count for word, if it exists */
  // printf("In find_word\n");
  WordCount *res = NULL;
  WordCount *wc = wchead;
  while(wc != NULL){
    if(strcmp(wc->word, word) == 0){
      res = wc;
      break;
    }
    wc = wc->next;
  }
  return res;
}

int add_word(WordCount **wclist, char *word) {
  /* If word is present in word_counts list, increment the count.
     Otherwise insert with count 1.
     Returns 0 if no errors are encountered in the body of this function; 1 otherwise.
  */
  // printf("In add_word\n");
  if(*wclist == NULL){
    *wclist = (WordCount*)malloc(sizeof(WordCount));
    if(*wclist == NULL){
      return 1;
    }
    (*wclist)->count = 1;
    (*wclist)->word = new_string(word);
    (*wclist)->next = NULL;
    return 0;
  }

  WordCount *wc = find_word(*wclist, word);

  if(wc == NULL){
    // printf("wc is null\n");    
    WordCount *new_wc = NULL;
    new_wc = (WordCount*)malloc(sizeof(WordCount));
    if(new_wc == NULL){
      return 1;
    }
    memset(new_wc, 0, sizeof(WordCount));
    
    new_wc->count = 1;
    new_wc->word = new_string(word);
    new_wc->next = NULL;

    if(new_wc->word == NULL){
      return 1;
    }
    WordCount *head = *wclist;
    while(head->next){
      head = head -> next;
    }
    head -> next = new_wc;
  }else{
    wc->count++;
  }
 return 0;
}

void fprint_words(WordCount *wchead, FILE *ofile) {
  /* print word counts to a file */
  WordCount *wc;
  for (wc = wchead; wc; wc = wc->next) {
    fprintf(ofile, "%i\t%s\n", wc->count, wc->word);
  }
}

