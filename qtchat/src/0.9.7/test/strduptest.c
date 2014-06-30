/* char *strdup(const char *); */

extern "C" {
#include <string.h>
}

int main(int argc, char *argv[]) {
	char *cp;
	cp = strdup("test");
}
