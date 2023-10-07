#include "project05.h"

bool verbose = false;

int main(int argc, char *argv[]) {
	int fd = presence_setup();
	int fd2 = chat_setup(argv[2]);

	struct users cs_users;
	cs_users.count = 0;

	struct user my_u;
	strcpy(my_u.status, "online");
	strcpy(my_u.username, argv[1]);
	strcpy(my_u.port, argv[2]);
	cs_users.userlist[0] = my_u;
	cs_users.count++;

	struct polls p;
	poll_setup(&p, fd, fd2);
	int counter = 0;
	int threshold = 2;
	bool eof = false;

	presence_write(fd, &my_u);
	print_user(&cs_users, &my_u);
	
	while (!eof) {
		int num_readable = poll(p.polls, p.num_polls, POLL_TIMEOUT);
		if(!(p.polls[2].fd & POLLIN) == 0)
			printf("chatfd is readable!\n");
		

		switch(num_readable) {
			case 0:
				break;
			case -1:
				fatalp("poll");
				break;
			default:
				VERBOSE("poll");
				eof = poll_results(&p, num_readable, &cs_users);
		}

		if (counter >= threshold) {
			presence_write(fd, &my_u);
			counter = 0;
		} else {
			counter++;
		}
	}

	strcpy(my_u.status, "offline");
	presence_write(fd, &my_u);
	
	for (int i = POLL_STDIN + 1; i < p.num_polls; i++) {
		int fd = p.polls[i].fd;
		if (fd != -1)
			close(fd);
	}

	return 0;
}
