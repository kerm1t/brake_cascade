#ifndef DRAW_PRIMITIVES_HPP
#define DRAW_PRIMITIVES_HPP
/*
class grid {
  // not used yet
};*/
std::vector<float> grid_vertices;
std::vector<float> grid_colors;

void grid_create() {
  for (int x = -50; x <= 50; x += 10) {
    // 2do, somewhat it didn't work to push a point to a vector of points, instead of a vector of floats <-- on ARM? 
    grid_vertices.push_back(x);
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back(x);
    grid_vertices.push_back(50.0f);
    grid_vertices.push_back(0.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
    grid_colors.push_back(1.0f);
  }
  for (int y = -50; y <= 50; y += 10) {
    grid_vertices.push_back(-50.0f);
    grid_vertices.push_back(y);
    grid_vertices.push_back(0.0f);
    grid_vertices.push_back(50.0f);
    grid_vertices.push_back(y);
    grid_vertices.push_back(0.0f);
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
  // (a) vertices
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_vertices.size(), &grid_vertices[0], GL_STATIC_DRAW);
  // hmm, so obviously this is not needed here, but maybe somewhere ...
  // to enable the vertexattribarray (good for all vbo)
//  glEnableVertexAttribArray(vpos_location);
//  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // (b) colors
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * grid_colors.size(), &grid_colors[0], GL_STATIC_DRAW);
//  glEnableVertexAttribArray(vcol_location);
//  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
};

void grid_render() {
  // now as we have multiple vbo (1 for point cloud, 1 for grid), we need to
  // (a) bind the buffer (pos+col) and 
  // (b) set the shader attribute
  // before drawing
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_col);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(grid_vertices.size()));
};

//enum draw_mode {dm_POINTS=0, dm_LINES=1, dm_VERTICES=2, dm_UNDEF=255};

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
    vtx_count = vertices.size();
    col_count = colors.size(); // exception, as I had forgot to set colorcount
    ind_count = indices.size();
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
      vertices.push_back(x);
      vertices.push_back(-50.0f);
      vertices.push_back(0.0f);
      vertices.push_back(x);
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
      float nz = -1 + i * 2;                           // z value of normal; -1 to 1

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

#endif // DRAW_PRIMITIVES_HPP
