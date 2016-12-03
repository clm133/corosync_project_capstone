#include "time_manager.h"

long get_microtime(long *time_ptr)
{
	struct timeval currentTime;
	
	gettimeofday(&currentTime, NULL);
	if(time_ptr != NULL){
		*time_ptr = currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
		return *time_ptr;
	}
	else{
		return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
	}
}

long delta_microtime(long start_time, long end_time)
{
	return (end_time - start_time);
}

float micro_to_milli(long microseconds)
{
	return microseconds * 1000;
}