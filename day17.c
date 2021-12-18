#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

static void do_step(int *x, int *y, int *dx, int *dy) {
    *x += *dx;
    if (*dx > 0)
        *dx -= 1;
    else if (*dx < 0)
        *dy += 1;
    *y += *dy;
    *dy -= 1;
}

int main() {
    int targetx1, targetx2, targety1, targety2;
    int result = scanf("target area: x=%d..%d, y=%d..%d\n", &targetx1, &targetx2, &targety1, &targety2);
    assert(result == 4);
    assert(0 < targetx1 && targetx1 <= targetx2);
    assert(targety1 <= targety2 && targety2 < 0);

    // Given a x velocity the probe moves v, v-1, v-2, ..., 0 units per step.
    // This means the maximum reachable range is the sum of [0; v] which is v(v+1)/2.
    // v(v+1)/2 = x
    // v²+v-2x = 0
    // v = (-1 +- sqrt(1 + 8x)) / 2
    int min_velx = ceil((sqrt(1 + 8 * targetx1) - 1) / 2);
    // The maximum x velocity must be smaller than target x + 1, so that the probe does
    // not immediately skip the goal area.
    int max_velx = targetx2;

    // About the y velocity:
    // The proble aways reaches y=0 again with a velocity of -start_velocity_y after 2*start_velocity_y+1 steps
    // Therefore the highest possible y velocity is targety1 and -targety1.
    // Higher speeds always miss the goal area.
    int min_vely = targety1;
    int max_vely = -targety1;

    int maxy = INT_MIN;
    int hit_count = 0;
    for (int velx = min_velx; velx <= max_velx; ++velx) {
        for (int vely = min_vely; vely <= max_vely; ++vely) {
            int dx = velx;
            int dy = vely;
            int x = 0;
            int y = 0;
            int this_maxy = INT_MIN;
            while (x <= targetx2 && y >= targety1) {
                if (targetx1 <= x && x <= targetx2 && targety1 <= y && y <= targety2) {
                    if (maxy < this_maxy) {
                        maxy = this_maxy;
                    }
                    hit_count += 1;
                    break;
                }
                do_step(&x, &y, &dx, &dy);
                if (this_maxy < y) {
                    this_maxy = y;
                }
            }
        }
    }
    printf("%d\n%d\n", maxy, hit_count);

    return 0;
}