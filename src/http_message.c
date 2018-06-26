
#include <assert.h>
#include <baselib/baselib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "http_content.h"
#include "http_utils.h"
#include "http_version.h"

#include "http_message.h"
#include "http_message_type.h"
#include "http_message_struct.h"


/* INTERNAL (declared in message_struct.h) */

void http_message_init_struct(HTTPMessage * message, HTTPMessageType mt)
{
  message->message_type = mt;
  message->version = HTTP_VERSION_1_1;
  message->headers = dictionary_new(DICTIONARY_TYPE_HASH_TABLE);
  message->cookies = list_new(LIST_TYPE_LINKED_LIST);
  message->content.data = NULL;
  message->content.length = 0;
}

void http_message_deinit_struct(HTTPMessage * message)
{
  dictionary_destroy_and_free(message->headers);
  list_destroy_and_user_free(
      message->cookies,
      (void (*)(void *)) http_cookie_destroy
      );

   /*TODO: REVIEW
  if (message->content.data)
    free(message->content.data);
    */
}

/* INTERNAL (not declared elsewere) */

static char * http_message_get_header_imp(HTTPMessage * message, char * name)
{
  Any value;

  assert(message);
  assert(name);

  if (dictionary_try_get(message->headers, name, &value))
    return any_to_str(value);
  else
    return NULL;
}

/* DESTRUCTOR */

void http_message_destroy(HTTPMessage * message)
{
  assert(message);
  message->destroy(message);
}

/* GETTERS */

HTTPMessageType http_message_get_type(HTTPMessage * message)
{
  assert(message);
  return message->message_type;
}

HTTPVersion http_message_get_version(HTTPMessage * message)
{
  assert(message);
  return message->version;
}

HTTPContent http_message_get_content(HTTPMessage * message)
{
  assert(message);
  return message->content;
}

List * http_message_list_header_keys(HTTPMessage * message)
{
  assert(message);
  return dictionary_get_keys(message->headers);
}

bool http_message_has_header(HTTPMessage * message, char * name)
{
  assert(message);
  return dictionary_has(message->headers, name);
}

char * http_message_get_header(HTTPMessage * message, char * name)
{
  char * ret;

  ret = http_message_get_header_imp(message, name);
  if (ret)
    return strings_clone(ret);
  else
    return NULL;
}
bool http_message_try_get_header(
    HTTPMessage * message,
    char * name,
    char ** value
    )
{
  char * ret = NULL;

  ret = http_message_get_header(message, name);

  if (value)
    *value = ret;
  else
    free(ret);

  return ret != NULL;
}
List * http_message_get_headers(HTTPMessage * message, char * name)
{
  List * ret;
  char * string;

  string = http_message_get_header_imp(message, name);
  if (!string)
    return list_new(LIST_TYPE_LINKED_LIST);

  ret = strings_split(string, ',');
  free(string);

  return ret;
}

size_t http_message_print_header(
    HTTPMessage * message,
    char * name,
    char * buffer,
    size_t buffer_length
    )
{
  char * str;
  unsigned int str_length;

  assert(buffer);
  assert(buffer_length);

  str = http_message_get_header_imp(message, name);
  if (!str)
    return 0;

  str_length = strings_length(str);

  if (str_length > buffer_length - 1)
    str_length = buffer_length - 1;

  memcpy(buffer, str, (str_length + 1) * sizeof(char));
  return str_length;
}
bool http_message_is_keep_alive(HTTPMessage * message)
{
  char * str;

  switch (message->version)
  {
    case HTTP_VERSION_1_1:
      return true;
    case HTTP_VERSION_0_9:
      return false;
    case HTTP_VERSION_1_0:
      str = http_message_get_header_imp(message, "Connection");
      if (!str)
        return false;
      else
        return strings_equals(str, "keep-alive");
    default:
      assert(0);
  }
}

time_t http_message_get_date(HTTPMessage * message)
{
  char * str = http_message_get_header_imp(message, "Date");

  if (!str)
    return (time_t) 0;

  return http_utils_parse_date(str);
}
ssize_t http_message_get_content_length(HTTPMessage * message)
{
  unsigned long long int value;
  char
    * str = http_message_get_header_imp(message, "Content-Length"),
    * endptr;

  if (!str)
    return (ssize_t) -1;

  value = strtoull(str, &endptr, 10);
  if (!value && endptr[0] != '\0')
    return (ssize_t) -1;

  return (ssize_t) value;
}


HTTPCookie * http_message_get_cookie(HTTPMessage * message, char * name)
{
  ListTraversal * trav;
  HTTPCookie * cookie;

  assert(message);
  assert(name);

  trav = list_get_traversal(message->cookies);
  while (!list_traversal_completed(trav))
  {
    cookie = (HTTPCookie *) list_traversal_next_ptr(trav);
    if (http_cookie_has_name(cookie, name))
    {
      list_traversal_destroy(trav);
      return cookie;
    }
  }
  
  return NULL;
}

List * http_message_get_cookies(HTTPMessage * message)
{
  assert(message);

  return list_clone(message->cookies);
}


/* SETTERS */

void http_message_set_version(HTTPMessage * message, HTTPVersion version)
{
  assert(message);

  message->version = version;
}
void http_message_set_content(HTTPMessage * message, HTTPContent content)
{
  assert(message);

  // free(message->content.data); /* TODO: REVIEW */
  message->content = content;
}

void http_message_set_header(HTTPMessage * message, char * name, char * value)
{
  assert(message);
  assert(name);

  dictionary_set_and_free(
    message->headers,
    name,
    str_to_any(strings_clone(value ? value : ""))
    );
}
void http_message_set_headers(HTTPMessage * message, char * name, List * values)
{
  StringBuilder * sb;
  ListTraversal * trav;
  char * str;
  bool first = true;

  assert(message);
  assert(name);
  assert(values);

  if (list_size(values) == 0)
    http_message_set_header(message, name, "");

  sb = string_builder_new();
  trav = list_get_traversal(values);

  while (!list_traversal_completed(trav))
  {
    if (first)
    {
      string_builder_append(sb, list_traversal_next_str(trav));
      first = false;
    }
    else
      string_builder_appendf(sb, ",%s", list_traversal_next_str(trav));
  }

  str = string_builder_to_string(sb);
  http_message_set_header(message, name, str);
  free(str);

  string_builder_destroy(sb);
}

void http_message_remove_header(HTTPMessage * message, char * name)
{
  assert(message);
  assert(name);

  Any value;

  if (dictionary_try_get(message->headers, name, &value))
  {
    free(any_to_str(value));
    dictionary_remove(message->headers, name);
  }
}

void http_message_set_date(HTTPMessage * message, time_t date)
{
  assert(message);

  dictionary_set_and_free(
    message->headers,
    "Date",
    str_to_any(http_utils_date_to_string(date, message->version))
    );
}
void http_message_set_content_length(HTTPMessage * message, size_t length)
{
  char * buffer;
  assert(message);

  buffer = strings_format("%llu", (unsigned long long) length);

  dictionary_set_and_free(
    message->headers,
    "Content-Length",
    str_to_any(buffer)
    );
}


void http_message_add_header(HTTPMessage * message, char * name, char * value)
{
  Any current;
  char * temp;

  assert(message);
  assert(name);
  assert(value);

  if (dictionary_try_get(message->headers, name, &current))
  {
    temp = strings_format("%s,%s", any_to_str(current), value);
    dictionary_set_and_free(message->headers, name, str_to_any(temp));
  }
  else
    dictionary_put(message->headers, name, str_to_any(strings_clone(value)));
}

void http_message_append_to_header(
    HTTPMessage * message,
    char * name,
    char * value
    )
{
  char * current, * temp;

  assert(message);
  assert(name);
  assert(value);

  current = any_to_str(dictionary_get(message->headers, name));
  temp = strings_concat(current, value);
  dictionary_set_and_free(message->headers, name, str_to_any(temp));
}


void http_message_add_cookie(HTTPMessage * message, HTTPCookie * cookie)
{
  assert(message);
  assert(cookie);

  list_add(message->cookies, ptr_to_any(cookie));
}

void http_message_add_cookies(HTTPMessage * message, List * cookies)
{
  assert(message);
  assert(cookies);

  list_add_range(message->cookies, cookies);
}

void http_message_remove_cookie(HTTPMessage * message, HTTPCookie * cookie)
{
  assert(message);
  assert(cookie);

  list_remove(message->cookies, ptr_to_any(cookie));
}


