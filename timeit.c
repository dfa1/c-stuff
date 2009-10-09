/*
 * timeit.c -- measure execution time of functions
 * Copyright (C) 2006, Davide Angelocola <davide.angelocola@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include <time.h>              
#include <sys/time.h>          

typedef struct _timer timer;

struct _timer {
    struct timeval t_start;
    struct timeval t_end;
    double t_total;
};

/* Microseconds per second. */
#define USEC_PER_SEC   1000000

#define TIMER_INIT(timer) \
    do {						\
        memset(&(timer), 0, sizeof(struct _timer));     \
    } while(0)


#define TIMER_START(timer) \
    do {					\
        gettimeofday(&(timer).t_start, NULL);   \
    } while (0)

#define TIMER_STOP(timer) \
    do {                                                                \
        struct timeval elapsed;                                         \
        gettimeofday(&(timer).t_end, NULL);                             \
        if ((timer).t_start.tv_usec > (timer).t_end.tv_usec) {          \
            (timer).t_end.tv_usec += USEC_PER_SEC;                      \
            (timer).t_end.tv_sec--;                                     \
        }                                                               \
        elapsed.tv_usec = (timer).t_end.tv_usec - (timer).t_start.tv_usec; \
        elapsed.tv_sec = (timer).t_end.tv_sec - (timer).t_start.tv_sec; \
        (timer).t_total = elapsed.tv_sec + ((double) elapsed.tv_usec / USEC_PER_SEC); \
    } while(0)

double 
timeit(void (*fptr)(void), int times)
{
    timer timer;
    int i;
    
    TIMER_INIT(timer);
    TIMER_START(timer);
    
    for (i = 0; i < times; i++) {
	fptr();
    }

    TIMER_STOP(timer);
    return timer.t_total;
}

int x = 1000;

void 
test_pow(void)
{
    int digits = log(x) - 1;
}

void
test_snprint_strlen(void)
{
    char buf[15];
    int digits;
    snprintf(buf, 15, "%d", x);
    digits = strlen(buf);
}

int 
main(int argc, char **argv)
{
#define TIMES 1000000
    printf("pow: %g\n", timeit(test_pow,TIMES));
    printf("snprintf: %g\n", timeit(test_snprint_strlen,TIMES));
    return 0;
}
