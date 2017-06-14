// Copyright 2017, Institute for Artificial Intelligence - University of Bremen


#pragma once

#include "URoboVision.h"
#include <chrono>
/**
 *
 */
class UROBOVISION_API StopTime
{
protected:
  const std::chrono::high_resolution_clock::time_point StartTime;

public:
  StopTime() : StartTime(std::chrono::high_resolution_clock::now())
  {
  }

  virtual ~StopTime()
  {
  }

  inline double GetTimePassed() const
  {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - StartTime).count() * 1000.0;
  }
};

class UROBOVISION_API ScopeTime : private StopTime
{
private:
  FString Function;
  FString Message;
  const int Line;

public:
  inline ScopeTime(const FString &_Function, const int _Line, const FString &_Message) :
    StopTime(), Function(_Function), Message(_Message), Line(_Line)
  {
  }

  virtual inline ~ScopeTime()
  {
    OUT_DEBUG(TEXT("%s: %f ms."), *Message, GetTimePassed());
  }
};

#ifndef MEASURE_TIME
#define MEASURE_TIME(MSG) ScopeTime scopeTime(FString(__FUNCTION__), __LINE__, FString(MSG))
#endif
