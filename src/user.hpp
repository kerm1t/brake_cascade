#ifndef USER_HPP
#define USER_HPP


int mx, my;

void process_event(SDL_Event& event) {
  if (event.type == SDL_MOUSEWHEEL)
  {
    if (event.wheel.y > 0) // scroll up
    {
      // Put code for handling "scroll up" here!
      zoom += 1.0f;
    }
    else if (event.wheel.y < 0) // scroll down
    {
      // Put code for handling "scroll down" here!
      zoom -= 1.0f;
    }
  }
  // https://stackoverflow.com/questions/27536941/sdl-mouse-input
  if (event.type == SDL_MOUSEMOTION &&
    event.button.button == SDL_BUTTON_LEFT)
  {
    cam.rot[0] -= (my - event.motion.y) * 3.14159f / 180.0f;
    cam.rot[1] += (mx - event.motion.x) * 3.14159f / 180.0f;
  }
  mx = event.motion.x;
  my = event.motion.y;

  if (event.type == SDL_MOUSEMOTION &&
    event.button.button == SDL_BUTTON_RIGHT)
  {
    cam.trans[1] += event.motion.x;
    cam.trans[0] += event.motion.y;
  }
  // https://www.libsdl.org/release/SDL-1.2.15/docs/html/guideinputkeyboard.html
  if (event.type == SDL_KEYUP)
  {
    if (event.key.keysym.scancode == SDL_SCANCODE_1) // color coding = 0
    {
      colorcoding = 0;
      enable_colorbuffer(colorcoding);
    }
    if (event.key.keysym.scancode == SDL_SCANCODE_2) // color coding = 1
    {
      colorcoding = 1;
      enable_colorbuffer(colorcoding);
    }
    if (event.key.keysym.scancode == SDL_SCANCODE_3) // color coding = 2
    {
      colorcoding = 2;
      enable_colorbuffer(colorcoding);
    }
    if (event.key.keysym.scancode == SDL_SCANCODE_P) // change pointcloud from HFL <--> Velo
    {
      // https://stackoverflow.com/questions/33607194/opengl-updating-vertices-array-buffer
      lloft::vertices.clear();
      lloft::colors[0].clear();
      lloft::colors[1].clear();
      lloft::colors[2].clear();
      //         numpoints = pointcloud_load(s[i_toggle]);
      //         i_toggle = 1 - i_toggle;
      //         gpu_push_buffers(numpoints);
    }
  }

  if (event.type == SDL_DROPFILE) {      // In case if dropped file
    char* dropped_filedir = event.drop.file;
    // Shows directory of dropped file
/*        SDL_ShowSimpleMessageBox(
          SDL_MESSAGEBOX_INFORMATION,
          "File dropped on window",
          dropped_filedir,
          window
        );
        */
    std::cout << "main.cpp: loading " << dropped_filedir << std::endl;
    ///       int numpoints = pointcloud_load(dropped_filedir);
    ///       if (numpoints > 0) gpu_push_buffers(numpoints);

    SDL_free(dropped_filedir);    // Free dropped_filedir memory
///    break;
  }

  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      resize(event.window.data1, event.window.data2);
    }
  }

}

#endif // INIT_HPP
