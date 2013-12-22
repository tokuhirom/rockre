#ifndef NANOUTF8_H_
// License: Public domain.

#include <stdint.h>
#include <stddef.h> // size_t
#include <stdbool.h>

/*
    Range/Offset  0         1         2         3
       0x00-0x7F  0xxxxxxx
      0x80-0x3FF  110xxxxx  10xxxxxx
    0x400-0xFFFF  1110xxxx  10xxxxxx  10xxxxxx
0x10000-0x1FFFFF  11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
*/

static size_t nanoutf8_next_size(char c) {
  if ((c & 0x80) == 0x00)
    return 1;
  if ((c & 0xE0) == 0xC0)
    return 2;
  if ((c & 0xF0) == 0xE0)
    return 3;
  if ((c & 0xF8) == 0xF0)
    return 4;

  return 1;
}

static size_t nanoutf8_encode(uint32_t ch, char *buf) {
  if (ch < 0x80) {
    buf[0] = ch & 0xff;
    return 1;
  } else if (ch < 0x400) {
    buf[0] = ((ch >> 6) & 0xff) | 0xc0;
    buf[1] = (ch & 0x3f) | 0x80;
    return 2;
  } else if (ch < 0x10000) {
    buf[0] = ((ch >> 12) & 0xff) | 0xe0;
    buf[1] = ((ch>>6) & 0x3f) | 0x80;
    buf[2] = (ch & 0x3f) | 0x80;
    return 3;
  } else if (ch < 0x200000) {
    buf[0] = ((ch >> 18) & 0xff) | 0xf0;
    buf[1] = ((ch>>12) & 0x3f) | 0x80;
    buf[2] = ((ch>>6) & 0x3f) | 0x80;
    buf[3] = (ch & 0x3f) | 0x80;
    return 4;
  } else {
    /* Invalid UTF-8 character */
    return 0;
  }
}

static uint32_t nanoutf8_peek_char(const char* const src, size_t srclen, size_t *lenp, bool *ok) {
  if (srclen < 1) {
    *ok = false;
    return 0;
  }
  *lenp = nanoutf8_next_size(src[0]);
  switch (*lenp) {
  case 1:
    *ok = true;
    return src[0];
  case 2:
    // 0x80-0x3FF  110xxxxx  10xxxxxx
    if (srclen < 2) {
      *ok = false;
      return 0;
    }
    *ok = true;
    return ( (src[0] & 0x1f) << 6)
          | (src[1] & 0x3f);
  case 3:
    // 0x400-0xFFFF  1110xxxx  10xxxxxx  10xxxxxx
    if (srclen < 3) {
      *ok = false;
      return 0;
    }
    *ok = true;
    return
            ( (src[0] & 0x0f) << 12)
          | ( (src[1] & 0x3f) <<  6)
          | ( (src[2] & 0x3f)      );
  case 4:
    // 0x10000-0x1FFFFF  11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
    if (srclen < 4) {
      *ok = false;
      return 0;
    }
    *ok = true;
    return
            ( (src[0] & 0x07) << 18)
          | ( (src[1] & 0x3f) << 12)
          | ( (src[2] & 0x3f) <<  6)
          | ( (src[3] & 0x3f)      );
  default:
    return 0;
  }
}

#endif
