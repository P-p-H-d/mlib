#if HAVE_CURL

#include <curl/curl.h>
#include "m-dict.h"
#include "m-tuple.h"
#include "m-string.h"
#include "m-list.h"
#include "m-serial-json.h"

/////////////////////////////////////////

// Define container types
// First a generic map string to string
DICT_DEF2(map_str, string_t, string_t)
#define M_OPL_map_str_t() DICT_OPLIST(map_str, STRING_OPLIST, STRING_OPLIST)
  
// This structure matches the JSON structure returns by the used WEB server
// for the GET command.
// It can be whatever you want provided it matches what is expected by the WEB server
TUPLE_DEF2(web_data,
           (args, map_str_t),
           (headers, map_str_t),
           (origin, string_t),
           (url, string_t))
#define M_OPL_web_data_t() TUPLE_OPLIST(web_data, M_OPL_map_str_t(), M_OPL_map_str_t(), STRING_T, STRING_T)

/////////////////////////////////////////


static size_t
write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
  (void) size; // Parameter not used
  char *src = buffer;
  string_ptr str = userp;
  for(size_t i = 0; i < nmemb; i++) {
    string_push_back(str, src[i]);
  }
  return nmemb;
}

static bool get_data(web_data_t data, const char url[])
{
  m_serial_return_code_t r = M_SERIAL_FAIL;
  CURLcode res;
  string_t str;
  CURL *handle = curl_easy_init();
  if (handle) {
      string_init(str);
      // Perform an http GET request and fill in str with the JSON answer
      curl_easy_setopt(handle, CURLOPT_URL, url);
      curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
      curl_easy_setopt(handle, CURLOPT_WRITEDATA, str);
      res = curl_easy_perform(handle);
      curl_easy_cleanup(handle);
      if (res == CURLE_OK) {
        // Parse the string and fill in the object
        m_serial_str_json_read_t serial;
        m_serial_str_json_read_init(serial, string_get_cstr(str));
        r = web_data_in_serial(data, serial);
        m_serial_str_json_read_clear(serial);
      }
      string_clear(str);
  }
  return r == M_SERIAL_OK_DONE;
}

/////////////////////////////////////////

static size_t
ignore_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
  (void) buffer; // Parameter not used
  (void) size; // Parameter not used
  (void) userp; // Parameter not used
  return nmemb;
}

static bool post_data(web_data_t data, const char url[], bool display_answer)
{
  m_serial_return_code_t r = M_SERIAL_FAIL;
  CURLcode res = 0;
  string_t str;
  CURL *handle = curl_easy_init();
  if (handle) {
    string_init(str);
    // Get a JSON string
    m_serial_str_json_write_t serial;
    m_serial_str_json_write_init(serial, str);
    r = web_data_out_serial(serial, data);
    m_serial_str_json_write_clear(serial);
    if (r == M_SERIAL_OK_DONE) {
      // Post JSON string
       struct curl_slist *headers=NULL;
       headers = curl_slist_append(headers, "Content-Type: text/json");
       if (headers != NULL) {
         curl_easy_setopt(handle, CURLOPT_POSTFIELDS, string_get_cstr(str));
         curl_easy_setopt(handle, CURLOPT_URL, url);
         curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
         if (display_answer == false) {
           curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, ignore_data);
           curl_easy_setopt(handle, CURLOPT_WRITEDATA, NULL);
         }
         res = curl_easy_perform(handle);
         curl_slist_free_all(headers);
       }
    }
    curl_easy_cleanup(handle);
    string_clear(str);
  }
  return (r == M_SERIAL_OK_DONE) && (res == CURLE_OK);
}

/////////////////////////////////////////

// Remote server URL
#define URL_GET "https://httpbin.org/get"
#define URL_POST "https://httpbin.org/post"
  
int main(void)
{
  CURLcode ret = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (ret != CURLE_OK) abort();

  web_data_t data;
  web_data_init(data);

  // Test http GET
  bool b = get_data(data, URL_GET);
  if (b) {
    // We success receiving the JSON object and fill in the C object web_data with it.
    // Let's display the object content:
    printf("SUCCESS. Receive data =\n");
    web_data_out_str(stdout, data);
    printf("\n");
  } else {
    printf("ERROR: Cannot GET data or get data are not in expected format\n");
  }

  // Test http POST
  web_data_emplace_origin(data, "This is the origin");
  web_data_emplace_url(data, "This is the URL");
  //web_data_set_origin(data, STRING_CTE("This is the origin"));
  //web_data_set_url(data, STRING_CTE("This is the URL"));
  b = post_data(data, URL_POST, false);
  if (b) {
    printf("SUCCESS: Post\n");
  } else {
    printf("ERROR post\n");
  }

  web_data_clear(data);

  curl_global_cleanup();
  return 0;
}

#else
int main(void) { return 0; }
#endif
