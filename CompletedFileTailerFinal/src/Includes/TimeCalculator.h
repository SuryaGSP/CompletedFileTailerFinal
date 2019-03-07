#pragma once
#include <fstream>
#include <chrono>
class Timer
{
  std::ofstream fileOut;
  std::chrono::system_clock::time_point start, end;
  double milli;
public:
  Timer();
  void TimerStart();
  void TimerStop();
};
