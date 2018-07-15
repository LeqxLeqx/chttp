

#include <assert.h>
#include <baselib/baselib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "buffered_reader.h"
#include "http_cookie.h"
#include "http_message.h"
#include "http_status_code.h"
#include "http_response.h"
#include "http_request.h"
#include "http_utils.h"
#include "http_version.h"
#include "http_writer.h"

#include "http_reader.h"

#define _HTTP_READER_BUFFER_LENGTH 0xFF


struct HTTPReader
{
  HTTPReaderSettings settings;

  char * error;
  int error_number, output_fd;
  HTTPStatusCode status_code;
  BufferedReader * br;
  bool expect_head_only;

  HTTPMessage * message;
  char * last_parsed_header;
  bool parsing_first_line;
  time_t header_start_time, content_start_time;

};

static void http_reader_reset(HTTPReader * reader)
{
  if (reader->message)
  {
    http_message_destroy(reader->message);
    reader->message = NULL;
  }
  
  free(reader->last_parsed_header);
  free(reader->error);

  reader->parsing_first_line = false;
  reader->last_parsed_header = NULL;
  reader->error = NULL;
  reader->header_start_time = 0;
  reader->content_start_time = 0;
}

static uint32_t http_reader_remaining_header_read_time(HTTPReader * reader)
{
  time_t now = time(NULL);

  if (reader->header_start_time +
      reader->settings.header_receive_timeout < now)
  {
    return 0;
  }
  else
    return reader->settings.header_receive_timeout -
           (now - reader->header_start_time);
}

static bool http_reader_content_read_expired(HTTPReader * reader)
{
  time_t now = time(NULL);
  if (reader->content_start_time +
      reader->settings.content_receive_timeout < now)
  {
    reader->error = strings_clone("content read timed out");
    reader->status_code = HTTP_STATUS_CODE_408_REQUEST_TIMEOUT;
    return true;
  }
  else
    return false;
}

static void http_reader_check_fd_error(HTTPReader * reader)
{
  if (errno)
  {
    reader->error_number = errno;
    reader->error = strings_format(
        "read error: %s",
        errors_get_errno_name(reader->error_number)
        );
  }
}

static char * http_reader_read_line(
    HTTPReader * reader,
    uint16_t max
    )
{
  char * ret = NULL;
  BufferedReaderError error;

  if (reader->error)
    return NULL;
  
  error = buffered_reader_read_line(
    reader->br, max, &ret,
    http_reader_remaining_header_read_time(reader)
    );
  switch (error)
  {
    case BUFFERED_READER_ERROR_NONE:
      break;
    case BUFFERED_READER_ERROR_TIMEOUT:
      reader->error = strings_clone("read timed out");
      reader->status_code = HTTP_STATUS_CODE_408_REQUEST_TIMEOUT;
      break;
    case BUFFERED_READER_ERROR_LINE_TOO_LONG:
      if (reader->parsing_first_line)
      {
        reader->error = strings_clone("start line too long");
        reader->status_code = HTTP_STATUS_CODE_414_URI_TOO_LONG;
      }
      else
      {
        reader->error = strings_clone("header line too long");
        reader->status_code =
          HTTP_STATUS_CODE_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
      }
      break;
    case BUFFERED_READER_ERROR_ENCOUNTERED_CC:
      reader->error = strings_clone("encounted unexpected control character");
      reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
      break;
    case BUFFERED_READER_ERROR_READ_FAILED:
      http_reader_check_fd_error(reader);
      if (!reader->error)
        reader->error = strings_clone("unknown read error");
      break;
  }

  return ret;
}

static HTTPMessage * http_reader_parse_status_line(
    HTTPReader * reader, char * line
    )
{
  HTTPResponse * ret;
  List * split;
  char * version_str, * status_code_str, * status_message;
  HTTPVersion version;
  HTTPStatusCode status_code;


  if (reader->error)
    return NULL;

  split = strings_split_up_to(line, ' ', 3);
  if (list_size(split) != 3)
  {
    reader->error = strings_clone("malformed status line");
    list_destroy_and_free(split);
    return NULL;
  }

  version_str = list_get_str(split, 0);
  status_code_str = list_get_str(split, 1);
  status_message = list_get_str(split, 2);
  
  version = http_version_parse(version_str);
  status_code = http_status_code_parse(status_code_str);

  if (version == HTTP_VERSION_NONE)
  {
    reader->error = strings_format("malformed HTTP version: %s", version_str);
    reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
    list_destroy_and_free(split);
    return NULL;
  }
  if (status_code == 0)
  {
    reader->error = strings_format("malformed status code: %s",status_code_str);
    reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
    list_destroy_and_free(split);
    return NULL;
  }

  ret = http_response_new();
  http_response_set_version(ret, version);
  http_response_set_status_code(ret, status_code);
  http_response_set_status_message(ret, status_message);

  list_destroy_and_free(split);

  return (HTTPMessage *) ret;
}

static HTTPMessage * http_reader_parse_request_line(
    HTTPReader * reader, char * line
    )
{
  HTTPRequest * ret;
  List * split;
  char * method_str, * target, * version_str;
  HTTPMethod method;
  HTTPVersion version;

  if (reader->error)
    return NULL;

  split = strings_split_up_to(line, ' ', 3);
  if (list_size(split) != 3)
  {
    reader->error = strings_clone("malformed request line");
    list_destroy_and_free(split);
    return NULL;
  }

  method_str = list_get_str(split, 0);
  target = list_get_str(split, 1);
  version_str = list_get_str(split, 2);
  
  method = http_method_parse(method_str);
  version = http_version_parse(version_str);

  if (method == HTTP_METHOD_NONE)
  {
    reader->error = strings_format("malformed HTTP method: %s", method_str);
    list_destroy_and_free(split);
    return NULL;
  }
  if (version == HTTP_VERSION_NONE)
  {
    reader->error = strings_format("malformed HTTP version: %s", version_str);
    list_destroy_and_free(split);
    return NULL;
  }

  ret = http_request_new();
  http_request_set_method(ret, method);
  http_request_set_target(ret, target);
  http_request_set_version(ret, version);

  list_destroy_and_free(split);

  return (HTTPMessage *) ret;
}


static void http_reader_parse_start_line(
    HTTPReader * reader
    )
{
  HTTPMessage * msg;
  char * line;
 
  reader->parsing_first_line = true;
  
  line = http_reader_read_line(reader, reader->settings.start_line_max_length);
      
  if (!line)
    return;
  
  if (strings_starts_with(line, "HTTP/"))
    msg = http_reader_parse_status_line(reader, line);
  else
    msg = http_reader_parse_request_line(reader, line);
  
  reader->parsing_first_line = false;

  free(line);

  reader->message = msg;
}

static void http_reader_add_header(
    HTTPReader * reader, char * name, char * value
    )
{
  bool is_cookie, is_set_cookie;
  HTTPMessageType type;
  List * cookies;

  free(reader->last_parsed_header);
  reader->last_parsed_header = strings_clone(name);

  is_cookie = strings_equals(name, "Cookie");
  is_set_cookie = strings_equals(name, "Set-Cookie");
  type = http_message_get_type(reader->message);

  if (is_cookie && type == HTTP_MESSAGE_TYPE_REQUEST)
  {
    cookies = http_utils_parse_cookie(value, false);
    if (!cookies)
      reader->error = strings_format("malformed cookie header");
    else
    {
      http_message_add_cookies(reader->message, cookies);
      list_destroy(cookies);
    }
  }
  else if (is_set_cookie && type == HTTP_MESSAGE_TYPE_RESPONSE)
  {
    cookies = http_utils_parse_set_cookie(value);
    if (!cookies)
      reader->error = strings_format("malformed Set-Cookie header");
    else
    {
      http_message_add_cookies(reader->message, cookies);
      list_destroy(cookies);
    }
  }
  else
    http_message_add_header(reader->message, name, value);

}

static void http_reader_append_folded_header(HTTPReader * reader, char * line)
{
  if (!reader->last_parsed_header)
  {
    reader->error = strings_clone("folded header-data missing name");
    reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
    return;
  }
  
  if (
    strings_equals(reader->last_parsed_header, "Set-Cookie") ||
    strings_equals(reader->last_parsed_header, "Cookie")
    )
  {
    reader->error = strings_clone(
        "folded header-data not supported for cookies"
        );
    reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
    return;
  }

  http_message_append_to_header(
      reader->message,
      reader->last_parsed_header,
      line
      );
}

static void http_reader_parse_header(HTTPReader * reader, char * line)
{
  char * name, * value, * trimmed_value = NULL;

  if (line[0] == ' ' || line[0] == '\t')
  {
    http_reader_append_folded_header(reader, line);
    return;
  }

  if (!http_utils_split_about_no_trim(line, &name, &value, ':'))
  {
    reader->error = strings_clone("malformed header");
    reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
    return;
  }

  if (strings_contains(name, ' '))
  {
    reader->error = strings_clone("header contains unacceptable white-space");
    reader->status_code = HTTP_STATUS_CODE_400_BAD_REQUEST;
  }
  else
  {
    trimmed_value = strings_trim(value);
    http_reader_add_header(reader, name, trimmed_value);
  }
  
  free(name);
  free(value);
  free(trimmed_value);
}


static void http_reader_parse_headers(HTTPReader * reader)
{
  char * line = NULL;

  do
  {
    free(line);
    line = http_reader_read_line(
        reader,
        reader->settings.header_max_line_length
        );
    if (!line || line[0] == '\0')
      continue;

    http_reader_parse_header(reader, line);
  }
  while (!strings_is_null_or_empty(line) && !reader->error);
  free(line);
}

static bool http_reader_can_presume_empty_by_method(HTTPReader * reader)
{
  HTTPRequest * request;
  HTTPMethod method;

  if (http_message_get_type(reader->message) != HTTP_MESSAGE_TYPE_REQUEST)
    return false;

  request = (HTTPRequest *) reader->message;
  method = http_request_get_method(request);

  switch (method)
  {
    case HTTP_METHOD_GET:
      return reader->settings.presume_get_empty;
    case HTTP_METHOD_HEAD:
      return true;
    case HTTP_METHOD_POST:
      return reader->settings.presume_post_empty;
    case HTTP_METHOD_PUT:
      return reader->settings.presume_put_empty;
    case HTTP_METHOD_DELETE:
      return true;
    case HTTP_METHOD_CONNECT:
      return reader->settings.presume_connect_empty;
    case HTTP_METHOD_OPTIONS:
      return reader->settings.presume_options_empty;
    case HTTP_METHOD_TRACE:
      return true;
    case HTTP_METHOD_PATCH:
      return reader->settings.presume_patch_empty;
    default:
      assert(0);
  }


}

static void http_reader_read_content(HTTPReader * reader, bool static_source)
{
  ssize_t stated_content_length, buffer_read;
  char buffer [_HTTP_READER_BUFFER_LENGTH];
  HTTPContent content;
  size_t read_attempt;

  content.data = NULL;
  content.length = 0;

  if (reader->expect_head_only)
    stated_content_length = 0;
  else
    stated_content_length = http_message_get_content_length(reader->message);
  if (stated_content_length == -1 && !static_source)
  {
    if (http_reader_can_presume_empty_by_method(reader))
      stated_content_length = 0;
    else if (
      http_message_is_keep_alive(reader->message) ||
      reader->settings.always_require_content_length
      )
    {
      reader->error = strings_clone("'Content-Length' header is required");
      reader->status_code = HTTP_STATUS_CODE_411_LENGTH_REQUIRED;
      return;
    }
  }
  if (stated_content_length == 0)
  {
    http_message_set_content(reader->message, content);
    return;
  }

  do
  {
    if (stated_content_length == -1)
      read_attempt = _HTTP_READER_BUFFER_LENGTH;
    else
    {
      read_attempt = stated_content_length - content.length;
      if (read_attempt > _HTTP_READER_BUFFER_LENGTH)
        read_attempt = _HTTP_READER_BUFFER_LENGTH;
    }
    buffer_read = buffered_reader_read(reader->br, buffer, read_attempt);
    if (buffer_read == 0) /* INTERPRET AS EOF */
    {
      if (stated_content_length != -1 &&
          stated_content_length != content.length)
      {
        /* PREMATURE EOD */
        reader->error = strings_clone("premature end of message");
      }
    }
    else if (buffer_read < 0)
    { 
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        if (!static_source)
          buffer_read = 0; /* ensure read continues */
        continue;
      }
      http_reader_check_fd_error(reader);
    }
    else
    {
      if (content.length == 0)
        content.data = malloc(buffer_read);
      else
        content.data = realloc(content.data, buffer_read + content.length);
      assert(content.data);

      memcpy(&content.data[content.length], buffer, buffer_read);
      content.length += buffer_read;

      if (content.length == stated_content_length)
        buffer_read = 0; /* KILLS LOOP */
    }
  }
  while (buffer_read > 0 &&
         !reader->error &&
         !http_reader_content_read_expired(reader));

  if (http_reader_content_read_expired(reader))
  {
    free(reader->error);
    reader->error = strings_clone("content read timed out");
  }

  if (reader->error)
  {
    if (content.length > 0)
      free(content.data);
  }
  else
    http_message_set_content(reader->message, content);
}

static void http_reader_send(
    HTTPReader * reader,
    HTTPResponse * response
    )
{
  HTTPWriter * writer;

  assert(reader);
  assert(response);

  writer = http_writer_new();
  http_writer_render(writer, (HTTPMessage *) response, reader->output_fd);

  if (http_writer_has_error(writer))
  {
    free(reader->error);
    reader->error = http_writer_get_error(writer);
  }

  http_writer_destroy(writer);
  http_response_destroy(response);
}

static void http_reader_send_continue(HTTPReader * reader)
{
  HTTPResponse * response;

  assert(reader);

  response = http_response_new();
  http_response_set_status_code(response, HTTP_STATUS_CODE_100_CONTINUE);
  http_response_set_version(
    response, 
    http_message_get_version(reader->message)
    );

  http_reader_send(reader, response);
}


static void http_reader_respond_to_expect_continue(HTTPReader * reader)
{
  char * expect;
  HTTPResponse * response;

  if (http_message_get_type(reader->message) != HTTP_MESSAGE_TYPE_REQUEST)
    return;

  expect = http_message_get_header(reader->message, "Expect");
  if (!expect)
    return;
  else if (!strings_equals(expect, "100-Continue"))
  {
    free(expect);
    return;
  }

  free(expect);

  if (!reader->settings.allow_expect_continue)
  {
    reader->error = strings_clone("`Expect: 100-Continue' header not allowed");
    reader->status_code = HTTP_STATUS_CODE_417_EXPECTATION_FAILED;
    return;
  }
  if (!reader->settings.send_continue_callback)
  {
    http_reader_send_continue(reader);
    return;
  }

  response = reader->settings.send_continue_callback(
      (HTTPRequest *) reader->message
      );
  if (!response)
    http_reader_send_continue(reader);
  else
  {
    reader->error = strings_clone("`Expect: 100-Continue' rejected");
    http_reader_send(reader, response);
  }
}


HTTPReader * http_reader_new(int fd)
{
  HTTPReader * ret = (HTTPReader *) malloc(sizeof(HTTPReader));
  assert(ret);

  ret->br = buffered_reader_new(fd);

  ret->error = NULL;
  ret->error_number = 0;
  ret->output_fd = fd;
  ret->status_code = 0;
  ret->expect_head_only = false;

  ret->message = NULL;
  ret->parsing_first_line = true;
  ret->last_parsed_header = NULL;
  
  ret->settings.start_line_max_length = 0x3FF;
  ret->settings.header_max_line_length = 0x7FF;
  ret->settings.max_header_count = 0x7FF;
  ret->settings.max_cookie_count = 0x7FF;

  ret->settings.max_get_length = 0xFFFF;
  ret->settings.max_post_length = 0xFFFF;
  ret->settings.max_put_length = 0xFFFF;
  ret->settings.max_connect_length = 0xFFFF;
  ret->settings.max_options_length = 0xFFFF;
  ret->settings.max_patch_length = 0xFFFF;

  ret->settings.header_receive_timeout = 15;
  ret->settings.content_receive_timeout = 30;

  ret->settings.always_require_content_length = true;
  ret->settings.presume_get_empty = true;
  ret->settings.presume_post_empty = false;
  ret->settings.presume_put_empty = false;
  ret->settings.presume_connect_empty = true;
  ret->settings.presume_options_empty = true;
  ret->settings.presume_patch_empty = false;
  ret->settings.presume_options_empty = true;
  ret->settings.presume_patch_empty = false;

  ret->settings.allow_expect_continue = false;
  ret->settings.send_continue_callback = NULL;

  return ret;
}

void http_reader_destroy(HTTPReader * reader)
{
  assert(reader);

  free(reader->error);
  free(reader->last_parsed_header);

  buffered_reader_destroy(reader->br);

  free(reader);
}

void http_reader_set_settings(HTTPReader * reader, HTTPReaderSettings settings)
{
  assert(reader);

  reader->settings = settings;
}
void http_reader_set_expect_head_only(HTTPReader * reader, bool value)
{
  assert(reader);

  reader->expect_head_only = value;
}

bool http_reader_has_error(HTTPReader * reader)
{
  assert(reader);
  return reader->error != NULL;
}

char * http_reader_get_error(HTTPReader * reader)
{
  assert(reader);
  return reader->error ? strings_clone(reader->error) : NULL;
}

int http_reader_get_errno(HTTPReader * reader)
{
  assert(reader);
  return reader->error_number;
}
HTTPStatusCode http_reader_get_status_code(HTTPReader * reader)
{
  assert(reader);
  return reader->status_code;
}
bool http_reader_buffer_is_empty(HTTPReader * reader)
{
  assert(reader);
  return buffered_reader_buffer_is_empty(reader->br);
}

void http_reader_clear_error(HTTPReader * reader)
{
  assert(reader);

  free(reader->error);
  reader->error = NULL;
  reader->error_number = 0;
  reader->status_code = 0;
}


static HTTPMessage * http_reader_next_imp(
    HTTPReader * reader, 
    bool static_source
    )
{
  HTTPMessage * ret;
  assert(reader);

  http_reader_reset(reader);

  reader->header_start_time = time(NULL);
  http_reader_parse_start_line(reader);
  if (reader->error)
    return NULL;

  http_reader_parse_headers(reader);
  if (reader->error)
  {
    http_message_destroy(reader->message);
    reader->message = NULL;
    return NULL;
  }

  if (!static_source)
  {
    http_reader_respond_to_expect_continue(reader);
    if (reader->error)
    {
      http_message_destroy(reader->message);
      reader->message = NULL;
      return NULL;
    }
  }

  reader->content_start_time = time(NULL);
  http_reader_read_content(reader, static_source);

  if (reader->error)
  {
    http_message_destroy(reader->message);
    reader->message = NULL;
    return NULL;
  }

  ret = reader->message;
  reader->message = NULL;
  return ret;
}

HTTPMessage * http_reader_next(HTTPReader * reader)
{
  return http_reader_next_imp(reader, false);
}

HTTPMessage * http_reader_next_from_static(HTTPReader * reader)
{
  return http_reader_next_imp(reader, true);
}



