
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "http_version.h"

#include "http_utils.h"

/* header fields which do not conform with
 * standard http header field naming convention
 */
#define _HTTP_UTILS_A_IM_HEADER "A-IM"
#define _HTTP_UTILS_TE_HEADER "TE"
#define _HTTP_UTILS_CONTENT_MD5_HEADER "Content-MD5"
#define _HTTP_UTILS_ETAG_HEADER "ETag"
#define _HTTP_UTILS_IM_HEADER "IM"
#define _HTTP_UTILS_WWW_AUTHENTICATE_HEADER "WWW-Authenticate"

#define HTTP_UTILS_SUNDAY 0
#define HTTP_UTILS_MONDAY 1
#define HTTP_UTILS_TUESDAY 2
#define HTTP_UTILS_WEDNESDAY 3
#define HTTP_UTILS_THURSDAY 4
#define HTTP_UTILS_FRIDAY 5
#define HTTP_UTILS_SATURDAY 6

#define HTTP_UTILS_JANUARY 0
#define HTTP_UTILS_FEBURARY 1
#define HTTP_UTILS_MARCH 2
#define HTTP_UTILS_APRIL 3
#define HTTP_UTILS_MAY 4
#define HTTP_UTILS_JUNE 5
#define HTTP_UTILS_JULY 6
#define HTTP_UTILS_AUGUST 7
#define HTTP_UTILS_SEPTEMBER 8
#define HTTP_UTILS_OCTOBER 9
#define HTTP_UTILS_NOVEMBER 10
#define HTTP_UTILS_DECEMBER 11


static char * http_utils_long_day_of_week_string(int k)
{
  switch (k)
  {
    case HTTP_UTILS_SUNDAY:
      return "Sunday";
    case HTTP_UTILS_MONDAY:
      return "Monday";
    case HTTP_UTILS_TUESDAY:
      return "Tuesday";
    case HTTP_UTILS_WEDNESDAY:
      return "Wednesday";
    case HTTP_UTILS_THURSDAY:
      return "Thursday";
    case HTTP_UTILS_FRIDAY:
      return "Friday";
    case HTTP_UTILS_SATURDAY:
      return "Saturday";

    default:
      assert(0);
  }
}

static char * http_utils_short_day_of_week_string(int k)
{
  switch (k)
  {
    case HTTP_UTILS_SUNDAY:
      return "Sun";
    case HTTP_UTILS_MONDAY:
      return "Mon";
    case HTTP_UTILS_TUESDAY:
      return "Tue";
    case HTTP_UTILS_WEDNESDAY:
      return "Wed";
    case HTTP_UTILS_THURSDAY:
      return "Thu";
    case HTTP_UTILS_FRIDAY:
      return "Fri";
    case HTTP_UTILS_SATURDAY:
      return "Sat";

    default:
      assert(0);
  }
}

static char * http_utils_month_string(int k)
{
  switch (k)
  {
    case HTTP_UTILS_JANUARY:
      return "Jan";
    case HTTP_UTILS_FEBURARY:
      return "Feb";
    case HTTP_UTILS_MARCH:
      return "Mar";
    case HTTP_UTILS_APRIL:
      return "Apr";
    case HTTP_UTILS_MAY:
      return "May";
    case HTTP_UTILS_JUNE:
      return "Jun";
    case HTTP_UTILS_JULY:
      return "Jul";
    case HTTP_UTILS_AUGUST:
      return "Aug";
    case HTTP_UTILS_SEPTEMBER:
      return "Sep";
    case HTTP_UTILS_OCTOBER:
      return "Oct";
    case HTTP_UTILS_NOVEMBER:
      return "Nov";
    case HTTP_UTILS_DECEMBER:
      return "Dec";
    
    default:
      assert(0);
  }
}

List * http_utils_parse_cookie(char * str, bool ignore_bad_names)
{
  HTTPCookie * cookie;
  List * ret, * split;
  ListTraversal * trav;
  char * cookie_str;
  char * name, * value;

  ret = list_new(LIST_TYPE_LINKED_LIST);
  if (strings_is_whitespace(str))
    return ret;

  split = strings_split(str, ';');
  trav = list_get_traversal(split);

  while (!list_traversal_completed(trav))
  {
    cookie_str = list_traversal_next_str(trav);

    if (strings_is_null_or_whitespace(cookie_str))
      continue;
    
    if (!http_utils_split_about(cookie_str, &name, &value, '='))
    {
      if (ignore_bad_names)
      {
        cookie_str = strings_trim(cookie_str);
        if (
          strings_equals(cookie_str, "HttpOnly") ||
          strings_equals(cookie_str, "Secure")
          )
        {
          free(cookie_str);
          continue;
        }
        free(cookie_str);
      }
      list_traversal_destroy(trav);
      list_destroy(split);
      list_destroy_and_user_free(ret, (void (*)(void *)) http_cookie_destroy);
      return NULL;
    }


    if (
      strings_equals(name, "Expires") ||
      strings_equals(name, "Max-Age") ||
      strings_equals(name, "Domain") ||
      strings_equals(name, "Path")
      )
    {
      
      free(name);
      free(value);

      if (ignore_bad_names)
        continue;
      else
      {
        list_traversal_destroy(trav);
        list_destroy(split);
        list_destroy_and_user_free(ret, (void (*)(void *)) http_cookie_destroy);
        return NULL;
      }
    }

    cookie = http_cookie_new();

    http_cookie_set_name(cookie, name);
    http_cookie_set_value(cookie, value);
    
    free(name);
    free(value);

    list_add(ret, ptr_to_any(cookie));
  }

  return ret;
}

static bool http_utils_parse_cookie_attribute(
        char * cookie_str, time_t * expiry, uint32_t * max_age,
        char ** domain, char ** path, bool * secure, bool * httponly
        )
{
  bool err = false;
  char
    * trim = strings_trim(cookie_str), * name, * value;

  if (strings_equals(trim, "Secure"))
    *secure = true;
  else if (strings_equals(trim, "HttpOnly"))
    *httponly = true;
  else
  {
    if (http_utils_split_about(trim, &name, &value, '='))
    {
      if (strings_equals(name, "Expiries"))
      {
        *expiry = http_utils_parse_date(value);
        if (!(*expiry))
          err = true;
      }
      else if (strings_equals(name, "Max-Age"))
      {
        *max_age = strtoul(value, NULL, 10);
        if (!(*max_age))
          err = true;
      }
      else if (strings_equals(name, "Domain"))
      {
        free(*domain);
        *domain = value;
        value = NULL; /* prevents from being freed */
      }
      else if (strings_equals(name, "Path"))
      {
        free(*path);
        *path = value;
        value = NULL;
      }
      
      free(name);
      free(value);
    }
  }

  free(trim);
  return !err;
}

static void http_utils_apply_attributes_to_cookie(
    HTTPCookie * cookie, time_t expiry, uint32_t max_age,
    char * domain, char * path, bool secure, bool httponly
    )
{
  if (expiry)
    http_cookie_set_expiry(cookie, expiry);
  if (max_age)
    http_cookie_set_max_age(cookie, max_age);
  if (domain)
    http_cookie_set_domain(cookie, domain);
  if (path)
    http_cookie_set_path(cookie, path);

  http_cookie_set_secure(cookie, secure);
  http_cookie_set_httponly(cookie, httponly);
}

List * http_utils_parse_set_cookie(char * str)
{
  HTTPCookie * cookie;
  List * ret, * split;
  ListTraversal * trav;
  char * cookie_str;
  time_t expiry = 0;
  uint32_t max_age = 0;
  char * domain  = NULL, * path = NULL;
  bool secure = false, httponly = false;

  assert(str);

  if (strings_is_whitespace(str))
    return list_new(LIST_TYPE_LINKED_LIST);

  split = strings_split(str, ';');

  trav = list_get_traversal(split);
  while (!list_traversal_completed(trav))
  {
    cookie_str = list_traversal_next_str(trav);
    if (strings_is_whitespace(cookie_str))
      continue;

    if (!http_utils_parse_cookie_attribute(
        cookie_str, &expiry, &max_age, &domain, &path, &secure, &httponly
        ))
    {
      list_traversal_destroy(trav);
      return NULL;
    }
  }

  list_destroy_and_free(split);

  ret = http_utils_parse_cookie(str, true);
  if (!ret)
  {
    free(domain);
    free(path);
    return NULL;
  }

  trav = list_get_traversal(ret);
  while (!list_traversal_completed(trav))
  {
    cookie = (HTTPCookie *) list_traversal_next_ptr(trav);
    http_utils_apply_attributes_to_cookie(
        cookie, expiry, max_age, domain, path, secure, httponly
        );
  }

  free(domain);
  free(path);

  return ret;
}

static int http_utils_parse_short_weekday(char * str)
{
  if (strings_equals_ignore_case(str, "sun"))
    return HTTP_UTILS_SUNDAY;
  else if (strings_equals_ignore_case(str, "mon"))
    return HTTP_UTILS_MONDAY;
  else if (strings_equals_ignore_case(str, "tue"))
    return HTTP_UTILS_TUESDAY;
  else if (strings_equals_ignore_case(str, "wed"))
    return HTTP_UTILS_WEDNESDAY;
  else if (strings_equals_ignore_case(str, "thu"))
    return HTTP_UTILS_THURSDAY;
  else if (strings_equals_ignore_case(str, "fri"))
    return HTTP_UTILS_FRIDAY;
  else if (strings_equals_ignore_case(str, "sat"))
    return HTTP_UTILS_SATURDAY;
  else
    return -1;
}

static int http_utils_parse_long_weekday(char * str)
{
  if (strings_equals_ignore_case(str, "sunday"))
    return HTTP_UTILS_SUNDAY;
  else if (strings_equals_ignore_case(str, "monday"))
    return HTTP_UTILS_MONDAY;
  else if (strings_equals_ignore_case(str, "tuesday"))
    return HTTP_UTILS_TUESDAY;
  else if (strings_equals_ignore_case(str, "wednesday"))
    return HTTP_UTILS_WEDNESDAY;
  else if (strings_equals_ignore_case(str, "thursday"))
    return HTTP_UTILS_THURSDAY;
  else if (strings_equals_ignore_case(str, "friday"))
    return HTTP_UTILS_FRIDAY;
  else if (strings_equals_ignore_case(str, "saturday"))
    return HTTP_UTILS_SATURDAY;
  else
    return -1;
}

static bool http_utils_parse_time(char * str, struct tm * t)
{
  char hour_buffer[3], minute_buffer[3], second_buffer[3], * endptr;
  
  memcpy(hour_buffer, str, 2);
  memcpy(minute_buffer, &str[3], 2);
  memcpy(second_buffer, &str[6], 2);

  hour_buffer[2] = 0;
  minute_buffer[2] = 0;
  second_buffer[2] = 0;

  t->tm_hour = strtoul(hour_buffer, &endptr, 10);
  if (endptr[0] != 0 || t->tm_hour > 23)
    return false;

  t->tm_min = strtoul(minute_buffer, &endptr, 10);
  if (endptr[0] != 0 || t->tm_min > 59)
    return false;

  t->tm_sec = strtoul(second_buffer, &endptr, 10);
  if (endptr[0] != 0 || t->tm_sec > 60) /* leap sconds */
    return false;

  return true;
}

static int http_utils_parse_month(char * str)
{
  if (strings_equals_ignore_case(str, "jan"))
    return HTTP_UTILS_JANUARY;
  else if (strings_equals_ignore_case(str, "feb"))
    return HTTP_UTILS_FEBURARY;
  else if (strings_equals_ignore_case(str, "mar"))
    return HTTP_UTILS_MARCH;
  else if (strings_equals_ignore_case(str, "apr"))
    return HTTP_UTILS_APRIL;
  else if (strings_equals_ignore_case(str, "may"))
    return HTTP_UTILS_MAY;
  else if (strings_equals_ignore_case(str, "jun"))
    return HTTP_UTILS_JUNE;
  else if (strings_equals_ignore_case(str, "jul"))
    return HTTP_UTILS_JULY;
  else if (strings_equals_ignore_case(str, "aug"))
    return HTTP_UTILS_AUGUST;
  else if (strings_equals_ignore_case(str, "sep"))
    return HTTP_UTILS_SEPTEMBER;
  else if (strings_equals_ignore_case(str, "oct"))
    return HTTP_UTILS_OCTOBER;
  else if (strings_equals_ignore_case(str, "nov"))
    return HTTP_UTILS_NOVEMBER;
  else if (strings_equals_ignore_case(str, "dec"))
    return HTTP_UTILS_DECEMBER;
  else
    return -1;
}

static bool http_utils_parse_rfc_882_time(
  char * str,
  unsigned int str_length,
  struct tm * t
  )
{
  char weekday_buffer[4], day_buffer[3], month_buffer[4],
       year_buffer[5], time_buffer[9], * endptr;

  if (str_length != 29)
    return false;

  if (str[3] != ',' || str[4] != ' ' || str[7] != ' ' ||
      str[11] != ' ' || str[16] != ' ' || str[25] != ' ' ||
      str[26] != 'G' || str[27] != 'M' || str[28] != 'T'
      )
    return false;

  memcpy(weekday_buffer, str, 3);
  memcpy(day_buffer, &str[5], 2);
  memcpy(month_buffer, &str[8], 3);
  memcpy(year_buffer, &str[12], 4);
  memcpy(time_buffer, &str[17], 8);

  weekday_buffer[3] = 0;
  day_buffer[2] = 0;
  month_buffer[3] = 0;
  year_buffer[4] = 0;
  time_buffer[8] = 0;

  if (!http_utils_parse_time(time_buffer, t))
    return false;

  t->tm_wday = http_utils_parse_short_weekday(weekday_buffer);
  if (t->tm_wday == -1)
    return false;
  t->tm_mday = strtoul(day_buffer, &endptr, 10);
  if (endptr[0] != 0 || t->tm_mday == 0 || t->tm_mday > 31)
    return false;
  t->tm_mon = http_utils_parse_month(month_buffer);
  if (t->tm_mon == -1)
    return false;
  t->tm_year = strtoul(year_buffer, &endptr, 10) - 1900;
  if (endptr[0] != 0 || t->tm_year < 0)
    return false;
  
  return true;
}

static bool http_utils_parse_rfc_850_time(
    char * str,
    unsigned int str_length,
    struct tm * t
    )
{
  int comma_index;
  char weekday_buffer [10], day_buffer [3], month_buffer[4],
       year_buffer [3], time_buffer[9], * endptr;

  comma_index = strings_index_of(str, ',');

  if (comma_index < 6 || comma_index > 9)
    return 0;
  if (str_length < comma_index + 24)
    return false;

  if (str[comma_index] != ',' || str[comma_index + 1] != ' ' ||
      str[comma_index + 4] != '-' || str[comma_index + 8] != '-' ||
      str[comma_index + 11] != ' ' || str[comma_index + 20] != ' ' ||
      str[comma_index + 21] != 'G' || str[comma_index + 22] != 'M' ||
      str[comma_index + 23] != 'T'
      )
    return false;

  memcpy(weekday_buffer, str, comma_index);
  memcpy(day_buffer, &str[comma_index + 2], 2);
  memcpy(month_buffer, &str[comma_index + 5], 3);
  memcpy(year_buffer, &str[comma_index + 9], 2);
  memcpy(time_buffer, &str[comma_index + 12], 8);

  weekday_buffer[comma_index] = 0;
  day_buffer[2] = 0;
  month_buffer[3] = 0;
  year_buffer[2] = 0;
  time_buffer[8] = 0;

  if (!http_utils_parse_time(time_buffer, t))
    return false;

  t->tm_wday = http_utils_parse_long_weekday(weekday_buffer);
  if (t->tm_wday == -1)
    return false;
  t->tm_mday = strtoul(day_buffer, &endptr, 10);
  if (endptr[0] != 0 || t->tm_mday == 0 || t->tm_mday > 31)
    return false;
  t->tm_mon = http_utils_parse_month(month_buffer);
  if (t->tm_mon == -1)
    return false;
  t->tm_year = strtoul(year_buffer, &endptr, 10);
  if (endptr[0] != 0)
    return false;

  if (t->tm_year < 69)
    t->tm_year += 100;
  
  return true;
}

static bool http_utils_parse_ansi_c_time(
    char * str,
    unsigned int str_length,
    struct tm * t)
{
  char weekday_buffer [4], day_buffer [3], month_buffer[4],
       year_buffer [5], time_buffer[9], * endptr;

  if (str_length != 24)
    return false;

  if (str[3] != ' ' || str[7] != ' ' || str[10] != ' ' || str[19] != ' ')
    return false;

  memcpy(weekday_buffer, str, 3);
  memcpy(day_buffer, &str[8], 2);
  memcpy(month_buffer, &str[4], 3);
  memcpy(year_buffer, &str[20], 4);
  memcpy(time_buffer, &str[11], 8);

  weekday_buffer[3] = 0;
  day_buffer[2] = 0;
  month_buffer[3] = 0;
  year_buffer[2] = 0;
  time_buffer[8] = 0;

  if (!http_utils_parse_time(time_buffer, t))
    return false;

  t->tm_wday = http_utils_parse_short_weekday(weekday_buffer);
  if (t->tm_wday == -1)
    return false;
  t->tm_mday = strtoul(day_buffer, &endptr, 10);
  if (endptr[0] != 0 || t->tm_mday == 0 || t->tm_mday > 31)
    return false;
  t->tm_mon = http_utils_parse_month(month_buffer);
  if (t->tm_mon == -1)
    return false;
  t->tm_year = strtoul(year_buffer, &endptr, 10) - 1900;
  if (endptr[0] != 0 || t->tm_year < 0)
    return false;
  
  return true;
}

time_t http_utils_parse_date(char * str)
{
  unsigned int str_length;
  bool successful;
  time_t ret;
  struct tm t, t_copy;

  assert(str);

  str_length = strings_length(str);
  if (str_length < 20)
    return 0;
  
  if (strings_ends_with(str, "GMT"))
  {
    if (str[3] == ',') /* RFC 822 */
      successful = http_utils_parse_rfc_882_time(str, str_length, &t);
    else /* RFC 850 */
      successful = http_utils_parse_rfc_850_time(str, str_length, &t);
  }
  else /* ANSI C asctime() format */
    successful = http_utils_parse_ansi_c_time(str, str_length, &t);

  if (!successful)
    return 0;

  memcpy(&t_copy, &t, sizeof(struct tm));

  ret = mktime(&t);
  if (ret == (time_t) -1)
    return 0;

  if ( /* dis-permits invalid date times which would be normalized by mktime */
    t.tm_mday != t_copy.tm_mday ||
    t.tm_wday!= t_copy.tm_wday ||
    t.tm_mon != t_copy.tm_mon ||
    t.tm_year != t_copy.tm_year ||
    t.tm_hour != t_copy.tm_hour ||
    t.tm_min != t_copy.tm_min ||
    t.tm_sec != t_copy.tm_sec
    )
    return 0;
  
  return ret;
}

char * http_utils_date_to_string(time_t date, HTTPVersion ver)
{
  struct tm time;

  gmtime_r(&date, &time);

  switch (ver)
  {
    case HTTP_VERSION_0_9:
      return strings_format(
        "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
        http_utils_long_day_of_week_string(time.tm_wday),
        time.tm_mday,
        http_utils_month_string(time.tm_mon),
        time.tm_year % 100,
        time.tm_hour,
        time.tm_min,
        time.tm_sec
        );

    default:
      return strings_format(
        "%s, %02d %s %04d %02d:%02d:%02d GMT",
        http_utils_short_day_of_week_string(time.tm_wday),
        time.tm_mday,
        http_utils_month_string(time.tm_mon),
        time.tm_year + 1900,
        time.tm_hour,
        time.tm_min,
        time.tm_sec
        );
      break;
  }
}


void http_utils_parse_query_parameters(Dictionary * dic, char * query)
{
  List * split;
  char * str, * name, * deescaped_name, * value;
  ListTraversal * trav;

  assert(dic);
  assert(query);

  if (strings_is_empty(query))
    return; /* NOTHING TO ADD TO DICTIONARY */

  split = strings_split(query, '&');
  trav = list_get_traversal(split);

  while (!list_traversal_completed(trav))
  {
    str = list_traversal_next_str(trav);
    if (strings_is_whitespace(str))
      continue;

    if (http_utils_split_about_no_trim(str, &name, &value, '='))
    {
      deescaped_name = http_utils_url_deescape(name);
      dictionary_set_and_free(
          dic,
          deescaped_name,
          str_to_any(http_utils_url_deescape(value))
          );

      free(deescaped_name);
      free(value);
      free(name);
    }
    else
    {
      deescaped_name = http_utils_url_deescape(str);
      dictionary_set_and_free(
        dic,
        deescaped_name,
        str_to_any(strings_empty())
        );

      free(deescaped_name);
    }
  }

  list_destroy_and_free(split);
}

char * http_utils_params_to_string(Dictionary * params)
{
  List * keys;
  ListTraversal * trav;
  StringBuilder * sb;
  char * key, * value;
  bool first = true;


  keys = dictionary_get_keys(params);
  if (list_size(keys) == 0)
  {
    list_destroy(keys);
    return NULL;
  }

  
  trav = list_get_traversal(keys);
  sb = string_builder_new();

  while (!list_traversal_completed(trav))
  {
    key = list_traversal_next_str(trav);
    value = any_to_str(dictionary_get(params, key));
    
    if (first)
      string_builder_appendf(sb, "%s=%s", key, value);
    else
      string_builder_appendf(sb, "&%s=%s", key, value);
  }

  list_destroy_and_free(keys);

  return string_builder_to_string_destroy(sb);
}

char * http_utils_url_escape(char * str)
{
  StringBuilder * sb;
  unsigned int str_length;
  char c;

  str_length = strings_length(str);
  sb = string_builder_new();

  for (unsigned int k = 0; k < str_length; k++)
  {
    c = str[k];
    if (!chars_is_alpha_or_digit(c) && c != '/' && c != '&' && c != '?')
      string_builder_appendf(sb, "%02x", (unsigned char) c);
    else
      string_builder_append_char(sb, c);
  }

  return string_builder_to_string_destroy(sb);
}

char * http_utils_url_deescape(char * str)
{
  StringBuilder * sb;
  unsigned int str_length;
  char c, hex_buffer[3];
  unsigned long escaped_code_point;
  
  str_length = strings_length(str);
  sb = string_builder_new();

  for (unsigned int k = 0; k < str_length; k++)
  {
    c = str[k];
    if (c == '%')
    {
      if (
        k + 2 >= str_length ||
        !chars_is_hex_digit(str[k + 1]) ||
        !chars_is_hex_digit(str[k + 2]) ||
        (str[k + 1] == '0' && str[k + 2] == '0')
        )
        string_builder_append_char(sb, c);
      else
      {
        memcpy(hex_buffer, &str[k + 1], 2);
        hex_buffer[3] = 0;
        escaped_code_point = strtoul(hex_buffer, NULL, 16);
        string_builder_append_char(sb, (char) escaped_code_point);
        k += 2;
      }
    }
    else
      string_builder_append_char(sb, c);
  }

  return string_builder_to_string_destroy(sb);
}


void http_utils_transfer_string_dictionary(Dictionary * to, Dictionary * from)
{
  assert(to);
  assert(from);

  char * key, * str_value;
  Any value;
  List * keys;
  ListTraversal * trav;

  keys = dictionary_get_keys(from);
  trav = list_get_traversal(keys);

  while (!list_traversal_completed(trav))
  {
    key = list_traversal_next_str(trav);
    value = dictionary_get(from, key);
    str_value = any_to_str(value);
    str_value = strings_clone(str_value);
    value = str_to_any(str_value);

    dictionary_set_and_free(to, key, value);
  }

  list_destroy_and_free(keys);
}

static bool http_utils_split_about_imp(
    char * src, char ** before, char ** after, char c, bool trim
    )
{
  int index, length;

  char * before_pre, * after_pre;

  assert(src);
  assert(before);
  assert(after);

  index = strings_index_of(src, c);
  if (index == -1)
    return false;

  length = strings_length(src);

  if (trim)
  {
    before_pre = strings_prefix(src, index);
    after_pre = strings_postfix(src, length - index - 1);

    *before = strings_trim(before_pre);
    *after = strings_trim(after_pre);

    free(before_pre);
    free(after_pre);
  }
  else
  {
    *before = strings_prefix(src, index);
    *after = strings_postfix(src, length - index - 1);
  }

  return true;
}
bool http_utils_split_about_no_trim(
    char * src, char ** before, char ** after, char c
    )
{
  return http_utils_split_about_imp(src, before, after, c, false);
}

bool http_utils_split_about(
    char * src, char ** before, char ** after, char c
    )
{
  return http_utils_split_about_imp(src, before, after, c, true);
}

static char * http_utils_headerize_common(char * header_name)
{
  if (strings_equals_ignore_case(header_name, _HTTP_UTILS_A_IM_HEADER))
    return strings_clone(_HTTP_UTILS_A_IM_HEADER);
  else if (strings_equals_ignore_case(header_name, _HTTP_UTILS_TE_HEADER))
    return strings_clone(_HTTP_UTILS_TE_HEADER);
  else if (
    strings_equals_ignore_case( header_name, _HTTP_UTILS_CONTENT_MD5_HEADER)
    )
    return strings_clone(_HTTP_UTILS_CONTENT_MD5_HEADER);
  else if (strings_equals_ignore_case(header_name, _HTTP_UTILS_ETAG_HEADER))
    return strings_clone(_HTTP_UTILS_ETAG_HEADER);
  else if (strings_equals_ignore_case(header_name, _HTTP_UTILS_IM_HEADER))
    return strings_clone(_HTTP_UTILS_IM_HEADER);
  else if (
    strings_equals_ignore_case(header_name, _HTTP_UTILS_WWW_AUTHENTICATE_HEADER)
    )
    return strings_clone(_HTTP_UTILS_WWW_AUTHENTICATE_HEADER);
  else
    return NULL;
}

char * http_utils_headerize(char * header_name)
{
  char * ret, c;
  bool capitalize;
  unsigned int length;

  assert(header_name);

  ret = http_utils_headerize_common(header_name);
  if (ret)
    return ret;

  length = strings_length(header_name);

  ret = malloc(sizeof(char) * (length + 1));
  assert(ret);

  ret[length] = 0; /* null terminate */

  capitalize = true;
  for (unsigned int k = 0; k < length; k++)
  {
    c = header_name[k];
    if (capitalize)
      ret[k] = chars_to_upper(c);
    else
      ret[k] = chars_to_lower(c);

    capitalize = !chars_is_alpha(c);
  }

  return ret;
}

