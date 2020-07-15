#include "StreamSpy.h"


StreamSpy::StreamSpy(Stream &stream) :
  _stream(&stream),
  _buffer(NULL),
  _head(NULL),
  _tail(NULL),
  _end(NULL),
  _read(NULL),
  _write(NULL)
{

}

void StreamSpy::begin(size_t buffer_size)
{
  end();

  if(buffer_size > 0)
  {
    _buffer = new uint8_t[buffer_size];
    _head = _tail = _buffer;
    _end = _buffer + buffer_size;
  }
}

void StreamSpy::end()
{
  if(_buffer)
  {
    delete _buffer;
    _buffer = _head = _tail = _end = NULL;
  }
}

int StreamSpy::peek(void)
{
  return _stream->peek();
}

int StreamSpy::read(void)
{
  int c = _stream->read();
  if(c >= 0) {
    uint8_t chr = (uint8_t)c;
    onData(false, &chr, sizeof(chr));
  }
  return c;
}

void StreamSpy::flush(void)
{
  _stream->flush();
}

size_t StreamSpy::write(uint8_t chr)
{
  onData(true, &chr, sizeof(chr));
  return _stream->write(chr);
}

size_t StreamSpy::write(const uint8_t *buffer, size_t size)
{
  onData(true, buffer, size);
  return _stream->write(buffer, size);
}

void StreamSpy::onData(bool write, const uint8_t *buffer, size_t size)
{
  if(_buffer)
  {
    for(size_t i = 0; i < size; i++) {
      *_head++ = buffer[i];
      if(_head >= _end) {
        _head = _buffer;
      }
      if(_head == _tail) {
        _tail++; 
        if(_tail >= _end) {
          _tail = _buffer;
        }
      }
    }
  }

  if(write) {
    if(_write) {
      _write(buffer, size);
    }
  } else {
    if(_read) {
      _read(buffer, size);
    }
  }
}

void StreamSpy::printBuffer(Print &print) 
{
  if(_head < _tail) {
    print.write(_tail, _end - _tail);
    print.write(_buffer, _head - _buffer);
  }
  else if(_head > _tail)
  {
    print.write(_tail, _head - _tail);
  }
}
