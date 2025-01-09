#include <cassert>
#include <iostream>

#include "../src/cbuf.cpp"

int main(int argc, char* argv[]) {
  std::cout << "Starting test\n";

  std::cout << "cbuf_init: ";
  size_t size = 10;
  int16_t *buf = (int16_t *) malloc(size * sizeof(int16_t));
  cbuf_t *cbuf = cbuf_init(buf, size);

  assert(cbuf->buffer == buf);
  assert(cbuf->max == size);
  assert(cbuf->head == 0);
  assert(cbuf->tail == 0);

  assert(cbuf_size(cbuf) == 0);
  assert(cbuf_full(cbuf) == false);
  assert(cbuf_empty(cbuf) == true);

  std::cout << "pass\n";


  int16_t source[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int16_t dest[10];

  std::cout << "cbuf_write (no overflow): ";

  assert(cbuf_write(cbuf, source, 2) == 2);
  assert(buf[0] == 1);
  assert(buf[1] == 2);
  assert(cbuf->head == 2);
  assert(cbuf_size(cbuf) == 2);

  std::cout << "pass\n";


  std::cout << "cbuf_read (no overflow): ";

  assert(cbuf_read(cbuf, dest, 5) == 2); // try to read more than is in the buffer
  assert(dest[0] == 1);
  assert(dest[1] == 2);
  assert(cbuf_size(cbuf) == 0);

  std::cout << "pass\n";

  std::cout << "cbuf_write (overflow): ";

  assert(cbuf_write(cbuf, source, 9) == 9);
  assert(buf[2] == 1);
  assert(buf[3] == 2);
  assert(buf[4] == 3);
  assert(buf[5] == 4);
  assert(buf[6] == 5);
  assert(buf[7] == 6);
  assert(buf[8] == 7);
  assert(buf[9] == 8);
  assert(buf[0] == 9);
  assert(cbuf_size(cbuf) == 9);
  assert(cbuf_full(cbuf) == true);

  std::cout << "pass\n";


  std::cout << "cbuf_read (overflow): ";

  assert(cbuf_read(cbuf, dest, 9) == 9);
  assert(dest[0] == 1);
  assert(dest[1] == 2);
  assert(dest[2] == 3);
  assert(dest[3] == 4);
  assert(dest[4] == 5);
  assert(dest[5] == 6);
  assert(dest[6] == 7);
  assert(dest[7] == 8);
  assert(dest[8] == 9);
  assert(cbuf_size(cbuf) == 0);
  assert(cbuf_empty(cbuf) == true);

  std::cout << "pass\n";
}
