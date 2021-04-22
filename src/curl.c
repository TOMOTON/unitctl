
/*
 * Copyright (C) TOMOTON, GmbH
 * Copyright (C) Dann Martens
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <sys/stat.h>

#include "constants.h"


/* Where the write function simply returns the number of received bytes: */
size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data) {
  (void)ptr;
  (void)data;
  return size * nmemb;
}


struct response_t {
  char *memory;
  size_t size;
};


static size_t write_function_memory(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct response_t *mem = (struct response_t *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}


int http_put_unit(char *socket, char *path) {
  CURL *curl;
  CURLcode res;

  struct response_t chunk;

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  struct stat file_info;
  // curl_off_t speed_upload, total_time;
  FILE *fd;

  fd = fopen(path, "rb"); /* open file to upload */
  if(!fd)
    return 1; /* can't continue */

  /* to get the file size */
  if(fstat(fileno(fd), &file_info) != 0)
    return 1; /* can't continue */

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);
    /* upload to this place */
    curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, socket);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/config");

    /* tell it to "upload" to the URL */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* set where to read from (on Windows you need to use READFUNCTION too) */
    curl_easy_setopt(curl, CURLOPT_READDATA, fd);

    /* and give the size of the upload (optional) */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)file_info.st_size);

    /* enable verbose for easier tracing */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function_memory);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    } else {
        printf("%s\n", chunk.memory);
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    free(chunk.memory);
  }
  fclose(fd);
  return 0;
}

int http_get_unit(char *socket) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    //curl_easy_setopt(curl, CURLOPT_STDERR, myNullFile);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
    //curl_easy_setopt(curl, CURLOPT_MUTE, 1);

    curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, socket);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/config");
    res = curl_easy_perform(curl);

    if(CURLE_OK == res) {
      char *ct;
      /* ask for the content-type */
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

      if((CURLE_OK == res) && ct)
        printf("We received Content-Type: %s\n", ct);
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}
