#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc
{
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
    {cmd_help, "?", "show this help menu"},
    {cmd_exit, "exit", "exit the command shell"},
    {cmd_pwd, "pwd", "print the current working directory"},
    {cmd_cd, "cd", "change the current working directory to the input directory"}};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens)
{
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) { exit(0); }

/*Print the current absolute path*/
int cmd_pwd(unused struct tokens *tokens)
{
  int size = 1024;
  char *path = (char *)calloc(1024, sizeof(path));
  if (path == NULL)
  {
    return -1;
  }

  getcwd(path, size);
  fprintf(stdout, "%s\n", path);
  free(path);
  return 0;
}

/*Change the working directory to the input directory*/
int cmd_cd(struct tokens *tokens)
{
  if (tokens == NULL)
  {
    return -1;
  }
  return chdir(tokens_get_token(tokens, 1));
}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[])
{
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell()
{
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive)
  {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN); // use the SIGTTIN signal to pause it

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int execute_single_process(char cmd[1024])
{
  // printf("cmd = %s\n", cmd);
  setpgid(getpid(), getpid());
  tcsetpgrp(0, getpid());
  signal(SIGINT, SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
  signal(SIGTSTP, SIG_DFL);
  signal(SIGCONT, SIG_DFL);
  signal(SIGTTIN, SIG_DFL);
  signal(SIGTTOU, SIG_DFL);
  struct tokens *tokens = tokenize(cmd);
  char **argv = (char **)calloc(1024, sizeof(char *));
  if (argv == NULL)
  {
    return -1;
  }
  /* REPLACE this to run commands as programs. */
  size_t len = tokens_get_length(tokens);
  bool has_input = 0;
  bool has_output = 0;
  char input[100];
  char output[100];
  int i = 0;
  for (; i < len; i++)
  {
    char *token = tokens_get_token(tokens, i);

    if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0)
    {
      break;
    }
    argv[i] = (char *)calloc((strlen(token) + 1), sizeof(char));
    strcpy(argv[i], token);
    // printf("argv[i] = %s\n", argv[i]);
  }

  argv[i] = NULL;
  for (; i < len; i++)
  {
    char *token = tokens_get_token(tokens, i);
    char *last_token = tokens_get_token(tokens, i - 1);
    if (strcmp(last_token, "<") == 0)
    {
      has_input = 1;
      strcpy(input, token);
    }
    else if (strcmp(last_token, ">") == 0)
    {
      has_output = 1;
      strcpy(output, token);
    }
  }

  if (has_input)
  {
    // printf("The input file is %s\n", input);
    int fd = open(input, O_RDONLY);
    dup2(fd, STDIN_FILENO);
    close(fd);
  }
  if (has_output)
  {
    // printf("The output file is %s\n", output);
    int fd = creat(output, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);
  }

  bool is_full_path = 0;
  int path_len = strlen(argv[0]);
  // fprintf(stdout, "The path(argv[0]) is %s\n", argv[0]);
  for (int i = 0; i < path_len; i++)
  {
    if (argv[0][i] == '/')
    {
      is_full_path = 1;
      break;
    }
  }
  // argv[0] is the path
  // store the full path and execv()
  if (is_full_path)
  {
    execv(argv[0], argv);
    // fprintf(stdout, "should not print this, then exit\n");
    exit(0);
  }
  else
  {
    // get the PATH environment variable
    char *PATH = getenv("PATH");
    char *context = NULL;
    char *token = strtok_r(PATH, ":", &context);
    char *abs_path = (char *)calloc(1024, sizeof(char));
    char name[100];
    strcpy(name, argv[0]);
    while (token != NULL)
    {
      strcpy(abs_path, token);
      strcat(abs_path, "/");
      strcat(abs_path, name);
      // fprintf(stdout, "The abs_path is %s\n", abs_path);
      argv[0] = abs_path;
      // printf("argv[0] = %s\n", argv[0]);
      execv(argv[0], argv);
      token = strtok_r(NULL, ":", &context);
    }
    // fprintf(stdout, "should not print this, then exit\n");
    exit(0);
  }
}

int execute_progs(char line[4096])
{
  int i = 0;
  int pos = 0;
  int pipe_id[2];
  while (line[i] != '\0')
  {
    if (line[i] == '|')
    {
      pos = i;
      break;
    }
    i++;
  }
  if (pos == 0)
  {
    execute_single_process(line);
  }
  else
  {
    char remain[4096];
    char cmd[1024];
    strncpy(cmd, line, pos);
    cmd[pos] = '\0';
    strcpy(remain, line + pos + 2);
    pipe(pipe_id);
    pid_t child_pid = fork();
    if (child_pid < 0)
    {
      printf("Error in forking\n");
      return -1;
    }
    else
    {
      
      if (child_pid == 0)
      { // child process
        close(pipe_id[0]);
        dup2(pipe_id[1], STDOUT_FILENO);
        execute_single_process(cmd);
      }else{
        close(pipe_id[1]);
        dup2(pipe_id[0], STDIN_FILENO);
        execute_progs(remain);
      }
    }
  }

  return 0;
}

int main(unused int argc, unused char *argv[])
{
  init_shell();

  setpgid(getpid(), getpid());

  tcsetpgrp(0, getpid());

  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGCONT, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin))
  {
    /* Split our line into words. */
    struct tokens *tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0)
    {
      cmd_table[fundex].fun(tokens);
    }
    else
    {

      pid_t child_pid = fork();
      // int child_status;
      if (child_pid > 0) // parent process
      {
        wait(NULL);
      }
      else // child process
      {
        execute_progs(line);
      }
    }

    if (shell_is_interactive)
    {
      /* Please only print shell prompts when standard input is not a tty */
      ++line_num;
    }
    /* Clean up memory */
    tokens_destroy(tokens);

    tcsetpgrp(0, getpid());
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCONT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
  }

  return 0;
}
