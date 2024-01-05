#pragma once

#include <chrono> // precision needed?
std::chrono::time_point<std::chrono::high_resolution_clock> stop;
float month;
std::chrono::time_point<std::chrono::high_resolution_clock> stop_earth;
float year;

float earth_spin = 0.0f;
float moon_spin = 0.0f;
#define PI 3.14159f // find a common place for math defines? vs. single header

int thread1(void *p)
{
  while (1)
  {
//    SDL_Delay(1); // now run at 1KHz
    SDL_Delay(5); // now run at 200Hz

    earth_spin += 0.005f;
    if (earth_spin >= 2 * PI) earth_spin = 0.0f;
    // measure 1 moon spin
    if (earth_spin < 0.005f)
    {
      auto start = stop_earth;
      stop_earth = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_earth - start);
      year = duration.count();
    }

    moon_spin += 0.01f; // RAD
    if (moon_spin >= 2 * PI) moon_spin = 0.0f;
    // measure 1 moon spin
    if (moon_spin < 0.01f)
    {
      auto start = stop;
      stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      month = duration.count();
    }
  }
  return 0;
}