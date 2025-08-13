#include "utils/types.h"
#include "utils/memory.h"
#include "utils/stack_allocator.h"

typedef enum {
    GL_FRAME_HEADER_FIELD__MIN,
    GL_FRAME_HEADER_FIELD_INDEX,
    GL_FRAME_HEADER_FIELD__MAX,
} gl_frame_header_field;

static u32 frame_header_r_field_offset(gl_frame_header_field _field);
static u32 frame_header_r_offset(u32 _frame);
static u32 frame_header_r_field(const u8* _header,
                                gl_frame_header_field _field);
static void frame_header_w_field(u8* _header,
                                 gl_frame_header_field _field,
                                 u32 _value);
static u32 stack_allocator_r_remaining_size(const gl_stack_allocator* _allocator);


static u32 frame_header_r_field_offset(gl_frame_header_field _field) {
    return (sizeof(u32) * (_field - 1));
}

static u32 frame_header_r_offset(u32 _frame) {
    const u32 header_size =
        frame_header_r_field_offset(GL_FRAME_HEADER_FIELD__MAX);
    return (header_size * _frame);
}

static u32 frame_header_r_field(const u8* _header,
                                gl_frame_header_field _field) {
    const u8* field = _header + frame_header_r_field_offset(_field);
    return deserialize_u32(field, sizeof(u32));
}

static void frame_header_w_field(u8* _header,
                                 gl_frame_header_field _field,
                                 u32 _value) {
    u8* field = _header + frame_header_r_field_offset(_field);
    serialize_u32(field, _value);
}

static u32 stack_allocator_r_remaining_size(const gl_stack_allocator* _allocator) {
    const u32 frame_section_size = frame_header_r_offset(_allocator->frame);
    const u8* frame_header = _allocator->data + frame_section_size;
    const u32 data_section_size = frame_header_r_field(frame_header,
                                                       GL_FRAME_HEADER_FIELD_INDEX);
    return (data_section_size - frame_section_size);
}

gl_stack_allocator gl_stack_allocator_init(u8* _data, u32 _cap) {
    gl_stack_allocator allocator;
    allocator.data = _data;
    allocator.cap = _cap;
    allocator.frame = 0;

    const u32 aligned_cap = align_down_8_u32(allocator.cap);
    if(aligned_cap > frame_header_r_field_offset(GL_FRAME_HEADER_FIELD__MAX)) {
        frame_header_w_field(allocator.data,
                             GL_FRAME_HEADER_FIELD_INDEX,
                             aligned_cap);
    }

    return allocator;
}

b32 gl_stack_allocator_can_allocate(const gl_stack_allocator* _allocator,
                                    u32 _size) {
    const u32 required_size = align_up_8_u32(_size) + frame_header_r_offset(1);
    const u32 remaining_size = stack_allocator_r_remaining_size(_allocator);
    return (required_size <= remaining_size);
}

gl_stack_allocator gl_stack_allocator_allocate(const gl_stack_allocator* _allocator,
                                               u32 _size) {
    u8* frame_header =
        _allocator->data + frame_header_r_offset(_allocator->frame);
    const u32 prev_index =
        frame_header_r_field(frame_header, GL_FRAME_HEADER_FIELD_INDEX);
    const u32 aligned_size = align_up_8_u32(_size);
    const u32 curr_index = prev_index - aligned_size;
    frame_header_w_field(frame_header, GL_FRAME_HEADER_FIELD_INDEX, curr_index);
    gl_stack_allocator allocator = *_allocator;
    allocator.frame++;
    return allocator;
}

gl_stack_allocator gl_stack_allocator_deallocate(const gl_stack_allocator* _allocator) {
    gl_stack_allocator allocator = * _allocator;
    allocator.frame--;
    return allocator;
}
