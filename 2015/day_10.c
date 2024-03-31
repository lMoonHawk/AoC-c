#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t index;
    char* seq;
    size_t* out;
    size_t n;
    size_t count;
    size_t round_count;
} Element;

// https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
Element decays[] = {
    {.index = 1,  .seq = "22",                                         .out = (size_t[]){1},                     .n = 1},
    {.index = 2,  .seq = "13112221133211322112211213322112",           .out = (size_t[]){72, 91, 1, 20, 3},      .n = 5},
    {.index = 3,  .seq = "312211322212221121123222112",                .out = (size_t[]){2},                     .n = 1},
    {.index = 4,  .seq = "111312211312113221133211322112211213322112", .out = (size_t[]){32, 20, 3},             .n = 3},
    {.index = 5,  .seq = "1321132122211322212221121123222112",         .out = (size_t[]){4},                     .n = 1},
    {.index = 6,  .seq = "3113112211322112211213322112",               .out = (size_t[]){5},                     .n = 1},
    {.index = 7,  .seq = "111312212221121123222112",                   .out = (size_t[]){6},                     .n = 1},
    {.index = 8,  .seq = "132112211213322112",                         .out = (size_t[]){7},                     .n = 1},
    {.index = 9,  .seq = "31121123222112",                             .out = (size_t[]){8},                     .n = 1},
    {.index = 10, .seq = "111213322112",                               .out = (size_t[]){9},                     .n = 1},
    {.index = 11, .seq = "123222112",                                  .out = (size_t[]){10},                    .n = 1},
    {.index = 12, .seq = "3113322112",                                 .out = (size_t[]){61, 11},                .n = 2},
    {.index = 13, .seq = "1113222112",                                 .out = (size_t[]){12},                    .n = 1},
    {.index = 14, .seq = "1322112",                                    .out = (size_t[]){13},                    .n = 1},
    {.index = 15, .seq = "311311222112",                               .out = (size_t[]){67, 14},                .n = 2},
    {.index = 16, .seq = "1113122112",                                 .out = (size_t[]){15},                    .n = 1},
    {.index = 17, .seq = "132112",                                     .out = (size_t[]){16},                    .n = 1},
    {.index = 18, .seq = "3112",                                       .out = (size_t[]){17},                    .n = 1},
    {.index = 19, .seq = "1112",                                       .out = (size_t[]){18},                    .n = 1},
    {.index = 20, .seq = "12",                                         .out = (size_t[]){19},                    .n = 1},
    {.index = 21, .seq = "3113112221133112",                           .out = (size_t[]){67, 91, 1, 20, 27},     .n = 5},
    {.index = 22, .seq = "11131221131112",                             .out = (size_t[]){21},                    .n = 1},
    {.index = 23, .seq = "13211312",                                   .out = (size_t[]){22},                    .n = 1},
    {.index = 24, .seq = "31132",                                      .out = (size_t[]){23},                    .n = 1},
    {.index = 25, .seq = "111311222112",                               .out = (size_t[]){24, 14},                .n = 2},
    {.index = 26, .seq = "13122112",                                   .out = (size_t[]){25},                    .n = 1},
    {.index = 27, .seq = "32112",                                      .out = (size_t[]){26},                    .n = 1},
    {.index = 28, .seq = "11133112",                                   .out = (size_t[]){30, 27},                .n = 2},
    {.index = 29, .seq = "131112",                                     .out = (size_t[]){28},                    .n = 1},
    {.index = 30, .seq = "312",                                        .out = (size_t[]){29},                    .n = 1},
    {.index = 31, .seq = "13221133122211332",                          .out = (size_t[]){63, 20, 89, 1, 20, 30}, .n = 6},
    {.index = 32, .seq = "31131122211311122113222",                    .out = (size_t[]){67, 31},                .n = 2},
    {.index = 33, .seq = "11131221131211322113322112",                 .out = (size_t[]){32, 11},                .n = 2},
    {.index = 34, .seq = "13211321222113222112",                       .out = (size_t[]){33},                    .n = 1},
    {.index = 35, .seq = "3113112211322112",                           .out = (size_t[]){34},                    .n = 1},
    {.index = 36, .seq = "11131221222112",                             .out = (size_t[]){35},                    .n = 1},
    {.index = 37, .seq = "1321122112",                                 .out = (size_t[]){36},                    .n = 1},
    {.index = 38, .seq = "3112112",                                    .out = (size_t[]){37},                    .n = 1},
    {.index = 39, .seq = "1112133",                                    .out = (size_t[]){38, 92},                .n = 2},
    {.index = 40, .seq = "12322211331222113112211",                    .out = (size_t[]){39, 1, 20, 43},         .n = 4},
    {.index = 41, .seq = "1113122113322113111221131221",               .out = (size_t[]){68, 40},                .n = 2},
    {.index = 42, .seq = "13211322211312113211",                       .out = (size_t[]){41},                    .n = 1},
    {.index = 43, .seq = "311322113212221",                            .out = (size_t[]){42},                    .n = 1},
    {.index = 44, .seq = "132211331222113112211",                      .out = (size_t[]){63, 20, 43},            .n = 3},
    {.index = 45, .seq = "311311222113111221131221",                   .out = (size_t[]){67, 44},                .n = 2},
    {.index = 46, .seq = "111312211312113211",                         .out = (size_t[]){45},                    .n = 1},
    {.index = 47, .seq = "132113212221",                               .out = (size_t[]){46},                    .n = 1},
    {.index = 48, .seq = "3113112211",                                 .out = (size_t[]){47},                    .n = 1},
    {.index = 49, .seq = "11131221",                                   .out = (size_t[]){48},                    .n = 1},
    {.index = 50, .seq = "13211",                                      .out = (size_t[]){49},                    .n = 1},
    {.index = 51, .seq = "3112221",                                    .out = (size_t[]){61, 50},                .n = 2},
    {.index = 52, .seq = "1322113312211",                              .out = (size_t[]){63, 20, 51},            .n = 3},
    {.index = 53, .seq = "311311222113111221",                         .out = (size_t[]){67, 52},                .n = 2},
    {.index = 54, .seq = "11131221131211",                             .out = (size_t[]){53},                    .n = 1},
    {.index = 55, .seq = "13211321",                                   .out = (size_t[]){54},                    .n = 1},
    {.index = 56, .seq = "311311",                                     .out = (size_t[]){55},                    .n = 1},
    {.index = 57, .seq = "11131",                                      .out = (size_t[]){56},                    .n = 1},
    {.index = 58, .seq = "1321133112",                                 .out = (size_t[]){57, 1, 20, 27},         .n = 4},
    {.index = 59, .seq = "31131112",                                   .out = (size_t[]){58},                    .n = 1},
    {.index = 60, .seq = "111312",                                     .out = (size_t[]){59},                    .n = 1},
    {.index = 61, .seq = "132",                                        .out = (size_t[]){60},                    .n = 1},
    {.index = 62, .seq = "311332",                                     .out = (size_t[]){61, 20, 30},            .n = 3},
    {.index = 63, .seq = "1113222",                                    .out = (size_t[]){62},                    .n = 1},
    {.index = 64, .seq = "13221133112",                                .out = (size_t[]){63, 20, 27},            .n = 3},
    {.index = 65, .seq = "3113112221131112",                           .out = (size_t[]){67, 64},                .n = 2},
    {.index = 66, .seq = "111312211312",                               .out = (size_t[]){65},                    .n = 1},
    {.index = 67, .seq = "1321132",                                    .out = (size_t[]){66},                    .n = 1},
    {.index = 68, .seq = "311311222",                                  .out = (size_t[]){67, 61},                .n = 2},
    {.index = 69, .seq = "11131221133112",                             .out = (size_t[]){68, 20, 27},            .n = 3},
    {.index = 70, .seq = "1321131112",                                 .out = (size_t[]){69},                    .n = 1},
    {.index = 71, .seq = "311312",                                     .out = (size_t[]){70},                    .n = 1},
    {.index = 72, .seq = "11132",                                      .out = (size_t[]){71},                    .n = 1},
    {.index = 73, .seq = "13112221133211322112211213322113",           .out = (size_t[]){72, 91, 1, 20, 74},     .n = 5},
    {.index = 74, .seq = "312211322212221121123222113",                .out = (size_t[]){73},                    .n = 1},
    {.index = 75, .seq = "111312211312113221133211322112211213322113", .out = (size_t[]){32, 20, 74},            .n = 3},
    {.index = 76, .seq = "1321132122211322212221121123222113",         .out = (size_t[]){75},                    .n = 1},
    {.index = 77, .seq = "3113112211322112211213322113",               .out = (size_t[]){76},                    .n = 1},
    {.index = 78, .seq = "111312212221121123222113",                   .out = (size_t[]){77},                    .n = 1},
    {.index = 79, .seq = "132112211213322113",                         .out = (size_t[]){78},                    .n = 1},
    {.index = 80, .seq = "31121123222113",                             .out = (size_t[]){79},                    .n = 1},
    {.index = 81, .seq = "111213322113",                               .out = (size_t[]){80},                    .n = 1},
    {.index = 82, .seq = "123222113",                                  .out = (size_t[]){81},                    .n = 1},
    {.index = 83, .seq = "3113322113",                                 .out = (size_t[]){61, 82},                .n = 2},
    {.index = 84, .seq = "1113222113",                                 .out = (size_t[]){83},                    .n = 1},
    {.index = 85, .seq = "1322113",                                    .out = (size_t[]){84},                    .n = 1},
    {.index = 86, .seq = "311311222113",                               .out = (size_t[]){67, 85},                .n = 2},
    {.index = 87, .seq = "1113122113",                                 .out = (size_t[]){86},                    .n = 1},
    {.index = 88, .seq = "132113",                                     .out = (size_t[]){87},                    .n = 1},
    {.index = 89, .seq = "3113",                                       .out = (size_t[]){88},                    .n = 1},
    {.index = 90, .seq = "1113",                                       .out = (size_t[]){89},                    .n = 1},
    {.index = 91, .seq = "13",                                         .out = (size_t[]){90},                    .n = 1},
    {.index = 92, .seq = "3",                                          .out = (size_t[]){91},                    .n = 1}
};


void incr_element(size_t amount, size_t element, Element* decays, size_t nb_elements) {
    // Increment element index by amount for the round
    for (size_t i = 0; i < nb_elements; i++) {
        if (element == decays[i].index) {
            decays[i].round_count += amount;
            return;
        }
    }
}

void simulate(Element* decays, size_t nb_elements) {
    for (size_t i = 0; i < nb_elements; i++) {
        size_t seq_n = decays[i].count;
        if (seq_n == 0) continue;

        decays[i].count = 0;
        for (size_t j = 0; j < decays[i].n; j++) incr_element(seq_n, decays[i].out[j], decays, nb_elements);
    }

    for (size_t i = 0; i < nb_elements; i++) {
        decays[i].count += decays[i].round_count;
        decays[i].round_count = 0;
    }
}

size_t get_length(Element* decays, size_t nb_elements) {
    size_t len = 0;
    for (size_t i = 0; i < nb_elements; i++) len += strlen(decays[i].seq) * decays[i].count;
    return len;
}


int main() {
    size_t n = sizeof(decays) / sizeof(Element);

    FILE* input = fopen("data/day_10.txt", "r");
    char buffer[11];
    fgets(buffer, sizeof(buffer), input);

    // Initialise the array of elements with the input
    for (size_t i = 0; i < n; i++) {
        if (strcmp(buffer, decays[i].seq) == 0) decays[i].count = 1;
    }

    size_t part1 = 0;
    size_t part2 = 0;

    for (size_t i = 1; i <= 50; i++) {
        simulate(decays, n);
        if (i == 40) part1 = get_length(decays, n);
    }
    part2 = get_length(decays, n);

    printf("Part 1: %zu\n", part1);
    printf("Part 2: %zu", part2);

    return 0;
}
