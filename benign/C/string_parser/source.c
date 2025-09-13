#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct {
    char *key;
    char *value;
} KeyValue;

typedef struct {
    KeyValue *pairs;
    int count;
    int capacity;
} ArgumentParser;

ArgumentParser* create_parser() {
    ArgumentParser *parser = malloc(sizeof(ArgumentParser));
    parser->pairs = malloc(sizeof(KeyValue) * 10);
    parser->count = 0;
    parser->capacity = 10;
    return parser;
}

void destroy_parser(ArgumentParser *parser) {
    if (parser) {
        for (int i = 0; i < parser->count; i++) {
            free(parser->pairs[i].key);
            free(parser->pairs[i].value);
        }
        free(parser->pairs);
        free(parser);
    }
}

void resize_parser(ArgumentParser *parser) {
    if (parser->count >= parser->capacity) {
        parser->capacity *= 2;
        parser->pairs = realloc(parser->pairs, sizeof(KeyValue) * parser->capacity);
    }
}

void add_key_value(ArgumentParser *parser, const char *key, const char *value) {
    resize_parser(parser);
    
    parser->pairs[parser->count].key = malloc(strlen(key) + 1);
    parser->pairs[parser->count].value = malloc(strlen(value) + 1);
    
    strcpy(parser->pairs[parser->count].key, key);
    strcpy(parser->pairs[parser->count].value, value);
    
    parser->count++;
}

char* find_value(ArgumentParser *parser, const char *key) {
    for (int i = 0; i < parser->count; i++) {
        if (strcmp(parser->pairs[i].key, key) == 0) {
            return parser->pairs[i].value;
        }
    }
    return NULL;
}

bool parse_command_line(ArgumentParser *parser, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-') {
                char *arg = argv[i] + 2;
                char *eq = strchr(arg, '=');
                
                if (eq) {
                    *eq = '\0';
                    add_key_value(parser, arg, eq + 1);
                    *eq = '=';
                } else {
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        add_key_value(parser, arg, argv[i + 1]);
                        i++;
                    } else {
                        add_key_value(parser, arg, "true");
                    }
                }
            } else {
                char flag[3] = {'-', argv[i][1], '\0'};
                
                if (strlen(argv[i]) > 2) {
                    add_key_value(parser, flag, argv[i] + 2);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    add_key_value(parser, flag, argv[i + 1]);
                    i++;
                } else {
                    add_key_value(parser, flag, "true");
                }
            }
        } else {
            char pos_key[20];
            snprintf(pos_key, sizeof(pos_key), "arg%d", i);
            add_key_value(parser, pos_key, argv[i]);
        }
    }
    return true;
}

void trim_whitespace(char *str) {
    char *end;
    
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return;
    
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
}

bool parse_config_line(const char *line, char *key, char *value, size_t key_size, size_t value_size) {
    if (!line || strlen(line) == 0 || line[0] == '#') {
        return false;
    }
    
    char *eq = strchr(line, '=');
    if (!eq) {
        return false;
    }
    
    size_t key_len = eq - line;
    size_t value_len = strlen(eq + 1);
    
    if (key_len >= key_size || value_len >= value_size) {
        return false;
    }
    
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    strcpy(value, eq + 1);
    
    trim_whitespace(key);
    trim_whitespace(value);
    
    if (value[0] == '"' && value[strlen(value) - 1] == '"') {
        value[strlen(value) - 1] = '\0';
        memmove(value, value + 1, strlen(value));
    }
    
    return true;
}

void print_parser_contents(ArgumentParser *parser) {
    printf("Parsed arguments (%d total):\n", parser->count);
    for (int i = 0; i < parser->count; i++) {
        printf("  %s = %s\n", parser->pairs[i].key, parser->pairs[i].value);
    }
    printf("\n");
}

void demonstrate_string_operations(const char *text) {
    printf("String operations demonstration:\n");
    printf("Original text: \"%s\"\n", text);
    printf("Length: %zu characters\n", strlen(text));
    
    char *copy = malloc(strlen(text) + 1);
    strcpy(copy, text);
    
    for (char *p = copy; *p; p++) {
        *p = toupper(*p);
    }
    printf("Uppercase: \"%s\"\n", copy);
    
    strcpy(copy, text);
    for (char *p = copy; *p; p++) {
        *p = tolower(*p);
    }
    printf("Lowercase: \"%s\"\n", copy);
    
    char *token = strtok(copy, " ");
    printf("Tokens: ");
    while (token) {
        printf("\"%s\" ", token);
        token = strtok(NULL, " ");
    }
    printf("\n\n");
    
    free(copy);
}

int main(int argc, char *argv[]) {
    printf("Command Line and String Parser Demo\n");
    printf("===================================\n\n");
    
    ArgumentParser *parser = create_parser();
    
    if (argc > 1) {
        printf("Parsing command line arguments...\n");
        parse_command_line(parser, argc, argv);
        print_parser_contents(parser);
        
        char *help = find_value(parser, "--help");
        char *verbose = find_value(parser, "-v");
        char *output = find_value(parser, "--output");
        
        if (help) {
            printf("Help flag detected: %s\n", help);
        }
        if (verbose) {
            printf("Verbose mode: %s\n", verbose);
        }
        if (output) {
            printf("Output file: %s\n", output);
        }
        printf("\n");
    } else {
        printf("No command line arguments provided.\n");
        printf("Try: ./binary.exe --help -v --output=test.txt input.dat\n");
        printf("Or:  ./binary.exe --name=\"John Doe\" --age=25 --debug\n\n");
    }
    
    printf("Simulating config file parsing:\n");
    const char *config_lines[] = {
        "name=Application Name",
        "version=1.2.3",
        "debug=true",
        "# This is a comment",
        "output_dir=\"/tmp/output\"",
        "max_connections=100",
        "",
        "invalid line without equals",
        NULL
    };
    
    char key[256], value[256];
    for (int i = 0; config_lines[i] != NULL; i++) {
        if (parse_config_line(config_lines[i], key, value, sizeof(key), sizeof(value))) {
            add_key_value(parser, key, value);
            printf("  %s = %s\n", key, value);
        } else {
            printf("  Skipped: \"%s\"\n", config_lines[i]);
        }
    }
    
    printf("\nFinal parsed data:\n");
    print_parser_contents(parser);
    
    demonstrate_string_operations("Hello World! This is a Test String.");
    
    destroy_parser(parser);
    return 0;
}