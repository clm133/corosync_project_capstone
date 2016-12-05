#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/**
 * Returns the current time in microseconds. (sets ptr also)
 */
long get_microtime(long *time_ptr);

long delta_time(long start_time, long end_time);

void print_time();

#endif