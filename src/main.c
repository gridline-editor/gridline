#include <string.h>
#include <stdio.h>

#include "utils/toml.h"

int main(void) {
    char* data = "\"\\u11ab \\U00033a1\"";
    printf("size: %zu\n", strlen(data));
    gl_source source = gl_source_init(0, (u8*) data, strlen(data));
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
