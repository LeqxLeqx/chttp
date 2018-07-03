
#ifndef __CHTTP_HTTP_READER_H
#define __CHTTP_HTTP_READER_H

#include <stdbool.h>

#include "http_reader_settings.h"

#include "http_message.h"

struct HTTPReader;
typedef struct HTTPReader HTTPReader;



HTTPReader * http_reader_new(int fd);
void http_reader_destroy(HTTPReader * reader);

void http_reader_set_settings(HTTPReader * reader, HTTPReaderSettings settings);

bool http_reader_has_error(HTTPReader * reader);
char * http_reader_get_error(HTTPReader * reader);
int http_reader_get_errno(HTTPReader * reader);
HTTPStatusCode http_reader_get_status_code(HTTPReader * reader);
bool http_reader_buffer_is_empty(HTTPReader * reader);

void http_reader_clear_error(HTTPReader * reader);

HTTPMessage * http_reader_next(HTTPReader * reader);


#endif
