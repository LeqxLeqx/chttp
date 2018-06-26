
#include <assert.h>
#include <baselib/baselib.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "http_utils.h"
#include "http_version.h"

#include "http_cookie.h"


struct HTTPCookie
{
  char
    * name,
    * value,
    * domain,
    * path,
    * extension;
  time_t expiry;
  uint32_t max_age;
  bool secure;
  bool httponly;
};

HTTPCookie * http_cookie_new()
{
  HTTPCookie * ret = (HTTPCookie *) malloc(sizeof(HTTPCookie));

  ret->name = NULL;
  ret->value = NULL;
  ret->domain = NULL;
  ret->path = NULL;
  ret->extension = NULL;
  ret->expiry = 0;
  ret->max_age = 0;
  ret->secure = false;
  ret->httponly = false;

  return ret;
}
void http_cookie_destroy(HTTPCookie * cookie)
{
  assert(cookie);

  free(cookie->name);
  free(cookie->value);
  free(cookie->domain);
  free(cookie->path);
  free(cookie->extension);
  free(cookie);

}

HTTPCookie * http_cookie_clone(HTTPCookie * original)
{
  HTTPCookie * ret;

  assert(original);

  ret = http_cookie_new();

  ret->name = original->name ? strings_clone(original->name) : NULL;
  ret->value = original->value ? strings_clone(original->value) : NULL;
  ret->domain = original->domain ? strings_clone(original->domain) : NULL;
  ret->path = original->path ? strings_clone(original->path) : NULL;
  ret->extension = original->extension ?
                     strings_clone(original->extension) :
                     NULL;
  ret->expiry = original->expiry;
  ret->max_age = original->max_age;
  ret->secure = original->secure;
  ret->httponly = original->httponly;

  return ret;
}

bool http_cookie_has_name(HTTPCookie * cookie, char * name)
{
  assert(cookie);

  if (!name && !cookie->name)
    return true;
  else if (!name || !cookie->name)
    return false;
  else
    return strings_equals(cookie->name, name);
}



char * http_cookie_get_name(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->name ? strings_clone(cookie->name) : NULL;
}

char * http_cookie_get_value(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->value ? strings_clone(cookie->value) : NULL;
}

time_t http_cookie_get_expiry(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->expiry;
}
uint32_t http_cookie_get_max_age(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->max_age;
}
char * http_cookie_get_domain(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->domain ? strings_clone(cookie->domain) : NULL;
}
char * http_cookie_get_path(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->path ? strings_clone(cookie->path) : NULL;
}
bool http_cookie_is_secure(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->secure;
}
bool http_cookie_is_httponly(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->httponly;
}

char * http_cookie_get_extension(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->extension ? strings_clone(cookie->extension) : NULL;
}
bool http_cookie_has_extension(HTTPCookie * cookie)
{
  assert(cookie);
  return cookie->extension != NULL;
}




void http_cookie_set_name(HTTPCookie * cookie, char * name)
{
  assert(cookie);
  free(cookie->name);
  cookie->name = name ? strings_clone(name) : NULL;
}
void http_cookie_set_value(HTTPCookie * cookie, char * value)
{
  assert(cookie);
  free(cookie->value);
  cookie->value = value ? strings_clone(value) : NULL;
}

void http_cookie_set_expiry(HTTPCookie * cookie, time_t expiry)
{
  assert(cookie);
  cookie->expiry = expiry;
}

void http_cookie_set_max_age(HTTPCookie * cookie, uint32_t max_age)
{
  assert(cookie);
  cookie->max_age = max_age;
}
void http_cookie_set_domain(HTTPCookie * cookie, char * domain)
{
  assert(cookie);
  free(cookie->domain);
  cookie->domain = domain ? strings_clone(domain) : NULL;
}
void http_cookie_set_path(HTTPCookie * cookie, char * path)
{
  assert(cookie);
  free(cookie->path);
  cookie->path = path ? strings_clone(path) : NULL;
}
void http_cookie_set_secure(HTTPCookie * cookie, bool secure)
{
  assert(cookie);
  cookie->secure = secure;
}
void http_cookie_set_httponly(HTTPCookie * cookie, bool httponly)
{
  assert(cookie);
  cookie->httponly = httponly;
}
void http_cookie_set_extension(HTTPCookie * cookie, char * extension)
{
  assert(cookie);
  free(cookie->extension);
  cookie->extension = extension ? strings_clone(extension) : NULL;
}



char * http_cookie_to_string(HTTPCookie * cookie, HTTPVersion version)
{
  StringBuilder * sb;
  char * temp_string;

  sb = string_builder_new();
  string_builder_appendf(
    sb,
    "%s=%s",
    cookie->name ? cookie->name : "",
    cookie->value? cookie->value : ""
    );

  if (cookie->expiry != 0)
  {
    temp_string = http_utils_date_to_string(cookie->expiry, version);
    string_builder_appendf(sb, "; Expires=%s", temp_string);
    free(temp_string);
  }
  if (cookie->max_age != 0)
    string_builder_appendf(sb,"; Max-Age=%lu", (unsigned long) cookie->max_age);
  if (cookie->domain)
    string_builder_appendf(sb, "; Domain=%s", cookie->domain);
  if (cookie->path)
    string_builder_appendf(sb, "; Path=%s", cookie->path);
  if (cookie->secure)
    string_builder_append(sb, "; Secure");
  if (cookie->httponly)
    string_builder_append(sb, "; HttpOnly");
  if (cookie->extension)
    string_builder_appendf(sb, "; %s", cookie->extension);

  return string_builder_to_string_destroy(sb);
}
