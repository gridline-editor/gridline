#include "utils/types.h"
#include "utils/memory.h"

typedef enum {
    GL_FRAME_HEADER_FIELD__MIN,
    GL_FRAME_HEADER_FIELD_INDEX,
    GL_FRAME_HEADER_FIELD__MAX,
} gl_frame_header_field;

static u32 frame_header_r_field_offset(gl_frame_header_field _field);
static void frame_header_w_field(u8* _header,
                                 gl_frame_header_field _field,
                                 u32 _value);


static u32 frame_header_r_field_offset(gl_frame_header_field _field) {
    return (sizeof(u32) * (_field - 1));
}

static void frame_header_w_field(u8* _header,
                                 gl_frame_header_field _field,
                                 u32 _value) {
    u8* field = _header + frame_header_r_field_offset(_field);
    serialize_u32(field, _value);
}
