
#include <assert.h>
#include <baselib/baselib.h>
#include <stdlib.h>

#include "http_status_code.h"


char * http_status_code_get_default_message(HTTPStatusCode code)
{
  switch (code)
  {

    case HTTP_STATUS_CODE_100_CONTINUE:
      return "Continue";
    case HTTP_STATUS_CODE_101_SWITCHING_PROTOCOLS:
      return "Switching Protocols";
    case HTTP_STATUS_CODE_102_PROCESSING:
      return "Processing";
    case HTTP_STATUS_CODE_103_EARLY_HINTS:
      return "Early Hints";

    case HTTP_STATUS_CODE_200_OK:
      return "OK";
    case HTTP_STATUS_CODE_201_CREATED:
      return "Created";
    case HTTP_STATUS_CODE_202_ACCEPTED:
      return "Accepted";
    case HTTP_STATUS_CODE_203_NON_AUTHORITATIVE_INFORMATION:
      return "Non-Authoritative Information";
    case HTTP_STATUS_CODE_204_NO_CONTENT:
      return "No Content";
    case HTTP_STATUS_CODE_205_RESET_CONTENT:
      return "Reset Content";
    case HTTP_STATUS_CODE_206_PARTIAL_CONTENT:
      return "Partial Content";
    case HTTP_STATUS_CODE_207_MULTI_STATUS:
      return "Multi-Status";
    case HTTP_STATUS_CODE_208_ALREADY_REPORTED:
      return "Already Reported";
    case HTTP_STATUS_CODE_226_IM_USED:
      return "IM Used";

    case HTTP_STATUS_CODE_300_MULTIPLE_CHOICES:
      return "Multiple Choices";
    case HTTP_STATUS_CODE_301_MOVED_PERMANENTLY:
      return "Moved Permanently";
    case HTTP_STATUS_CODE_302_FOUND:
      return "Found";
    case HTTP_STATUS_CODE_303_SEE_OTHER:
      return "See Other";
    case HTTP_STATUS_CODE_304_NOT_MODIFIED:
      return "Not Modified";
    case HTTP_STATUS_CODE_305_USE_PROXY:
      return "Use Proxy";
    case HTTP_STATUS_CODE_306_SWITCH_PROXY:
      return "Switch Proxy";
    case HTTP_STATUS_CODE_307_TEMPORARY_REDIRECT:
      return "Temporary Redirect";
    case HTTP_STATUS_CODE_308_PERMANENT_REDIRECT:
      return "Permanent Redirect";

    case HTTP_STATUS_CODE_400_BAD_REQUEST:
      return "Bad Request";
    case HTTP_STATUS_CODE_401_UNAUTHORIZED:
      return "Unauthorized";
    case HTTP_STATUS_CODE_402_PAYMENT_REQUIRED:
      return "Payment Required";
    case HTTP_STATUS_CODE_403_FORBIDDEN:
      return "Forbidden";
    case HTTP_STATUS_CODE_404_NOT_FOUND:
      return "Not Found";
    case HTTP_STATUS_CODE_405_METHOD_NOT_ALLOWED:
      return "Method Not Allowed";
    case HTTP_STATUS_CODE_406_NOT_ACCEPTABLE:
      return "Not Acceptable";
    case HTTP_STATUS_CODE_407_PROXY_AUTHENTICATION_REQUIRED:
      return "Proxy Authentication Required";
    case HTTP_STATUS_CODE_408_REQUEST_TIMEOUT:
      return "Request Timeout";
    case HTTP_STATUS_CODE_409_CONFLICT:
      return "Conflict";
    case HTTP_STATUS_CODE_410_GONE:
      return "Gone";
    case HTTP_STATUS_CODE_411_LENGTH_REQUIRED:
      return "Length Required";
    case HTTP_STATUS_CODE_412_PRECONDITION_FAILED:
      return "Precondition Failed";
    case HTTP_STATUS_CODE_413_PAYLOAD_TOO_LARGE:
      return "Payload Too Large";
    case HTTP_STATUS_CODE_414_URI_TOO_LONG:
      return "URI Too Long";
    case HTTP_STATUS_CODE_415_UNSUPPORTED_MEDIA_TYPE:
      return "Unsupported Media Type";
    case HTTP_STATUS_CODE_416_RANGE_NOT_SATISFIABLE:
      return "Range Not Satisfiable";
    case HTTP_STATUS_CODE_417_EXPECTATION_FAILED:
      return "Expectation Failed";
    case HTTP_STATUS_CODE_418_IM_A_TEAPOT:
      return "I'm a teapot";
    case HTTP_STATUS_CODE_421_MISDIRECTED_REQUEST:
      return "Misdirected Request";
    case HTTP_STATUS_CODE_422_UNPROCESSABLE_ENTITY:
      return "Unprocessable Entity";
    case HTTP_STATUS_CODE_423_LOCKED:
      return "Locked";
    case HTTP_STATUS_CODE_424_FAILED_DEPENDENCY:
      return "Failed Dependency";
    case HTTP_STATUS_CODE_426_UPGRADE_REQUIRED:
      return "Upgrade Required";
    case HTTP_STATUS_CODE_428_PRECONDITION_REQUIRED:
      return "Precondition Required";
    case HTTP_STATUS_CODE_429_TOO_MANY_REQUESTS:
      return "Too Many Requests";
    case HTTP_STATUS_CODE_431_REQUEST_HEADER_FIELDS_TOO_LARGE:
      return "Request Header Fields Too Large";
    case HTTP_STATUS_CODE_451_UNAVAILABLE_FOR_LEGAL_REASONS:
      return "Unavailable For Legal Reasons";

    case HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR:
      return "Internal Server Error";
    case HTTP_STATUS_CODE_501_NOT_IMPLEMENTED:
      return "Not Implemented";
    case HTTP_STATUS_CODE_502_BAD_GATEWAY:
      return "Bad Gateway";
    case HTTP_STATUS_CODE_503_SERVICE_UNAVAILABLE:
      return "Service Unavailable";
    case HTTP_STATUS_CODE_504_GATEWAY_TIMEOUT:
      return "Gateway Timeout";
    case HTTP_STATUS_CODE_505_HTTP_VERSION_NOT_SUPPORTED:
      return "HTTP Version Not Supported";
    case HTTP_STATUS_CODE_506_VARIANT_ALSO_NEGOTIATES:
      return "Variant Alo Negotiates";
    case HTTP_STATUS_CODE_507_INSUFFICIENT_STORAGE:
      return "Insufficient Storage";
    case HTTP_STATUS_CODE_508_LOOP_DETECTED:
      return "Loop Detected";
    case HTTP_STATUS_CODE_510_NOT_EXTENDED:
      return "Not Extended";
    case HTTP_STATUS_CODE_511_NETWORK_AUTHENTICATION_REQUIRED:
      return "Network Authentication Required";

    default:
      return "Unspecified";
  }
}


HTTPStatusCode http_status_code_parse(char * str)
{
  char * endptr;
  unsigned long ret;

  assert(str);

  if (strings_length(str) != 3)
    return (HTTPStatusCode) 0;

  ret = strtoul(str, &endptr, 10);
  if (endptr[0] != '\0')
    return (HTTPStatusCode) 0;

  if (ret < 100 || ret > 999)
    ret = 0;

  return (HTTPStatusCode) ret;
}
