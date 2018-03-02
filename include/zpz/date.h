#ifndef _zpz_utilities_date_h_
#define _zpz_utilities_date_h_

/* Zepu Zhang
 *   2006-12-25
 *   2006-01-10
 */

#include <math.h>

namespace zpz {

struct calendar {
    int year; /* Year number */
    int mon; /* Month of the year [1, 12] */
    /*  int week; */ /* Week of the year [1, 53] */
    int mday; /* Day of the month [1, 31] */
    /*  int yday; */ /* Day of the year [1, 366] */
    /*  int wday; */ /* Day of the week [1, 7] */
    int hour; /* Hour of the day [0, 23] */
    int min; /* Minute of the hour [0, 59] */
    int sec; /* Second of the minute [0, 59] */
    long int nsec; /* Nanosecond [0, 10^9 - 1] */
    double jdate; /* Julian date */
};

/* Set date structure given Gregorian calendar items. */
int dateset_greg(struct calendar* cal, int, int, int, int, int, int, long int);

/* Set date structure given Julian date value. */
int dateset_jul(struct calendar* cal, double);

void datecpy(struct calendar* dest, const struct calendar* src);

/* Given the Julian date number for a date and time,
 * return a structure with Gregorian calendar components.
 *
 * Ref:
 *   http://aa.usno.navy.mil/faq/docs/JD_Formula.html
 */
int dateset_jul(struct calendar* cal, double jdate)
{

    double frac, x;
    long int i, j, k, m, n;

    frac = jdate - floor(jdate);
    if (frac < 0.5) {
        m = (long)(floor(jdate)) + (long)68569;
        frac = frac + 0.5;
    } else {
        m = (long)(ceil(jdate)) + (long)68569;
        frac = frac - 0.5;
    }
    n = 4 * m / (long)146097;
    m = m - ((long)146097 * n + 3) / 4;
    i = 4000 * (m + 1) / (long)1461001;
    m = m - 1461 * i / 4 + 31;
    j = 80 * m / 2447;
    k = m - 2447 * j / 80;
    m = j / 11;
    j = j + 2 - 12 * m;
    i = 100 * (n - 49) + i + m;

    cal->year = (int)i;
    cal->mon = (int)j;
    cal->mday = (int)k;
    cal->hour = (int)(frac * 24);
    cal->min = (int)(fmod(frac * 24., 1.) * 60);
    x = fmod(frac * 24. * 60., 1.) * 60.;
    cal->sec = (int)x;
    cal->nsec = (long)((x - cal->sec) * 1.e9);

    cal->jdate = jdate;

    return 0;
}

/* Given date and time on the Gregorian caldendar,
 * return its  Julian date number which is the number of days
 * since noon on January 1, -4712, i.e., January 1, 4713 BC (Seidelmann 1992).
 * The Julian date is a whole number at noon (not at midnight)
 * because its reference time is noon.
 *
 * Because the Julian date is a huge number that may cause precision trouble,
 * always use the -fdefault-real-8 option while compiling this procedure.
 * This option makes double precision the default real kind, 
 * Double precision is enforced in this function;
 * but using that compiler option will make caller programs
 * that declare 'real' variables consistent with this function.
 *
 * Some call the day numeric in the year the 'Julian date'.
 * It is wrong and should be avoided.
 *
 * If hour, minute, and second are present, add the fraction of the day.
 *
 * Remember that day lengths vary; over time, the length of a day is increasing.  
 * Counting days is not an absoulte measure of time in the strict sense.
 * But it's okay for the time concept of most measurement data.
 *
 * A formula given by Danby (1988) and Sinnott (1991) for Gregorian calendar dates
 * 1901-2099 is used.
 *
 * Ref:
 *
 * A clever computer algorithm for converting calendar dates to Julian
 * days was developed using FORTRAN integer arithmetic (H. F. Fliegel'
 * and T. C. Van Flandern, "A Machine Algorithm for Processing'
 * Calendar Dates," Communications of the ACM 11 [1968]: 657). 
 * In FORTRAN integer arithmetic, multiplication and division are
 * performed left to right in the order of occurrence, and the
 * absolute value of each result is truncated to the next lower
 * integer value after each operation, so that both 2/12 and -2/12
 * become 0. If I is the year, J the numeric order value of the month,
 * and K the day of the month, then the algorithm is: [WRAPPED]
 * [JD = K - 32075 + 1461 * (I + 4800 + (J -14)/12)/4 + 367 *
 * (J-2-(J-14)/12*12)/12 - 3 * ((I + 4900 + (J-14)/12)/100)/4. ]
 *
 * Some reference date conversion results for checking:
 *   http://www1.jsc.nasa.gov/er/seh/math16.html
 *   http://ecsinfo.gsfc.nasa.gov/sec2/papers/noerdlinger2.html
 *   http://aa.usno.navy.mil/data/docs/JulianDate.html
 */
int dateset_greg(struct calendar* cal, int year, int month, int day,
    int hour, int minute, int second, long int nsecond)
{
    double x;

    cal->year = year;
    cal->mon = month;
    cal->mday = day;
    cal->hour = hour;
    cal->min = minute;
    cal->sec = second;
    cal->nsec = nsecond;

    x = cal->mday - 32075 + 1461 * (cal->year + 4800 + (cal->mon - 14) / 12) / 4
        + 367 * (cal->mon - 2 - (cal->mon - 14) / 12 * 12) / 12
        - 3 * ((cal->year + 4900 + (cal->mon - 14) / 12) / 100) / 4
        - 0.5;

    x = x + cal->hour / 24.0 + cal->min / 60.0 / 24.0
        + cal->sec / 3600.0 / 24.0 + cal->nsec * 1.e-9 / 3600.0 / 24.0;

    cal->jdate = x;

    return 0;
}

void datecpy(struct calendar* dest, const struct calendar* src)
{
    dest->year = src->year;
    dest->mon = src->mon;
    dest->mday = src->mday;
    dest->hour = src->hour;
    dest->min = src->min;
    dest->sec = src->sec;
    dest->nsec = src->nsec;
    dest->jdate = src->jdate;
}

} // namespace zpz
#endif // _zpz_utilities_date_h_
