#include "utils/types.h"
#include "utils/memory.h"
#include "utils/stack_allocator.h"

typedef enum {
    GL_FRAME_SECTION__MIN,
    GL_FRAME_SECTION_INDEX,
    GL_FRAME_SECTION_OFFSET,
    GL_FRAME_SECTION__MAX
} gl_frame_section;

static u32 align_to_8(u32 _size);
static u32 frame_section_to_offset(gl_frame_section _section);
static u32 frame_section_total_size(void);
static u32 frame_header_r_at(const u8* _header, gl_frame_section _section);
static void frame_header_w_at(u8* _header,
                              gl_frame_section _section,
                              u32 _data);


static u32 align_to_8(u32 _size) {
    return ((_size + 7) & ~(7));
}

static u32 frame_section_to_offset(gl_frame_section _section) {
    return (sizeof(u32) * (_section - 1));
}

static u32 frame_section_total_size(void) {
    return frame_section_to_offset(GL_FRAME_SECTION__MAX);
}

static u32 frame_header_r_at(const u8* _header, gl_frame_section _section) {
    const u32 offset = frame_section_to_offset(_section);
    return deserialize_u32(_header + offset, 4);
}

static void frame_header_w_at(u8* _header,
                              gl_frame_section _section,
                              u32 _data) {
    const u32 offset = frame_section_to_offset(_section);
    serialize_u32(_header + offset, _data);
}

gl_stack_allocator gl_stack_allocator_init(u8* _data, u32 _cap) {
    gl_stack_allocator allocator;
    allocator.data = _data;
    allocator.frame = 0;
    allocator.cap = _cap;
    frame_header_w_at(allocator.data, GL_FRAME_SECTION_INDEX, allocator.cap);
    frame_header_w_at(allocator.data, GL_FRAME_SECTION_OFFSET, 0);
    return allocator;
}

b32 gl_stack_allocator_can_allocate(const gl_stack_allocator* _allocator,
                                   u32 _size) {
    const u32 aligned_size = align_to_8(_size);
    return (aligned_size <= gl_stack_allocator_r_remaining_size(_allocator));
}

u32 gl_stack_allocator_r_remaining_size(const gl_stack_allocator* _allocator) {
    const u32 header_size = (_allocator->frame + 1) * frame_section_total_size();
    const u32 curr_index = frame_header_r_at(_allocator->data,
                                             GL_FRAME_SECTION_INDEX);
    const u32 remaining_size = curr_index - header_size;
    return remaining_size;
    
}

gl_stack_allocator gl_stack_allocator_deallocate(const gl_stack_allocator* _allocator) {
    gl_stack_allocator allocator = * _allocator;
    allocator.frame--;
    return allocator;
}

gl_stack_allocator gl_stack_allocator_allocate(const gl_stack_allocator* _allocator,
                                             u32 _size) {
    const u32 aligned_size = align_to_8(_size);
    const u32 prev_index = frame_header_r_at(_allocator->data,
                                             GL_FRAME_SECTION_INDEX);
    const u32 curr_index = prev_index - aligned_size;
    gl_stack_allocator allocator = *_allocator;
    frame_header_w_at(_allocator->data, GL_FRAME_SECTION_INDEX, curr_index);
    frame_header_w_at(_allocator->data, GL_FRAME_SECTION_OFFSET, 0);
    allocator.frame++;
    return allocator;
}

u8* gl_stack_allocator_realize(gl_stack_allocator* _allocator, u32 _size) {
    const u32 aligned_size = align_to_8(_size);
    const u32 offset = frame_header_r_at(_allocator->data,
                                         GL_FRAME_SECTION_OFFSET);
    u8* data = _allocator->data + offset;
    frame_header_w_at(_allocator->data,
                      GL_FRAME_SECTION_OFFSET,
                      offset + aligned_size);
    return data;
}

