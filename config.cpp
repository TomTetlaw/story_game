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
    char single_char = 0;
    char name[64] = {};
    char value[64] = {};
    char string[64] = {};
};

bool is_single_char(char c) {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '=' || c == ',' || c == ';' || c == '"';
}

bool is_number(char c) {
    return (c >= '0' && c <= '9') || c == '.';
}

void tokenize(const char *text, Array<Token> &tokens) {
	const char *position = text;

    bool should_advance = true;

    while(*text) {
        while(*text <= 32) {
            if(!*text) break;

            text++;
            continue;
        }

        if(is_single_char(*text)) {
            if(*text == '"') {
                text++;

                Token t;
                t.type = TT_STRING;
                int n = 0;
                while(*text != '"') {
                    if(!*text) break;
                    t.string[n] = *text;
                    n++;
                    text++;
                }
                tokens.append(t);
                text++;
            } else {
                Token t;
                t.type = TT_SINGLE_CHAR;
                t.single_char = *text;
                tokens.append(t);
            }
        } else {
            if(*text >= '0' && *text <= '9') {
                Token t;
                t.type = TT_VALUE;

                int n = 0;
                while(*text > 32) {
                    if(!is_number(*text)) {
                        should_advance = false;
                        break;
                    }

                    t.value[n] = *text;
                    n++;
                    text++;
                }

                tokens.append(t);
            } else {
                Token t;
                t.type = TT_NAME;
                int n = 0;

                while(*text > 32) {
                    if(is_single_char(*text)) {
                        should_advance = false;
                        break;
                    }

                    t.name[n] = *text;
                    n++;
                    text++;
                }

                tokens.append(t);
            }
        }

        if(should_advance) text++;
        should_advance = true;
    }
}

struct Ast_Node {
    char name[64] = {};
    Ast_Node *children = nullptr;
    Ast_Node *next = nullptr;
};

bool load_config_file(Config_File *config, const char *file_name) {
    Load_File_Result file = load_file(file_name);
    if(!file.data) return false;

    Array<Token> tokens;
    tokenize(file.data, tokens);

    For(tokens) {
        if(it.type == TT_SINGLE_CHAR) printf("Single char: %c\n", it.single_char);
        else if(it.type == TT_NAME) printf("Name: %s\n", it.name);
        else if (it.type == TT_VALUE) printf("Value: %s\n", it.value);
        else if (it.type == TT_STRING) printf("String: %s\n", it.string);
    }}}

    return true;
}