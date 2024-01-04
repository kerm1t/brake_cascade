// 
// pointcloud viewer / processor for ARM
//

#include <iostream> 
#define GLEW_STATIC                     // damit GLEW statisch gelinkt werden kann
#include <GL/glew.h>
//#define SDL_MAIN_HANDLED                // eigene Hauptfunktion für SDL                                                                                                                                                                         #ifdef _WIN32                           // nur für Windows                                                              #include 

#include "imgui.h"
#ifdef _WIN32               // nur fuer Windows
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#else
#include "src/imgui_impl_sdl2.h"
#include "src/imgui_impl_opengl3.h"
#endif

#ifdef _WIN32               // nur fuer Windows
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
#include <SDL2/SDL.h>                   // Linux / MacOS  
#endif

#include "pointcloud.hpp"

#include "draw.hpp"

#include "init.hpp"
#include "user.hpp"

#include <chrono>

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

  // Our state
  bool show_demo_window = false;
  bool show_another_window = false;
// --> draw.hpp  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Game Loop
  bool close = false;
  do
  {
    // 2do: split up into
        // (a) sim step
        // (b) collision detection
        // (c) draw
    render(window, p.numpoints);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

      ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
      ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);
      ImGui::Text("moon spin = %f", moon_spin);
      ImGui::Text("month = %f [s]", month/1000000); // ms --> s

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
      ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

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
