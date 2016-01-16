#include <math.h>

struct calendar {
    int year;   /* Year number */
    int mon;    /* Month of the year [1, 12] */
/*  int week; */  /* Week of the year [1, 53] */
    int mday;   /* Day of the month [1, 31] */
/*  int yday; */  /* Day of the year [1, 366] */
/*  int wday; */  /* Day of the week [1, 7] */
    int hour;   /* Hour of the day [0, 23] */
    int min;    /* Minute of the hour [0, 59] */
    int sec;    /* Second of the minute [0, 59] */
    long int nsec; /* Nanosecond [0, 10^9 - 1] */
    double jdate;  /* Julian date */
    };


/* Set date structure given Gregorian calendar items. */
int dateset_greg(struct calendar *cal, int, int, int, int, int, int, long int);

/* Set date structure given Julian date value. */
int dateset_jul(struct calendar *cal, double);

void datecpy(struct calendar *dest, const struct calendar *src);

