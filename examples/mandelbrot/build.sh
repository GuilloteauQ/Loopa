gcc -o mandelbrot mandelbrot.c `pkg-config --libs gsl lua` -O3 -Wall -Wextra
