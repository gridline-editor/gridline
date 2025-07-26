#include <string.h>
#include <stdio.h>

#include "utils/toml.h"

int main(void) {
    char* data = "\"\\u11ab\"\n \'hello\'\n";
    printf("size: %zu\n", strlen(data));
    gl_source source = gl_source_init(0, (u8*) data, strlen(data));
    gl_toml_lexer lexer = gl_toml_lexer_init(&source);
    u32 last_index = 0;
    u32 cp = 0;

    while(1) {
        last_index = lexer.pos.index;
        lexer = gl_toml_lexer_lex(&lexer);
        cp = source.data[lexer.token_pos.index];
        if(cp == '\n') {
            printf("\'\\n\' (%u:%u)\n",
                   lexer.token_pos.ln,
                   lexer.token_pos.col);
        } else {
            printf("\'%c\'  (%u:%u)\n",
                   cp,
                   lexer.token_pos.ln,
                   lexer.token_pos.col);
        }

        if(lexer.pos.index == last_index) {
            break;
        }
    }

    printf("end: %u\n", lexer.pos.index);
    return 0;
}
