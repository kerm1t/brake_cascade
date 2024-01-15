//#include "common.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

//#include "obj_load.h"
//#include "yocto_math.h"
#include "linmath.h"

#include <vector>
#include <unordered_map>

#include <cstdio>

//using namespace yocto;
struct indexed_mesh
{
  unsigned int num_indices = 0;
  unsigned int num_streams;

//  std::vector<vec3> vertex_data; // TMP binding needs to be done sokol way ... s. -> "bind"
//  std::vector<vec3> normal_data; // TMP
//  std::vector<uint32_t> indices; // TMP

  vec3 bb_min;
  vec3 bb_max;
//  sg_bindings bind;
};

// useful things for an eventual future:
// https://github.com/jpcy/xatlas https://developer.blender.org/diffusion/B/browse/master/intern/mikktspace/mikktspace.h
bool load_obj_pos_norm(const char* file, indexed_mesh& out)
{
  fastObjMesh* m = fast_obj_read(file);

  if (!m) return false;
  if (m->face_count == 0) return false;
  if (m->position_count == 0) return false;
  if (m->normal_count == 0) return false; // we could compute normals, but right now, we don't

  std::vector<vec3> vertex_data;
  std::vector<vec3> normal_data;
  std::vector<uint32_t> indices;

  vertex_data.reserve(m->position_count);
  indices.reserve(m->position_count);

  uint32_t current_idx = 0;
  fastObjIndex* current_vtx = m->indices;
  out = indexed_mesh();
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
        vec3 temp;

        memcpy(&temp, &m->positions[idx_p * 3], sizeof(vec3));
        vertex_data.push_back(temp);
//        out.bb_min = min(out.bb_min, temp);
//        out.bb_max = max(out.bb_max, temp);

        memcpy(&temp, &m->normals[idx_n * 3], sizeof(vec3));
        normal_data.push_back(temp);

        objFaceVertexToMeshIndex[mergedIndices] = current_idx;
        indices.push_back(current_idx);
        current_idx++;
      }
      else
        indices.push_back(idx_merged->second);
    }
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
  out.num_indices = int(indices.size());
  return true;
}

