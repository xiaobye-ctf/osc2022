#ifndef __SHELL_H__
#define __SHELL_H__

void clear();
void boot_msg();

void cmd_input(char *cmd);
void cmd_resolve(char *cmd);

void help();
void hello();
void reboot(int tick);
void mailbox();
void ls();
void cat();

#endif