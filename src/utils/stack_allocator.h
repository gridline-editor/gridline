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
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GRIDLINE_STACK_ALLOCATOR_H
