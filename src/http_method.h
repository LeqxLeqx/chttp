
#ifndef __CHTTP_HTTP_METHOD_H
#define __CHTTP_HTTP_METHOD_H


enum HTTPMethod
{
  HTTP_METHOD_NONE    = 0000, /* USED AS ERROR CODE */
  HTTP_METHOD_GET     = 0001,
  HTTP_METHOD_HEAD    = 0002,
  HTTP_METHOD_POST    = 0004,
  HTTP_METHOD_PUT     = 0010,
  HTTP_METHOD_DELETE  = 0020,
  HTTP_METHOD_CONNECT = 0040,
  HTTP_METHOD_OPTIONS = 0100,
  HTTP_METHOD_TRACE   = 0200,
  HTTP_METHOD_PATCH   = 0400,
};
typedef enum HTTPMethod HTTPMethod;


char * http_method_get_string(HTTPMethod method);
HTTPMethod http_method_parse(char * str);



#endif


