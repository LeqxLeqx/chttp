
#ifndef __CHTTP_STATUS_CODE_H
#define __CHTTP_STATUS_CODE_H


enum HTTPStatusCode
{
  HTTP_STATUS_CODE_100_CONTINUE                          = 100,
  HTTP_STATUS_CODE_101_SWITCHING_PROTOCOLS               = 101,
  HTTP_STATUS_CODE_102_PROCESSING                        = 102,
  HTTP_STATUS_CODE_103_EARLY_HINTS                       = 103,

  HTTP_STATUS_CODE_200_OK                                = 200,
  HTTP_STATUS_CODE_201_CREATED                           = 201,
  HTTP_STATUS_CODE_202_ACCEPTED                          = 202,
  HTTP_STATUS_CODE_203_NON_AUTHORITATIVE_INFORMATION     = 203,
  HTTP_STATUS_CODE_204_NO_CONTENT                        = 204,
  HTTP_STATUS_CODE_205_RESET_CONTENT                     = 205,
  HTTP_STATUS_CODE_206_PARTIAL_CONTENT                   = 206,
  HTTP_STATUS_CODE_207_MULTI_STATUS                      = 207,
  HTTP_STATUS_CODE_208_ALREADY_REPORTED                  = 208,
  HTTP_STATUS_CODE_226_IM_USED                           = 226,

  HTTP_STATUS_CODE_300_MULTIPLE_CHOICES                  = 300,
  HTTP_STATUS_CODE_301_MOVED_PERMANENTLY                 = 301,
  HTTP_STATUS_CODE_302_FOUND                             = 302,
  HTTP_STATUS_CODE_303_SEE_OTHER                         = 303,
  HTTP_STATUS_CODE_304_NOT_MODIFIED                      = 304,
  HTTP_STATUS_CODE_305_USE_PROXY                         = 305,
  HTTP_STATUS_CODE_306_SWITCH_PROXY                      = 306,
  HTTP_STATUS_CODE_307_TEMPORARY_REDIRECT                = 307,
  HTTP_STATUS_CODE_308_PERMANENT_REDIRECT                = 308,

  HTTP_STATUS_CODE_400_BAD_REQUEST                       = 400,
  HTTP_STATUS_CODE_401_UNAUTHORIZED                      = 401,
  HTTP_STATUS_CODE_402_PAYMENT_REQUIRED                  = 402,
  HTTP_STATUS_CODE_403_FORBIDDEN                         = 403,
  HTTP_STATUS_CODE_404_NOT_FOUND                         = 404,
  HTTP_STATUS_CODE_405_METHOD_NOT_ALLOWED                = 405,
  HTTP_STATUS_CODE_406_NOT_ACCEPTABLE                    = 406,
  HTTP_STATUS_CODE_407_PROXY_AUTHENTICATION_REQUIRED     = 407,
  HTTP_STATUS_CODE_408_REQUEST_TIMEOUT                   = 408,
  HTTP_STATUS_CODE_409_CONFLICT                          = 409,
  HTTP_STATUS_CODE_410_GONE                              = 410,
  HTTP_STATUS_CODE_411_LENGTH_REQUIRED                   = 411,
  HTTP_STATUS_CODE_412_PRECONDITION_FAILED               = 412,
  HTTP_STATUS_CODE_413_PAYLOAD_TOO_LARGE                 = 413,
  HTTP_STATUS_CODE_414_URI_TOO_LONG                      = 414,
  HTTP_STATUS_CODE_415_UNSUPPORTED_MEDIA_TYPE            = 415,
  HTTP_STATUS_CODE_416_RANGE_NOT_SATISFIABLE             = 416,
  HTTP_STATUS_CODE_417_EXPECTATION_FAILED                = 417,
  HTTP_STATUS_CODE_418_IM_A_TEAPOT                       = 418,
  HTTP_STATUS_CODE_421_MISDIRECTED_REQUEST               = 421,
  HTTP_STATUS_CODE_422_UNPROCESSABLE_ENTITY              = 422,
  HTTP_STATUS_CODE_423_LOCKED                            = 423,
  HTTP_STATUS_CODE_424_FAILED_DEPENDENCY                 = 424,
  HTTP_STATUS_CODE_426_UPGRADE_REQUIRED                  = 426,
  HTTP_STATUS_CODE_428_PRECONDITION_REQUIRED             = 428,
  HTTP_STATUS_CODE_429_TOO_MANY_REQUESTS                 = 429,
  HTTP_STATUS_CODE_431_REQUEST_HEADER_FIELDS_TOO_LARGE   = 431,
  HTTP_STATUS_CODE_451_UNAVAILABLE_FOR_LEGAL_REASONS     = 451,

  HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR             = 500,
  HTTP_STATUS_CODE_501_NOT_IMPLEMENTED                   = 501,
  HTTP_STATUS_CODE_502_BAD_GATEWAY                       = 502,
  HTTP_STATUS_CODE_503_SERVICE_UNAVAILABLE               = 503,
  HTTP_STATUS_CODE_504_GATEWAY_TIMEOUT                   = 504,
  HTTP_STATUS_CODE_505_HTTP_VERSION_NOT_SUPPORTED        = 505,
  HTTP_STATUS_CODE_506_VARIANT_ALSO_NEGOTIATES           = 506,
  HTTP_STATUS_CODE_507_INSUFFICIENT_STORAGE              = 507,
  HTTP_STATUS_CODE_508_LOOP_DETECTED                     = 508,
  HTTP_STATUS_CODE_510_NOT_EXTENDED                      = 510,
  HTTP_STATUS_CODE_511_NETWORK_AUTHENTICATION_REQUIRED   = 511,
};
typedef enum HTTPStatusCode HTTPStatusCode;


char * http_status_code_get_default_message(HTTPStatusCode code);
HTTPStatusCode http_status_code_parse(char * str);

#endif


