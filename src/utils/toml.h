#ifndef GRIDLINE_TOML_H
#define GRIDLINE_TOML_H

#include "utils/types.h"

typedef struct gl_source gl_source;
typedef struct gl_pos gl_pos;

struct gl_source {
  const char* pathname;
  u8* data;
  u32 size;
};

struct gl_pos {
    u32 ln;
    u32 col;
    u32 index;
};


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
gl_source gl_source_init(const char* _pathname, u8* _data, u32 _size);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GRIDLINE_TOML_H
