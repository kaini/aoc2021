/*
Each iteration is of the same structure:

inp w
mul x 0
add x z
mod x 26   ; x1 = z % 26
div z $a   ; z1 = z / $a
add x $b
eql x w
eql x 0    ; x2 = (x1 + $b) != w
mul y 0
add y 25
mul y x
add y 1    ; y1 = 25 * x2 + 1
mul z y    ; z2 = z1 * y1
mul y 0
add y w
add y $c
mul y x    ; y2 = (w + $c) * x2
add z y    ; z3 = z2 + y2

Only z is carried between iterations (w, x and y are overwritten).
This leads to (given z, initial 0, leading to z')
    q = ((z % 26 + $b) != w)
    z' = (z / $a) * (25 * q + 1) + (w + $c) * q

The goal is to find an input sequence such that z is 0 after all iterations.

$a can 1 or 26, $b can be any integer, $c can be any positive integer.
Therefore no term (except a temporary term in q) can be negative!
Therefore, the second term of the sum (w + $c) * q is always >= 0. It can only be 0 if q is 0.
The first term of the sum is zero if z == 0 in $a == 1 iterations or if z < 26 in a == 26 iterations.

Once z is >= the product of the remaining $a it can never reach zero!
I use this property to serach the result.
*/

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define LEN (14)

static bool search(int result[LEN], int a[LEN], int b[LEN], int c[LEN], int at, long z, bool largest) {
    if (at == LEN && z == 0) {
        return true;
    }

    long limit = 1;
    for (int i = at; i < LEN; ++i) {
        limit *= a[i];
    }
    if (z >= limit) {
        return false;
    }

    for (int w = largest ? 9 : 1; largest ? w > 0 : w < 10; w = largest ? w - 1 : w + 1) {
        result[at] = w;
        long q = ((z % 26) + b[at]) != w;
        long next_z = (z / a[at]) * (25 * q + 1) + (w + c[at]) * q;
        if (search(result, a, b, c, at + 1, next_z, largest)) {
            return true;
        }
    }

    return false;
}

int main() {
    int a[LEN], b[LEN], c[LEN];
    for (int i = 0; i < LEN; ++i) {
        int result = scanf("inp w\n");
        assert(result == 0);
        result = scanf("mul x 0\n");
        assert(result == 0);
        result = scanf("add x z\n");
        assert(result == 0);
        result = scanf("mod x 26\n");
        assert(result == 0);
        result = scanf("div z %d\n", &a[i]);
        assert(result == 1);
        result = scanf("add x %d\n", &b[i]);
        assert(result == 1);
        result = scanf("eql x w\n");
        assert(result == 0);
        result = scanf("eql x 0\n");
        assert(result == 0);
        result = scanf("mul y 0\n");
        assert(result == 0);
        result = scanf("add y 25\n");
        assert(result == 0);
        result = scanf("mul y x\n");
        assert(result == 0);
        result = scanf("add y 1\n");
        assert(result == 0);
        result = scanf("mul z y\n");
        assert(result == 0);
        result = scanf("mul y 0\n");
        assert(result == 0);
        result = scanf("add y w\n");
        assert(result == 0);
        result = scanf("add y %d\n", &c[i]);
        assert(result == 1);
        result = scanf("mul y x\n");
        assert(result == 0);
        result = scanf("add z y\n");
        assert(result == 0);
    }
    fprintf(stderr, "a = ");
    for (int i = 0; i < LEN; ++i) {
        fprintf(stderr, "% 3d ", a[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "b = ");
    for (int i = 0; i < LEN; ++i) {
        fprintf(stderr, "% 3d ", b[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "c = ");
    for (int i = 0; i < LEN; ++i) {
        fprintf(stderr, "% 3d ", c[i]);
    }
    fprintf(stderr, "\n");

    int result[LEN];
    int found = search(result, a, b, c, 0, 0, true);
    assert(found);
    for (int i = 0; i < LEN; ++i) {
        printf("%d", result[i]);
    }
    printf("\n");

    found = search(result, a, b, c, 0, 0, false);
    assert(found);
    for (int i = 0; i < LEN; ++i) {
        printf("%d", result[i]);
    }
    printf("\n");

    return 0;
}