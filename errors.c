#include "project05.h"

void fatalp(char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

void fatal_str(char *str) {
	fprintf(stderr, "%s\n", str);
	exit(EXIT_FAILURE);
}
