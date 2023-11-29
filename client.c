#include "mytest.h"

int client_oriented_io() {

#ifdef TIMES
        struct timeval stime,etime;
        int time_result;
#endif
	/* Client_oriented_io. Measure io time, communication time, and time for the rest.
        */

#ifdef TIMES
        gettimeofday(&stime, NULL);
#endif
           

	printf("**Program IO, communication and the rest\n");
	printf("**Program IO\n");
	printf("**Program communication\n");

#ifdef TIMES
        gettimeofday(&etime, NULL);
        time_result = etime.tv_usec - stime.tv_usec;
        printf("Client_oriented_io TIMES == %ld %ld %ld\n", etime.tv_usec, stime.tv_usec, time_result);
#endif

}

