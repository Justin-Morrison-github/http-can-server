#include  "utils.h"

void split(char* str, char delimiter, char* left, char* right) {
    size_t len = strlen(str);
    if (str == NULL || left== NULL || right==NULL || len == 0) {
        return;
    }

    size_t i;

    for (i = 0; i < len; i++) {
        if (str[i] == delimiter) {
            // Now i is the index of the delimiter
            break;
        }
    }

    // Left will equal str[0..i-1] inclusive
    size_t left_len = i; // -1 and +1 cancel out
    memcpy(left, str, left_len);
    left[left_len] = '\0';

    // Right will equal str[i+1..len+1] inclusive (include null terminator)
    size_t right_len = len - left_len;
    memcpy(right, str + i + 1, right_len);
}

void strip(char* str) {
    if (str == NULL || strlen(str) == 0) {
        return;
    }

    size_t start = 0;
    size_t end = strlen(str);

    while (start < end && isspace(str[start])) {
        start++; 
    }

    while (end > start && isspace(str[end - 1])){
        end--;
    }
    str[end] = '\0';

    size_t len = end - start;
    memmove(str, str + start, len + 1);
}