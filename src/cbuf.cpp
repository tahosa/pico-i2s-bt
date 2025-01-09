#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "cbuf.hpp"


void cbuf_reset(cbuf_t *cbuf) {
  cbuf->head = 0;
  cbuf->tail = 0;
}

cbuf_t *cbuf_init(int16_t *buf, size_t size) {
  cbuf_t *cbuf = (cbuf_t *) malloc(sizeof(cbuf_t));
  cbuf->buffer = buf;
  cbuf->max = size;

  cbuf_reset(cbuf);
  return cbuf;
}

void cbuf_free(cbuf_t *cbuf) {
  free(cbuf);
}

bool cbuf_full(cbuf_t *cbuf) {
  size_t head = cbuf->head + 1;
  if(head == cbuf->max) { // Handle wraparound
    head = 0;
  }
  return head == cbuf->tail;
}

bool cbuf_empty(cbuf_t *cbuf) {
  return cbuf->head == cbuf->tail;
}

size_t cbuf_size(cbuf_t *cbuf) {
  size_t size = cbuf->max - 1;
  if(!cbuf_full(cbuf)) {
    if (cbuf->head >= cbuf->tail) {
      size = cbuf->head - cbuf->tail;
    } else {
      size = cbuf->max + cbuf->head - cbuf->tail;
    }
  }

  return size;
}

size_t cbuf_write(cbuf_t *cbuf, int16_t *elem, size_t count) {
  // Cannot write more than the buffer will hold
  if (count > cbuf->max - 1) {
    return 0;
  }

  if (cbuf->head + count < cbuf->max) {
    memcpy(cbuf->buffer + cbuf->head, elem, WORD * count);
    cbuf->head += count % cbuf->max; // Still mod max to handle the == max case
  } else {
    size_t split = cbuf->max - cbuf->head;
    memcpy(cbuf->buffer + cbuf->head, elem, WORD * split);
    memcpy(cbuf->buffer, elem + split, WORD * (count - split));
    cbuf->head = count - split;
  }
  return count;
}

size_t cbuf_read(cbuf_t *cbuf, int16_t *out, size_t count) {
  size_t size = cbuf_size(cbuf);

  // Cannot read more than is in the buffer
  if (count > size) {
    count = size;
  }

  if (cbuf->tail + count < cbuf->max) {
    memcpy(out, cbuf->buffer + cbuf->tail, WORD * count);
    cbuf->tail += count % cbuf->max; // Still mod max to handle the == max case
  } else {
    size_t split = cbuf->max - cbuf->tail;
    memcpy(out, cbuf->buffer + cbuf->tail, WORD * split);
    memcpy(out + split, cbuf->buffer, WORD * (count - split));
    cbuf->tail = count - split;
  }
  return count;
}
