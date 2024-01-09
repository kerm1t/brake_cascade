#pragma once

// Our state
bool show_demo_window = false;
bool show_another_window = false;
// --> draw.hpp  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

//extern bool sim_start;

void draw_imgui(ImGuiIO& io) {
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
    ImGui::Text("month = %f [s]", month / 1000000); // ms --> s
    ImGui::Text("year = %f [s]", year / 1000000); // ms --> s
    ImGui::Text("v = %f [mph]", sim::v);
//    ImGui::PlotLines("v[mph]", sim::a_v, sim::a_v.size());// , i_sim, 0, -1.0f, 1.0f, ImVec2(0, 80));
    if (ImGui::Button("start sim")) {
// --> need to init -->      i_sim = 0;
      sim_start = true;
    }
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

}