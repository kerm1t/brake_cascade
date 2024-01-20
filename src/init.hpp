#ifndef INIT_HPP
#define INIT_HPP

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
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);      // 8 bit für rot
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);    // 8 bit für grün
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);     // 8 bit für blau
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);      // 8 bit für alpha
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);  // 32 bit für Pixel (optional)
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // Double Buffering aktivieren

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
//  glEnable(GL_CULL_FACE);
//  glCullFace(GL_BACK);

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
#endif // INIT_HPP
