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

long get_millitime(long *time_ptr)
{
	/* sadly this is broked, just calls get microtime in the meanwhile
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	if(time_ptr != NULL){
		*time_ptr = currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
		return *time_ptr;
	}
	else{
		return currentTime.tv_sec * (int)1e3 + (currentTime.tv_usec/1000);
	} */
	return get_microtime(time_ptr);
}

long delta_time(long start_time, long end_time)
{
	return (end_time - start_time);
}

void print_time()
{
	time_t mytime;
	mytime = time(NULL);
	printf(ctime(&mytime));
}