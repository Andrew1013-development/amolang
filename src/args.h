#ifndef ARGS_H
#define ARGS_H

#define NO_KEY '|'

typedef enum {
    ARG_OPTIONAL,
    ARG_POSITIONAL,
} ArgumentType;
typedef struct {
    const char key; // shorthand, prefixed with '-' (e.g. -h)
    const char *name; // proper name, prefixed with '--' (e.g. --help)
    const ArgumentType type; // type of argument
    const char *doc; // documentation of argument
} Argument;
Argument args[] = {
    {'o', "output", ARG_POSITIONAL, "???"},
    {'h', "help", ARG_OPTIONAL, "Display this information."},
    {'v', "verbose", ARG_OPTIONAL, "Display additional information during compilation. (implies all debugging flags are on)"},
    {NO_KEY, "version", ARG_OPTIONAL, "Display compiler version information."},
    {NO_KEY, "debug-preprocessor", ARG_OPTIONAL, "Dumps all debug output from the preprocessor."},
    {NO_KEY, "debug-lexer", ARG_OPTIONAL, "Dumps all debug output from the lexer."},
    {NO_KEY, "debug-parser", ARG_OPTIONAL, "Dumps all debug output from the parser."},
};

#endif // ARGS_H
