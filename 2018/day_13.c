#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DARRAY_IMPLEMENTATION
#include "../common/darray.h"
#define GEOM_IMPLEMENTATION
#include "../common/geom.h"

#define LINE_MAX 256

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST,
} Orientation;

typedef enum {
    LEFT,
    STRAIGHT,
    RIGHT,
} Turn;

typedef struct {
    Point2 position;
    Orientation orientation;
    Turn turn;
    bool destroyed;
} Cart;

Point2 directions[4] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };

static inline int mod(int a, int b) {
    return (a % b + b) % b;
}

int cartcmp(const void* a, const void* b) {
    const Cart* cart_a = a;
    const Cart* cart_b = b;
    if (cart_a->position.y == cart_b->position.y)
        return cart_a->position.x - cart_b->position.x;
    return cart_a->position.y - cart_b->position.y;
}

void cart_update(Cart* cart, char tile) {
    if (
        (tile == '/' && (cart->orientation == NORTH || cart->orientation == SOUTH)) ||
        (tile == '\\' && (cart->orientation == EAST || cart->orientation == WEST))) {
        cart->orientation = mod(cart->orientation + 1, 4);
    } else if (
        (tile == '/' && (cart->orientation == EAST || cart->orientation == WEST)) ||
        (tile == '\\' && (cart->orientation == NORTH || cart->orientation == SOUTH))) {
        cart->orientation = mod(cart->orientation - 1, 4);
    } else if (tile == '+') {
        if (cart->turn == LEFT) {
            cart->orientation = mod(cart->orientation - 1, 4);
        } else if (cart->turn == RIGHT) {
            cart->orientation = mod(cart->orientation + 1, 4);
        }
        cart->turn = mod(cart->turn + 1, 3);
    }
}

void* parse_input(FILE* input, Cart** carts, size_t* stride) {
    char* cart_types = "^>v<";
    char* grid = da_create(sizeof(*grid));
    *carts = da_create(sizeof(Cart));

    int x = 0, y = 0;
    char buffer[LINE_MAX] = { 0 };
    while (fgets(buffer, sizeof(buffer), input) != NULL && buffer[0] != '\n' && buffer[0] != '\r') {
        while (buffer[x] != '\n' && buffer[x] != '\r') {
            char* cart_type = strchr(cart_types, buffer[x]);
            if (cart_type != NULL) {
                Orientation orientation = cart_type - cart_types;
                da_append(*carts, ((Cart) { { x, y }, orientation, LEFT, false }));
            }
            da_append(grid, buffer[x]);
            ++x;
        }
        *stride = x;
        ++y;
        x = 0;
    }
    return grid;
}

int main() {
    FILE* input = fopen("data/day_13.txt", "r");
    Cart* carts = NULL;
    size_t stride = 0;
    char* grid = parse_input(input, &carts, &stride);
    fclose(input);

    size_t remaining_carts = da_length(carts);
    Point2 first_crash_position = { 0 };
    while (remaining_carts > 1) {
        qsort(carts, da_length(carts), sizeof(Cart), cartcmp);
        for (size_t i = 0; i < da_length(carts); ++i) {
            Cart* cart = &carts[i];
            if (cart->destroyed)
                continue;

            cart->position = p2add(cart->position, directions[cart->orientation]);
            for (size_t j = 0; j < da_length(carts); ++j) {
                Cart* other = &carts[j];
                if (i == j || other->destroyed)
                    continue;
                if (cart->position.x == other->position.x && cart->position.y == other->position.y) {
                    cart->destroyed = true;
                    other->destroyed = true;
                    remaining_carts -= 2;
                }
            }
            if (!cart->destroyed) {
                char tile = grid[cart->position.x + cart->position.y * stride];
                cart_update(cart, tile);
            } else if (remaining_carts == da_length(carts) - 2) {
                first_crash_position = cart->position;
            }
        }
    }
    da_free(grid);

    Cart last_cart = { 0 };
    for (size_t i = 0; i < da_length(carts); ++i) {
        if (!carts[i].destroyed) {
            last_cart = carts[i];
            break;
        }
    }
    da_free(carts);

    printf("Part 1: %d,%d\n", first_crash_position.x, first_crash_position.y);
    printf("Part 2: %d,%d\n", last_cart.position.x, last_cart.position.y);
}