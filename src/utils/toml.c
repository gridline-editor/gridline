#include "utils/types.h"
#include "utils/memory.h"
#include "utils/toml.h"

typedef struct gl_codepoint gl_codepoint;

struct gl_codepoint {
    u32 data;
    u32 size;
};

static const u32 utf8_sizes[] = {
    1, 1, 1, 1,
    1, 1, 1, 1,
    0, 0, 0, 0,
    2, 2, 3, 4
};

static gl_pos pos_init(u32 _ln, u32 _col, u32 _idx);
static gl_pos pos_zero(void);
static gl_pos pos_w_next_line(const gl_pos* _pos, u32 _bytes);
static gl_pos pos_w_next_col(const gl_pos* _pos, u32 _bytes);
static u32 utf8_r_size(const u8* _data);
static b32 char_is_hori_whitespace(u32 _cp);
static b32 char_is_vert_whitespace(u32 _cp);
static b32 char_is_whitespace(u32 _cp);
static b32 char_is_comment(u32 _cp);
static b32 char_is_nontab_control(u32 _cp);
static b32 char_is_bare_key(u32 _cp);
static b32 char_is_string(u32 _cp);
static b32 char_is_simple_escape(u32 _cp);
static b32 char_is_unicode_escape(u32 _cp);
static b32 char_is_decimal_prefix(u32 _cp);
static b32 char_is_binary(u32 _cp);
static b32 char_is_decimal(u32 _cp);
static b32 char_is_octal(u32 _cp);
static b32 char_is_hexadecimal(u32 _cp);
static b32 char_is_punctuator(u32 _cp);
static gl_toml_token token_zero(void);
static gl_toml_lexer lexer_skip_to_token(const gl_toml_lexer* _lexer);
static gl_token_type token_type_from_punctuator(const gl_codepoint* _cp);
static b32 lexer_can_advance(const gl_toml_lexer* _lexer, u32 _bytes);
static gl_codepoint lexer_r_codepoint(const gl_toml_lexer* _lexer);
static gl_codepoint lexer_r_next_codepoint(const gl_toml_lexer* _lexer, u32 _n);
static gl_toml_lexer lexer_skip_whitespace(const gl_toml_lexer* _lexer,
                                           const gl_codepoint* _cp);
static gl_toml_lexer lexer_skip_commnet(const gl_toml_lexer* _lexer,
                                        const gl_codepoint* _cp);
static gl_toml_lexer lexer_skip_escaped_unicode(const gl_toml_lexer* _lexer,
                                                const gl_codepoint* _cp);
static gl_toml_lexer lexer_skip_binary(const gl_toml_lexer* _lexer,
                                        const gl_codepoint* _cp);
static gl_toml_lexer lexer_skip_octal(const gl_toml_lexer* _lexer,
                                        const gl_codepoint* _cp);
static gl_toml_lexer lexer_skip_decimal(const gl_toml_lexer* _lexer,
                                        const gl_codepoint* _cp);
static gl_toml_lexer lexer_skip_hexadecimal(const gl_toml_lexer* _lexer,
                                            const gl_codepoint* _cp);
static gl_toml_lexer lexer_collect_bare_key(const gl_toml_lexer* _lexer,
                                            const gl_codepoint* _cp);
static gl_toml_lexer lexer_collect_string(const gl_toml_lexer* _lexer,
                                          const gl_codepoint* _cp);
static gl_toml_lexer lexer_collect_integer(const gl_toml_lexer* _lexer,
                                           const gl_codepoint* _cp);
static b32 token_is_only_digits(const gl_toml_lexer* _start, u32 _n);


static gl_pos pos_init(u32 _ln, u32 _col, u32 _idx) {
    gl_pos pos;
    pos.ln = _ln;
    pos.col = _col;
    pos.index = _idx;
    return pos;
}

static gl_pos pos_zero(void) {
    gl_pos pos;
    pos.ln = 0;
    pos.col = 0;
    pos.index = 0;
    return pos;
}

static gl_pos pos_w_next_line(const gl_pos* _pos, u32 _bytes) {
    gl_pos pos = *_pos;
    pos.ln++;
    pos.col = 1;
    pos.index += _bytes;
    return pos;
}

static gl_pos pos_w_next_col(const gl_pos* _pos, u32 _bytes) {
    gl_pos pos = *_pos;
    pos.col++;
    pos.index += _bytes;
    return pos;
}

static u32 utf8_r_size(const u8* _data) {
    const u32 index = (_data[0] & 0xff) >> 4;
    return utf8_sizes[index];
}

static b32 char_is_hori_whitespace(u32 _cp) {
    return ((_cp == ' ') || (_cp == '\t'));
}

static b32 char_is_vert_whitespace(u32 _cp) {
    return ((_cp == '\r') || (_cp == '\n'));
}

static b32 char_is_whitespace(u32 _cp) {
    return (char_is_hori_whitespace(_cp) || char_is_vert_whitespace(_cp));
}

static b32 char_is_comment(u32 _cp) {
    return (_cp == '#');
}

static b32 char_is_nontab_control(u32 _cp) {
    return (
        ((_cp >= 0x0) && (_cp <= 0x8)) ||
        ((_cp >= 0xa) && (_cp <= 0x1f)) ||
        (_cp == 0x7f)
    );
}

static b32 char_is_bare_key(u32 _cp) {
    return (
        (_cp == '_') || (_cp == '-') ||
        ((_cp >= 'a') && (_cp <= 'z')) ||
        ((_cp >= 'A') && (_cp <= 'Z')) ||
        ((_cp >= '0') && (_cp <= '9'))
    );
}

static b32 char_is_string(u32 _cp) {
    return ((_cp == '\"') || (_cp == '\''));
}

static b32 char_is_simple_escape(u32 _cp) {
    return (
        (_cp == 'n')  || (_cp == 't') || (_cp == 'r') ||
        (_cp == '\"') || (_cp == '\\') || (_cp == '\'') ||
        (_cp == 'b') || (_cp == 'f')  || (_cp == 'v') ||
        (_cp == 'a')
    );
}

static b32 char_is_unicode_escape(u32 _cp) {
    return ((_cp == 'u') || (_cp == 'U'));
}

static b32 char_is_decimal_prefix(u32 _cp) {
    return ((_cp == '+') || (_cp == '-'));
}

static b32 char_is_binary(u32 _cp) {
    return ((_cp == '0') || (_cp == '1'));
}

static b32 char_is_decimal(u32 _cp) {
    return ((_cp >= '0') && (_cp <= '9'));
}

static b32 char_is_octal(u32 _cp) {
    return ((_cp >= '0') && (_cp <= '7'));
}

static b32 char_is_hexadecimal(u32 _cp) {
    return (
        char_is_decimal(_cp) ||
        ((_cp >= 'a') && (_cp <= 'f')) ||
        ((_cp >= 'A') && (_cp <= 'F'))
    );
}

static b32 char_is_punctuator(u32 _cp) {
    return (
        (_cp == '[') || (_cp == ']') || (_cp == '{') ||
        (_cp == '}') || (_cp == '.') || (_cp == ',') ||
        (_cp == '=') || (_cp == ':')
    );
}

static gl_toml_token token_zero(void) {
    gl_toml_token token;
    token.type = GL_TOKEN_TYPE_UNKNOWN;
    token.start = pos_zero();
    token.end = pos_zero();
    return token;
}

static gl_toml_lexer lexer_skip_to_token(const gl_toml_lexer* _lexer) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = {0};
    while(lexer_can_advance(&lexer, cp.size)) {
        cp = lexer_r_codepoint(&lexer);
        if(char_is_whitespace(cp.data)) {
            lexer = lexer_skip_whitespace(&lexer, &cp);
        } else if(char_is_comment(cp.data)) {
            lexer = lexer_skip_commnet(&lexer, &cp);
        } else {
            if(lexer.first_nonblank.col == 0) {
                lexer.first_nonblank = lexer.pos;
            }

            break;
        }
    }

    return lexer;
}

static gl_token_type token_type_from_punctuator(const gl_codepoint* _cp) {
    gl_token_type type = GL_TOKEN_TYPE_UNKNOWN;
    switch(_cp->data) {
        case '[':
            type = GL_TOKEN_TYPE_LBRACKET;
            break;
        case ']':
            type = GL_TOKEN_TYPE_RBRACKET;
            break;
        case '{':
            type = GL_TOKEN_TYPE_LCURLY;
            break;
        case '}':
            type = GL_TOKEN_TYPE_RCURLY;
            break;
        case '.':
            type = GL_TOKEN_TYPE_DOT;
            break;
        case ',':
            type = GL_TOKEN_TYPE_COMMA;
            break;
        case '=':
            type = GL_TOKEN_TYPE_EQUALS;
            break;
        case ':':
            type = GL_TOKEN_TYPE_COLON;
            break;
        default:
            break;
    }

    return type;
}

static b32 lexer_can_advance(const gl_toml_lexer* _lexer, u32 _bytes) {
    const u32 remaining_bytes = _lexer->source->size - _lexer->pos.index;
    return (_bytes < remaining_bytes);
}

static gl_codepoint lexer_r_codepoint(const gl_toml_lexer* _lexer) {
    const u8* curr = _lexer->source->data + _lexer->pos.index;
    gl_codepoint cp = {0};
    cp.size = utf8_r_size(curr);
    cp.data = deserialize_u32(curr, cp.size);
    return cp;
}

static gl_codepoint lexer_r_next_codepoint(const gl_toml_lexer* _lexer,
                                           u32 _n) {
    const u8* curr = _lexer->source->data + _lexer->pos.index;
    u32 byte_sum = 0;
    u32 bytes = 0;
    gl_codepoint cp = {0};
    u32 i = 0;

    for(; i < _n; i++) {
        bytes = utf8_r_size(curr + byte_sum);
        if(lexer_can_advance(_lexer, byte_sum + bytes)) {
            byte_sum += bytes;
        }
    }

    if(i == (_n - 1)) {
        cp.size = utf8_r_size(curr + byte_sum);
        cp.data = deserialize_u32(curr + byte_sum, cp.size);
    }

    return cp;
}

static gl_toml_lexer lexer_skip_whitespace(const gl_toml_lexer* _lexer,
                                           const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    u32 newline_size = 0;
    while(lexer_can_advance(&lexer, cp.size)) {
        if((cp.data == ' ') || (cp.data == '\t')) {
            lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        } else if(cp.data == '\r') {
            gl_codepoint next_cp = lexer_r_next_codepoint(&lexer, 1);
            if(!lexer_can_advance(&lexer, cp.size) || (next_cp.data != '\n')) {
                // TODO: handle malformed line termination
                lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
            } else {
                newline_size = cp.size + next_cp.size;
            }
        } else if(cp.data == '\n') {
            newline_size = cp.size;
        } else {
            break;
        }

        if(newline_size) {
            lexer.pos = pos_w_next_line(&lexer.pos, newline_size);
            newline_size = 0;
            lexer.first_nonblank = pos_zero();
        }

        cp = lexer_r_codepoint(&lexer);
    }

    return lexer;
}

static gl_toml_lexer lexer_skip_commnet(const gl_toml_lexer* _lexer,
                                        const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        cp = lexer_r_codepoint(&lexer);
        if(char_is_vert_whitespace(cp.data)) {
            break;
        } else if(char_is_nontab_control(cp.data)) {
            // TODO: handle non-tab control characters
        }

        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
    }

    return lexer;
}

static gl_toml_lexer lexer_skip_escaped_unicode(const gl_toml_lexer* _lexer,
                                                const gl_codepoint* _cp) {
    const u32 expected_bytes = (_cp->data == 'u')?
        4: 8;

    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    for(u32 i = 0; i < expected_bytes; i++) {
        if(!lexer_can_advance(&lexer, cp.size)) {
            // TODO: handle incomplete escape sequence
            break;
        }

        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(!char_is_hexadecimal(cp.data)) {
            // TODO: handle incomplete escape sequence
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_skip_binary(const gl_toml_lexer* _lexer,
                                       const gl_codepoint* _cp) {

    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(!char_is_binary(cp.data)) {
            break;
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_skip_octal(const gl_toml_lexer* _lexer,
                                      const gl_codepoint* _cp) {

    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(!char_is_octal(cp.data)) {
            break;
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_skip_decimal(const gl_toml_lexer* _lexer,
                                        const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(!char_is_decimal(cp.data)) {
            break;
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_skip_hexadecimal(const gl_toml_lexer* _lexer,
                                            const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(!char_is_hexadecimal(cp.data)) {
            break;
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_collect_bare_key(const gl_toml_lexer* _lexer,
                                            const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(!char_is_bare_key(cp.data)) {
            break;
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_collect_string(const gl_toml_lexer* _lexer,
                                          const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        cp = lexer_r_codepoint(&lexer);
        if(cp.data == _cp->data) {
            if(lexer_can_advance(&lexer, cp.size)) {
                lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
            }

            break;
        }

        if(cp.data == '\\') {
            if(!lexer_can_advance(&lexer, cp.size)) {
                // TODO: handle incomplete escape sequence
                break;
            }

            const gl_codepoint next = lexer_r_next_codepoint(&lexer, 1);
            if(char_is_simple_escape(next.data)) {
                lexer.pos = pos_w_next_col(&lexer.pos, next.size);
            } else if(char_is_unicode_escape(next.data)) {
                lexer = lexer_skip_escaped_unicode(&lexer, &cp);
            } else {
                // TODO: handle reserved escape sequence
            }
        }
    }

    return lexer;
}

static gl_toml_lexer lexer_collect_integer(const gl_toml_lexer* _lexer,
                                           const gl_codepoint* _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = *_cp;
    if((cp.data == '0') && lexer_can_advance(&lexer, cp.size)) {
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        switch(cp.data) {
        case 'b':
            lexer = lexer_skip_binary(&lexer, &cp);
            break;
        case 'o':
            lexer = lexer_skip_octal(&lexer, &cp);
            break;
        case 'x':
            lexer = lexer_skip_hexadecimal(&lexer, &cp);
            break;
        default:
            if(char_is_decimal(cp.data)) {
                // TODO: handle leading zero number
                lexer = lexer_skip_decimal(&lexer, &cp);
            }

            break;
        }
    } else if(char_is_decimal(cp.data)) {
        lexer = lexer_skip_decimal(&lexer, &cp);
    }

    return lexer;
}

static b32 token_is_only_digits(const gl_toml_lexer* _start, u32 _n) {
    gl_toml_lexer lexer = *_start;
    b32 is_true = 1;
    gl_codepoint cp;
    for(u32 i = 0; i < _n; i++) {
        cp = lexer_r_codepoint(&lexer);
        lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        if(!char_is_decimal(cp.data)) {
            is_true = 0;
            break;
        }
    }

    return is_true;
}

gl_source gl_source_init(const char* _pathname, u8* _data, u32 _size) {
    gl_source source;
    source.pathname = _pathname;
    source.data = _data;
    source.size = _size;
    return source;
}

gl_toml_lexer gl_toml_lexer_init(const gl_source* _source) {
    gl_toml_lexer lexer;
    lexer.source = _source;
    lexer.pos = pos_init(1, 1, 0);
    lexer.token = token_zero();
    return lexer;
}

gl_toml_lexer gl_toml_lexer_lex(const gl_toml_lexer* _lexer) {
    gl_toml_lexer lexer = *_lexer;
    lexer.token.type = GL_TOKEN_TYPE_UNKNOWN;
    lexer = lexer_skip_to_token(&lexer);
    gl_codepoint cp = lexer_r_codepoint(&lexer);
    if(!lexer_can_advance(&lexer, cp.size)) {
        lexer.token.type = GL_TOKEN_TYPE_EOF;
        lexer.token.start = lexer.pos;
    }

    if(char_is_bare_key(cp.data)) {
        const gl_toml_lexer pre_lex = lexer;
        lexer.token.start = lexer.pos;
        lexer = lexer_collect_bare_key(&lexer, &cp);
        lexer.token.end = lexer.pos;
        const u32 diff = lexer.token.end.index - lexer.token.start.index;
        if(!token_is_only_digits(&pre_lex, diff)) {
            lexer.token.type = GL_TOKEN_TYPE_KEY;
        } else {
            lexer.token.type = GL_TOKEN_TYPE_KEY_OR_INTEGER;
        }
    } else if(char_is_decimal_prefix(cp.data)) {
        lexer.token.type = GL_TOKEN_TYPE_INTEGER;
        lexer.token.start = lexer.pos;
        lexer = lexer_collect_integer(&lexer, &cp);
        lexer.token.end = lexer.pos;
    } else if(char_is_string(cp.data)) {
        lexer.token.type = GL_TOKEN_TYPE_STRING;
        lexer.token.start = lexer.pos;
        lexer = lexer_collect_string(&lexer, &cp);
        lexer.token.end = lexer.pos;
    } else if (char_is_punctuator(cp.data)) {
        lexer.token.type = token_type_from_punctuator(&cp);
        lexer.token.start = lexer.pos;
        if(lexer_can_advance(&lexer, cp.size)) {
            lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        }

        lexer.token.end = lexer.pos;
    }

    return lexer;
}

gl_toml_token gl_toml_lexer_r_token(const gl_toml_lexer* _lexer) {
    return _lexer->token;
}
