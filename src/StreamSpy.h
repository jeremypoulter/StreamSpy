#ifndef _STREAMSPY_H
#define _STREAMSPY_H

#include <Arduino.h>
#include <functional>

typedef std::function<void(const uint8_t *buffer, size_t size)> ReadWriteHandler;

class StreamSpy : public Stream
{
  protected:
    Stream *_stream;
    uint8_t *_buffer;
    uint8_t *_head;
    uint8_t *_tail;
    uint8_t *_end;

    ReadWriteHandler _read;
    ReadWriteHandler _write;

    void onData(bool write, const uint8_t *buffer, size_t size);
  public:
    StreamSpy(Stream &stream);

    void begin(size_t buffer_size);
    void end();

    int available(void) {
      return _stream->available();
    }
    int peek(void);
    int read(void);
    void flush(void);
    size_t write(uint8_t chr);
    size_t write(const uint8_t *buffer, size_t size);

    inline size_t write(const char * s)
    {
      return write((uint8_t*) s, strlen(s));
    }
    inline size_t write(unsigned long n)
    {
      return write((uint8_t) n);
    }
    inline size_t write(long n)
    {
      return write((uint8_t) n);
    }
    inline size_t write(unsigned int n)
    {
      return write((uint8_t) n);
    }
    inline size_t write(int n)
    {
      return write((uint8_t) n);
    }

    inline void onRead(ReadWriteHandler read) {
      _read = read;
    }
    inline void onWrite(ReadWriteHandler write) {
      _write = write;
    }
    void printBuffer(Print &print);
};

#endif // !_STREAMSPY_H
