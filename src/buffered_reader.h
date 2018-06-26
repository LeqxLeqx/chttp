
#ifndef __CHTTP_BUFFERED_READER_H
#define __CHTTP_BUFFERED_READER_H

#include <stdint.h>
#include <sys/types.h>

enum BufferedReaderError
{
  BUFFERED_READER_ERROR_NONE = 0,
  BUFFERED_READER_ERROR_TIMEOUT = 1,
  BUFFERED_READER_ERROR_LINE_TOO_LONG = 2,
  BUFFERED_READER_ERROR_ENCOUNTERED_CC = 3,
  BUFFERED_READER_ERROR_READ_FAILED = 4,
};
typedef enum BufferedReaderError BufferedReaderError;


struct BufferedReader;
typedef struct BufferedReader BufferedReader;

BufferedReader * buffered_reader_new(int fd);
void buffered_reader_destroy(BufferedReader * reader);

ssize_t buffered_reader_read(
    BufferedReader * reader,
    char * data,
    size_t data_length
    );
BufferedReaderError buffered_reader_read_line(
  BufferedReader * reader,
  size_t max,
  char ** out_ptr,
  uint32_t wait_time
  );



#endif

