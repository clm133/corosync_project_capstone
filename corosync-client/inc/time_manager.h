#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <stdlib.h>
#include <sys/time.h>

/**
 * Returns the current time in microseconds. (sets ptr also)
 */
long get_microtime(long *time_ptr);

long delta_microtime(long start_time, long end_time);

float micro_to_milli(long microseconds);

#endif