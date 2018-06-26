
#include <assert.h>
#include <stdlib.h>

#include "http_status_code.h"

#include "http_response.h"
#include "http_message.h"
#include "http_message_type.h"
#include "http_message_struct.h"

struct HTTPResponse
{
  HTTPMessage base;
  HTTPStatusCode status_code;
  char * status_message;
};


HTTPResponse * http_response_new()
{
  HTTPResponse * ret = (HTTPResponse *) malloc(sizeof(HTTPResponse));
  assert(ret);
  
  http_message_init_struct(&ret->base, HTTP_MESSAGE_TYPE_RESPONSE);
  ret->base.destroy = (void (*)(HTTPMessage *)) http_response_destroy;

  ret->status_code = HTTP_STATUS_CODE_200_OK;
  ret->status_message = NULL;

  return ret;
}

void http_response_destroy(HTTPResponse * response)
{
  assert(response);

  http_message_deinit_struct(&response->base);

  free(response->status_message);
  free(response);
}


/* GETTERS */

HTTPStatusCode http_response_get_status_code(HTTPResponse * response)
{
  assert(response);

  return response->status_code;
}

char * http_response_get_status_message(HTTPResponse * response)
{
  char * ret;

  assert(response);

  if (response->status_message)
    ret = response->status_message;
  else
    ret = http_status_code_get_default_message(response->status_code);

  return strings_clone(ret);
}


/* SETTERS */

void http_response_set_status_code(
    HTTPResponse * response,
    HTTPStatusCode code
    )
{
  assert(response);

  response->status_code = code;
}

void http_response_set_status_message(
    HTTPResponse * response,
    char * status_message
    )
{
  assert(response);

  free(response->status_message);
  response->status_message = strings_clone(status_message);
}

