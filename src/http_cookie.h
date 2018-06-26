

#ifndef __CHTTP_HTTP_COOKIE_H
#define __CHTTP_HTTP_COOKIE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "http_version.h"


struct HTTPCookie;
typedef struct HTTPCookie HTTPCookie;

HTTPCookie * http_cookie_new();
void http_cookie_destroy(HTTPCookie * cookie);

HTTPCookie * http_cookie_clone(HTTPCookie * original);

bool http_cookie_has_name(HTTPCookie * cookie, char * name);

/* GETTERS */

char * http_cookie_get_name(HTTPCookie * cookie);
char * http_cookie_get_value(HTTPCookie * cookie);

time_t http_cookie_get_expiry(HTTPCookie * cookie);
uint32_t http_cookie_get_max_age(HTTPCookie * cookie);
char * http_cookie_get_domain(HTTPCookie * cookie);
char * http_cookie_get_path(HTTPCookie * cookie);
bool http_cookie_is_secure(HTTPCookie * cookie);
bool http_cookie_is_httponly(HTTPCookie * cookie);

char * http_cookie_get_extension(HTTPCookie * cookie);
bool http_cookie_has_extension(HTTPCookie * cookie);


/* SETTERS */

void http_cookie_set_name(HTTPCookie * cookie, char * name);
void http_cookie_set_value(HTTPCookie * cookie, char * value);

void http_cookie_set_expiry(HTTPCookie * cookie, time_t expiry);
void http_cookie_set_max_age(HTTPCookie * cookie, uint32_t max_age);
void http_cookie_set_domain(HTTPCookie * cookie, char * domain);
void http_cookie_set_path(HTTPCookie * cookie, char * path);
void http_cookie_set_secure(HTTPCookie * cookie, bool secure);
void http_cookie_set_httponly(HTTPCookie * cookie, bool httponly);
void http_cookie_set_extension(HTTPCookie * cookie, char * extension);



char * http_cookie_to_string(HTTPCookie * cookie, HTTPVersion version);

#endif


