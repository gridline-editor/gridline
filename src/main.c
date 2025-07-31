#include <math.h>
#include <string.h>
#include <stdio.h>

#include "utils/toml.h"

int main(void) {
    char* data = "0123 = 1234\ntmp = 123abc\n";
    printf("size: %zu\n", strlen(data));
    const gl_source source = gl_source_init(0, (u8*) data, strlen(data));
    gl_toml_lexer lexer = gl_toml_lexer_init(&source);
    u32 last_index = 0;
    gl_toml_token token = {0};
    printf("Start  End   Type\n");
    while(1) {
        last_index = lexer.pos.index;
        lexer = gl_toml_lexer_lex(&lexer);
        token = gl_toml_lexer_r_token(&lexer);
        printf("%*u:%-3u%3u:%-3u \x1b[34m%s\x1b[0m\n",
               (int) log10f(token.start.ln),
               token.start.ln,
               token.start.col,
               token.end.ln,
               token.end.col,
               gl_toml_token_type_to_str(token.type));

        if(lexer.pos.index == last_index) {
            break;
        }
    }

    printf("end: %u\n", lexer.pos.index);
    return 0;
}
