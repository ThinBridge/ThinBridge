#ifndef INTERNAL_H
#define INTERNAL_H

struct strbuf {
	int alloc;
	int count;
	char *buf;
};

#define alloc_nr(x) (((x) + 32) * 2)

void *xmalloc(int size);
void *xrealloc(char *ptr, int size);

void strbuf_putchar(struct strbuf *sb, char chr);
void strbuf_concat(struct strbuf *sb, char *str);
void strbuf_concat_jsonstr(struct strbuf *sb, char *str, int len);

/*
 * Callback functions
 */
int cb_config(char *cmd);
int cb_query(char *cmd);
int cb_resource(char *cmd);

void talk_response(const char *msg, ...);

#endif
