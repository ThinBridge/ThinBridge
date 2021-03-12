/*
 * "DummyTalker.exe" is a simple program that produces a fake
 *  commands. Use this program for debugging.
 *
 *  C:> DummyTalker.exe "C chrome" | ThinBridgeTalk.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int error(char *msg)
{
	fprintf(stderr, "error: %s\n", msg);
	fprintf(stderr, "usage: DummyTalker.exe <command>\n");
	return -1;
}

int main(int argc, char *argv[])
{
	char *url;
	int len;

	if (argc < 2)
	    return error("missing argument");

	url = argv[1];
	len = strlen(url) + 2;

	fwrite(&len, sizeof(len), 1, stdout);
	fprintf(stdout, "\"%s\"", url);
	fflush(stdout);
	return 0;
}
