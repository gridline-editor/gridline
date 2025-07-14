#ifndef GRIDLINE_TOML_H
#define GRIDLINE_TOML_H

#include "utils/types.h"

typedef struct gl_source gl_source;
typedef struct gl_pos gl_pos;
typedef struct gl_toml_lexer gl_toml_lexer;

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


struct gl_toml_lexer {
    const gl_source* source;
    gl_pos pos;
    gl_pos token_pos;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
gl_source gl_source_init(const char* _pathname, u8* _data, u32 _size);
gl_toml_lexer gl_toml_lexer_init(const gl_source* _source);
gl_toml_lexer gl_toml_lexer_lex(const gl_toml_lexer* _lexer);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GRIDLINE_TOML_H
