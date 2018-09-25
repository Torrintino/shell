#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

int main(int argc, char** argv) {
  loop();

  return EXIT_SUCCESS;
}

void loop() {
  char* line;
  char** args;
  int status;

  do {
    printf("> ");
    line = read_line();
    args = split_line(line);
    status = execute(args);

    free(line);
    free(args);
  } while(status);
}

#define RL_BUFFER_SIZE 1024
char* read_line() {
  int buffer_size = RL_BUFFER_SIZE;
  int position = 0;
  char* buffer = malloc(sizeof(char) * buffer_size);
  int c;

  if(!buffer) {
    fprintf(stderr, "sh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while(1) {
    c = getchar();

    if(c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    if(position >= buffer_size) {
      buffer_size += RL_BUFFER_SIZE;
      buffer = realloc(buffer, buffer_size * sizeof(char));
      if(!buffer) {
	fprintf(stderr, "sh: allocation error\n");
	exit(EXIT_FAILURE);
      }
    }
  }
}

#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"
char** split_line(char* line) {
  int buffer_size = TOKEN_BUFFER_SIZE;
  int position = 0;

  char** tokens = malloc(buffer_size * sizeof(char*));
  char* token;

  if(!tokens) {
    fprintf(stderr, "sh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line , TOKEN_DELIMITER);
  while(token != NULL) {
    tokens[position] = token;
    position++;

    if(position >= buffer_size) {
      buffer_size += TOKEN_BUFFER_SIZE;
      tokens = realloc(tokens, buffer_size * sizeof(char*));
      if(!tokens) {
	fprintf(stderr, "sh: allocation error\n");
	exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOKEN_DELIMITER);
  }
  tokens[position] = NULL;
  return tokens;
}

int launch(char** args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if(pid == 0) {
    // Child process
    if(execvp(args[0], args) == -1)
      perror("sh");
    exit(EXIT_FAILURE);
  } else if(pid < 0) {
    // Error forking
    perror("sh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

char* builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char**) = {
  &sh_cd,
  &sh_help,
  &sh_exit
};

int num_builtins() {
  return sizeof(builtin_str) / sizeof(char*);
}

int sh_cd(char** args) {
  if(args[1] == NULL) {
    fprintf(stderr, "sh: expected argument to \"cd\"\n");
  } else {
    if(chdir(args[1]) != 0) {
      perror("sh");
    }
  }
  return 1;
}

int sh_help(char** args) {
  printf("This is work in progress\n");
  printf("The following commands are built in:\n");
  for(int i=0; i < num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  return 1;
}

int sh_exit(char** args) {
  return 0;
}

int execute(char** args) {
  if (args[0] == NULL) {
    return 1;
  }

  for(int i=0; i < num_builtins(); i++) {
    if(strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}
