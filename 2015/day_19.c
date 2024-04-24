#include <stdio.h>
#include <ctype.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"

#define LINE_MAX 1024

typedef struct {
    char* from;
    char* to;
} Replacement;


void parse_input(FILE* input, Replacement** replacements, char molecule[LINE_MAX]) {
    char buffer[LINE_MAX] = { 0 };
    fgets(buffer, sizeof(buffer), input);
    while (buffer[0] != '\n') {
        buffer[strcspn(buffer, "\n")] = '\0';

        char* molecule = strtok(buffer, " ");
        char* molecule_rep = strtok(NULL, " =>");
        Replacement entry = {};
        entry.from = strdup(molecule);
        entry.to = strdup(molecule_rep);
        da_append(*replacements, entry);
        fgets(buffer, sizeof(buffer), input);
    }

    fgets(molecule, LINE_MAX, input);
    molecule[strcspn(molecule, "\n")] = '\0';
}

void add_possibility(char*** possibilities, const char* buffer, const char* mol_buff, size_t size_buff, size_t bp, Replacement* replacements) {
    for (size_t i = 0; i < da_length(replacements); ++i) {
        if (strcmp(mol_buff, replacements[i].from) == 0) {

            size_t size_repl = strlen(replacements[i].to);
            char* str = calloc(size_buff + size_repl + 1, sizeof(char));

            strncat(str, buffer, bp - strlen(mol_buff) - 1);
            strncat(str, replacements[i].to, size_repl);
            strcat(str, buffer + bp - 1);

            // Search in the possibilities if the molecule is already there.
            // If it is: free, otherwise add it to the list of possibilities
            int found = 0;
            for (size_t i = 0; i < da_length(*possibilities); ++i) {
                if (strcmp((*possibilities)[i], str) == 0) {
                    found = 1;
                    break;
                }
            }
            if (found) free(str);
            else da_append(*possibilities, str);
        }
    }
}

long possibilities_cnt(Replacement* replacements, char* buffer, size_t size_buff) {
    char** possibilities = da_create(sizeof(char*));
    char c;
    char mol_buff[3] = { 0 };
    size_t bp = 0;
    while ((c = buffer[bp++]) != '\0') {
        if (isupper(c) && mol_buff[0] != '\0') {
            add_possibility(&possibilities, buffer, mol_buff, size_buff, bp, replacements);
            mol_buff[0] = '\0';
        }
        strncat(mol_buff, &c, 1);
    }
    add_possibility(&possibilities, buffer, mol_buff, size_buff, bp, replacements);

    long result = da_length(possibilities);

    for (size_t i = 0; i < da_length(possibilities); ++i) {
        free(possibilities[i]);
    }
    da_free(possibilities);

    return result;
}


void next_molecule(char* molecule_next, char* molecule, char* sub_mol, ssize_t max_rep_size, Replacement* replacements) {
    ssize_t mol_size = strlen(molecule);
    for (ssize_t ep = mol_size - 1; ep >= 0; --ep) {

        for (ssize_t sp = ep; sp > ep - max_rep_size && sp >= 0; --sp) {
            if (!isupper(molecule[sp]))
                continue;
            strncpy(sub_mol, molecule + sp, ep - sp + 1);
            sub_mol[ep - sp + 1] = '\0';

            for (size_t i = 0; i < da_length(replacements); ++i) {
                if (strcmp(replacements[i].to, sub_mol) == 0) {
                    strncat(molecule_next, molecule, sp);
                    strcat(molecule_next, replacements[i].from);
                    strncat(molecule_next, molecule + ep + 1, mol_size + 1 - ep);
                    return;
                }
            }

        }
    }
}

int main() {
    FILE* input = fopen("data/day_19.txt", "r");
    Replacement* replacements = da_create(sizeof(Replacement));

    char* molecule = calloc(LINE_MAX, sizeof(char));
    parse_input(input, &replacements, molecule);

    size_t size_mol = strlen(molecule);
    long cnt = possibilities_cnt(replacements, molecule, size_mol);

    ssize_t max_rep_size = 0;
    for (size_t i = 0; i < da_length(replacements); ++i) {
        ssize_t n = strlen(replacements[i].to);
        if (n > max_rep_size) max_rep_size = n;
    }


    char sub_mol[max_rep_size + 1];
    char* molecule_next = calloc(size_mol + 1, sizeof(char));
    long steps = 0;
    while (1) {
        next_molecule(molecule_next, molecule, sub_mol, max_rep_size, replacements);
        ++steps;
        if (strcmp(molecule_next, "e") == 0)
            break;
        char* tmp = molecule;
        molecule = molecule_next;
        molecule_next = tmp;
        molecule_next[0] = '\0';
    }

    printf("Part 1: %ld\n", cnt);
    printf("Part 2: %ld", steps);

    // Cleanup
    free(molecule);
    free(molecule_next);
    for (size_t i = 0; i < da_length(replacements); ++i) {
        free(replacements[i].from);
        free(replacements[i].to);
    }
    da_free(replacements);
    fclose(input);
}