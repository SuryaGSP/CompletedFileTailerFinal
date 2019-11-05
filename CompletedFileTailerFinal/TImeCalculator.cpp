#include <chrono>
#include <iostream>
#include "TimeCalculator.h"

Timer::Timer()
{
  fileOut.open("D:\\TestDir2\\TimeCalculator.txt",std::ios_base::app);
  milli = 1000000;
}

void Timer::TimerStart()
{
  start = std::chrono::system_clock::now();
}

void Timer::TimerStop()
{
  end = std::chrono::system_clock::now();
  fileOut << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)).count() / milli << " ";
}
