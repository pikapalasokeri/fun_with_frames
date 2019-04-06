#ifndef _TIMER_HPP_
#define  _TIMER_HPP_

#include <string>
#include <chrono>

class Timer
{
public:
  Timer(const std::string& name);
  ~Timer();
  void start();
  void stop();
private:
  void report() const;
  const std::string m_name;
  int m_count;
  std::chrono::time_point<std::chrono::steady_clock> m_startTime;
  std::chrono::microseconds m_duration;
};

#endif
