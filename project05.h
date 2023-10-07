#ifndef _PROJECT05_H
#define _PROJECT05_H

#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


#define BUF_SIZE 128        // for strings
#define MAX_POLLS 64         // max number of pollfds
#define POLL_STDIN 0       	// index 0 of pollfds array
#define POLL_PRESENCE 1    	// index 1 of pollfds array
#define POLL_CHAT 2    		// index 2 of pollfds array
#define POLL_TIMEOUT 400	// milliseconds for poll() timeout
#define PORT "8056"			// my port

struct polls {
	struct pollfd polls[MAX_POLLS];
  	int num_polls;
};

struct user {
	char username[64];
	char status[64];
	char port[64];
	char hostname[64];
};

struct users {
	struct user userlist[60];
	int count;
};

void fatalp(char *str);
void fatal_str(char *str);

void poll_init(struct polls *p, struct pollfd *polls, int fd, int fd2);
void poll_setup(struct polls *p, int fd, int fd2);
bool poll_results(struct polls *p, int num_results, struct users *cs_users);
void insert_chat_fd(struct polls *p, int chat_fd);

void update_users(struct users *cs_users, struct user *u, int fd);
void print_user(struct users *cs_users, struct user *u);
char *get_port(struct users *cs_users, struct user *u);
char *get_hostname(struct users *cs_users, struct user *u);
char *get_username(struct users *cs_users, struct user *u);
int presence_setup();
void presence_read(int fd, struct user *u, struct users *cs_users);
void presence_write(int fd, struct user *u);

int accum_chat_str(char *s, int len);

int chat_setup(char *port);
int read_message(int fd, struct users *cs_users);
int send_message(char *string, struct users *cs_users);
void create_message(char *string, char *name, char *message);

extern bool verbose;
#define VERBOSE(s) if(verbose)printf("  %s\n", s)

#endif
