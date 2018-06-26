
#include <assert.h>
#include <baselib/baselib.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#include "buffered_reader.h"


#define _BUFFERED_READER_BUFFER_LENGTH 0xFF

struct BufferedReader
{
  int fd;
  char data [_BUFFERED_READER_BUFFER_LENGTH];
  size_t ptr_diff, data_length;
};

static void buffered_reader_forward_buffer(BufferedReader * reader, size_t i)
{
  reader->data_length -= i;
  if (reader->data_length == 0)
  {
    reader->ptr_diff = 0;
  }
  else
    reader->ptr_diff += i;
}

static void buffered_reader_accumulate(
    char ** buffer_ptr, size_t * buffer_length_ptr, size_t increase
    )
{
  *buffer_length_ptr += increase;
  if (*buffer_ptr)
    *buffer_ptr = realloc(*buffer_ptr, *buffer_length_ptr);
  else
    *buffer_ptr = malloc(*buffer_length_ptr);
}

BufferedReader * buffered_reader_new(int fd)
{
  BufferedReader * reader = (BufferedReader *) malloc(sizeof(BufferedReader));

  reader->fd = fd;
  reader->ptr_diff = 0;
  reader->data_length = 0;
  return reader;
}

void buffered_reader_destroy(BufferedReader * reader)
{
  assert(reader);

  free(reader->data);
  free(reader);
}

ssize_t buffered_reader_read(
    BufferedReader * reader,
    char * data,
    size_t data_length
    )
{
  assert(reader);

  if (reader->data_length == 0)
  {
    return read(reader->fd, data, data_length);
  }
  else
  {
    if (data_length > reader->data_length)
      data_length = reader->data_length;

    memcpy(data, &reader->data[reader->ptr_diff], data_length);
    buffered_reader_forward_buffer(reader, data_length);

    return data_length;
  }
}

BufferedReaderError buffered_reader_read_line(
  BufferedReader * reader,
  size_t max,
  char ** out_ptr,
  uint32_t wait_time
  )
{
  BufferedReaderError err = BUFFERED_READER_ERROR_NONE;
  char * buffer = NULL, last = '\0', c;
  ssize_t receive_length;
  size_t copy_start, buffer_size = 0, read_size = 0, line_length = 0;
  time_t start_time = time(NULL);

  if (reader->data_length)
  {
    buffered_reader_accumulate(&buffer, &buffer_size, reader->data_length);
    memcpy(buffer, &reader->data[reader->ptr_diff], reader->data_length);
  }

  do
  {
    for (size_t k = read_size; k < buffer_size && !line_length && !err; k++)
    {
      if (k > max)
        err = BUFFERED_READER_ERROR_LINE_TOO_LONG;
      read_size++;

      c = buffer[k];
      if (c == '\n')
      {
          if (last == '\r')
            line_length = k;
          else
            err = BUFFERED_READER_ERROR_ENCOUNTERED_CC;
      }
      else if ((c < 0x20 && c != '\r') || last == '\r')
        err = BUFFERED_READER_ERROR_ENCOUNTERED_CC;

      last = c;
    }

    if (!line_length)
    {
      if (start_time + wait_time < time(NULL))
      {
        err = BUFFERED_READER_ERROR_TIMEOUT;
        continue;
      }

      copy_start = buffer_size;
      buffered_reader_accumulate(
          &buffer,
          &buffer_size,
          _BUFFERED_READER_BUFFER_LENGTH
          );
      
      errno = 0;
      receive_length = read(
          reader->fd,
          &buffer[copy_start],
          _BUFFERED_READER_BUFFER_LENGTH
          );

      if (receive_length <= 0)
      {
        if (errno != EWOULDBLOCK && errno != EAGAIN && errno != 0)
          err = BUFFERED_READER_ERROR_READ_FAILED;
        else
          buffer_size -= _BUFFERED_READER_BUFFER_LENGTH;

      }
      else
        buffer_size -= _BUFFERED_READER_BUFFER_LENGTH - receive_length;
    }
  }
  while (!line_length && !err);

  if (!err)
  {
    buffer[line_length - 1] = '\0';
    *out_ptr = strings_clone(buffer);
    reader->ptr_diff = 0;

    if (line_length + 1 == buffer_size)
      reader->data_length = 0;
    else
    {
      reader->data_length = buffer_size - line_length - 1;
      assert(reader->data_length < _BUFFERED_READER_BUFFER_LENGTH);
      memcpy(reader->data, &buffer[line_length + 1], reader->data_length);
    }
  }

  free(buffer);

  return err;
}



