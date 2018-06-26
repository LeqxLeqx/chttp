
#include <assert.h>
#include <baselib/baselib.h>
#include <stdlib.h>

#include "http_utils.h"

#include "http_message.h"
#include "http_message_type.h"
#include "http_message_struct.h"
#include "http_request.h"

struct HTTPRequest
{
  HTTPMessage base;
  HTTPMethod method;
  char * path, * query;
  Dictionary * params;
};


HTTPRequest * http_request_new()
{
  HTTPRequest * ret;
  
  ret = (HTTPRequest *) malloc(sizeof(HTTPRequest));
  assert(ret);

  http_message_init_struct(&ret->base, HTTP_MESSAGE_TYPE_REQUEST);
  ret->base.destroy = (void (*)(HTTPMessage *)) http_request_destroy;

  ret->method = HTTP_METHOD_GET;
  ret->path = strings_clone("/");
  ret->query = strings_clone("");
  ret->params = dictionary_new(DICTIONARY_TYPE_HASH_TABLE);

  return ret;
}

void http_request_destroy(HTTPRequest * request)
{
  assert(request);

  http_message_deinit_struct(&request->base);
  free(request->path);
  free(request->query);
  dictionary_destroy_and_free(request->params);
}


/* GETTERS */

HTTPMethod http_request_get_method(HTTPRequest * request)
{
  assert(request);

  return request->method;
}

char * http_request_get_target(HTTPRequest * request)
{
  char * params_str, * ret;

  assert(request);

  params_str = http_utils_params_to_string(request->params);
  if (!params_str)
    ret = strings_clone(request->path);
  else
  {
    ret = strings_format("%s?%s", request->path, params_str);
    free(params_str);
  }

  return ret;
}

char * http_request_get_path(HTTPRequest * request)
{
  assert(request);

  return strings_clone(request->path);
}

char * http_request_get_query(HTTPRequest * request)
{
  assert(request);

  return strings_clone(request->query);
}

Dictionary * http_request_get_parameters(HTTPRequest * request)
{
  Dictionary * ret;
  assert(request);

  ret = dictionary_new(DICTIONARY_TYPE_HASH_TABLE);

  http_utils_transfer_string_dictionary(ret, request->params);

  return ret;
}



/* SETTERS */

void http_request_set_method(HTTPRequest * request, HTTPMethod method)
{
  assert(request);

  request->method = method;
}

void http_request_set_target(HTTPRequest * request, char * target)
{
  int q_index;
  unsigned int target_length;
  char * temp_path = NULL, * temp_query = NULL, * temp = NULL;

  assert(request);
  assert(target);

  target_length = strings_length(target);

  q_index = strings_index_of(target, '?');

  if (q_index == -1)
  {
    temp_path = http_utils_url_deescape(target);

    http_request_set_path(request, temp_path);
    http_request_set_query(request, "");
  }
  else
  {
    temp = strings_prefix(target, q_index);
    temp_path = http_utils_url_deescape(temp);
    temp_query = strings_postfix(target, target_length - q_index - 1);

    http_request_set_path(request, temp_path);
    http_request_set_query(request, temp_query);
  }

  free(temp);
  free(temp_path);
  free(temp_query);
}

void http_request_set_path(HTTPRequest * request, char * path)
{
  assert(request);

  free(request->path);
  request->path = strings_clone(path);
}

void http_request_set_query(HTTPRequest * request, char * query)
{
  assert(request);

  free(request->query);
  request->query = strings_clone(query);

  dictionary_clear_and_free(request->params);
  http_utils_parse_query_parameters(request->params, query);
}

void http_request_set_parameters(HTTPRequest * request, Dictionary * params)
{
  assert(request);

  dictionary_clear_and_free(request->params);
  http_utils_transfer_string_dictionary(request->params, params);
}


