#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct {
    char **matches;
    int count;
    int capacity;
} MatchResult;

void print_usage() {
    printf("Usage: binary.exe [PATTERN] [TEXT]\n");
    printf("  PATTERN: Simple pattern to match (supports *, ?, basic character classes)\n");
    printf("  TEXT:    Text to search in\n");
    printf("\nExample: binary.exe \"*@*.com\" \"Contact john@example.com or mary@test.org\"\n");
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_alnum(char c) {
    return is_digit(c) || is_alpha(c);
}

bool match_char_class(char c, const char *class_str) {
    if (strncmp(class_str, "[digit]", 7) == 0) return is_digit(c);
    if (strncmp(class_str, "[alpha]", 7) == 0) return is_alpha(c);
    if (strncmp(class_str, "[alnum]", 7) == 0) return is_alnum(c);
    if (strncmp(class_str, "[space]", 7) == 0) return isspace(c);
    if (strncmp(class_str, "[lower]", 7) == 0) return islower(c);
    if (strncmp(class_str, "[upper]", 7) == 0) return isupper(c);
    return false;
}

bool simple_match(const char *pattern, const char *text) {
    const char *p = pattern;
    const char *t = text;
    
    while (*p && *t) {
        if (*p == '*') {
            p++;
            if (!*p) return true;
            
            while (*t) {
                if (simple_match(p, t)) return true;
                t++;
            }
            return false;
        } else if (*p == '?') {
            p++;
            t++;
        } else if (*p == '[') {
            char class_name[20];
            const char *end = strchr(p, ']');
            if (!end) return *p == *t && (p++, t++, true);
            
            int len = end - p + 1;
            if ((size_t)len < sizeof(class_name)) {
                strncpy(class_name, p, len);
                class_name[len] = '\0';
                if (match_char_class(*t, class_name)) {
                    p = end + 1;
                    t++;
                } else {
                    return false;
                }
            } else {
                return *p == *t && (p++, t++, true);
            }
        } else if (*p == '\\' && *(p+1)) {
            p++;
            if (*p == 'd') {
                if (!is_digit(*t)) return false;
            } else if (*p == 'w') {
                if (!is_alnum(*t) && *t != '_') return false;
            } else if (*p == 's') {
                if (!isspace(*t)) return false;
            } else {
                if (*p != *t) return false;
            }
            p++;
            t++;
        } else {
            if (*p != *t) return false;
            p++;
            t++;
        }
    }
    
    while (*p == '*') p++;
    return !*p && !*t;
}

int find_all_matches(const char *pattern, const char *text, MatchResult *result) {
    result->count = 0;
    result->capacity = 10;
    result->matches = malloc(result->capacity * sizeof(char*));
    
    int text_len = strlen(text);
    
    for (int i = 0; i <= text_len; i++) {
        for (int j = i; j <= text_len; j++) {
            char *substr = malloc(j - i + 1);
            strncpy(substr, text + i, j - i);
            substr[j - i] = '\0';
            
            if (simple_match(pattern, substr)) {
                if (result->count >= result->capacity) {
                    result->capacity *= 2;
                    result->matches = realloc(result->matches, result->capacity * sizeof(char*));
                }
                
                bool duplicate = false;
                for (int k = 0; k < result->count; k++) {
                    if (strcmp(result->matches[k], substr) == 0) {
                        duplicate = true;
                        break;
                    }
                }
                
                if (!duplicate && strlen(substr) > 0) {
                    result->matches[result->count] = malloc(strlen(substr) + 1);
                    strcpy(result->matches[result->count], substr);
                    result->count++;
                }
            }
            
            free(substr);
        }
    }
    
    return result->count;
}

void free_match_result(MatchResult *result) {
    if (result->matches) {
        for (int i = 0; i < result->count; i++) {
            free(result->matches[i]);
        }
        free(result->matches);
    }
}

void test_pattern(const char *pattern, const char *text, const char *description) {
    printf("\n=== %s ===\n", description);
    printf("Pattern: %s\n", pattern);
    printf("Text: %s\n", text);
    
    MatchResult result;
    int count = find_all_matches(pattern, text, &result);
    
    if (count > 0) {
        printf("Found %d match%s:\n", count, count == 1 ? "" : "es");
        for (int i = 0; i < count; i++) {
            printf("  %d: \"%s\"\n", i + 1, result.matches[i]);
        }
    } else {
        printf("No matches found.\n");
    }
    
    free_match_result(&result);
}

void find_substrings(const char *text, const char *substring, const char *description) {
    printf("\n=== %s ===\n", description);
    printf("Searching for: %s\n", substring);
    printf("Text: %s\n", text);
    
    const char *pos = text;
    int count = 0;
    
    while ((pos = strstr(pos, substring)) != NULL) {
        count++;
        printf("  %d: Found at position %d: \"", count, (int)(pos - text));
        
        int context_start = (pos - text > 10) ? -10 : -(pos - text);
        int context_end = 10;
        if (pos + strlen(substring) + context_end > text + strlen(text)) {
            context_end = strlen(text) - (pos + strlen(substring) - text);
        }
        
        for (int i = context_start; i < (int)strlen(substring) + context_end; i++) {
            if (i >= 0 && i < (int)strlen(substring)) {
                printf("\033[1m%c\033[0m", pos[i]);
            } else {
                printf("%c", pos[i]);
            }
        }
        printf("\"\n");
        
        pos += strlen(substring);
    }
    
    if (count == 0) {
        printf("No matches found.\n");
    }
}

void demonstrate_patterns() {
    printf("Simple Pattern Matching Demonstrations\n");
    printf("======================================\n");
    
    find_substrings("The year is 2024 and the temperature is 25 degrees.", 
                   "20", "Finding substring '20'");
    
    find_substrings("Contact us at support@example.com or admin@test.org for help.",
                   "@", "Finding email separators");
    
    test_pattern("*@*.com", "Contact us at support@example.com or admin@test.org for help.",
                "Email addresses ending in .com");
    
    test_pattern("[digit][digit][digit][digit]", "Born in 1985, graduated in 2007, retired in 2045.",
                "Four consecutive digits");
    
    test_pattern("*tion", "The organization needs attention to this situation.",
                "Words ending in 'tion'");
    
    test_pattern("\\d\\d-\\d\\d-\\d\\d\\d\\d", "Dates: 01-15-2024, 12-25-2023, and 03-14-2025.",
                "Date patterns DD-MM-YYYY");
    
    find_substrings("Items cost $15, $29.99, and $150.00 respectively.",
                   "$", "Currency symbols");
}

void interactive_mode() {
    char pattern[256], text[1024];
    
    printf("\nInteractive pattern matching mode:\n");
    printf("Supported patterns: *, ?, [digit], [alpha], [alnum], \\d, \\w, \\s\n");
    printf("(Enter empty pattern to exit)\n");
    
    while (1) {
        printf("\nEnter pattern: ");
        fflush(stdout);
        
        if (!fgets(pattern, sizeof(pattern), stdin)) break;
        
        pattern[strcspn(pattern, "\n")] = 0;
        if (strlen(pattern) == 0) break;
        
        printf("Enter text to search: ");
        fflush(stdout);
        
        if (!fgets(text, sizeof(text), stdin)) break;
        text[strcspn(text, "\n")] = 0;
        
        test_pattern(pattern, text, "User input");
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Simple Regular Expression Matcher\n");
        printf("=================================\n\n");
        demonstrate_patterns();
        interactive_mode();
    } else if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_usage();
            return 0;
        } else {
            printf("Error: Missing text argument\n");
            print_usage();
            return 1;
        }
    } else if (argc == 3) {
        const char *pattern = argv[1];
        const char *text = argv[2];
        
        test_pattern(pattern, text, "Command line input");
    } else {
        printf("Error: Too many arguments\n");
        print_usage();
        return 1;
    }
    
    return 0;
}