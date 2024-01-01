#pragma once

//#include "kitty_evaluate_object.hpp"

//#include "Vec3f.hxx" // to be included after pcl/eigen
#include "linmath.h"
#include "CSV.hxx"
#include <vector>

#ifdef _WIN32				// nur für Windows
#include <windows.h> // read_directory()
#else
#include <filesystem>
///namespace fsys = std::filesystem;
#endif
#include <iostream>
#include <fstream>
#include <sstream> // save

#include <string>

#include <time.h>

//bool b_Filechanged = false;

//std::vector<std::string> v_argv; // vector of .pcd (.bin) filenames, passed by argument

//int iFile = 0;                   // current position in that v_argv vector of .pcd files

typedef std::vector<std::string> stringvec;
//stringvec v_files;
struct PointXYZ
{
  float x;
  float y;
  float z;
};
typedef std::vector<PointXYZ> PointCloud;

struct PointXYZI
{
  float x;
  float y;
  float z;
  float intensity;
};
typedef std::vector<PointXYZI> PointCloudI;

///PointCloudI * p_cloud; // later move to hrl_fw.h
PointCloudI cloud;


enum e_pointcloudfiletype { ft_unknown=0, ft_PCD=1, ft_BIN=2, ft_CSV=3, ft_XYZ=4 };
// pointcloud file being loaded
struct s_pointcloudfile
{
  std::string filepath;  // c:/leckere/erdbeermarmelade.honig
  std::string pathname;  // c:/leckere/
  std::string filename;  // erdbeermarmelade.honig
  std::string file_ext;  // .honig
  e_pointcloudfiletype ft;
};


int loadPCDbin(const std::string &filename, PointCloudI &cloud)
{
  // https://stackoverflow.com/questions/19614581/reading-floating-numbers-from-bin-file-continuosly-and-outputting-in-console-win

  float f;
  std::ifstream fin(filename, std::ios::binary);
  int i = 0;
  PointXYZI bincoord;
  std::vector<PointXYZI> bincloud;
  while (fin.read(reinterpret_cast<char*>(&f), sizeof(float)))
  {
    if (i == 0) bincoord.x = f;
    if (i == 1) bincoord.y = f;
    if (i == 2) bincoord.z = f;
    if (i == 3) bincoord.intensity = f;
    i++;
    if (i == 4)
    {
      bincloud.push_back(bincoord);
      i = 0;
    }
  }
  fin.close();
  for (int i = 0; i < bincloud.size(); i++)
  {
    PointXYZI p;
    p.x = bincloud[i].x;
    p.y = bincloud[i].y;
    p.z = bincloud[i].z;
    cloud.push_back(p);
  }
  return 0;
}

int loadPCDFile(const std::string &filename, PointCloudI &cloud) // this can only read ASCII content files!, takes 0.8sec to load a 5MB .PCD (release build)
{
  FILE *f;
  char line[255];
  char tmp0[55];
  char tmp1[55];
  char tmp2[55];
  char tmp3[55];
  float coord[4];
  bool bheader = true;

///  fopen_s(&f, filename.c_str(), "r");
  f = fopen(filename.c_str(), "r");
  while (fgets(line, sizeof(line), f))
  {
    if (bheader) sscanf(line, "%s %s %s %s", tmp0, tmp1, tmp2, tmp3);

    if (!bheader)
    {
//      sscanf(line, "%f %f %f %f", &coord[0], &coord[1], &coord[2]);
      sscanf(line, "%f %f %f %f", &coord[0], &coord[1], &coord[2], &coord[3]);
      PointXYZI p;
      p.x = coord[0];
      p.y = coord[1];
      p.z = coord[2];
      p.intensity = coord[3];
      cloud.push_back(p);
    }

    if (bheader)
      if (std::string(tmp0) == "DATA")// && (std::string(tmp2) == "ascii")) // omitting the "ascii" seems considerably faster !?
        bheader = false;
  }
  fclose(f);
  std::cout << "done with loading " << filename << std::endl;
  return 0;
}

int loadCSV_from_AEye_Endor(const std::string &filename, PointCloudI &cloud)
{
  std::ifstream file(filename);

  for (CSVIterator loop(file); loop != CSVIterator(); ++loop)
  {
    PointXYZI p;
    p.x = ::atof((*loop)[2].c_str());
    p.y = ::atof((*loop)[3].c_str());
    p.z = ::atof((*loop)[4].c_str());
    p.intensity = ::atof((*loop)[11].c_str());
    cloud.push_back(p);
  }
  return 0;
}

int loadCSV_from_VeloView(const std::string &filename, PointCloudI &cloud)
{
  std::ifstream file(filename);

  for (CSVIterator loop(file); loop != CSVIterator(); ++loop)
  {
    PointXYZI p;
    p.x = ::atof((*loop)[0].c_str());
    p.y = ::atof((*loop)[1].c_str());
    p.z = ::atof((*loop)[2].c_str());
    p.intensity = ::atof((*loop)[3].c_str());
    cloud.push_back(p);
  }
  return 0;
}

bool isPointcloudFileExtension(const std::string &filename_only)
{
#ifdef _WIN32				// nur für Windows
//  std::string file_ext = PathFindExtension(filename_only.c_str());
//  return (
//    (file_ext.compare(".bin") == 0) ||
//    (file_ext.compare(".pcd") == 0) ||
//    (file_ext.compare(".csv") == 0)
//    );
  return true;
#else
// 2do: fix this!
  return true;
#endif
}

float loadPointCloud(const std::string &filename, PointCloudI &cloud) // return load time [sec]
{
  std::cout << "loading" << filename << std::endl;
  s_pointcloudfile pf;

  clock_t begin_time = clock();
//  begin_time = clock();

#ifdef _WIN32				// nur für Windows
/*  pf.filepath = filename;
  std::string s = PathFindFileName(filename.c_str());
  pf.pathname = filename.substr(0, filename.find(s));
  pf.filename = s.substr(0, s.find_last_of("."));        // e.g. "veloout"
  pf.file_ext = PathFindExtension(filename.c_str());     // e.g. ".pcd"
  if (pf.file_ext.compare(".bin") == 0) pf.ft = ft_BIN;
  if (pf.file_ext.compare(".pcd") == 0) pf.ft = ft_PCD;
  if (pf.file_ext.compare(".csv") == 0) pf.ft = ft_CSV;  // Veloview output
  if (pf.file_ext.compare(".xyz") == 0) pf.ft = ft_XYZ;  // e.g. Cirrus dataset
*/  pf.ft = ft_PCD; // 2do!!
#else
///  fsys::path p = fsys::path(filename.c_str()); // C++17 :-)
  pf.ft = ft_PCD; // 2do!!
#endif
//  pclfile = pf; // set global var!

  if (pf.ft == ft_BIN)
  {
    if (loadPCDbin(filename, cloud) == -1)
    {
//      PCL_ERROR("Couldn't read file .bin\n");
      system("pause");
      return (-1);
    }
  }
  if (pf.ft == ft_PCD)
  {
//    if (pcl::io::loadPCDFile<pcl::PointXYZ>(filename, *p_cloud) == -1)
    std::cout << "loading pcd: " << filename << std::endl;
    if (loadPCDFile(filename, cloud) == -1)
    {
//      PCL_ERROR("Couldn't read file .pcd\n");
      system("pause");
      return (-1);
    }
  }
  if (pf.ft == ft_CSV)
  {
    // there is no autoselect which csv-loader to use
    if (loadCSV_from_AEye_Endor(filename, cloud) == -1)
//    if (loadCSV_from_VeloView(filename, cloud) == -1)
    {
//      PCL_ERROR("Couldn't read file .csv\n");
      system("pause");
      return (-1);
    }
  }
  if (pf.ft == ft_XYZ)
  {
    if (loadCSV_from_VeloView(filename, cloud) == -1)
    {
      //      PCL_ERROR("Couldn't read file .csv\n");
      system("pause");
      return (-1);
    }
  }
  std::cout << "Loaded " << cloud.size() << " data points from .pcd/.bin with the following fields: " << std::endl;
//  std::cout << "Load__ = " << float(clock() - begin_time) / CLOCKS_PER_SEC << " [s]" << std::endl;
  return float(clock() - begin_time) / CLOCKS_PER_SEC;
}

int savePCLbin(const std::string &filename, PointCloud &cloud)
{
  std::ofstream fout(filename, std::ios::binary);
  for (int j = 0; j < cloud.size(); j++)
  {
    PointXYZI pi{255,255,255,255};
    pi.x = cloud[j].x;
    pi.y = cloud[j].y;
    pi.z = cloud[j].z;
    float f_p[4];
    f_p[0] = pi.x;
    f_p[1] = pi.y;
    f_p[2] = pi.z;
    f_p[3] = pi.intensity;
    fout.write(reinterpret_cast<char*>(&f_p), sizeof(f_p));
  }
  fout.close();
  std::cout << "Pointcloud written to " << filename << std::endl;
  return 0;
}

#if 0
int saveLabel(const std::string &filename) // store objects (start with cars) ín Kitty Label Format
{
  std::ofstream outfile;
  outfile.open(filename);

  /*
    example:                  Darknet:
    =======                   =======
    obj_class   Car     vs.   class: class of object, in integer starting from 0
    truncation  0.00
    occulation  2
    alpha      -1.55
    x1        548.00    vs.   x, y: relative position of the center point of the rectangle, which are two floats range in (0, 1)
    y1        171.33
    x2        572.40
    y2        194.42
    h           1.48    vs.   h: relative height of the cuboid, which is float range in (0, 1)
    w           1.56    vs.   w, l: relative size of the rectangle, which are also two floats range in (0, 1)
    l           3.62
    y          -2.72
    z           0.82    vs.   z: relative height of the center of the cuboid, which is float range in (0, 1)
    x          48.22
    rz         -1.62    vs.   rz: orientation, which range in (-pi/2, pi/2)
  */

  stringstream stream;
  // check!!:  ry --> rz
  //           t1 --> x, t2 --> y, t3 --> z
  for (int i = 0; i < v_BBoxes.size(); i++)
  {
    if (v_BBoxes[i].objtype > obj_unknown)
    {
      tGroundtruth lbl;
      switch (v_BBoxes[i].objtype)
      {
      case tree:       lbl.box.type = "__Tree"; break; // not supported yet
      case car:        lbl.box.type = "Car";  break;
      case truck:      lbl.box.type = "__Truck"; break;
      case guardrail:  lbl.box.type = "__Guardrail"; break;
      case pedestrian: lbl.box.type = "Pedestrian";  break;
      }
      lbl.t1 = v_BBoxes[i].mass_center(0);
      lbl.t2 = v_BBoxes[i].mass_center(1);
      lbl.t3 = v_BBoxes[i].mass_center(2);
      lbl.h = v_BBoxes[i].OBB.h; // AUTOSAR: w ... 2do: fix!!
      lbl.w = v_BBoxes[i].OBB.w; // AUTOSAR: h ... 2do: fix!!
      lbl.l = v_BBoxes[i].OBB.l;
      lbl.ry = 0.0f;
      stream << lbl.box.type << " " <<
        std::fixed << setprecision(2) <<
        lbl.truncation << " " << lbl.occlusion << " " << lbl.box.alpha << " " <<
        lbl.box.x1 << " " << lbl.box.y1 << " " << lbl.box.x2 << " " << lbl.box.y2 << " " <<
        lbl.h << " " << lbl.w << " " << lbl.l << " " <<
        lbl.t1 << " " << lbl.t2 << " " << lbl.t3 << " " << lbl.ry << "\n"; // https://stackoverflow.com/questions/5373766/adding-a-newline-to-file-in-c
    }
  }
  outfile.write(stream.str().c_str(), stream.str().length());
  outfile.close();

  std::cout << "Labels written to " << filename << std::endl;
  return 0;
}
#endif

// return angle of plane normal vN to vUp (if vN is facing upward) or vDown
typedef float float32; // Linux
float32 planeAngleRAD(vec3 vN)
{
  // https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
//  Vec3f vN = Vec3f(plane.coeff[0], plane.coeff[1], plane.coeff[2]);
  vec3 vUp = {0,0,0};
  if (vN[2] > 0.0) vUp[2] = 1; else vUp[2] = -1; // vDown ... also das ist wirklich unpraktisch an der vec<n> struktur 
  float fdot = vec3_mul_inner(vN, vUp); // dot-product
  float fLenN = vec3_len(vN);
  float fLenUp = vec3_len(vUp);
  float fAngleRAD = acos(fdot / sqrt(fLenN * fLenUp));
  //  float fAngle = fAngleRAD* 180.0f / M_PI;
  return fAngleRAD;
}

std::string GetDirectory(const std::string& path)
{
  size_t found = path.find_last_of("/\\");
  return(path.substr(0, found));
}
#ifdef _WIN32				// nur für Windows
// http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
void read_directory(const std::string& name, stringvec& v)
{
  std::string pattern(name);
  pattern.append("\\*");
  WIN32_FIND_DATA data;
  HANDLE hFind;
  if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
    do {
      std::string fname = data.cFileName;
      if (
        (fname.compare(".") != 0) &&
        (fname.compare("..") != 0)
        )
        v.push_back(data.cFileName);
    } while (FindNextFile(hFind, &data) != 0);
    FindClose(hFind);
  }
}
#endif
