#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <string>
#include <cassert>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

// OS X does not define clock_gettime, use clock_get_time instead
// https://gist.github.com/1087739
// http://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
int current_utc_time(struct timespec *ts_) {
#ifdef __MACH__
  clock_serv_t cclock;
  mach_timespec_t mts_;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts_);
  mach_port_deallocate(mach_task_self(), cclock);
  ts_->tv_sec = mts_.tv_sec;
  ts_->tv_nsec = mts_.tv_nsec;
  return 0;
#else
  return clock_gettime(CLOCK_REALTIME, ts_);
#endif
}

/*
 * A stopwatch that counts in milliseconds.
 *
 *     Timer t("Kernel");
 *     for (...) {
 *       t.start();
 *       //kernel
 *       t.stop_and_add_to_total();
 *     }
 *     time_in_ms = t.get_total_time();
 *
 */
class Timer {
  private:
    struct timespec ts_start, ts_end;
    double total_time;
    std::string name;

  public:
    Timer() : total_time(0.0f) {}
    Timer(std::string name) : total_time(0.0f), name(name) {}
    ~Timer() {}

    inline void start() {
      int err = current_utc_time(&ts_start);
      assert(err == 0);
      assert(ts_start.tv_nsec > 0);
      assert(ts_start.tv_nsec < 1000000000);
    };
    inline void stop()  {
      int err = current_utc_time(&ts_end);
      assert(err == 0);
      assert(ts_end.tv_nsec > 0);
      assert(ts_end.tv_nsec < 1000000000);
    };
    inline double add_to_total() {
      double delta =
        (double)(ts_end.tv_sec  - ts_start.tv_sec )*1.0e3 +
        (double)(ts_end.tv_nsec - ts_start.tv_nsec)/1.0e6;
      total_time += delta;
      return delta;
    };
    inline double stop_and_add_to_total() {
      stop();
      return add_to_total();
    };
    inline double get_total_time() { return total_time; };
    inline std::string get_name() { return name; };
    inline void reset() { total_time = 0.0; };
    inline void set_total_time(double t) { total_time = t; };
};

#endif
