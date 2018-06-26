

#ifndef __CHTTP_HTTP_CONTENT_H
#define __CHTTP_HTTP_CONTENT_H

#include <sys/types.h>

struct HTTPContent
{
  char * data;
  size_t length;
};
typedef struct HTTPContent HTTPContent;


#endif

