#ifndef GPU_PRIMITIVES_GL_HPP
#define GPU_PRIMITIVES_GL_HPP

#include <vector>
#include "yocto_math.h"
typedef yocto::vec3f vec3f;

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

// 2do: implement

namespace gpuprim_GL {

  GLenum err;

  class vertex_buffer {
  public:
    GLuint vbo;
    GLuint vbo_col;
    // 2do: common vbo that holds verts, cols and texture u,v's
    vertex_buffer(const std::vector<GLfloat> vertices, const std::vector<GLfloat> colors,const int num_vertices) {
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices, &vertices[0], GL_STATIC_DRAW);
      glGenBuffers(1, &vbo_col);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices, &colors[0], GL_STATIC_DRAW);
    }
    ~vertex_buffer() {
      if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
        glDeleteBuffers(1, &vbo_col);
        vbo_col = 0;
      }
    }
  };

  class index_buffer {
  public:
    GLuint ebo;
    index_buffer(const std::vector<GLfloat> indices, const int num_indices) {
      glGenBuffers(1, &ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * num_indices, &indices[0], GL_STATIC_DRAW);
    }
    ~index_buffer() {
      if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
      }
    }
  };

  class gpu_obj {
  protected:
    std::vector<float> vertices;
    std::vector<float> colors;
    uint32_t n_vertices;
    uint32_t n_indices;

    vertex_buffer* vert_buf;
    index_buffer* ind_buf;

    GLenum draw_mode = GL_TRIANGLES; // GL_LINES 

//    GLuint program;
    GLuint vao;

  public:
//    obj() {}
    gpu_obj(vec3f rgb, vec3f pos, GLuint shaderprog) : vertices(),colors(),n_vertices(0),n_indices(0),vert_buf(nullptr),ind_buf(nullptr) {
      vertices = { -8, 0.055854f,  500, // Tri1 ccw, these are actually coordinates, a vertex is rather (x,y,z)
                8, 0.055854f,  500,
               -8, 0.055854f, -500,
                8, 0.055854f,  500, // Tri2
                8, 0.055854f, -500,
               -8, 0.055854f, -500
      };
      n_vertices = vertices.size();

      float r = rgb.x;
      float g = rgb.y;
      float b = rgb.z;
      colors = { r, g, b, // Tri1
                 r, g, b,
                 r, g, b,
                 r, g, b, // Tri2
                 r, g, b,
                 r, g, b
      };

      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
      vert_buf = new vertex_buffer(vertices, colors, n_vertices);
//      ind_buf = new index_buffer(indices, n_indices * sizeof(indices[0]));
    }

    ~gpu_obj() {
      delete vert_buf;
      vert_buf = nullptr;
    }

    void render() {
      glBindVertexArray(vao);
      
      glUseProgram(program);
      GLuint vpos_location = glGetAttribLocation(program, "vPos");
      GLuint vcol_location = glGetAttribLocation(program, "vCol");
      
      glBindBuffer(GL_ARRAY_BUFFER, vert_buf->vbo);
      glEnableVertexAttribArray(vpos_location); // attrib location
      glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      glBindBuffer(GL_ARRAY_BUFFER, vert_buf->vbo_col);
      glEnableVertexAttribArray(vcol_location); // attrib location
      glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      // check that vtx_count is set, i.e. > 0
      // also color count has to be same size, otherwise draw will cause runtime exception
      if (n_indices == 0) {
        glDrawArrays(draw_mode, 0, static_cast<GLsizei>(n_vertices));
      }
      else {
        // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(n_indices), GL_UNSIGNED_INT, (void*)0);
      }
      err = glGetError();
      if (err != 0)
      {
        std::cout << "GL err: " << err << std::endl;
      }
      glDisableVertexAttribArray(vpos_location);
      glDisableVertexAttribArray(vcol_location);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glUseProgram(0);
    }
  };

  gpu_obj* tmp_obj; // 2do, move to

  class sphere : public gpu_obj {};
  class grid : public gpu_obj {};
/*  class plane : public gpu_obj {
    plane(vec3f rgb) {
      vertices = { -8, 0.055854f,  500, // Tri1 ccw, these are actually coordinates, a vertex is rather (x,y,z)
                8, 0.055854f,  500,
               -8, 0.055854f, -500,
                8, 0.055854f,  500, // Tri2
                8, 0.055854f, -500,
               -8, 0.055854f, -500
      };

      float r = rgb.x;
      float g = rgb.y;
      float b = rgb.z;
      colors = { r, g, b, // Tri1
                 r, g, b,
                 r, g, b,
                 r, g, b, // Tri2
                 r, g, b,
                 r, g, b
      };
    }
  };
  */
  class mesh : public gpu_obj {};
  class gen_tunnel : public gpu_obj {}; // ... s. starduck3d

} // namespace gpuprim_GL

#endif // GPU_PRIMITIVES_GL_HPP
