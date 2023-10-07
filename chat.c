#include "project05.h"

int chat_setup(char *port) {
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo *results;
	int e = getaddrinfo(NULL, port, &hints, &results);
	if (e != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(e));
		exit(-1);
	}

	int fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	if (fd == -1)
		fatalp("tcp socket");

	int enable = 1;
	int n0 = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if(n0 != 0)
		fatalp("tcp setsockopt reuseaddr");

	int n1 = ioctl(fd, FIONBIO, (char *) &enable);
	if(n1 != 0)
		fatalp("tcp ioctl");

	int n2 = bind(fd, results->ai_addr, results->ai_addrlen);	
	if(n2 != 0)
		fatalp("tcp bind");

	int n3 = listen(fd, MAX_POLLS);
	if(n3 != 0)
		fatalp("tcp listen");

	return fd;
}


int read_message(int fd, struct users *cs_users){
	char buf[64];
	char host[64];
	char user[64];
	struct user u;
	struct sockaddr_storage peer;
	unsigned int peer_size = sizeof(peer);
		
	int n = recv(fd, &buf, sizeof(buf), 0);
	if (n == -1)
		fatalp("tcp recv");
	if (n == 0)
		return 0;

	int n2 = getpeername(fd, (struct sockaddr*) &peer, &peer_size);
		if (n2 != 0)
		gai_strerror(n2);

	char service[NI_MAXSERV];
	int n3 = getnameinfo((struct sockaddr*) &peer, peer_size, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
		if (n3 != 0)
		gai_strerror(n2);

	strcpy(u.hostname, host);
	strcpy(user, get_username(cs_users, &u));
	
	printf("%s says:%s\n", user, buf);
	return 1;
}


void create_message(char *string, char *name, char* message){
	string++;
	char copy[64];
	strcpy(copy, string);

	for (size_t i = 0; i < strlen(string); i++) {
		if(string[i] == ':') {
			copy[i] = '\0';
			strcpy(name, copy);
			string = string + i + 2;
			break;
		}
	}
	strcpy(message, string);
}


int send_message(char *string, struct users *cs_users){
	if (string[0] != '@')
		return -1;
		
	char name[64];
	char message[64];

	create_message(string, name, message);
	
	message[strlen(message)] = '\0';
	name[strlen(name)] = '\0';	

	struct user u;
	char port[64];
	char hostname[64];
	strcpy(u.username, name);
	strcpy(port, get_port(cs_users, &u));
	strcpy(hostname, get_hostname(cs_users, &u));

	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;
	
	struct addrinfo *results;
	int e = getaddrinfo(hostname, port, &hints, &results); //null will be replaced by the host
	if (e != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(e));
		exit(-1);
	}
	
	int fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	if (fd == -1)
		fatalp("tcp socket");

	if (connect(fd, results->ai_addr, results->ai_addrlen) == -1)
		fatalp("tcp connect");

	int s = send(fd, message, sizeof(message), 0);
	if (s == -1)
		fatalp("tcp connect");	
}
