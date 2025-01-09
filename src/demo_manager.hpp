#ifndef DEMO_MANAGER_H
#define DEMO_MANAGER_H

#include "gpu_primitives.hpp"
#include "gpu_prim_GL.hpp"

class demo_manager {
private:
public:
  fastObjMesh* mesh;
  int create_scene() {
    // (c) grid
    grid_create(-380, 230, 25);
    grid_gpu_push_buffers();

    gpuprim_GL::tmp_obj = new gpuprim_GL::gpu_obj(vec3f(1.0f, 0.5f, .5f),vec3f(0,0,0),program);

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

    delete(gpuprim_GL::tmp_obj);
    
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

  void create(std::string s_obj) {
    create_scene();
    load_objs();

    mesh_gpu_create();
    // https://github.com/thisistherk/fast_obj
    mesh = fast_obj_read(s_obj.c_str());
    //  ...do stuff with mesh...
    mesh_gpu_push_buffers_1(mesh);
    // https://aras-p.info/blog/2022/05/14/comparing-obj-parse-libraries/

    ///  two_tris.create_buffers(0.0,0.0,1.0);
    ///  two_tris_i.create_buffers_from_faces();

    lane_normal.create_buffers(0.5f, 0.5f, .5f);
    lane_wet.create_buffers(0.0f, 0.0f, 1.0f);
    lane_icy.create_buffers(1.0f, 0.9f, 1.0f);
  }

  void free() {
    
    fast_obj_destroy(mesh);

    free_objs();
    free_scene();
  }
};

#endif
