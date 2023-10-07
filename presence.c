#include "project05.h"


void update_users(struct users *cs_users, struct user *u, int fd){

	for (int i = 0; i < cs_users->count; i++){
		if(strcmp(cs_users->userlist[i].username, u->username) == 0){
			if(strcmp(cs_users->userlist[i].status, u->status) != 0) {
				printf("%s: %s\n", u->status, cs_users->userlist[i].username);
				strcpy(cs_users->userlist[i].status, u->status);
				break;
			}
		}
	}	
}


void print_user(struct users *cs_users, struct user *u){
	for (int i = 0; i < cs_users->count; i++){
		if(strcmp(cs_users->userlist[i].username, u->username) == 0){
			printf("%s: %s\n", u->status, cs_users->userlist[i].username);
			break;
		}			
	}
}


char *get_port(struct users *cs_users, struct user *u){
	for (int i = 0; i < cs_users->count; i++){
		if(strcmp(cs_users->userlist[i].username, u->username) == 0){
			return cs_users->userlist[i].port;
			break;
		}		
	}
	return NULL;
}


char *get_hostname(struct users *cs_users, struct user *u){
	for (int i = 0; i < cs_users->count; i++){
		if(strcmp(cs_users->userlist[i].username, u->username) == 0){
			return cs_users->userlist[i].hostname;
			break;
		}		
	}
	return NULL;
}	


char *get_username(struct users *cs_users, struct user *u){
	for (int i = 0; i < cs_users->count; i++){
		if(strcmp(cs_users->userlist[i].hostname, u->hostname) == 0){
			return cs_users->userlist[i].username;
			break;
		}		
	}
	return NULL;
}	


int presence_setup() {
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	
	struct addrinfo *results;
	int e = getaddrinfo(NULL, "8221", &hints, &results);
	if (e != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(e));
		exit(-1);
	}

	int fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	if (fd == -1)
		fatalp("udp socket");

	int enable = 1;

	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int)) != 0)
		fatalp("setsockopt broadcast");

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
		fatalp("setsockopt reuseaddr");

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) != 0)
		fatalp("setsockopt reuseport");

	if (bind(fd, results->ai_addr, results->ai_addrlen) != 0)
		fatalp("udp bind");

	freeaddrinfo(results);
	return fd;
}


void presence_read(int fd, struct user *u, struct users *cs_users){
	struct sockaddr_storage stg;
	unsigned int size = 128;
	char buf[64];
	buf[0] = '\0';
	bool inList = false;
	
	recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *) &stg, &size);
	sscanf(buf, "%s %s %s", u->username, u->status, u->port);

	char service[NI_MAXSERV];
	int rc = getnameinfo((struct sockaddr *) &stg, sizeof(stg), u->hostname, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
	if (rc != 0)
		gai_strerror(rc);

	for (int i = 0; i < cs_users->count; i++){
		if(strcmp(cs_users->userlist[i].username, u->username) == 0){
			inList = true;
			break;	
		}
	}

	if(inList == false){
		cs_users->userlist[cs_users->count] = *u;
		cs_users->count++;
		print_user(cs_users, u);
	}

	buf[0] = '\0';
}


void presence_write(int fd, struct user *u){
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8221);
	inet_pton(addr.sin_family, "10.10.13.255", &addr.sin_addr);

	char buffer[64] = "";
	strcat(buffer, u->username);
	strcat(buffer, " ");
	strcat(buffer, u->status);
	strcat(buffer, " ");
	strcat(buffer, u->port);
	buffer[strlen(buffer)] = '\0';
	
	sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr, sizeof(addr));
	buffer[0] = '\0';

	
}
