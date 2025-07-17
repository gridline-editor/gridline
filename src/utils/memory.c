#include "utils/types.h"
#include "utils/memory.h"

u32 deserialize_u32(const u8* _data, u32 _bytes) {
    u32 value = 0;
    for(u32 i = 0; i < _bytes; i++) {
        value |= _data[i] << (8 * i);
    }

    return value;
}

void serialize_u32(u8* _data, u32 _value) {
    _data[0] = (_value >> 24) & 0xff;
    _data[1] = (_value >> 16) & 0xff;
    _data[2] = (_value >> 8) & 0xff;
   _data[3] = _value & 0xff;
}
