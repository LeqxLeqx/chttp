

#ifndef __CHTTP_HTTP_WRITER_H
#define __CHTTP_HTTP_WRITER_H

#include <stdbool.h>

#include "http_message.h"

struct HTTPWriter;
typedef struct HTTPWriter HTTPWriter;


HTTPWriter * http_writer_new();
void http_writer_destroy(HTTPWriter * writer);

void http_writer_set_timeout_point(HTTPWriter * writer, struct timeval time);

bool http_writer_has_error(HTTPWriter * writer);
char * http_writer_get_error(HTTPWriter * writer);
int http_writer_get_errno(HTTPWriter * writer);

void http_writer_clear_error(HTTPWriter * writer);

void http_writer_render(HTTPWriter * writer, HTTPMessage * msg, int fd);


#endif


