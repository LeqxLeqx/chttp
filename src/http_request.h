
#ifndef __CHTTP_HTTP_REQUEST_H
#define __CHTTP_HTTP_REQUEST_H

#include <baselib/baselib.h>

#include "http_method.h"

struct HTTPRequest;
typedef struct HTTPRequest HTTPRequest;


HTTPRequest * http_request_new();
void http_request_destroy(HTTPRequest * request);


/* GETTERS */

#define http_request_get_type(m) \
        http_message_get_type((HTTPMessage *) m)

#define http_request_get_version(m) \
        http_message_get_version((HTTPMessage *) m)
#define http_request_get_content(m) \
        http_message_get_content((HTTPMessage *) m)

#define http_request_list_header_keys(m) \
        http_message_list_header_keys((HTTPMessage *) m)

#define http_request_has_header(m, n) \
        http_message_has_header((HTTPMessage *) m, n)
#define http_request_get_header(m, n) \
        http_message_get_header((HTTPMessage *) m, n)
#define http_request_try_get_header(m, n, v) \
        http_message_try_get_header((HTTPMessage *) m, n, v)
#define http_request_get_headers(m, n) \
        http_message_get_headers((HTTPMessage *) m, n)
#define http_request_print_header(m, n, b, l) \
        http_message_print_header((HTTPMessage *) m, n, b, l)

#define http_request_get_date(m) \
        http_message_get_date((HTTPMessage *) m)
#define http_request_get_content_length(m) \
        http_message_get_content_length((HTTPMessage *) m)

#define http_request_get_cookie(m, n) \
        http_message_get_cookie((HTTPMessage *) m, n)
#define http_request_get_cookies(m) \
        http_message_get_cookies((HTTPMessage *) m)


HTTPMethod http_request_get_method(HTTPRequest * request);

char * http_request_get_target(HTTPRequest * request);
char * http_request_get_path(HTTPRequest * request); /* path of target */
char * http_request_get_query(HTTPRequest * request);
Dictionary * http_request_get_parameters(HTTPRequest * message);


/* SETTERS */

#define http_request_set_version(m, v) \
        http_message_set_version((HTTPMessage *) m, v)
#define http_request_set_content(m, c) \
        http_message_set_content((HTTPMessage *) m, c)

#define http_request_add_header(m, n, v) \
        http_message_add_header((HTTPMessage *) m, n, v)

#define http_request_set_header(m, n, v) \
        http_message_set_header((HTTPMessage *) m, n, v)
#define http_request_set_headers(m, n, v) \
        http_message_set_headers((HTTPMessage *) m, n, v)
#define http_request_remove_header(m, n) \
        http_message_remove_header((HTTPMessage *) m, n)

#define http_request_set_date(m, d) \
        http_message_set_date((HTTPMessage *) m, d)
#define http_request_set_content_length(m, l) \
        http_message_set_content_length((HTTPMessage *) m, l)

#define http_request_add_cookie(m, c) \
        http_message_add_cookie((HTTPMessage *) m, c)
#define http_request_add_cookies(m, c) \
        http_message_add_cookies((HTTPMessage *) m, c)
#define http_request_remove_cookie(m, c) \
        http_message_remove_cookie((HTTPMessage *) m, c)


void http_request_set_method(HTTPRequest * request, HTTPMethod method);

void http_request_set_target(HTTPRequest * request, char * target);
void http_request_set_path(HTTPRequest * request, char * path);
void http_request_set_query(HTTPRequest * request, char * query);
void http_request_set_parameters(HTTPRequest * request, Dictionary * params);

#endif


