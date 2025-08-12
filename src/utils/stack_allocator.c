#include "utils/types.h"
#include "utils/memory.h"
#include "utils/stack_allocator.h"

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

gl_stack_allocator gl_stack_allocator_init(u8* _data, u32 _cap) {
    gl_stack_allocator allocator;
    allocator.data = _data;
    allocator.cap = _cap;
    allocator.frame = 0;

    if(allocator.cap > frame_header_r_field_offset(GL_FRAME_HEADER_FIELD__MAX)) {
        frame_header_w_field(allocator.data, GL_FRAME_HEADER_FIELD_INDEX, 0);
    }

    return allocator;
}
