#ifndef GRIDLINE_TOML_H
#define GRIDLINE_TOML_H

#include "utils/types.h"

typedef enum {
    GL_TOKEN_ERROR_NONE,
    GL_TOKEN_ERROR_LEADING_ZERO,
    GL_TOKEN_ERROR_MISUSED_UNDERSCORE,
    GL_TOKEN_ERROR_CONTROL_CHARACTER,
    GL_TOKEN_ERROR_MALFORMED_NEWLINE
} gl_token_error;

typedef enum {
    GL_TOKEN_TYPE_UNKNOWN,
    GL_TOKEN_TYPE_EOF,
    GL_TOKEN_TYPE_KEY_OR_INTEGER,
    GL_TOKEN_TYPE_KEY,
    GL_TOKEN_TYPE_INTEGER,
    GL_TOKEN_TYPE_STRING,
    GL_TOKEN_TYPE_LBRACKET,
    GL_TOKEN_TYPE_RBRACKET,
    GL_TOKEN_TYPE_LCURLY,
    GL_TOKEN_TYPE_RCURLY,
    GL_TOKEN_TYPE_DOT,
    GL_TOKEN_TYPE_COMMA,
    GL_TOKEN_TYPE_EQUALS,
    GL_TOKEN_TYPE_COLON
} gl_token_type;

typedef struct gl_source gl_source;
typedef struct gl_pos gl_pos;
typedef struct gl_toml_token gl_toml_token;
typedef struct gl_toml_lexer gl_toml_lexer;
typedef struct gl_toml_lexer_pass gl_toml_lexer_pass;

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

struct gl_toml_token {
    gl_token_type type;
    gl_pos start;
    gl_pos end;
    u32 index;
};

struct gl_toml_lexer {
    const gl_source* source;
    gl_pos pos;
    gl_pos first_nonblank;
    gl_toml_token token;
};

struct gl_toml_lexer_pass {
    u32 tokens;
    u32 lexeme_bytes;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
gl_source gl_source_init(const char* _pathname, u8* _data, u32 _size);
gl_toml_lexer gl_toml_lexer_init(const gl_source* _source);
gl_toml_lexer gl_toml_lexer_lex(const gl_toml_lexer* _lexer);
gl_toml_token gl_toml_lexer_r_token(const gl_toml_lexer* _lexer);
gl_toml_lexer_pass gl_toml_lexer_first_pass(const gl_toml_lexer* _lexer);
const char* gl_toml_token_type_to_str(gl_token_type _type);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GRIDLINE_TOML_H
