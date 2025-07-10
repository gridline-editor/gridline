#ifndef GRIDLINE_MEMORY_H
#define GRIDLINE_MEMORY_H

#include "utils/types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
u32 deserialize_u32(const u8* _data, u32 _bytes);
void serialize_u32(u8* _data, u32 _value);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GRIDLINE_MEMORY_H
