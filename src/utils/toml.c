#include "utils/types.h"
#include "utils/toml.h"

static gl_pos pos_init(void);
static gl_pos pos_w_next_line(const gl_pos* _pos, u32 _bytes);
static gl_pos pos_w_next_col(const gl_pos* _pos, u32 _bytes);


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
