#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>


#define LOOPA_IMPLEMENTATION
#define LOOPA_ACTUATOR_TYPE int
#define LOOPA_SENSOR_TYPE double
#include "../../loopa.h"

#define MAX_THREADS 100

#define ITERATIONS 500000
#define ITERS 500

LOOPA_ACTUATOR_TYPE actuator = 2;
pthread_mutex_t mutex_actuator = PTHREAD_MUTEX_INITIALIZER;


typedef struct {
  size_t iter_to_do;
  uint64_t count;
} data_t;


void* f(void* info) {
  data_t* data = (data_t*) info;
  uint64_t count = 0;
  for (size_t j = 0; j < data->iter_to_do; j ++) {
      float rand_x = (float) rand() / (float) RAND_MAX;
      float rand_y = (float) rand() / (float) RAND_MAX;
      count += (rand_x * rand_x + rand_y * rand_y < 1) ? 1 : 0;
  }
  data->count = count;
  return NULL;
}


void montecarlo(void) {
  pthread_t threads[MAX_THREADS];
  data_t datas[MAX_THREADS];
  srand(0);
  uint64_t count = 0;
  int nb_threads = 10;
  uint64_t total_count = 0;
  for (size_t i = 0; i < ITERATIONS; i++) {
    pthread_mutex_lock(&mutex_actuator);
    nb_threads = actuator;
    pthread_mutex_unlock(&mutex_actuator);

    for (int t = 0; t < nb_threads; t++) {
      datas[t] = (data_t){ .iter_to_do = ITERS, .count = 0};
      pthread_create(&threads[t], NULL, f, &datas[t]);
    }

    for (int t = 0; t < nb_threads; t++) {
      pthread_join(threads[t], NULL);
      count += datas[t].count;
    }
    total_count += (ITERS) * nb_threads;
  }
  float pi_approx = (float) (count) / (float) total_count;
  printf("PI ~ %f\n", 4 * pi_approx);
}

void* controller_thread() {
  loopa_State* L = loopa_init("controller.lua");

  LOOPA_ACTUATOR_TYPE loopa_actuator;
  double loadavgs[3];

  while (1) {
    getloadavg(loadavgs, 3);

    loopa_send_sensor(L, loadavgs[0]);
    loopa_actuator = loopa_recv_actuator(L);
    
    pthread_mutex_lock(&mutex_actuator);
    actuator = loopa_actuator;
    pthread_mutex_unlock(&mutex_actuator);

    sleep(5);
  }

  loopa_close(L);
  return NULL;
}

int main(void) {
  pthread_t tid;
  pthread_create(&tid, NULL, &controller_thread, NULL);
  montecarlo();
  pthread_cancel(tid);
  return 0;
}
