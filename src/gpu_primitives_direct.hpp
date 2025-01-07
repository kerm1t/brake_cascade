#ifndef GPU_PRIMITIVES_DIRECT_HPP
#define GPU_PRIMITIVES_DIRECT_HPP

#include <vector>
#include <unordered_map>
#include "yocto_math.h"

std::vector<float> grid_vertices;
std::vector<float> grid_colors;

void grid_create(const int startx, const int stopx, const int dX) {
  for (int x = startx; x <= stopx; x += dX) {
    // 2do, somewhat it didn't work to push a point to a vector of points, instead of a vector of floats <-- on ARM?
    // i tweaked this to be in same orientation with the trucks
    // --> need an obj "grid" with orientation
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back((float)x);
    grid_vertices.push_back(100.0f);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back((float)x);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
  }
  // ... this is a 1d only grid
  glGenBuffers(1, &grid_vbo);
  glGenBuffers(1, &grid_vbo_col);
};

void grid_free() {
  glDeleteBuffers(1, &grid_vbo);
  glDeleteBuffers(1, &grid_vbo_col);
};

void grid_gpu_push_buffers() {
  glGenVertexArrays(1, &VAO_grid);
  glBindVertexArray(VAO_grid);
  // (a) vertices
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_vertices.size(), &grid_vertices[0], GL_STATIC_DRAW);
  // hmm, so obviously this is not needed here, but maybe somewhere ...
  // to enable the vertexattribarray (good for all vbo)
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(vpos_location);

  // (b) colors
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_colors.size(), &grid_colors[0], GL_STATIC_DRAW);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(vcol_location);
};

void grid_render() {
  glBindVertexArray(VAO_grid);
  // now as we have multiple vbo (1 for point cloud, 1 for grid), we need to
  // (a) bind the buffer (pos+col) and 
  // (b) set the shader attribute
  // before drawing
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);

  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(grid_vertices.size()));
};


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
    //        int i = 1;
  }
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  err = glGetError();
  if (err != 0)
  {
    //        int i = 1;
  }
  // OpenGL Vertex Format angeben
  glEnableVertexAttribArray(vpos_location);
  err = glGetError();
  if (err != 0)
  {
    //        int i = 1;
  }
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  err = glGetError();
  if (err != 0)
  {
    //        int i = 1;
  }

  // (b) color
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  err = glGetError();
  if (err != 0)
  {
    //        int i = 1;
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
  glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
  //  glPopMatrix();
}

void gpu_cube_free() {
  glDeleteBuffers(1, &cube_vbo);
  glDeleteBuffers(1, &cube_vbo_col);
}

void gpu_push_buffers(int numpoints) { // for pointcloud ?    2do --> move to pointcloud
  // (a) vertices
  GLenum err = glGetError();
  if (err != 0)
  {
    //        int i = 1;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  err = glGetError();
  if (err != 0)
  {
    //        int i = 1;
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

#endif // DRAW_PRIMITIVES_DIRECT_HPP
