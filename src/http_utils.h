

#ifndef __CHTTP_HTTP_UTILS_H
#define __CHTTP_HTTP_UTILS_H

#include <baselib/baselib.h>
#include <time.h>

#include "http_cookie.h"
#include "http_version.h"

List * http_utils_parse_cookie(char * str, bool ignore_bad_names);
List * http_utils_parse_set_cookie(char * str);

time_t http_utils_parse_date(char * str);
char * http_utils_date_to_string(time_t date, HTTPVersion ver);

void http_utils_parse_query_parameters(Dictionary * dic, char * query);
char * http_utils_params_to_string(Dictionary * params);

char * http_utils_url_escape(char * str);
char * http_utils_url_deescape(char * str);

void http_utils_transfer_string_dictionary(Dictionary * to, Dictionary * from);
bool http_utils_split_about(
    char * src, char ** before, char ** after, char c
    );
bool http_utils_split_about_no_trim(
    char * src, char ** before, char ** after, char c
    );

#endif


