#include <string.h>
#include <stdio.h>

#include "utils/toml.h"

int main(void) {
    char* data = "0x12a34\n0b10010001\n0o577\n000123\n0\n123\n";
    printf("size: %zu\n", strlen(data));
    gl_source source = gl_source_init(0, (u8*) data, strlen(data));
    gl_toml_lexer lexer = gl_toml_lexer_init(&source);
    u32 last_index = 0;
    u32 cp = 0;

    gl_toml_token token = {0};
    while(1) {
        last_index = lexer.pos.index;
        lexer = gl_toml_lexer_lex(&lexer);
        token = gl_toml_lexer_r_token(&lexer);
        cp = source.data[token.start.index];
        if(cp == '\n') {
            printf("\'\\n\' start: (%u:%u), end: (%u:%u)\n",
                   token.start.ln,
                   token.start.col,
                   token.end.ln,
                   token.end.col);
        } else {
            printf("\'%c\'  start: (%u:%u), end: (%u:%u)\n",
                   lexer.source->data[token.start.index],
                   token.start.ln,
                   token.start.col,
                   token.end.ln,
                   token.end.col);
        }

        if(lexer.pos.index == last_index) {
            break;
        }
    }

    printf("end: %u\n", lexer.pos.index);
    return 0;
}
