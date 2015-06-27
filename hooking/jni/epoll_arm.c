/*
 *  Collin's Binary Instrumentation Tool/Framework for Android
 *  Collin Mulliner <collin[at]mulliner.org>
 *  http://www.mulliner.org/android/
 *
 *  (c) 2012,2013
 *
 *  License: LGPL v2.1
 *
 */

#include <sys/types.h>
#include <stdio.h>

#undef log

#define log(...) \
        {FILE *fp = fopen("/data/local/tmp/arm_example.log", "a+"); if (fp) {\
        fprintf(fp, __VA_ARGS__);\
        fclose(fp);}}

extern int my_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

int my_epoll_wait_arm(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    log("cippa dentro epoll arm \n");
	return my_epoll_wait(epfd, events, maxevents, timeout);
}
