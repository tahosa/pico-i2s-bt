typedef struct {
  int16_t *buffer;
  size_t head;
  size_t tail;
  size_t max;
} cbuf_t;

size_t const WORD = sizeof(int16_t);

void cbuf_reset(cbuf_t *cbuf);
cbuf_t *cbuf_init(int16_t *buf, size_t size);
void cbuf_free(cbuf_t *cbuf);
bool cbuf_empty(cbuf_t *cbuf);
size_t cbuf_write(cbuf_t *cbuf, int16_t *elem, size_t count);
size_t cbuf_read(cbuf_t *cbuf, int16_t *out, size_t count);
