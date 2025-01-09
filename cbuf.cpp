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

bool cbuf_empty(cbuf_t *cbuf) {
  return cbuf->head == cbuf->tail;
}

size_t cbuf_write(cbuf_t *cbuf, int16_t *elem, size_t count) {
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
  if (cbuf->tail + count < cbuf->max) {
    memcpy(out, cbuf->buffer + cbuf->tail, WORD * count);
    cbuf->tail += count % cbuf->max; // Still mod max to handle the == max case
  } else {
    size_t split = cbuf->max - cbuf->tail;
    memcpy(out, cbuf->buffer + cbuf->tail, WORD * split);
    memcpy(out, cbuf->buffer, WORD * (count - split));
    cbuf->tail = count - split;
  }
  return count;
}
