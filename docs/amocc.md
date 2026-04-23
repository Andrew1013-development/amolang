# amocc

## Arguments
This text can be found by running `amocc --help`.
```
Usage: amocc [options] file...
Options:
        -o, --output <option>    Place the output into <file>
        -h, --help               Display this information.
        -v, --verbose            Display additional information during compilation. (implies all debugging flags are on)
        --version                Display compiler version information.
        --debug-preprocessor     Dumps all debug output from the preprocessor.
        --debug-lexer            Dumps all debug output from the lexer.
        --debug-parser           Dumps all debug output from the parser.
```

## Error codes
### General
| Code | Description                                   |
|------|-----------------------------------------------|
| 1    | no arguments specified                        |
| 12   | invalid argument flag                         |
| 13   | compilation with multiple files not supported |
| 14   | no input file specified                       |

### Preprocessor
| Code | Description                                   |
|------|-----------------------------------------------|
| 2    | unable to read file                           |
| 3    | something went wrong while reading file       |
| 4    | compilation with multiple files not supported |

### Memory Allocator
| Code | Description               |
|------|---------------------------|
| 5    | unable to allocate memory |

### Parser
| Code | Description                                   |
|------|-----------------------------------------------|
| 6    | invalid top-level declaration                 |
| 7    | malformed declaration                         |
| 8    | mismatched token types                        |
| 9    | function argument has invalid or no type      |
| 10   | expected variable assignment or function call |
| 11   | expected expression                           |
| 15   | expected statement                            |