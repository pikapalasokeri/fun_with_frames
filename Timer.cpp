#include "Timer.hpp"
#include <iostream>
#include <chrono>

Timer::Timer(const std::string& name)
  : m_name(name),
    m_count(0)
{}

Timer::~Timer()
{
  report();
}

void Timer::start()
{
  m_startTime = std::chrono::steady_clock::now();
}

void Timer::stop()
{
  const std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
  m_duration += std::chrono::duration_cast<std::chrono::microseconds>(now - m_startTime);
  ++m_count;
}

void Timer::report() const
{
  std::cout << m_name << std::endl
            << "  " << m_duration.count() << " total us" << std::endl
            << "  " << m_count << " counts" << std::endl
            << "  " << (m_duration.count() / m_count) << " avg us/count" << std::endl;
}
