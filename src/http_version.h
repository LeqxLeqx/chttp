
#ifndef __CHTTP_HTTP_VERSION_H
#define __CHTTP_HTTP_VERSION_H


enum HTTPVersion
{
  HTTP_VERSION_NONE =  0,
  HTTP_VERSION_0_9 =  90,
  HTTP_VERSION_1_0 = 100,
  HTTP_VERSION_1_1 = 110,
};
typedef enum HTTPVersion HTTPVersion;

char * http_version_get_string(HTTPVersion version);
HTTPVersion http_version_parse(char * str);


#endif

