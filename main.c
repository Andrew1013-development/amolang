#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "preprocessor.h"
#include "lexer.h"

int main(int argc, char** argv) {
    Lexer lexer;
    Token token;
    char* buf;

    if (argc != 2) exit_with_error("incorrect number of arguments", 1);

    // preprocessing
    buf = preprocess(argv[1]);

    // lexical analysis
    init_lexer(&lexer, buf);
    do {
        token = next_token(&lexer);
        print_token(token);
    } while (token.type != TOK_EOF && token.type != TOK_ERROR);

    // cleanup
    free(buf);
    return 0;
}
