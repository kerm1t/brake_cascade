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
#include "draw_primitives.hpp"

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

SDL_Window* init_SDL()
{
  // Fenster initialisieren
  SDL_Window* window;
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return NULL;
  }

  // OpenGL initialisieren
  // Frame Buffer definieren
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);		// 8 bit für rot
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);		// 8 bit für grün
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);		// 8 bit für blau
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);		// 8 bit für alpha
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);	// 32 bit für Pixel (optional)
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	// Double Buffering aktivieren

  // Fenster erzeugen
  window = SDL_CreateWindow("OpenGL Engine + eCAL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  return window;
}

void init_GlEW()
{
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cout << "glewInit Error: " << glewGetErrorString(err) << std::endl;
    return;
  }

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

void init_GL() {
  glEnable(GL_DEPTH_TEST);
  //  glDepthFunc(GL_NEVER);

    // Enable face culling (faster rendering)
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //  glEnable(GL_PROGRAM_POINT_SIZE); // weired effect on Raspi (dyn. point size with azimuth)
    // 2do: set background color
}

ImGuiIO& init_Imgui(SDL_Window* window, SDL_GLContext gl_context) {

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
//  const char* glsl_version = "#version 130"; // Hack!!, 130 is not avail on raspi
  const char* glsl_version = "#version 120"; // btw. what about 1.00 ES and 3.00 ES ?
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != nullptr);

  return io;
}

#ifdef eCAL_ON
void Pointcloud_Callback(const pcl::PointCloud2 pointcloud_msg)
{
  pio.ecal_callback(pointcloud_msg); // 2do: directly give this to AddReceiveCallback
//  gpu_push_buffers(p.numpoints);
}
#endif // eCAL_ON

int mx, my;
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
        std::cout << "main.cpp: loading " << (std::string)dropped_filedir << std::endl;
 ///       int numpoints = pointcloud_load(dropped_filedir);
 ///       if (numpoints > 0) gpu_push_buffers(numpoints);

        SDL_free(dropped_filedir);    // Free dropped_filedir memory
        break;
      }

      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          resize(event.window.data1, event.window.data2);
        }
      }

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
