

#ifndef __CHTTP_HTTP_READER_SETTINGS_H
#define __CHTTP_HTTP_READER_SETTINGS_H


#include <stdint.h>

struct HTTPReaderSettings
{
  uint16_t
    start_line_max_length,
    header_max_line_length,
    max_header_count,
    max_cookie_count;
  uint32_t
    max_get_length,
    max_post_length,
    max_put_length,
    max_connect_length,
    max_options_length,
    max_patch_length,
    header_receive_timeout, /* in seconds */
    content_receive_timeout; /* also in seconds */
  bool
    always_require_content_length,
    presume_get_empty,
    presume_post_empty,
    presume_put_empty,
    presume_connect_empty,
    presume_options_empty,
    presume_patch_empty,
    allow_expect_continue;
  HTTPResponse * (*send_continue_callback)(HTTPRequest *);
};
typedef struct HTTPReaderSettings HTTPReaderSettings;



#endif




