#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

#define NO_KEY '|'
#define NO_OFFSET -1
#define NUM_ARGS 7

typedef enum {
    ARG_FLAG,
    ARG_OPTION,
} ArgumentType;
typedef enum {
    ACT_HELP,
    ACT_VERSION,
    ACT_SET_TRUE,
    ACT_SET_STRING,
} ArgumentAction;
typedef struct {
    const char key; // shorthand, prefixed with '-' (e.g. -h)
    const char *name; // proper name, prefixed with '--' (e.g. --help)
    const ArgumentType type; // type of argument
    const ArgumentAction action; // action to do when parsing this argument
    const size_t configuration_field; // offset to locate to field of configuration
    const char *doc; // documentation of argument
} Argument;
Argument args[] = {
    {'o', "output", ARG_OPTION, ACT_SET_STRING, offsetof(Configuration, output_file), "Place the output into <file>"},
    {'h', "help", ARG_FLAG, ACT_HELP, NO_OFFSET, "Display this information."},
    {'v', "verbose", ARG_FLAG, ACT_SET_TRUE, offsetof(Configuration, verbose), "Display additional information during compilation. (implies all debugging flags are on)"},
    {NO_KEY, "version", ARG_FLAG, ACT_VERSION, NO_OFFSET, "Display compiler version information."},
    {NO_KEY, "debug-preprocessor", ARG_FLAG, ACT_SET_TRUE, offsetof(Configuration, debug_preprocessor), "Dumps all debug output from the preprocessor."},
    {NO_KEY, "debug-lexer", ARG_FLAG, ACT_SET_TRUE, offsetof(Configuration, debug_lexer), "Dumps all debug output from the lexer."},
    {NO_KEY, "debug-parser", ARG_FLAG, ACT_SET_TRUE, offsetof(Configuration, debug_parser), "Dumps all debug output from the parser."},
};

#endif // ARGS_H
