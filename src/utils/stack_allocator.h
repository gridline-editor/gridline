#ifndef GRIDLINE_STACK_ALLOCATOR_H
#define GRIDLINE_STACK_ALLOCATOR_H

#include "utils/types.h"

typedef struct gl_stack_allocator gl_stack_allocator;

struct gl_stack_allocator {
    u8* data;
    u32 cap;
    u32 frame;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
gl_stack_allocator gl_stack_allocator_init(u8* _data, u32 _cap);
b32 gl_stack_allocator_can_allocate(const gl_stack_allocator* _allocator,
                                    u32 _size);
u32 gl_stack_allocator_r_remaining_size(const gl_stack_allocator* _allocator);
gl_stack_allocator gl_stack_allocator_allocate(const gl_stack_allocator* _allocator,
                                               u32 _size);
u8* gl_stack_allocator_realize(gl_stack_allocator* _allocator, u32 _size);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GRIDLINE_STACK_ALLOCATOR_H
