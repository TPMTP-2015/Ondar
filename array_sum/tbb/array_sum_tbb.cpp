#include "tbb/tbb.h"
#include "tbb/task_scheduler_init.h"
#include <iostream>

const long SIZE = 1000000;

class SummingArray {
  double *array;
public:
  double sum;
  void operator() (const tbb::blocked_range<size_t>& r ) {
    // using local variables to improve performance
    double *a = array;
    double s = sum;
    size_t end = r.end();

    for(size_t i = r.begin(); i != end; ++i) {
      s += a[i];
    }
    sum = s;
  }

  // splitting cs
  SummingArray( SummingArray& x, tbb::split ) : array(x.array), sum(0) {}
  
  // when the subtask completes, the following method is used to get result of the subtask
  void join( const SummingArray& y) { 
    sum += y.sum;
  }

  // cs
  SummingArray (double *a ) : array(a), sum(0) { }
};

void parallelSummingArray( double *array, size_t n ) {
  int def_num_threads = tbb::task_scheduler_init::default_num_threads();
  int num_threads;

  std::cout << "Default number of threads is " << def_num_threads << std::endl;
  std::cout << "You may use another amount of threads: ";
  std::cin >> num_threads;

  // by default, intel tbb 2.2 automatically creates a task scheduler at first time
  // but we use an instance of this class to control the number of threads used by the task scheduler
  // construct task scheduler with num_threads threads
  tbb::task_scheduler_init init(num_threads); 
  SummingArray sa(array);

  // timing begin
  tbb::tick_count tbegin = tbb::tick_count::now();

  // parallel algorithm
  tbb::parallel_reduce(tbb::blocked_range<size_t>(0, SIZE), sa );

  // timing end
  tbb::tick_count tend = tbb::tick_count::now();

  // print results
  double exec_time = (tend - tbegin).seconds();
  std::cout << "Execution time = " << exec_time << " sec, using " << num_threads << "thread(s)" << std::endl;
  std::cout << "Sum = " << sa.sum << std::endl;
}

int main() {
  double *array = new double[SIZE];

  for(size_t i = 0; i < SIZE; i++) {
    array[i] = 0.1;
  }

  parallelSummingArray(array, SIZE);
  delete[] array;
  return 0;
}
