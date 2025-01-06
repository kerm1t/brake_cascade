#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "math/linmath.h" // 2do: replace with yocto_math.h
#include "math/mathbase.h"

#include <memory.h>
#include <vector>

namespace lloft {
  // semantic
  // zenseact

  // OpenGL
  std::vector<float> vertices;
  std::vector<float> colors[3]; // height, dist, intensity

  struct point {
    float x;
    float y;
    float z;
  };
  typedef std::vector<point> points_raw;
/*    int inten;
    int az;   // RAD/10000
    int el;   // RAD/10000
    float d;
*/
  // the following vector has the same size as "points_raw" vector, it will be given to GPU
  struct point_rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  };
  typedef std::vector<point_rgb> points_rgb;

  enum point_class { road, below_road, above_road, lane_marker };
  struct point_sem {
    float x;
    float y;
    float z;
    int inten;
    int az;   // RAD/10000
    int el;   // RAD/10000
    float d;
    point_class pclass;
  };
  typedef std::vector<point> points_semantic;

  enum color_coding { cc_height, cc_distance }; // --> to visualization ?
  color_coding colorcoding = cc_height;
  int intensityclamp[2] = { 0, 10000 };

/* HRL specific
  ===============
  max col + row differ for scan patterns, e.g.
  
  Berlin 2492 x 1080,
  VW          x 810,
  Volvo  1700 x 1070

  however not all opf these col and rows are used
  also the acquisition is rather by oversampling a line.
  so point cloud is unordered (column wise) 
  next up: order/sort the unordered hrl point cloud
*/
#define MAXCOL 2500
#define MAXROW 1200

#define MAXUSEDROW 80
#define MAXUSEDCOL 1700

  int points_set[MAXROW + 1][MAXCOL + 1];
  int rows_set[MAXROW + 1];
  int cols_set[MAXCOL + 1];
  int image[MAXUSEDROW][MAXUSEDCOL];
  // whoooo big data coming
/*  struct point_ord {
    float x;
    float y;
    float z;
    float d;
    float az; // <- int
    float el; // <- int
    int inten;
  };
  point_ord points_ordered[MAXUSEDROW][MAXUSEDCOL]; 
  */

  class pointbase {
  };
  
  class pointcloud : public pointbase {
  public:
    int frameid;
    int numpoints;
    bool ordered;
    int numrows;
    int numcols;
    points_raw pts_raw;
    points_rgb pts_rgb;
    points_raw* pts_ord; // optional

    int size(); // = numpoints = width * height
    float loadtime();
    vec4 bbox;
  };
  
  class pointcloud_HRL : public pointcloud {
    void sort();
    // to_pointcloud2()
  };

  int pointcloud::size() {
    return this->numpoints;
  }

  float pointcloud::loadtime() {
    return 0; // replace dummy
  }

}
#endif // POINTCLOUD_H
