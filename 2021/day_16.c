#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

int64_t parse_packet(char** bits_ptr, int* sum_version);

char* parse_input(FILE* input) {
    char* bits = da_create(1);

    char c = fgetc(input);
    char bit_repr[4];
    while (c != EOF && c != '\n' && c != '\r') {
        int value = (c >= '0' && c <= '9') ? c - '0' : c - 'A' + 10;
        for (int i = 0; i < 4; ++i)
            bit_repr[i] = (value & (1 << (3 - i))) ? '1' : '0';
        da_extend(bits, bit_repr, 4);
        c = fgetc(input);
    }
    da_append(bits, (char) { '\0' });
    return bits;
}

int64_t sum_packets(int64_t* subpackets) {
    int64_t result = 0;
    for (size_t i = 0; i < da_length(subpackets); ++i)
        result += subpackets[i];
    return result;
}

int64_t mul_packets(int64_t* subpackets) {
    int64_t result = 1;
    for (size_t i = 0; i < da_length(subpackets); ++i)
        result *= subpackets[i];
    return result;
}

int64_t min_packets(int64_t* subpackets) {
    int64_t result = subpackets[0];
    for (size_t i = 1; i < da_length(subpackets); ++i)
        result = subpackets[i] < result ? subpackets[i] : result;
    return result;
}

int64_t max_packets(int64_t* subpackets) {
    int64_t result = subpackets[0];
    for (size_t i = 1; i < da_length(subpackets); ++i)
        result = subpackets[i] > result ? subpackets[i] : result;
    return result;
}

int parse_int(char** bits_ptr, int n) {
    int result = 0;
    for (int i = 0; i < n; ++i) {
        result <<= 1;
        result |= *((*bits_ptr)++) - '0';
    }
    return result;
}

int64_t parse_literal(char** bits_ptr) {
    int64_t result = 0;
    uint8_t prefix = 1;
    while (prefix) {
        int group = parse_int(bits_ptr, 5);
        prefix = group >> 4;
        result = (result << 4) | (group & 0xF);
    }
    return result;
}

int64_t do_operation(int type_id, int64_t* subpackets) {
    switch (type_id) {
        case 0: return sum_packets(subpackets);
        case 1: return mul_packets(subpackets);
        case 2: return min_packets(subpackets);
        case 3: return max_packets(subpackets);
        case 5: return subpackets[0] > subpackets[1];
        case 6: return subpackets[0] < subpackets[1];
        case 7: return subpackets[0] == subpackets[1];
    }
    assert(0 && "UNREACHABLE");
}

void parse_operator(char** bits_ptr, int64_t** subpackets, int* sum_version) {
    uint8_t length_id = parse_int(bits_ptr, 1);
    if (length_id == 0) {
        uint16_t sublength = parse_int(bits_ptr, 15);
        char* start = *bits_ptr;
        while (*bits_ptr < start + sublength) {
            int64_t value = parse_packet(bits_ptr, sum_version);
            da_append(*subpackets, value);
        }
    } else if (length_id == 1) {
        uint16_t subcount = parse_int(bits_ptr, 11);
        for (int i = 0; i < subcount; ++i) {
            int64_t value = parse_packet(bits_ptr, sum_version);
            da_append(*subpackets, value);
        }
    }
}

int64_t parse_packet(char** bits_ptr, int* sum_version) {
    *sum_version += parse_int(bits_ptr, 3);
    int type_id = parse_int(bits_ptr, 3);
    if (type_id == 4)
        return parse_literal(bits_ptr);
    int64_t* subpackets = da_create(sizeof(*subpackets));
    parse_operator(bits_ptr, &subpackets, sum_version);
    int64_t result = do_operation(type_id, subpackets);
    da_free(subpackets);
    return result;
}

int main() {
    FILE* input = fopen("data/day_16.txt", "r");
    char* bits = parse_input(input);
    fclose(input);

    char* stream = bits;
    int sum_version = 0;
    int64_t result = parse_packet(&stream, &sum_version);
    da_free(bits);

    printf("Part 1: %d\n", sum_version);
    printf("Part 2: %" PRId64 "\n", result);
}