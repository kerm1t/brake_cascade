#pragma once

int thread1(void *p)
{
  while (1)
  {
    SDL_Delay(1);

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