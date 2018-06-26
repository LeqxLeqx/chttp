
#include <assert.h>
#include <baselib/baselib.h>

#include "http_version.h"


char * http_version_get_string(HTTPVersion version)
{
  switch (version)
  {
    case HTTP_VERSION_NONE:
      return "HTTP/NONE";
    case HTTP_VERSION_0_9:
      return "HTTP/0.9";
    case HTTP_VERSION_1_0:
      return "HTTP/1.0";
    case HTTP_VERSION_1_1:
      return "HTTP/1.1";
    default:
      assert(0);
  }
}

HTTPVersion http_version_parse(char * str)
{
  if (strings_equals(str, "HTTP/0.9"))
    return HTTP_VERSION_0_9;
  else if (strings_equals(str, "HTTP/1.0"))
    return HTTP_VERSION_1_0;
  else if (strings_equals(str, "HTTP/1.1"))
    return HTTP_VERSION_1_1;
  else
    return HTTP_VERSION_NONE;
}


