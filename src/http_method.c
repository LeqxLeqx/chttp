
#include <assert.h>
#include <baselib/baselib.h>

#include "http_method.h"

char * http_method_get_string(HTTPMethod method)
{
  switch (method)
  {
    case HTTP_METHOD_NONE:
      return "NONE";
    case HTTP_METHOD_GET:
      return "GET";
    case HTTP_METHOD_HEAD:
      return "HEAD";
    case HTTP_METHOD_POST:
      return "POST";
    case HTTP_METHOD_PUT:
      return "PUT";
    case HTTP_METHOD_DELETE:
      return "DELETE";
    case HTTP_METHOD_CONNECT:
      return "CONNECT";
    case HTTP_METHOD_OPTIONS:
      return "OPTIONS";
    case HTTP_METHOD_TRACE:
      return "TRACE";
    case HTTP_METHOD_PATCH:
      return "PATCH";
    
    default:
      assert(0);

  }
}

HTTPMethod http_method_parse(char * str)
{
  if (strings_equals(str, "GET"))
    return HTTP_METHOD_GET;
  else if (strings_equals(str, "HEAD"))
    return HTTP_METHOD_HEAD;
  else if (strings_equals(str, "POST"))
    return HTTP_METHOD_POST;
  else if (strings_equals(str, "PUT"))
    return HTTP_METHOD_PUT;
  else if (strings_equals(str, "DELETE"))
    return HTTP_METHOD_DELETE;
  else if (strings_equals(str, "CONNECT"))
    return HTTP_METHOD_CONNECT;
  else if (strings_equals(str, "OPTIONS"))
    return HTTP_METHOD_OPTIONS;
  else if (strings_equals(str, "TRACE"))
    return HTTP_METHOD_TRACE;
  else if (strings_equals(str, "PATCH"))
    return HTTP_METHOD_PATCH;
  else
    return HTTP_METHOD_NONE;
}

