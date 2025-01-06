#ifndef SIM_HPP
#define SIM_HPP

#include <chrono> // precision needed?
std::chrono::time_point<std::chrono::high_resolution_clock> stop;
float month;
std::chrono::time_point<std::chrono::high_resolution_clock> stop_earth;
float year;

float earth_spin = 0.0f;
float moon_spin = 0.0f;
#define PI 3.14159f // find a common place for math defines? vs. single header

#include "cascade_jerk3.hpp"
#include "yocto_math.h"

namespace sim {
  brake_cascade brk_normal;
  brake_cascade brk_wet;
  brake_cascade brk_icy;
  // hmm, would be nice to have a class e.g. "obj_phys" with different simulations attached :-)

  float d_truck = 0.0f;
  float d_truck_wet = 0.0f;
  float d_truck_icy = 0.0f;

  int i_sim;
  bool sim_start = false;
  yocto::vec3f startpos = { -350+100,0,0 }; // we use only x atm

  // https://blog.iceslicer.com/understanding-friction-testing
  void init() {
    brk_normal.init(1.0f); // winter: 0.8
    brk_wet.init(.6f);
    brk_icy.init(.2f);

    d_truck = startpos.x;
    d_truck_wet = startpos.x;
    d_truck_icy = startpos.x;

    sim::i_sim = 0;

  }

  int thread1(void* p)
  {
    while (1)
    {
      //    std::cout << "thread" << std::endl;
      //    SDL_Delay(1); // now run at 1KHz
      SDL_Delay(5); // now run at 200Hz

      if (sim_start)
      {
        earth_spin += 0.005f;
        if (earth_spin >= 2 * PI) earth_spin = 0.0f;
        // measure 1 moon spin
        if (earth_spin < 0.005f)
        {
          auto start = stop_earth;
          stop_earth = std::chrono::high_resolution_clock::now();
          auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_earth - start);
          year = (float)duration.count();
        }

        moon_spin += 0.01f; // RAD
        if (moon_spin >= 2 * PI) moon_spin = 0.0f;
        // measure 1 moon spin
        if (moon_spin < 0.01f)
        {
          auto start = stop;
          stop = std::chrono::high_resolution_clock::now();
          auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
          month = (float)duration.count();
        }

        // (a) linear
    //    d_truck += 0.02f;
    //    if (d_truck > 10.0f) d_truck = 0.0f;

        // (b) brake cascade
  ///      d_truck = -100+sim::brake(i_sim++);
        d_truck     = startpos.x + brk_normal.brake(i_sim++);
        d_truck_wet = startpos.x + brk_wet.brake(i_sim++);
        d_truck_icy = startpos.x + brk_icy.brake(i_sim++);
      }

      // ... return will leave / kill the thread
    }
    return 0;
  }
} // namespace sim
#endif // SIM_HPP
