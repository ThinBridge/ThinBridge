#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>
#include "internal.h"

/*
 * A simple string buffer implementation.
 */
void *xmalloc(int size)
{
	void *buf = malloc(size);
	if (buf == NULL) {
	    fprintf(stderr, "malloc: out of memory (%i)", errno);
	    exit(98);
	}
	return buf;
}

void *xrealloc(char *ptr, int size)
{
	void *buf = realloc(ptr, size);
	if (buf == NULL) {
	    fprintf(stderr, "realloc: out of memory (%i)", errno);
	    exit(99);
	}
	return buf;
}

void strbuf_putchar(struct strbuf *sb, char chr)
{
	if (sb->alloc - sb->count < 2) {
	    sb->buf = xrealloc(sb->buf, alloc_nr(sb->alloc) * sizeof(char));
	    sb->alloc = alloc_nr(sb->alloc);
	}
	sb->buf[sb->count++] = chr;
	sb->buf[sb->count] = '\0';
}

void strbuf_concat(struct strbuf *sb, char *str)
{
	while (*str)
	    strbuf_putchar(sb, *str++);
}

void strbuf_concat_jsonstr(struct strbuf *sb, char *str, int len)
{
	char *end = str + len;

	strbuf_putchar(sb, '"');
	while (str < end) {
	    switch (*str) {
	        case '\\': strbuf_concat(sb, "\\\\"); break;
	        case '\"': strbuf_concat(sb, "\\\""); break;
	        case '\b': strbuf_concat(sb, "\\b"); break;
	        case '\f': strbuf_concat(sb, "\\f"); break;
	        case '\n': strbuf_concat(sb, "\\n"); break;
	        case '\r': strbuf_concat(sb, "\\r"); break;
	        case '\t': strbuf_concat(sb, "\\t"); break;
	        default: strbuf_putchar(sb, *str);
	    }
	    str++;
	}
	strbuf_putchar(sb, '"');
}

/*
 * Return a response to the Chrome extension.
 *
 * Host programs are expected to return the following data.
 * (4 byte header + JSON payload)
 *
 *   +--+--+--+--+=========================+
 *   |  LENGTH   |  JSON paylod ...	    |
 *   +--+--+--+--+=========================+
 */
void talk_response(const char *msg, ...)
{
	va_list args;
	int len = 0;

	/* Prevent Windows from translating CRLF */
	setmode(_fileno(stdout), O_BINARY);

	/* Print 4-byte header */
	va_start(args, msg);
	len = vsnprintf(NULL, 0, msg, args);
	fwrite(&len, sizeof(len), 1, stdout);
	va_end(args);

	/* Push the remaining body to stdout */
	va_start(args, msg);
	vfprintf(stdout, msg, args);
	fflush(stdout);
	va_end(args);
}

int main(int argc, char *argv[])
{
	char *cmd;
	char *buf;
	int len;
	int ret;

	/* Prevent Windows from corrupting data */
	setmode(_fileno(stdin), O_BINARY);

	ret = fread(&len, sizeof(len), 1, stdin);
	if (ret < 1) {
	    fprintf(stderr, "cannot read %i bytes", sizeof(len));
	    return -1;
	}

	if (len < 3) {
	    fprintf(stderr, "command too short (%i bytes)", len);
	    return -1;
	}

	buf = (char *) calloc(1, len);
	if (buf == NULL) {
	    perror("calloc");
	    return -1;
	}

	ret = fread(buf, len, 1, stdin);
	if (ret < 1) {
	    fprintf(stderr, "cannot read %i bytes", len);
	    free(buf);
	    return -1;
	}

	/* Trim double-quotes from input */
	buf[len - 1] = '\0';
	cmd = buf + 1;

	/*
	 * Talk protocol defines two operations.
	 *
	 *  "C edge"
	 *    ... Read and return ThinBridgeBHO.ini.
	 *  "Q edge https://google.com"
	 *    ... Open URL in another browser.
	 */
	switch (cmd[0]) {
	case 'C':
	    ret = cb_config(cmd);
	    break;
	case 'Q':
	    ret = cb_query(cmd);
	    break;
	case 'R':
	    ret = cb_resource(cmd);
	    break;
	default:
	    fprintf(stderr, "unknown command '%s'", cmd);
	    break;
	}
	free(buf);
	return 0;
}
