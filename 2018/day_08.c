#include <stdio.h>

int parse_number(FILE* input) {
    int out = 0;
    char c = fgetc(input);
    while (c != EOF && c != '\n' && c != '\r' && c != ' ') {
        out = out * 10 + c - '0';
        c = fgetc(input);
    }
    return out;
}

int get_metadata(FILE* input) {
    int children_count = parse_number(input);
    int meta_count = parse_number(input);
    int total = 0;
    for (int i = 0; i < children_count; ++i)
        total += get_metadata(input);
    for (int i = 0; i < meta_count; ++i)
        total += parse_number(input);
    return total;
}

int get_value(FILE* input) {
    int children_count = parse_number(input);
    int meta_count = parse_number(input);
    int total = 0;

    if (children_count == 0) {
        for (int i = 0; i < meta_count; ++i)
            total += parse_number(input);
    } else {
        int values[children_count];
        for (int i = 0; i < children_count; ++i)
            values[i] = get_value(input);
        for (int i = 0; i < meta_count; ++i) {
            int meta = parse_number(input);
            if (meta > 0 && meta <= children_count)
                total += values[meta - 1];
        }
    }
    return total;
}


int main() {
    FILE* input = fopen("data/day_08.txt", "r");

    int metadata = get_metadata(input);
    fseek(input, 0, SEEK_SET);
    int value = get_value(input);
    fclose(input);

    printf("Part 1: %d\n", metadata);
    printf("Part 2: %d\n", value);
}