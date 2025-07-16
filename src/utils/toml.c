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

static gl_pos pos_init(void);
static gl_pos pos_w_next_line(const gl_pos* _pos, u32 _bytes);
static gl_pos pos_w_next_col(const gl_pos* _pos, u32 _bytes);
static u32 utf8_r_size(const u8* _data);
static b32 char_is_whitespace(u32 _cp);
static b32 lexer_can_advance(const gl_toml_lexer* _lexer, u32 _bytes);
static gl_codepoint lexer_r_codepoint(const gl_toml_lexer* _lexer);
static gl_codepoint lexer_r_next_codepoint(const gl_toml_lexer* _lexer);
static gl_toml_lexer lexer_skip_whitespace(const gl_toml_lexer* _lexer,
                                           const gl_codepoint _cp);


static gl_pos pos_init(void) {
    gl_pos pos;
    pos.ln = 1;
    pos.col = 1;
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

static b32 char_is_whitespace(u32 _cp) {
    return (
        (_cp == ' ') || (_cp == '\t') || (_cp == '\r') ||
        (_cp == '\n')
    );
}
static b32 lexer_can_advance(const gl_toml_lexer* _lexer, u32 _bytes) {
    const u32 remaining_bytes = _lexer->source->size - _lexer->pos.index;
    return (_bytes < remaining_bytes);
}

static gl_codepoint lexer_r_codepoint(const gl_toml_lexer* _lexer) {
    const u8* curr = _lexer->source->data + _lexer->pos.index;
    gl_codepoint cp;
    cp.size = utf8_r_size(curr);
    cp.data = deserialize_u32(curr, cp.size);
    return cp;
}

static gl_codepoint lexer_r_next_codepoint(const gl_toml_lexer* _lexer) {
    const u8* curr = _lexer->source->data + _lexer->pos.index;
    const u32 bytes = utf8_r_size(curr);    
    gl_codepoint cp;
    cp.size = utf8_r_size(curr + bytes);
    cp.data = deserialize_u32(curr, cp.size);
    return cp;
}

static gl_toml_lexer lexer_skip_whitespace(const gl_toml_lexer* _lexer,
                                           const gl_codepoint _cp) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = _cp;
    while(lexer_can_advance(&lexer, cp.size)) {
        if((cp.data == ' ') || (cp.data == '\t')) {
            lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
        } else if(cp.data == '\r') {
            gl_codepoint next_cp = lexer_r_next_codepoint(&lexer);
            if(!lexer_can_advance(&lexer, cp.size) ||
               (next_cp.data != '\n')) {
                // TODO: handle malformed line termination
                lexer.pos = pos_w_next_col(&lexer.pos, cp.size);
            } else {
                lexer.pos = pos_w_next_line(&lexer.pos,
                                            cp.size + next_cp.size);
            }
        } else if(cp.data == '\n') {
            lexer.pos = pos_w_next_line(&lexer.pos, cp.size);
        } else {
            break;
        }

        cp = lexer_r_codepoint(&lexer);
    }

    return lexer;
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
    lexer.pos = pos_init();
    lexer.token_pos = pos_init();
    return lexer;
}

gl_toml_lexer gl_toml_lexer_lex(const gl_toml_lexer* _lexer) {
    gl_toml_lexer lexer = *_lexer;
    gl_codepoint cp = {0};

    do {
        cp = lexer_r_codepoint(&lexer);
        if(char_is_whitespace(cp.data)) {
            lexer = lexer_skip_whitespace(&lexer, cp);
        } else {
            break;
        }
    } while(lexer_can_advance(&lexer, cp.size));
    return lexer;
}
