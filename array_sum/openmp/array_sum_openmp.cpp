/*
 * This program sums up array's elements using OpenMP
 */

#include <omp.h>
#include <iostream>

const int SIZE = 10000000;

double parallelSummingArray(double *array, size_t size, int num_threads) {
  double result = 0;
  double start_time, end_time;

  omp_set_num_threads(num_threads);

  start_time = omp_get_wtime();
  // parallel region
  #pragma omp parallel
  {
    int thread_id = omp_get_thread_num();
    // local sum
    double s = 0;
    double *a;
    size_t offset = (size / num_threads) * thread_id;
    size_t subarray_size;

    if(thread_id == num_threads - 1) {
      // this means that the thread with last ID
      // will work with the last part of the array
      subarray_size = size - offset;
    } else {
      subarray_size = size / num_threads;
    }

    a = array + offset;
    for(size_t i = 0; i < subarray_size; i++)
      s += a[i];

    // atomic protects the read/update of shared var 'result'
    #pragma omp atomic
    result += s;
  }
  end_time = omp_get_wtime();
  std::cout << "Work took " << end_time - start_time << "sec" << std::endl;
  return result;
}

int main() {
  double *array = new double[SIZE]; 
  int num_threads;

  for(size_t i = 0; i < SIZE; i++) {
    array[i] = 0.001;
  }
  std::cout << "Number of threads: ";
  std::cin >> num_threads;
  std::cout << "Sum = " << parallelSummingArray(array, SIZE, num_threads) << std::endl;

  delete[] array;
  return 0;
}
