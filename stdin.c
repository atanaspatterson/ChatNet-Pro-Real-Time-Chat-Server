#include "project05.h"

int accum_chat_str(char *s, int len) {
	bool eof = false;
	char ch;
	int res;
	
	res = read(STDIN_FILENO, &ch, 1);
	switch (res) {
		case -1:
			fatalp("read");
		case 0:
			eof = true;
			break;
		case 1:
			s[len] = ch;
			break;
	}
	return eof;
}
