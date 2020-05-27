#include "include.h"

static const int MAX_TOKEN_LENGTH = 2048;

enum Token_Type {
    TT_NAME,
    TT_VALUE,
    TT_SINGLE_CHAR,
    TT_STRING,
};

struct Token {
    Token_Type type;

    union {
        char name[1024] = {};
        char value[1024];
        char string[1024];
        char single_char;
    };

    int line_number = 0;
    int line_first_char = 0;
    int start_char = 0;
    int end_char = 0;
};

bool is_single_char(char c) {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '=' || c == ',' || c == ';' || c == '"' || c == ':';
}

bool is_number(char c) {
    return (c >= '0' && c <= '9') || c == '.';
}

struct Line {
    char text[1024] = {};
    int cursor = 0;
};

void tokenize(const char *text, Array<Token> &tokens, Array<Line> &lines) {
	const char *position = text;

    bool should_advance = true;

    int line_number = 1;
    int line_position = 1;
    Line new_line;

    auto advance = [&text, &line_position, &new_line]() -> void {
        if(*text != '\n') {
            new_line.text[new_line.cursor] = *text;
            new_line.cursor++;
        }
        
        text++;
        line_position++; 
    };

    while(*text) {
        if(*text == '\t') printf("no tabs allowed! idiot.\n");

        while(*text <= 32) {
            if(!*text) break;

            if(*text == '\n') {
                lines.append(new_line);
                new_line.cursor = 0;
                memset(new_line.text, 0, 1024);

                line_number++;
                line_position = 0;
            }

            advance();
            continue;
        }

        if(is_single_char(*text)) {
            if(*text == '"') {
                advance();

                Token t;
                t.type = TT_STRING;
                t.line_number = line_number;
                t.start_char = line_position;
                int n = 0;
                while(*text != '"') {
                    if(!*text) break;
                    t.string[n] = *text;
                    n++;
                    advance();
                }
                t.end_char = line_position;
                tokens.append(t);
                advance();
                should_advance = false;
            } else {
                Token t;
                t.line_number = line_number;
                t.start_char = t.end_char = line_position;
                t.type = TT_SINGLE_CHAR;
                t.single_char = *text;
                tokens.append(t);
            }
        } else {
            if(*text >= '0' && *text <= '9') {
                Token t;
                t.line_number = line_number;
                t.start_char = line_position;
                t.type = TT_VALUE;
                int n = 0;
                while(*text > 32) {
                    if(!is_number(*text)) {
                        should_advance = false;
                        break;
                    }

                    t.value[n] = *text;
                    n++;
                    advance();
                }
                t.end_char = line_position;
                tokens.append(t);
            } else {
                Token t;
                t.line_number = line_number;
                t.start_char = line_position;
                t.type = TT_NAME;
                int n = 0;

                while(*text > 32) {
                    if(is_single_char(*text)) {
                        should_advance = false;
                        break;
                    }

                    t.name[n] = *text;
                    n++;
                    advance();
                }
                t.end_char = line_position;
                tokens.append(t);
            }
        }

        if(should_advance) advance();
        should_advance = true;
    }

    lines.append(new_line);
}

void report_error(Config_File *config, Token *t, const char *text, Array<Line> &lines) {
    printf("Error in %s at line %d: %s:\n", config->file_name, t->line_number, text);

    printf("%s\n", lines[t->line_number - 1].text);

    for(int i = 0; i < t->start_char - 1; i++) printf(" ");
    for(int i = 0; i < t->end_char - t->start_char; i++) printf("^");

    printf("\n");
}

Token *get_next_token(Array<Token> &tokens, int current) {
    if(current + 1 >= tokens.num) return nullptr;
    return &tokens[current + 1];
}

struct Node {
    char name[1024] = {};

    Node *next = nullptr;
    Node *children = nullptr;
};

bool load_config_file(Config_File *config, const char *file_name) {
    Load_File_Result file = load_file(file_name);
    if(!file.data) return false;

    config->file_name = file_name;

    Array<Token> tokens;
    Array<Line> lines;
    tokenize(file.data, tokens, lines);

    if(tokens.num < 1) {
        printf("File %s has no contents.\n", file_name);
        return false;
    }

    int scope_level = 0;
    int current_index = 0;
    Token *t = &tokens[0];

    auto advance = [&current_index, &t, &tokens]() -> void { current_index++; t = &tokens[current_index]; };
    while(true) {
        if(t->type != TT_NAME) {
            report_error(config, t, "declarations must start with a name", lines);
            return false;
        }
    }

    //For(tokens) {
    //    if(it.type == TT_SINGLE_CHAR)   printf("Single char (%d) (%d - %d): %c\n", it.line_number, it.start_char, it.end_char, it.single_char);
    //    else if(it.type == TT_NAME)     printf("Name        (%d) (%d - %d): %s\n", it.line_number, it.start_char, it.end_char, it.name);
    //    else if (it.type == TT_VALUE)   printf("Value       (%d) (%d - %d): %s\n", it.line_number, it.start_char, it.end_char, it.value);
    //    else if (it.type == TT_STRING)  printf("String      (%d) (%d - %d): %s\n", it.line_number, it.start_char, it.end_char, it.string);
    //}}}

    return true;
}