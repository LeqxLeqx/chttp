
#ifndef __CHTTP_HTTP_MESSAGE_STRUCT_H
#define __CHTTP_HTTP_MESSAGE_STRUCT_H

#include <baselib/baselib.h>

#include "http_content.h"
#include "http_version.h"
#include "http_message_type.h"

struct HTTPMessage
{
  HTTPMessageType message_type;
  HTTPVersion version;
  Dictionary * headers;
  HTTPContent content;
  List * cookies;

  void (*destroy)(HTTPMessage * message);
};

void http_message_init_struct(HTTPMessage * message, HTTPMessageType mt);
void http_message_deinit_struct(HTTPMessage * message);

#endif

