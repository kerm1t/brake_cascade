// 
// pointcloud viewer / processor for ARM
//

#include <iostream>
#define GLEW_STATIC                     // damit GLEW statisch gelinkt werden kann
#include <GL/glew.h>
//#define SDL_MAIN_HANDLED                // eigene Hauptfunktion für SDL                                                                                                                                                                         #ifdef _WIN32                           // nur für Windows                                                              #include 

#include "imgui.h"
#ifdef _WIN32               // Win
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#else
#include "src/imgui_impl_sdl2.h"
#include "src/imgui_impl_opengl3.h"
#endif

#ifdef _WIN32               // Win
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
#include <SDL2/SDL.h>       // Linux / MacOS  
#endif

#include "pointcloud.hpp" // <-- need to be fixed, uncomment this and gpu_primitives will not work anymore ...1/5/2024

// simulation
#include <chrono>
#include <SDL_thread.h>
#include "sim.hpp"

#include "draw.hpp"
#include "draw_imgui.hpp"

#include "init.hpp"
#include "user.hpp"


// eCAL
#ifdef eCAL_ON
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>
#include <ecal/msg/protobuf/subscriber.h>
// proto i/o
#include "pcl.pb.h"
#include "lane.pb.h"
#endif // eCAL_ON


// LLoFT
#include "pointcloud/pointcloud.hpp"
#include "pointcloud/pointcloud_synth.hpp"
#include "pointcloud/pointcloud_io.hpp"

#include "demo_manager.hpp"


// lloft objects initialized with an instance of pointcloud
lloft::pointcloud p;
lloft::pointcloud_synth psynth(p);
lloft::pointcloud_io pio(p);

demo_manager dman;


#ifdef eCAL_ON
void Pointcloud_Callback(const pcl::PointCloud2 pointcloud_msg)
{
  pio.ecal_callback(pointcloud_msg); // 2do: directly give this to AddReceiveCallback
//  gpu_push_buffers(p.numpoints);
}
#endif // eCAL_ON


int main(int argc, char** argv)
{
  SDL_Window* window = init_SDL();

  SDL_GLContext glContext = SDL_GL_CreateContext(window);

  // GLEW initialisieren (Context muss bereits existieren)
  init_GlEW();

  gpu_create_shaders(); // openGL: init GPU structures
  gpu_create_buffers(); // openGL: init GPU structures
  gpu_create_variables(); // openGL: init GPU structures

// (a) cube
  gpu_cube();

  // (b) point cloud
#ifdef __linux__
  std::string s[2] = { "./data/veloout_0_10.pcd","./data/hrlframe_0_back.pcd" };
#endif
#ifdef WIN32
  std::string s[2] = { "../data/veloout_0_10.pcd","../data/hrlframe_0_back.pcd" };
#endif


#ifdef eCAL_ON
  // Initialize eCAL
  eCAL::Initialize(argc, argv, "Lane Prototype (eCal)");
  // Create a protobuf subscriber
  // possible topics:
  // - AEyeSensorPointCloudData
  // - gpf_non_ground
  // - meta_pcl
  eCAL::protobuf::CSubscriber<pcl::PointCloud2> subscriber_hrl("AEyeSensorPointCloudData");
  // Set the Callback
  subscriber_hrl.AddReceiveCallback(std::bind(&Pointcloud_Callback, std::placeholders::_2));
  // create publisher
  eCAL::protobuf::CPublisher<lane::Lane_out> publisher("Lane_out");
  // set eCAL state to healthy (--> eCAL Monitor)
  eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "LANE eCAL publishers initialized");
#endif // eCAL_ON


  int i_toggle = 1;
  std::cout << "main.cpp: loading " << s[0] << std::endl;
//  int numpoints = pointcloud_load(s[0]);
//  gpu_push_buffers(numpoints);

  dman.create_scene();
  dman.load_objs();

  init_GL();

  // Setup Dear ImGui context
  ImGuiIO& io = init_Imgui(window, glContext);

  SDL_Thread *tsim;
  tsim = SDL_CreateThread(thread1, "sim", NULL);

  // Game Loop
  bool close = false;
  do
  {
    // 2do: split up into
        // (a) sim step             --> done 1/4/2024 --> thread
        // (b) collision detection
        // (c) draw
    render(window, p.numpoints);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    draw_imgui(io);

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Double Buffering (angezeigter Buffer tauschen)
    SDL_GL_SwapWindow(window);

    // user interaction (mouse, keys, ...)
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      
      process_event(event); // mouse wheel, drag & drop, ..., quit

      if (event.type == SDL_QUIT)
      {
        close = true;
      }
    }
  } while (!close);

  gpu_free_buffers(); // free point cloud vbo

  dman.free_objs();
  dman.free_scene();

  SDL_Quit();

  return 0;
}
