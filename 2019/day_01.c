#include <stdio.h>
#include <stdlib.h>

#define LINE_MAX 256

int main() {
    FILE* input = fopen("data/day_01.txt", "r");
    char buffer[LINE_MAX] = { 0 };
    int total_fuel = 0;
    int total_fuel_rec = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        int module_fuel = atoi(buffer) / 3 - 2;
        total_fuel += module_fuel;
        while (module_fuel > 0) {
            total_fuel_rec += module_fuel;
            module_fuel = module_fuel / 3 - 2;
        }
    }
    fclose(input);
    printf("Part 1: %d\n", total_fuel);
    printf("Part 2: %d\n", total_fuel_rec);
}