
#ifndef __CHTTP_HTTP_MESSAGE_H
#define __CHTTP_HTTP_MESSAGE_H

#include <baselib/baselib.h>
#include <time.h>

#include "http_content.h"
#include "http_cookie.h"
#include "http_version.h"

#include "http_message_type.h"


struct HTTPMessage;
typedef struct HTTPMessage HTTPMessage;


void http_message_destroy(HTTPMessage * message);

/* GETTERS */

HTTPMessageType http_message_get_type(HTTPMessage * message);

HTTPVersion http_message_get_version(HTTPMessage * message);
HTTPContent http_message_get_content(HTTPMessage * message);

List * http_message_list_header_keys(HTTPMessage * message);
bool http_message_has_header(HTTPMessage * message, char * name);
char * http_message_get_header(HTTPMessage * message, char * name);
bool http_message_try_get_header(
    HTTPMessage * message,
    char * name,
    char ** value
    );
size_t http_message_print_header(
    HTTPMessage * message,
    char * name,
    char * buffer,
    size_t buffer_length
    );
List * http_message_get_headers(HTTPMessage * message, char * name);
bool http_message_is_keep_alive(HTTPMessage * message);

time_t http_message_get_date(HTTPMessage * message);
ssize_t http_message_get_content_length(HTTPMessage * message);

HTTPCookie * http_message_get_cookie(HTTPMessage * message, char * name);
List * http_message_get_cookies(HTTPMessage * message);


/* SETTERS */

void http_message_set_version(HTTPMessage * message, HTTPVersion version);
void http_message_set_content(HTTPMessage * message, HTTPContent content);

void http_message_set_header(HTTPMessage * message, char * name, char * value);
void http_message_set_headers(HTTPMessage * message, char * name, List * values);
void http_message_remove_header(HTTPMessage * message, char * name);

void http_message_set_date(HTTPMessage * message, time_t date);
void http_message_set_content_length(HTTPMessage * message, size_t length);


/* MUTATORS */

void http_message_add_header(HTTPMessage * message, char * name, char * value);
void http_message_append_to_header(
    HTTPMessage * message,
    char * name,
    char * value
    );

void http_message_add_cookie(HTTPMessage * message, HTTPCookie * cookie);
void http_message_add_cookies(HTTPMessage * message, List * cookies);
void http_message_remove_cookie(HTTPMessage * messagse, HTTPCookie * cookie);

#endif

