
#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_content.h"
#include "http_message.h"
#include "http_request.h"
#include "http_response.h"
#include "http_utils.h"
#include "http_version.h"

#include "http_writer.h"




struct HTTPWriter
{
  struct timeval timeout_point;
  char * error;
  int error_number;
};


static bool http_writer_timed_out(HTTPWriter * writer)
{
  struct timeval now;

  if (writer->timeout_point.tv_sec == 0 && writer->timeout_point.tv_usec == 0)
    return false;

  gettimeofday(&now, NULL);

  if (now.tv_sec > writer->timeout_point.tv_sec)
    return true;
  else if (now.tv_sec == writer->timeout_point.tv_sec)
    return now.tv_usec >= writer->timeout_point.tv_usec;
  else
    return false;
}

static bool http_writer_check_fd_error(HTTPWriter * writer)
{
  if (http_writer_timed_out(writer))
  {
    writer->error = strings_clone("write timed out");
    writer->error_number = EAGAIN;
  }
  else if (errno)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      pthread_yield();
      return true; /* reattempt write */
    }

    writer->error_number = errno;
    writer->error = strings_format(
        "write error: %s", 
        errors_get_errno_name(writer->error_number)
        );
  }

  return false;
}

static void http_writer_write(
    HTTPWriter * writer,
    int fd,
    char * data,
    size_t data_length
    )
{
  if (writer->error)
    return;

  errno = 0;
  do
  {
    write(fd, data, data_length);
  }
  while (http_writer_check_fd_error(writer));
}

static void http_writer_render_crlf(HTTPWriter * writer, int fd)
{
  http_writer_write(writer, fd, "\r\n", 2);
}

static void http_writer_render_request_line(
    HTTPWriter * writer,
    HTTPRequest * request,
    int fd
    )
{
  char * target, * escaped_target, * line;
  unsigned int line_length;

  if (writer->error)
    return;

  target = http_request_get_target(request);
  escaped_target = http_utils_url_escape(target);
  line = strings_format(
      "%s %s %s",
      http_method_get_string(http_request_get_method(request)),
      target,
      http_version_get_string(http_request_get_version(request))
      );

  line_length = strings_length(line);

  http_writer_write(writer, fd, line, line_length);

  free(target);
  free(escaped_target);
  free(line);
}

static void http_writer_render_status_line(
    HTTPWriter * writer, 
    HTTPResponse * response,
    int fd
    )
{
  char buffer [0xFF], * status_message, * line;
  unsigned int line_length;

  if (writer->error)
    return;

  sprintf(
    buffer,
    "%s %d",
    http_version_get_string(http_response_get_version(response)),
    http_response_get_status_code(response)
    );

  status_message = http_response_get_status_message(response);
  line = strings_format("%s %s", buffer, status_message);

  line_length = strings_length(line);
  http_writer_write(writer, fd, line, line_length);

  free(status_message);
  free(line);

}

static void http_writer_render_headers(
    HTTPWriter * writer, 
    HTTPMessage * msg,
    int fd
    )
{
  bool erred = false;
  char * key, * value, * line;
  unsigned int line_length;
  List * keys;
  ListTraversal * trav;

  if (writer->error)
    return;

  keys = http_message_list_header_keys(msg);
  trav = list_get_traversal(keys);

  while (!erred && !list_traversal_completed(trav))
  {
    key = list_traversal_next_str(trav);
    value = http_message_get_header(msg, key);
    line = strings_format("%s: %s", key, value);
    line_length = strings_length(line);
    
    http_writer_write(writer, fd, line, line_length);
    http_writer_render_crlf(writer, fd);

    free(value);
    free(line);

    if (writer->error)
    {
      erred = true;
      list_traversal_destroy(trav);
    }
  }
  
  list_destroy_and_free(keys);

}

static void http_writer_render_cookies(
    HTTPWriter * writer,
    HTTPMessage * msg,
    int fd
    )
{
  bool erred = false;
  char * key, * cookie_string, * line;
  unsigned int line_length;
  HTTPCookie * cookie;
  List * cookies;
  ListTraversal * trav;

  if (writer->error)
    return;

  if (http_message_get_type(msg) == HTTP_MESSAGE_TYPE_REQUEST)
    key = "Cookie";
  else
    key = "Set-Cookie";

  cookies = http_message_get_cookies(msg);
  trav = list_get_traversal(cookies);

  while (!erred && !list_traversal_completed(trav))
  {
    cookie = (HTTPCookie *) list_traversal_next_ptr(trav);
    cookie_string = http_cookie_to_string(
        cookie,
        http_message_get_version(msg)
        );
    line = strings_format("%s: %s", key, cookie_string);
    line_length = strings_length(line);
    
    write(fd, line, line_length);
    http_writer_render_crlf(writer, fd);

    free(cookie_string);
    free(line);

    if (writer->error)
    {
      erred = true;
      list_traversal_destroy(trav);
    }
  }
  
  list_destroy(cookies);
}


HTTPWriter * http_writer_new()
{
  HTTPWriter * ret = (HTTPWriter *) malloc(sizeof(HTTPWriter));
  assert(ret);

  ret->timeout_point.tv_sec = 0;
  ret->timeout_point.tv_usec = 0;
  ret->error = NULL;
  ret->error_number = 0;

  return ret;
}

void http_writer_destroy(HTTPWriter * writer)
{
  assert(writer);

  free(writer->error);
  free(writer);
}

void http_writer_set_timeout_point(HTTPWriter * writer, struct timeval time)
{
  assert(writer);

  writer->timeout_point = time;
}

bool http_writer_has_error(HTTPWriter * writer)
{
  assert(writer);
  return writer->error != NULL;
}

char * http_writer_get_error(HTTPWriter * writer)
{
  assert(writer);
  return writer->error ? strings_clone(writer->error) : NULL;
}
int http_writer_get_errno(HTTPWriter * writer)
{
  assert(writer);
  return writer->error_number;
}
void http_writer_clear_error(HTTPWriter * writer)
{
  assert(writer);
  free(writer->error);
  writer->error_number = 0;
}

void http_writer_render(HTTPWriter * writer, HTTPMessage * msg, int fd)
{
  HTTPContent content;

  assert(writer);
  assert(msg);
  assert(fd >= 0);

  if (http_message_get_type(msg) == HTTP_MESSAGE_TYPE_REQUEST)
    http_writer_render_request_line(writer, (HTTPRequest *) msg, fd);
  else /* therefore HTTP_MESSAGE_TYPE_RESPONSE */
    http_writer_render_status_line(writer, (HTTPResponse *) msg, fd);

  http_writer_render_crlf(writer, fd);

  http_writer_render_headers(writer, msg, fd);
  http_writer_render_cookies(writer, msg, fd);

  http_writer_render_crlf(writer, fd);
  
  content = http_message_get_content(msg);
  if (content.length != 0)
  {
    http_writer_write(writer, fd, content.data, content.length);
  }

}





