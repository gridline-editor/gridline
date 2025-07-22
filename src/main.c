#include <stdio.h>

#include "utils/toml.h"

int main(void) {
    u8 data[] = "hi1234\r\n   \"hello from string\"";
    printf("size: %zu\n", sizeof(data));
    gl_source source = gl_source_init(0, &data[0], sizeof(data));
    gl_toml_lexer lexer = gl_toml_lexer_init(&source);
    u32 last_index = 0;

    do {
        last_index = lexer.pos.index;
        lexer = gl_toml_lexer_lex(&lexer);
        printf("\'%c\' (%u:%u): %u\n",
               source.data[lexer.token_pos.index],
               lexer.token_pos.ln,
               lexer.token_pos.col,
               lexer.first_nonblank.col);
    } while(lexer.pos.index != last_index);

    printf("end: %u\n", lexer.pos.index);
    return 0;
}
