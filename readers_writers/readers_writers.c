
/*
 * This is a solution to the "readers-writers" problem
 * It uses 2 mutexes (wr_mutex, r_mutex):
 *   a) wr_mutex allows to change shared resource (var x) by single writer-thread
 *   b) r_mutex is used to change readers_count var, which shows count of reader-threads at this moment
 *
 * Ondar A., 276 gr
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define READERS_NUMBER 10
#define WRITERS_NUMBER 10 
#define READ_ITERATION_NUMBER 100000
#define WRITE_ITERATION_NUMBER 1000

long x = 0;
long readers_count = 0;
pthread_mutex_t wr_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r_mutex = PTHREAD_MUTEX_INITIALIZER;

#define PRINT_TIME(end, begin)                                                                              \
  do {                                                                                                      \
    if((end).tv_usec >= (begin).tv_usec)                                                                    \
      printf("%ld.%06ld sec\n", (end).tv_sec - (begin).tv_sec, (end).tv_usec - (begin).tv_usec);            \
    else                                                                                                    \
      printf("%ld.%06ld sec\n", (end).tv_sec - (begin).tv_sec - 1, 1000000+(end).tv_usec - (begin).tv_usec);\
  } while (0)

static void *thread_reading(void *arg) {
  long i;
  long my_x;

  for(i = 0; i < READ_ITERATION_NUMBER; i++) {
    pthread_mutex_lock(&r_mutex);
    readers_count++;
    // if it's the first reader-thread, then it should lock wr_mutex
    // to prevent writing during reading
    if(readers_count == 1)
      pthread_mutex_lock(&wr_mutex);
    pthread_mutex_unlock(&r_mutex);
    // reading x
    my_x = x;
    pthread_mutex_lock(&r_mutex);
    readers_count--;
    // if it's the last reader-thread, then it shoud unlock wr_mutex
    // to allow writing
    if(readers_count == 0)
      pthread_mutex_unlock(&wr_mutex);
    pthread_mutex_unlock(&r_mutex);
  }

  return NULL;
}

static void *thread_writing(void *arg) {
  long i;

  for(i = 0; i < WRITE_ITERATION_NUMBER; i++) {
    pthread_mutex_lock(&wr_mutex);
    x++;
    pthread_mutex_unlock(&wr_mutex);
  }
  return NULL;
}

#define PRINT_ERROR(message) printf("ERROR: %s\n", message)

int main() {
  pthread_t readers[READERS_NUMBER];
  pthread_t writers[WRITERS_NUMBER];
  struct timeval begin, end;

  long i;

#define THREAD_CREATE(thid, th_function)                                                \
  do {                                                                                  \
    if(pthread_create(&(thid), (pthread_attr_t *)NULL, (th_function), NULL) != 0) {     \
      PRINT_ERROR("creating thread");                                                   \
      exit(-1);                                                                         \
    }                                                                                   \
  } while(0)

  gettimeofday(&begin, NULL);
  for(i = 0; i < READERS_NUMBER; i++)
    THREAD_CREATE(readers[i], thread_reading);
  for(i = 0; i < WRITERS_NUMBER; i++)
    THREAD_CREATE(writers[i], thread_writing);
#undef THREAD_CREATE

#define THREAD_JOIN(thid)                             \
  do {                                                \
    if(pthread_join(thid, (void **)NULL) != 0) {      \
      PRINT_ERROR("joining thread\n");                \
      exit(-1);                                       \
    }                                                 \
  } while(0)

  for(i = 0; i < READERS_NUMBER; i++)
    THREAD_JOIN(readers[i]);
  for(i = 0; i < WRITERS_NUMBER; i++)
    THREAD_JOIN(writers[i]);
#undef THREAD_JOIN
  gettimeofday(&end, NULL);
  PRINT_TIME(end, begin);
  if(pthread_mutex_destroy(&wr_mutex) != 0 && pthread_mutex_destroy(&r_mutex) != 0) {
    PRINT_ERROR("mutex destroy\n");
    exit(-1);
  }
  printf("x = %ld\n", x);
  return 0;
}

#undef PRINT_ERROR
#undef PRINT_TIME
