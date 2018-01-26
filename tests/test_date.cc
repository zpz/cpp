#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "date.h"

int
main(void)
{
    int y, m, d, h, mi, s;
    long int ns;
    struct calendar cal;
    int i;

    printf("Testing Gregorian/Julian calendar conversion functions:\n\n");

    for (i = 0; i < 10; i++) {
        y = (int)(rand() * 300. / RAND_MAX + 1800);
        m = (int)(rand() * 11. / RAND_MAX + 1);
        d = (int)(rand() * 27. / RAND_MAX + 1);
        h = (int)(rand() * 23. / RAND_MAX);
        mi = (int)(rand() * 59. / RAND_MAX);
        s = (int)(rand() * 59. / RAND_MAX);
        ns = (long int)(rand() * (1.e9 - 1.) / RAND_MAX);

        dateset_greg(&cal, y, m, d, h, mi, s, ns);

        printf("%5d %4d %4d %4d %4d %4d %12ld  %f\n",
                y, m, d, h, mi, s, ns, ns * 1.e-9);

        dateset_jul(&cal, cal.jdate);
        printf("%5d %4d %4d %4d %4d %4d %12ld  %f",
                cal.year, cal.mon, cal.mday,
                cal.hour, cal.min, cal.sec, cal.nsec,
                cal.nsec * 1.e-9);

        printf("............... ");
        if (cal.year == y && cal.mon == m && cal.mday == d && cal.hour == h
                && cal.min == mi && cal.sec == s
                && labs(cal.nsec - ns) * 1.e-9 < 0.0001) {
            printf("PASS\n");
        } else {
            printf("FAIL\n");
        }
    }

    return(0);
}

