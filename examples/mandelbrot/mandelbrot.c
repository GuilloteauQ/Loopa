#include <stdio.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define LOOPA_IMPLEMENTATION
#define LOOPA_ACTUATOR_TYPE uint64_t
#define LOOPA_SENSOR_TYPE double
#include "../../loopa.h"

bool converge(gsl_complex c, double eps, uint64_t iterations) {
    gsl_complex z = gsl_complex_rect(0.0, 0.0);
    uint64_t i = 0;
    while (i < iterations && gsl_complex_abs(z) < eps) {
       z = gsl_complex_add(gsl_complex_mul(z, z), c); 
       i++;
    }
    return (i == iterations);
}

void mandelbrot(loopa_State* L) {
  gsl_complex c;
  double min_x = -2;
  double max_x = 0.5;
  double min_y = -1;
  double max_y = 1;
  size_t steps_x = 2 << 5;
  size_t steps_y = 2 << 6;
  clock_t start, end;
  double cpu_time_used;
  uint64_t iterations = 100;

  for (size_t zoom = 0; zoom < 100; zoom++) {
    start = clock();
    for (size_t y = 0; y < steps_y; y++) {
      for (size_t x = 0; x < steps_x; x++) {
        double x_scaled = min_x + x * (max_x - min_x) / steps_x;
        double y_scaled = min_y + y * (max_y - min_y) / steps_y;
        c = gsl_complex_rect(x_scaled, y_scaled);
        converge(c, 10000000, iterations);
      }
    }
    min_x *= (1.0 - 0.001);
    min_x *= (1.0 - 0.001);
    max_y *= (1.0 - 0.001);
    max_y *= (1.0 - 0.001);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    loopa_send_sensor(L, cpu_time_used);
    iterations = loopa_recv_actuator(L);
    // printf("used: %f, iterations: %ld\n", cpu_time_used, iterations);
  }
}

int main(void) {
  loopa_State* L = loopa_init("controller.lua");
  mandelbrot(L);
  loopa_close(L);
  return 0;
}
