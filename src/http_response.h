
#ifndef __CHTTP_HTTP_RESPONSE_H
#define __CHTTP_HTTP_RESPONSE_H

#include "http_status_code.h"

struct HTTPResponse;
typedef struct HTTPResponse HTTPResponse;


HTTPResponse * http_response_new();
void http_response_destroy(HTTPResponse * response);


/* GETTERS */

#define http_response_get_type(m) \
        http_message_get_type((HTTPMessage *) m)

#define http_response_get_version(m) \
        http_message_get_version((HTTPMessage *) m)
#define http_response_get_content(m) \
        http_message_get_content((HTTPMessage *) m)

#define http_response_list_header_keys(m) \
        http_message_list_header_keys((HTTPMessage *) m)

#define http_response_has_header(m, n) \
        http_message_has_header((HTTPMessage *) m, n)
#define http_response_get_header(m, n) \
        http_message_get_header((HTTPMessage *) m, n)
#define http_response_try_get_header(m, n, v) \
        http_message_try_get_header((HTTPMessage *) m, n, v)
#define http_response_get_headers(m, n) \
        http_message_get_headers((HTTPMessage *) m, n)
#define http_response_print_header(m, n, b, l) \
        http_message_print_header((HTTPMessage *) m, n, b, l)

#define http_response_get_date(m) \
        http_message_get_date((HTTPMessage *) m)
#define http_response_get_content_length(m) \
        http_message_get_content_length((HTTPMessage *) m)

#define http_response_get_cookie(m, n) \
        http_message_get_cookie((HTTPMessage *) m, n)
#define http_response_get_cookies(m) \
        http_message_get_cookies((HTTPMessage *) m)

HTTPStatusCode http_response_get_status_code(HTTPResponse * response);
char * http_response_get_status_message(HTTPResponse * response);


/* SETTERS */

#define http_response_set_version(m, v) \
        http_message_set_version((HTTPMessage *) m, v)
#define http_response_set_content(m, c) \
        http_message_set_content((HTTPMessage *) m, c)

#define http_response_add_header(m, n, v) \
        http_message_add_header((HTTPMessage *) m, n, v)

#define http_response_set_header(m, n, v) \
        http_message_set_header((HTTPMessage *) m, n, v)
#define http_response_set_headers(m, n, v) \
        http_message_set_headers((HTTPMessage *) m, n, v)
#define http_response_remove_header(m, n) \
        http_message_remove_header((HTTPMessage *) m, n)

#define http_response_set_date(m, d) \
        http_message_set_date((HTTPMessage *) m, d)
#define http_response_set_content_length(m, l) \
        http_message_set_content_length((HTTPMessage *) m, l)


#define http_response_add_cookie(m, c) \
        http_message_add_cookie((HTTPMessage *) m, c)
#define http_response_add_cookies(m, c) \
        http_message_add_cookies((HTTPMessage *) m, c)
#define http_response_remove_cookie(m, c) \
        http_message_remove_cookie((HTTPMessage *) m, c)


void http_response_set_status_code(
    HTTPResponse * response,
    HTTPStatusCode code
    );
void http_response_set_status_message(
    HTTPResponse * response,
    char * status_message
    );


#endif

