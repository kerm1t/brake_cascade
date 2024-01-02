#ifndef DEMO_MANAGER_H
#define DEMO_MANAGER_H

#include "gpu_primitives.hpp"

class demo_manager {
private:
public:
  int create_scene() {
    // (c) grid
    grid_create();
    grid_gpu_push_buffers();

    // (d) oop-grid ;-)
    grid1 = new grid();
    grid1->create_buffers();
    grid1->gpu_push_buffers();
    return 0;
  }

  void free_scene() {
    grid_free();
    gpu_cube_free();
    grid1->gpu_free();
    delete(grid1);
  }

  int load_objs() {
    // load from yaml
    
    // (e) sphere
    sph1 = new sphere();
    sph1->r = 0.0f; sph1->g = sph1->b = 1.0f;
    sph1->create_buffers();
    sph1->gpu_push_buffers();

    moon = new sphere();
    moon->radius = 0.6f;
    moon->r = moon->g = moon->b = 1.0f;
    moon->create_buffers();
    moon->gpu_push_buffers();
    /*
      sun = new sphere();
      sun->radius = 6.0f;
      sun->r = 255.0f/255.0f; sun->g = 165.0f / 255.0f; sun->b = 0.0f;
      sun->create_buffers();
      sun->gpu_push_buffers();
    */
    htor = new cylinder();
    htor->r = htor->g = htor->b = 1.0f;
    htor->create_buffers();
    htor->gpu_push_buffers();
    return 0;
  }

  void free_objs() {
    sph1->gpu_free();
    delete(sph1);
    moon->gpu_free();
    delete(moon);
    if (sun) {
      sun->gpu_free();
      delete(sun);
    }
    htor->gpu_free();
  }
};

#endif
