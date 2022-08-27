#pragma once

#include "defines.h"

struct clock {
  f64 start_time;
  f64 elapsed;
};

void clock_update(clock *clock);
void clock_start(clock *clock);
void clock_stop(clock *clock);
