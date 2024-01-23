#ifndef DRAW_HPP
#define DRAW_HPP

//#include "linmath.h"

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

int colorcoding = 0;

// 2do: move to camera class -->
class camera
{
public:
  vec3 rot;
  vec3 trans;
};
camera cam;
float zoom = -30.0f;

// small explanation of GLSL entities
// ----------------------------------
// attribute is an openGL variable space. contrary to uniform it cannot be ... (used in a shader?) 
// uniform
// varying   # deprecated > version 140
const char* vertex_shader_text =
#include "./shader/start.vs"
;
const char* fragment_shader_text =
#include "./shader/start.fs"
;

GLuint program;
GLint mvp_location;
GLint T_location;

// --------------
// vertex + color
// --------------
// (a) gpu data storage
// vertex buffer object, e.g. for ...
// ... point cloud
GLuint vertex_buffer;
GLuint colorbuffer[3]; // 3 color buffer (Gl-)objects to store precalculated color codings (height, dist, ...)
// ... grid
GLuint grid_vbo;
GLuint grid_vbo_col;
// ... cube
GLuint cube_vbo;
GLuint cube_vbo_col;
GLuint cube_vbo_tex;
// mesh
GLuint mesh_vbo;
GLuint mesh_vbo_col;
GLuint mesh_vbo_ind;

// (b) gpu shader attributes
GLint vpos_location, vcol_location, tex_location;

#include "gpu_primitives.hpp" // include after vpos_location, 2do: fix this

gpu_prim two_tris; // to be after incl. gpu_primitives
gpu_prim lane_normal;
gpu_prim lane_wet;
gpu_prim lane_icy;
gpu_prim_indexed two_tris_i;



void gpu_create_shaders() {  
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);
  // https://www.khronos.org/opengl/wiki/Example/GLSL_Shader_Compile_Error_Testing
  GLint isCompiled = 0;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(vertex_shader); // Don't leak the shader.
    return;
  }
  // Shader compilation is successful.

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);
  isCompiled = 0;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(fragment_shader); // Don't leak the shader.
    return;
  }
  // Shader compilation is successful.

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
}

void gpu_create_buffers() { // Buffer auf der Grafikkarte erstellen
    // generate <n> buffer object names
    glGenBuffers(1, &vertex_buffer);
    GLenum err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glGenBuffers(3, colorbuffer);
    err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
}

void gpu_free_buffers() {
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(3, colorbuffer);
}

void gpu_create_variables() {
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    tex_location = glGetAttribLocation(program, "texCoord");
    T_location = glGetUniformLocation(program, "T"); // translation
    GLenum err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
}

void gpu_cube() { // set cube data and push to gpu vbo
//    glPushMatrix();


    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
    };
    // hmm, genbuffers hatte ich erst vergessen, waer vielleicht gut eine classe zu haben, die diese funktionen verlangt
// (a) vertices
    glGenBuffers(1, &cube_vbo);
    glGenBuffers(1, &cube_vbo_col);

    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    GLenum err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    err = glGetError(); 
    if (err != 0)
    {
        int i = 1;
    }
    // OpenGL Vertex Format angeben
    glEnableVertexAttribArray(vpos_location);
    err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }

// (b) color
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }

//    glPopMatrix();

    glGenBuffers(1, &cube_vbo_tex);
    // (c) add texture coordinates. even if we do not use textures, it will be very handy
    // to create shader effects
//  https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_06
    GLfloat g_tex_buffer_data[2 * 4 * 6] = {
      // front
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
    };
    for (int i = 1; i < 6; i++)
      memcpy(&g_tex_buffer_data[i * 4 * 2], &g_tex_buffer_data[0], 2 * 4 * sizeof(GLfloat));
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_tex_buffer_data), g_tex_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(tex_location);
    glVertexAttribPointer(tex_location, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
};

void gpu_cube_render() {
//  glPushMatrix();  doesn't work, instead use uniform
//  glTranslatef(0.0f, 20.0f, 0.0f);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_tex);
//  glVertexAttribPointer(tex_location, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_TRIANGLES, 0, 12*3);
  //  glPopMatrix();
}

void gpu_cube_free() {
  glDeleteBuffers(1, &cube_vbo);
  glDeleteBuffers(1, &cube_vbo_col);
}

void gpu_push_buffers(int numpoints) {
    // (a) vertices
    GLenum err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lloft::vertices.size(), &lloft::vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // (b) colors --> x3 :-)

    // create a new buffer object data store
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[0]);
    // create new data store for a buffer object, copy <data> into data store
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lloft::vertices.size(), &lloft::colors[0][0], GL_STATIC_DRAW);
    // enable generic attribute --> s. link to shader ... vcol_location = glGetAttribLocation(program, "vCol");
    glEnableVertexAttribArray(vcol_location);
    // define (specify location and data format of) an array of attribute data
    // If pointer is not NULL, a non-zero named buffer object must be bound to the GL_ARRAY_BUFFER target 
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // push color buffer1 and 2, but no need to bind them to shader attribute yet (user input 1,2,3,...)
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lloft::vertices.size(), &lloft::colors[1][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lloft::vertices.size(), &lloft::colors[2][0], GL_STATIC_DRAW);
}

void enable_colorbuffer(int color_coding) { // toggle color buffers (distance, height, intensity)
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[color_coding]);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

mat4x4 mvp;
void cam_update(int width, int height) { // or just view update
  float aspect;
  mat4x4 view, projection;
//  mat4x4 m, p;

  // Calculate aspect of window
  if (height > 0)
    aspect = (float)width / (float)height;
  else
    aspect = 1.f;

  // Setup perspective projection matrix
  glMatrixMode(GL_PROJECTION);
  mat4x4_perspective(projection,
    65.f * (float)M_PI / 180.f,
    aspect,
    0.1f, 800.f); // frustrum

  glMatrixMode(GL_MODELVIEW);
  {
    vec3 eye = { 3.f, 1.5f, 3.f };
    vec3 center = { 0.f, 0.f, 0.f };
    vec3 up = { 0.f, 1.f, 0.f };
    mat4x4_look_at(view, eye, center, up);
  }

  mat4x4_translate(view, cam.trans[0], cam.trans[1], zoom + cam.trans[2]);
  //    mat4x4_rotate_Z(m, m, (float)glfwGetTime());
  mat4x4_rotate_X(view, view, cam.rot[0]);
  mat4x4_rotate_Y(view, view, 180.0f + cam.rot[1]);
  mat4x4_rotate_Z(view, view, 90.0f + cam.rot[2]);

  mat4x4_mul(mvp, projection, view);
}

//float earth_spin = 0.0f;
vec2 earth;
vec2 moonpos;
//float moon_spin = 0.0f;
void render(SDL_Window* window, int numpoints, fastObjMesh* mesh) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    cam_update(width, height);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // https://stackoverflow.com/questions/46020871/opengl-depth-testing-not-working-glew-sdl2

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);


    // dman.render() <-- no single obj render calls anymore in here
/*
    // (a) draw/render point cloud
    glPointSize(4);
    int npoints = lloft::vertices.size();
    // now as we have multiple vbo (1 for point cloud, 1 for grid), we need to
    // (a) bind the buffer (pos+col) and 
    // (b) set the shader attribute
    // before drawing
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer[colorcoding]); // 2do, there is colorcoding and color_coding
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(npoints));
*/    
    grid_render();
/*
// (b) draw cube
    glUniform3f(T_location, d_truck, 0.0f, 0.0f); // pseudo camera position
///    gpu_cube_render();
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f);

//    grid1->render();

// (c) draw earth vbo (animated) ... this could be decoupled, i.e. (i) anim/sim step, (ii) draw
    float r = 20.0f;
//    earth_spin += 0.005f;
//    if (earth_spin >= 360.0f) earth_spin = 0.0f;
    earth[0] = r * cos(earth_spin);
    earth[1] = r * sin(earth_spin);
    glUniform3f(T_location, earth[0], earth[1], 0.0f); // pseudo camera position
    sph1->render();
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f);

// (d) draw moon (animated)
    r = 3.f;
//    moon_spin += 0.01f; // RAD
//    if (moon_spin >= 2*PI) moon_spin = 0.0f;
    moonpos[0] = earth[0] + r * cos(moon_spin);
    moonpos[1] = earth[1] + r * sin(moon_spin);
    glUniform3f(T_location, moonpos[0], moonpos[1], 0.0f); // pseudo camera position
    moon->render();
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f);
    */
/*    // measure 1 moon spin
    if (moon_spin < 0.01f)
    {
      auto start = stop;
      stop = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      month = duration.count();
    }
    */
    /*
// (e) draw sun
if (sun)    sun->render();

    glUniform3f(T_location, 5.0f, 0.0f, 0.0f); // pseudo camera position
//    htor->render();
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f); // pseudo camera position
*/

// draw 3 "trucks"
    float h = 9.1f;
    glUniform3f(T_location, 0.0f, h, -sim::d_truck); // pseudo camera position
    mesh_render(mesh);

    // 2 more instances
    glUniform3f(T_location, 25.0f, h, -sim::d_truck_wet); // pseudo camera position
    mesh_render(mesh);

    glUniform3f(T_location, -25.0f, h, -sim::d_truck_icy); // pseudo camera position
    mesh_render(mesh);
    glUniform3f(T_location, 0.0f, 0.0f, 0.0f); // pseudo camera position

// draw 3 "lanes"
///    two_tris.render();
    lane_normal.render();
    glUniform3f(T_location, 25.0f, 0.0f, 0.0f); // pseudo camera position
///    two_tris.render();
    lane_wet.render();
    glUniform3f(T_location, -25.0f, 0.0f, 0.0f); // pseudo camera position
///    two_tris.render();
    lane_icy.render();

///    two_tris_i.render(); // <-- not being drawn ? 


    GLenum err = glGetError();
    if (err != 0)
    {
        int i = 1;
    }
}
void resize(int w, int h) {
  if (h == 0)                     // Prevent A Divide By Zero By
  {
    h = 1;                      // Making Height Equal One
  }
  glViewport(0, 0, w, h);   // Reset The Current Viewport
 }

#endif // DRAW_HPP
