
#ifndef __CHTTP_HTTP_METHOD_H
#define __CHTTP_HTTP_METHOD_H


enum HTTPMethod
{
  HTTP_METHOD_NONE    = 0, /* USED AS ERROR CODE */
  HTTP_METHOD_GET     = 1,
  HTTP_METHOD_HEAD    = 2,
  HTTP_METHOD_POST    = 3,
  HTTP_METHOD_PUT     = 4,
  HTTP_METHOD_DELETE  = 5,
  HTTP_METHOD_CONNECT = 6,
  HTTP_METHOD_OPTIONS = 7,
  HTTP_METHOD_TRACE   = 8,
  HTTP_METHOD_PATCH   = 9,
};
typedef enum HTTPMethod HTTPMethod;


char * http_method_get_string(HTTPMethod method);
HTTPMethod http_method_parse(char * str);



#endif


