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

StreamSpy::StreamSpy(Stream *stream) :
  _stream(stream),
  _buffer(NULL),
  _head(NULL),
  _tail(NULL),
  _end(NULL),
  _read(NULL),
  _write(NULL)
{

}

StreamSpy::StreamSpy() :
  _stream(NULL),
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
  return _stream ? _stream->peek() : 0;
}

int StreamSpy::read(void)
{
  int c = _stream ? _stream->read() : -1;
  if(c >= 0) {
    uint8_t chr = (uint8_t)c;
    onData(false, &chr, sizeof(chr));
  }
  return c;
}

void StreamSpy::flush(void)
{
  if(_stream) {
    _stream->flush();
  }
}

size_t StreamSpy::write(uint8_t chr)
{
  onData(true, &chr, sizeof(chr));
  return _stream ? _stream->write(chr) : sizeof(chr);
}

size_t StreamSpy::write(const uint8_t *buffer, size_t size)
{
  onData(true, buffer, size);
  return _stream ? _stream->write(buffer, size) : size;
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

StreamSpyReader::StreamSpyReader() :
  _stream(NULL),
  _head(NULL),
  _tail(NULL)
{
}

StreamSpyReader::~StreamSpyReader()
{
}

void StreamSpyReader::attach(StreamSpy &stream)
{
  _stream = &stream;
  _head = stream._head;
  _tail = stream._tail;
  stream.onRead([this](const uint8_t *buffer, size_t size) { writeBuffer(size); });
  stream.onWrite([this](const uint8_t *buffer, size_t size) { writeBuffer(size); });
}

void StreamSpyReader::printBuffer(Print &print) 
{
  while(available() > 0) {
    uint8_t *buf;
    size_t len;

    getBuffer(buf, len);
    print.write(buf, len);
    readBuffer(len);
  }
}

size_t StreamSpyReader::available(void)
{
  size_t available = 0;
  if(_head < _tail) {
    available += _stream->_end - _tail;
    available += _head - _stream->_buffer;
  }
  else if(_head > _tail)
  {
    available += _head - _tail;
  }
  return available;
}

void StreamSpyReader::getBuffer(uint8_t *&buffer, size_t &len)
{
  buffer = _tail;
  len = _head < _tail ?
    _stream->_end - _tail : 
    _head - _tail;
}

void StreamSpyReader::readBuffer(size_t len)
{
  _tail += len;
  if(_tail >= _stream->_end) {
    _tail -= _stream->_end - _stream->_buffer;
  }
}

void StreamSpyReader::writeBuffer(size_t len)
{
  _head += len;
  if(_head >= _stream->_end) {
    _head -= _stream->_end - _stream->_buffer;
  }
}
