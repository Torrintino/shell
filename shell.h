#ifndef SHELL_H
#define SHELL_H

void loop();
char* read_line();
char** split_line(char* line);
int execute(char** args);

// Builtin commands
int sh_cd(char** args);
int sh_help(char** args);
int sh_exit(char** args);

#endif
