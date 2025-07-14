#include "utils/types.h"
#include "utils/toml.h"

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

gl_source gl_source_init(const char* _pathname, u8* _data, u32 _size) {
    gl_source source;
    source.pathname = _pathname;
    source.data = _data;
    source.size = _size;
    return source;
}
