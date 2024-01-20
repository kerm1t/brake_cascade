#ifndef GPU_PRIMITIVES_HPP
#define GPU_PRIMITIVES_HPP


//#ifndef FAST_OBJ_IMPLEMENTATION
//#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"
//#endif // FAST_OBJ_IMPLEMENTATION

#include <vector>
#include <unordered_map>
#include "linmath.h"
#include "yocto_math.h"

//using namespace yocto;
/*
class grid {
  // not used yet
};*/
std::vector<float> grid_vertices;
std::vector<float> grid_colors;

GLenum err;

unsigned int VAO_grid;
unsigned int VAO_mesh;

void grid_create() {
  for (int x = -50; x <= 50; x += 10) {
    // 2do, somewhat it didn't work to push a point to a vector of points, instead of a vector of floats <-- on ARM?
    // i tweaked this to be in same orientation with the trucks
    // --> need an obj "grid" with orientation
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back((float)x);
    grid_vertices.push_back(50.0f);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back((float)x);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
  }
  for (int y = -50; y <= 50; y += 10) {
    grid_vertices.push_back((float)y);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back((float)y);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back(50.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
  }
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
//  glEnableVertexAttribArray(vpos_location);
//  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(vpos_location);

  // (b) colors
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_colors.size(), &grid_colors[0], GL_STATIC_DRAW);
//  glEnableVertexAttribArray(vcol_location);
//  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
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
  //  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//  glEnableVertexAttribArray(vpos_location);
  
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  //  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//  glEnableVertexAttribArray(vcol_location); 
  
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(grid_vertices.size()));
};

//enum draw_mode {dm_POINTS=0, dm_LINES=1, dm_VERTICES=2, dm_UNDEF=255};

class gpu_prim { // vertices + colors! (either explicitly or via normals)
protected:
  std::vector<float> vertices;
  uint32_t n_vertices;

  unsigned int VAO; // easy switch between vertex buffers and vertex attributes/configs --> works!
  unsigned int VBO;
  unsigned int VBO_col; // ~hacky
public:
  void create_buffers() {
    // (1) define data and send to GPU
    std::vector<GLfloat> vertices = { -0.967722, 0.055854, 1.980188, // Tri1 ccw, these are actually coordinates, a vertex is rather (x,y,z)
                                       2.970117, 0.055854, 1.980188,
                                      -0.967722, 0.055854, 0.009238,
                                       2.970117, 0.055854, 1.980188, // Tri2
                                       2.970117, 0.055854, 0.009238,
                                      -0.967722, 0.055854, 0.009238
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // create gpu_buffer (here: VBuff --> VBO)
  // need to be global, as render needs it -->  unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // <-- umpf, hatte ich vergessen
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW); // copy user data to buffer
    n_vertices = vertices.size(); // <-- braucht man fuer zweites dreieck
    // GL_STREAM_DRAW : the data is set only once and used by the GPU at most a few times.
    // GL_STATIC_DRAW : the data is set only once and used many times.
    // GL_DYNAMIC_DRAW : the data is changed a lot and used many times.

    // (2) write vertex shader --> s. start.vs
    // (2a) upload to GPU + compile vertex shader --> s. draw.hpp  // is this compiled by GPU or by OpenGL ?

    // (3) write fragment shader --> s. start.fs
    // (3a) compile fragment shader --> s. draw.hpp

// moved to (render.)main    gpu_create_shaders(); // hmmm, hatte ich vergessen, doof

    // (4) define vertex attributes / parameters
    //        0 = location of attri.
    //        3 = size of attrib
    // GL_FLOAT = type of data
    // GL_FALSE = normalize data
    //  3*float = stride
    // (void*)0 = start offset
///    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // <-- malt das zweite dreieck nicht, hier muss #vertices, noeoe falsch
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
///    glEnableVertexAttribArray(0); // attrib location
    glEnableVertexAttribArray(vpos_location); // attrib location

    // farben --> oder normals --> oder textures :-) 2do!
    glGenBuffers(1, &VBO_col);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_col); // <-- umpf, hatte ich vergessen
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW); // copy user data to buffer
///    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
/// nope    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(vcol_location);
  }
  void render() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // <-- umpf, hatte ich vergessen
    glBindBuffer(GL_ARRAY_BUFFER, VBO_col); // mal sehn
    err = glGetError();
    glDrawArrays(GL_TRIANGLES, 0, n_vertices); // <-- hmm, hier malt immer 3 vertices
  }
  void cleanup() {
    // needed?    glDeleteBuffers(VBO);
  }
};

class gpu_prim_indexed : gpu_prim {
protected:
///  std::vector<unsigned int> faces; // vertices //// + normals (+textures)
  unsigned int n_indices;

  unsigned int EBO; // or IBO

public:
  void create_buffers_from_faces() {
    std::vector<GLfloat> vertices = { 5.967722, 0.055854, 1.980188, // ccw
                                   22.970117, 0.055854, 1.980188,
                                  5.967722, 0.055854, 0.009238,
                                   22.970117, 0.055854, 0.009238 };
    std::vector<unsigned int> face_indices = { 1,2,0, 1,3,2 }; // ccw
//    std::vector<unsigned int> face_indices = { 0,2,1, 1,2,3 }; // cw

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    ///    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face_indices), &face_indices[0], GL_STATIC_DRAW); // sizeof(indices) ... grrrrr!
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*face_indices.size(), &face_indices[0], GL_STATIC_DRAW); // <-- unsigned int statt float
    n_indices = face_indices.size();

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // <-- umpf, hatte ich vergessen
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW); // copy user data to buffer
    n_vertices = vertices.size(); // <-- braucht man fuer zweites dreieck

    // without the attrib description, doesn't draw second tri (next 2 lines)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // attrib location
    err = glGetError();
    err = glGetError();
  };
  void render() {
    glBindVertexArray(VAO);
    err = glGetError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    err = glGetError();
    glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0); // hier crash, weil ich oben glGenBuffers vergessen habe
    err = glGetError();
  };
};

// sphere = oop-Pilot
class obj {
protected:
  std::vector<float> vertices;
  std::vector<float> colors;
  std::vector<float> normals;  // new
  std::vector<float> texCoords;// new
  std::vector<unsigned int> indices; // new
  // OpenGL specific
  GLuint _vbo;
  GLuint _vbo_col;
  GLuint vbo_ind;
  GLuint vtx_count; // <-- glDrawArrays()
  GLuint col_count;
  GLuint ind_count;
  GLenum draw_mode = GL_TRIANGLES; // 2do; POINTS, LINES, VERTICES 
//  draw_mode drawmode = dm_VERTICES;
public:
  virtual void create_buffers() = 0; // 2do: and push to gpu ... need to set count
  virtual void gpu_free() {
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vbo_col);
  };
  virtual void gpu_push_buffers() { // ... this could be obsolete then
    vtx_count = (GLuint)vertices.size();
    col_count = (GLuint)colors.size(); // exception, as I had forgot to set colorcount
    ind_count = (GLuint)indices.size();
    // (a) vertices
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vtx_count, &vertices[0], GL_STATIC_DRAW);
    // (b) colors
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * col_count, &colors[0], GL_STATIC_DRAW);
    // opt: (c) indices
    if (ind_count > 0) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ind);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ind_count, &indices[0], GL_STATIC_DRAW);
    }
    // now free vectors
    vertices.clear();
    colors.clear();
    GLenum err = glGetError();
    if (err != 0)
    {
      int i = 1;
    }
  };
  void render() { // draw
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    GLenum err = glGetError();
    if (err != 0)
    {
      int i = 1;
    }
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_col);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // check that vtx_count is set, i.e. > 0
    // also color count has to be same size, otherwise draw will cause runtime exception
    if (ind_count == 0) {
      glDrawArrays(draw_mode, 0, static_cast<GLsizei>(vtx_count));
    }
    else {
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ind_count), GL_UNSIGNED_INT, (void*)0);
    }
    err = glGetError();
    if (err != 0)
    {
      int i = 1;
    }
  };
};

class grid : public obj {
//  std::vector<float> vertices;
//  std::vector<float> colors;
public:
  void create_buffers() override {
//    vtx_count = 0;
    for (int x = -50; x <= 50; x += 10) {
      // 2do, somewhat it didn't work to push a point to a vector of points, instead of a vector of floats <-- on ARM? 
      vertices.push_back((float)x);
      vertices.push_back(-50.0f);
      vertices.push_back(0.0f);
      vertices.push_back((float)x);
      vertices.push_back(50.0f);
      vertices.push_back(0.0f);
//      vtx_count++;
      colors.push_back(1.0f);
      colors.push_back(0.0f);
      colors.push_back(0.0f);
      colors.push_back(0.0f);
      colors.push_back(1.0f);
      colors.push_back(0.0f);
    }
    glGenBuffers(1, &_vbo);    // move to obj.create
    glGenBuffers(1, &_vbo_col);// move to obj.create
    GLenum err = glGetError();
    if (err != 0)
    {
      int i = 1;
    }
  };
//  void gpu_free();
//  void gpu_push_buffers();
//  void render(); // draw
};
grid* grid1; // move somewhere else

#define PI 3.14159f
class sphere : public obj {
public:
  float radius = 2.0f;
  float r, g, b; // 2do: move to constructor parameter
  int sectorCount = 15;
  int stackCount = 15;
// http://www.songho.ca/opengl/gl_sphere.html
  void create_buffers() override {
    // clear memory of prev arrays
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
      stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
      xy = radius * cosf(stackAngle);             // r * cos(u)
      z = radius * sinf(stackAngle);              // r * sin(u)

      // add (sectorCount+1) vertices per stack
      // the first and last vertices have same position and normal, but different tex coords
      for (int j = 0; j <= sectorCount; ++j)
      {
        sectorAngle = j * sectorStep;           // starting from 0 to 2pi

        // vertex position (x, y, z)
        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
        y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        // normalized vertex normal (nx, ny, nz)
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;
        normals.push_back(nx);
        normals.push_back(ny);
        normals.push_back(nz);

        // vertex tex coord (s, t) range between [0, 1]
        s = (float)j / sectorCount;
        t = (float)i / stackCount;
        texCoords.push_back(s);
        texCoords.push_back(t);

        vec3 f;
        f[0] = rand() / 32767.0f;
        f[1] = rand() / 32767.0f;
        f[2] = rand() / 32767.0f;
        colors.push_back(r * f[0]); // b
        colors.push_back(g * f[1]); // r
        colors.push_back(b * f[2]);
      }
    }
    // generate CCW index list of sphere triangles
// k1--k1+1
// |  / |
// | /  |
// k2--k2+1
//    std::vector<int> indices;
    std::vector<int> lineIndices;
    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
      k1 = i * (sectorCount + 1);     // beginning of current stack
      k2 = k1 + sectorCount + 1;      // beginning of next stack

      for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
      {
        // 2 triangles per sector excluding first and last stacks
        // k1 => k2 => k1+1
        if (i != 0)
        {
          indices.push_back(k1);
          indices.push_back(k2);
          indices.push_back(k1 + 1);
        }

        // k1+1 => k2 => k2+1
        if (i != (stackCount - 1))
        {
          indices.push_back(k1 + 1);
          indices.push_back(k2);
          indices.push_back(k2 + 1);
        }

        // store indices for lines
        // vertical lines for all stacks, k1 => k2
        lineIndices.push_back(k1);
        lineIndices.push_back(k2);
        if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
        {
          lineIndices.push_back(k1);
          lineIndices.push_back(k1 + 1);
        }
      }
    }
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vbo_col);
    glGenBuffers(1, &vbo_ind);
    GLenum err = glGetError();
    if (err != 0)
    {
      int i = 1;
    }
  };
};
sphere* sph1; // move somewhere else
sphere* moon;
sphere* sun;

// https://www.songho.ca/opengl/gl_cylinder.html
class cylinder : public obj {
public:
  int sectorCount = 16;
  float r, g, b; // 2do: move to constructor parameter
  float height = 3.0f;
  float radius = 1.0f;
  // generate a unit circle on XY-plane
  std::vector<float> getUnitCircleVertices()
  {
//    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / sectorCount;
    float sectorAngle;  // radian

    std::vector<float> unitCircleVertices;
    for (int i = 0; i <= sectorCount; ++i)
    {
      sectorAngle = i * sectorStep;
      unitCircleVertices.push_back(cos(sectorAngle)); // x
      unitCircleVertices.push_back(sin(sectorAngle)); // y
      unitCircleVertices.push_back(0);                // z
    }
    return unitCircleVertices;
  }
  // generate vertices for a cylinder
//  void buildVerticesSmooth(float height, float radius)
  void create_buffers() override {
    // clear memory of prev arrays
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);

    // get unit circle vectors on XY-plane
    std::vector<float> unitVertices = getUnitCircleVertices();

    // put side vertices to arrays
    for (int i = 0; i < 2; ++i)
    {
      float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
      float t = 1.0f - i;                              // vertical tex coord; 1 to 0

      for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
      {
        float ux = unitVertices[k];
        float uy = unitVertices[k + 1];
        float uz = unitVertices[k + 2];
        // position vector
        vertices.push_back(ux * radius);             // vx
        vertices.push_back(uy * radius);             // vy
        vertices.push_back(h);                       // vz
        // normal vector
        normals.push_back(ux);                       // nx
        normals.push_back(uy);                       // ny
        normals.push_back(uz);                       // nz
        // texture coordinate
        texCoords.push_back((float)j / sectorCount); // s
        texCoords.push_back(t);                      // t
// hmm, should be rather automatic happen
        vec3 f;
        f[0] = rand() / 32767.0f;
        f[1] = rand() / 32767.0f;
        f[2] = rand() / 32767.0f;
        colors.push_back(r * f[0]); // b
        colors.push_back(g * f[1]); // r
        colors.push_back(b * f[2]);
        // hmm, should be rather automatic happen
      }
    }

    // the starting index for the base/top surface
    //NOTE: it is used for generating indices later
    int baseCenterIndex = (int)vertices.size() / 3;
    int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

    // put base and top vertices to arrays
    for (int i = 0; i < 2; ++i)
    {
      float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
      float nz = -1 + (float)i * 2;                    // z value of normal; -1 to 1

      // center point
      vertices.push_back(0);     vertices.push_back(0);     vertices.push_back(h);
      normals.push_back(0);      normals.push_back(0);      normals.push_back(nz);
      texCoords.push_back(0.5f); texCoords.push_back(0.5f);
      colors.push_back(0.5f);    colors.push_back(0.5f);    colors.push_back(0.5f);

      for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
      {
        float ux = unitVertices[k];
        float uy = unitVertices[k + 1];
        // position vector
        vertices.push_back(ux * radius);             // vx
        vertices.push_back(uy * radius);             // vy
        vertices.push_back(h);                       // vz
        // normal vector
        normals.push_back(0);                        // nx
        normals.push_back(0);                        // ny
        normals.push_back(nz);                       // nz
        // texture coordinate
        texCoords.push_back(-ux * 0.5f + 0.5f);      // s
        texCoords.push_back(-uy * 0.5f + 0.5f);      // t
// hmm, should be rather automatic happen
        vec3 f;
        f[0] = rand() / 32767.0f;
        f[1] = rand() / 32767.0f;
        f[2] = rand() / 32767.0f;
        colors.push_back(r * f[0]); // b
        colors.push_back(g * f[1]); // r
        colors.push_back(b * f[2]);
        // hmm, should be rather automatic happen
      }
    }

// hier fehlt noch das kreieren der indices, oben werden nur points hinzugefügt

// hmmmm
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vbo_col);
    glGenBuffers(1, &vbo_ind);
    GLenum err = glGetError();
    if (err != 0)
    {
      int i = 1;
    }
// hmmmm
  } // buildVerticesSmooth()
};
cylinder* htor; // don't define here but elsewhere, add manager!


void mesh_gpu_create() {
  glGenBuffers(1, &mesh_vbo);
  glGenBuffers(1, &mesh_vbo_col);
  glGenBuffers(1, &mesh_vbo_ind);
};
int mesh_index_count; // hack!!
///#if false
void mesh_gpu_push_buffers(fastObjMesh* mesh) { // this is wrong, as not vertices/positions define the object, but faces
  // (a) vertices
  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->position_count, mesh->positions, GL_STATIC_DRAW);
  // hmm, so obviously this is not needed here, but maybe somewhere ...
  // to enable the vertexattribarray (good for all vbo)
//  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
//  glEnableVertexAttribArray(vpos_location);

  // (b) colors // 2do --> check if 0!!
  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_col);
///  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->color_count, mesh->colors, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->normal_count, mesh->normals, GL_STATIC_DRAW);
//  glEnableVertexAttribArray(vcol_location);
//  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  unsigned int* mesh_vtx_ind;
  fastObjIndex* p_mesh;
  mesh_vtx_ind = (unsigned int*)malloc(mesh->index_count*sizeof(unsigned int));
  p_mesh = mesh->indices;
  for (unsigned int i = 0; i < mesh->index_count; i++) {
    mesh_vtx_ind[i] = p_mesh->p;
    p_mesh++;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbo_ind);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->index_count, mesh->indices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->index_count, mesh_vtx_ind, GL_STATIC_DRAW);
  free(mesh_vtx_ind);
  mesh_index_count = mesh->index_count;
};
///#endif
///int mesh_index_count; // hack!!
bool mesh_gpu_push_buffers_1(fastObjMesh* m) {
  if (!m) return false;
  if (m->face_count == 0) return false;
  if (m->position_count == 0) return false;
  if (m->normal_count == 0) return false; // we could compute normals, but right now, we don't

  std::vector<yocto::vec3f> vertex_data;
  std::vector<yocto::vec3f> normal_data;
  std::vector<uint32_t> indices;

/////  vertex_data.reserve(m->position_count);
/////  indices.reserve(m->position_count);

  uint32_t current_idx = 0;
  fastObjIndex* current_vtx = m->indices;
//  out = indexed_mesh();
//  out.bb_min = vec3f_flt_max;
//  out.bb_max = vec3f_flt_neg_max;

  // Wavefront objs do keep separate indices for pos/normal/uv, we need to merge them
  std::unordered_map<uint64_t, uint32_t> objFaceVertexToMeshIndex;

  for (unsigned int face = 0; face < m->face_count; face++)
  {
    unsigned int num_vtx = m->face_vertices[face];
    if (num_vtx != 3) // we could triangulate (at least quads!), but right now, we don't
    {
      current_vtx += num_vtx;
      continue;
    }

    for (int vtx = 0; vtx < 3; vtx++)
    {
      // Note: wavefront obj stores vertices counter-clockwise, we want to reverse to CW
      auto reverse_vtx_p = current_vtx + (2 - vtx);

      uint32_t idx_p = reverse_vtx_p->p;
      uint32_t idx_n = reverse_vtx_p->n;
      uint64_t mergedIndices = (uint64_t)idx_p | ((uint64_t)(idx_n) << 32);

      auto idx_merged = objFaceVertexToMeshIndex.find(mergedIndices);
      if (idx_merged == objFaceVertexToMeshIndex.end())
      {
        yocto::vec3f temp;

        memcpy(&temp, &m->positions[idx_p * 3], sizeof(vec3));
        vertex_data.push_back(temp*0.1f); // hack!! --> scaling
//        out.bb_min = min(out.bb_min, temp);
//        out.bb_max = max(out.bb_max, temp);

        memcpy(&temp, &m->normals[idx_n * 3], sizeof(vec3));
        normal_data.push_back(temp);

        objFaceVertexToMeshIndex[mergedIndices] = current_idx;
        indices.push_back(current_idx);
        current_idx++;
      }
      else
        indices.push_back(idx_merged->second)
        ;
    }
///    std::vector<unsigned int> face_indices = { 1,2,0, 1,3,2 }; //hack!!!!
    current_vtx += 3;
  }

  //  vec4f renorm = get_bb_renorm_factors(out.bb_min, out.bb_max);
  //  apply_renorm_vecarray(vertex_data.data(), sizeof(vec3f), int(vertex_data.size()), renorm);
  //  apply_renorm_vecarray(&out.bb_min, sizeof(vec3f), 2, renorm); // UGLY - relies on bb_max to follow bb_min
  /*
    auto bD = sg_buffer_desc();
    {
      bD.size = int(vertex_data.size() * sizeof(vec3f));
      bD.content = vertex_data.data();
      bD.label = "loaded-obj-vertices";
    }
    out.bind.vertex_buffers[0] = sg_make_buffer(&bD);

    bD = sg_buffer_desc();
    {
      bD.size = int(normal_data.size() * sizeof(vec3f));
      bD.content = normal_data.data();
      bD.label = "loaded-obj-normals";
    }
    out.bind.vertex_buffers[1] = sg_make_buffer(&bD);
    out.num_streams = 2;

    bD = sg_buffer_desc();
    {
      bD.type = SG_BUFFERTYPE_INDEXBUFFER;
      bD.size = int(indices.size() * sizeof(uint32_t));
      bD.content = indices.data();
      bD.label = "loaded-obj-indices";
    }
    out.bind.index_buffer = sg_make_buffer(&bD);
  */
//  out.num_indices = int(indices.size());

  glGenVertexArrays(1, &VAO_mesh);
  glBindVertexArray(VAO_mesh);

  // (a) vertices
  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
  err = glGetError();
///  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);
  err = glGetError();
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // <-- ?
  glEnableVertexAttribArray(vpos_location);

  // (b) colors // 2do --> besser wären normals!!
  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_col);
  err = glGetError();
///  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normal_data.size(), &normal_data[0], GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * normal_data.size(), &normal_data[0], GL_STATIC_DRAW);
  err = glGetError();
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(vcol_location);

  // (c) indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbo_ind);
  err = glGetError();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
  err = glGetError();
  mesh_index_count = indices.size(); // hack!!

  return true;
}

void mesh_render(fastObjMesh* mesh) { // 2do: besser den pointer auf das mesh speichern
  glBindVertexArray(VAO_mesh);
  // now as we have multiple vbo (1 for point cloud, 1 for grid), we need to
  // (a) bind the buffer (pos+col) and 
  // (b) set the shader attribute
  // before drawing
/*  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // <-- ?
  glEnableVertexAttribArray(vpos_location);

  glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_col);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  glEnableVertexAttribArray(vcol_location);
  err = glGetError();
//  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->position_count/3));
//  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh_index_count));
*/
//  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // <-- ?
//  glEnableVertexAttribArray(vpos_location);
//  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//  glEnableVertexAttribArray(vcol_location);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbo_ind);
// this doesn't work -->  glScalef(0.1f, 0.1f, 0.1f); 
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh_index_count), GL_UNSIGNED_INT, 0);// (void*)3); // hack!!!
  err = glGetError();
};

#endif // DRAW_PRIMITIVES_HPP
