#include "project05.h"

void poll_init(struct polls *p, struct pollfd *polls, int fd, int fd2) {		
	for (int i = 0; i < MAX_POLLS; i++) {	
		if (i == 0)
			polls[i].fd = STDIN_FILENO;
		else if (i == 1)
			polls[i].fd = fd;
		else if (i == 2)
			polls[i].fd = fd2;
		else
			polls[i].fd = -1;
								
		polls[i].events = POLLIN;
		polls[i].revents = 0;
		p->num_polls++;
		
	}
}


void poll_setup(struct polls *p, int fd, int fd2) {
	p->num_polls = 0;
	poll_init(p, p->polls, fd, fd2);
}


void insert_chat_fd(struct polls *p, int chat_fd) {
	for (int i = 3; i < MAX_POLLS; i++) {
		if (p->polls[i].fd == chat_fd)
			break;
			
		if (p->polls[i].fd == -1) {
			p->polls[i].fd = chat_fd;
			break;
		}
	}
}


bool poll_results(struct polls *p, int num_results, struct users *cs_users) {
	static char char_str[BUF_SIZE];
	static int chat_len = 0;
	int n;
	
	bool eof = false;
	
	for (int i = 0; i < p->num_polls; i++) {
		if (!(p->polls[i].revents & POLLIN))
			continue;

		
			
		num_results--;
		switch (i) {
			case POLL_STDIN:

				eof = accum_chat_str(char_str, chat_len);

				if (char_str[chat_len] == '\n') {
					char_str[chat_len + 1] = '\0';
					n = send_message(char_str, cs_users);
					if (n == -1) {
						printf("wrong format, message not sent\n");
						printf("format: @username: message\n");
					}
					chat_len = 0;
					char_str[0] = '\0';
				} else {
					chat_len++;
				}
				break;
				
			case POLL_PRESENCE:
				struct user u;
				presence_read(p->polls[i].fd, &u, cs_users);
				update_users(cs_users, &u, p->polls[i].fd);
				break;
				
			case POLL_CHAT:
				int chat_fd = accept(p->polls[i].fd, NULL, NULL);
				insert_chat_fd(p,chat_fd);
				break;
				
			default:
				int num = read_message(p->polls[i].fd, cs_users);
				if (num == 0)
					p->polls[i].fd = -1;
				break;
			
		}
		
		if (num_results == 0)
			break;
	}
	return eof;
}
