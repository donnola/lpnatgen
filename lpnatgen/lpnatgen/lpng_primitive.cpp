#include "lpng_primitive.h"
#include <algorithm>
#include <unordered_map>


lpng::Sphere* lpng::Sphere::p_instance = nullptr;

static struct CleanUp
{
  ~CleanUp()
  {
    lpng::Sphere::Destroy();
  }
} cleanup;


lpng::Sphere::Sphere()
{
  float phi = (1.0f + sqrt(5.0f)) * 0.5f;
  float a = 1.0f;
  float b = 1.0f / phi;
  float2 v(a, b);
  Normalize(v);
  v *= size_coef;
  a = v.x;
  b = v.y;
 
  sphere.vertexCoords.emplace_back(0, b, -a);
  sphere.vertexCoords.emplace_back(b, a, 0);
  sphere.vertexCoords.emplace_back(-b, a, 0);
  sphere.vertexCoords.emplace_back(0, b, a);
  sphere.vertexCoords.emplace_back(0, -b, a);
  sphere.vertexCoords.emplace_back(-a, 0, b);
  sphere.vertexCoords.emplace_back(0, -b, -a);
  sphere.vertexCoords.emplace_back(a, 0, -b);
  sphere.vertexCoords.emplace_back(a, 0, b);
  sphere.vertexCoords.emplace_back(-a, 0, -b);
  sphere.vertexCoords.emplace_back(b, -a, 0);
  sphere.vertexCoords.emplace_back(-b, -a, 0);

  sphere.faces.push_back(Face({ 3, 2, 1 }));
  sphere.faces.push_back(Face({ 2, 3, 4 }));
  sphere.faces.push_back(Face({ 6, 5, 4 }));
  sphere.faces.push_back(Face({ 5, 9, 4 }));
  sphere.faces.push_back(Face({ 8, 7, 1 }));
  sphere.faces.push_back(Face({ 7, 10, 1 }));
  sphere.faces.push_back(Face({ 12, 11, 5 }));
  sphere.faces.push_back(Face({ 11, 12, 7 }));
  sphere.faces.push_back(Face({ 10, 6, 3 }));
  sphere.faces.push_back(Face({ 6, 10, 12 }));
  sphere.faces.push_back(Face({ 9, 8, 2 }));
  sphere.faces.push_back(Face({ 8, 9, 11 }));
  sphere.faces.push_back(Face({ 3, 6, 4 }));
  sphere.faces.push_back(Face({ 9, 2, 4 }));
  sphere.faces.push_back(Face({ 10, 3, 1 }));
  sphere.faces.push_back(Face({ 2, 8, 1 }));
  sphere.faces.push_back(Face({ 12, 10, 7 }));
  sphere.faces.push_back(Face({ 8, 11, 7 }));
  sphere.faces.push_back(Face({ 6, 12, 5 }));
  sphere.faces.push_back(Face({ 11, 9, 5 }));

  for (int i = 1; i < 3; ++i)
  {
    Subdiv();
  }
}


void lpng::Sphere::RaiseToMinSubdiv(size_t min_point_count)
{
  while (vertex_num < min_point_count)
  {
    Subdiv();
  }
}


void lpng::Sphere::Subdiv()
{
  std::unordered_map<Edge, int, EdgeHash, EdgeEqual> edge_div;
  std::vector<Face> new_faces;
  for (const Face& f : sphere.faces)
  {
    float3 v1;
    int v1_id;
    float3 v2;
    int v2_id;
    float3 v3;
    int v3_id;

    if (edge_div.find(Edge(f.vi[0], f.vi[1])) != edge_div.end())
    {
      v1_id = edge_div[Edge(f.vi[0], f.vi[1])];
      v1 = sphere.vertexCoords[v1_id - 1];
    }
    else
    {
      v1 = (sphere.vertexCoords[f.vi[0] - 1] + sphere.vertexCoords[f.vi[1] - 1]) / 2;
      Normalize(v1);
      v1 *= size_coef;
      sphere.vertexCoords.push_back(v1);
      v1_id = sphere.vertexCoords.size();
      edge_div[Edge(f.vi[0], f.vi[1])] = v1_id;
    }


    if (edge_div.find(Edge(f.vi[1], f.vi[2])) != edge_div.end())
    {
      v2_id = edge_div[Edge(f.vi[1], f.vi[2])];
      v2 = sphere.vertexCoords[v2_id - 1];
    }
    else
    {
      v2 = (sphere.vertexCoords[f.vi[1] - 1] + sphere.vertexCoords[f.vi[2] - 1]) / 2;
      Normalize(v2);
      v2 *= size_coef;
      sphere.vertexCoords.push_back(v2);
      v2_id = sphere.vertexCoords.size();
      edge_div[Edge(f.vi[1], f.vi[2])] = v2_id;
    }
    

    if (edge_div.find(Edge(f.vi[2], f.vi[0])) != edge_div.end())
    {
      v3_id = edge_div[Edge(f.vi[2], f.vi[0])];
      v3 = sphere.vertexCoords[v3_id - 1];
    }
    else
    {
      v3 = (sphere.vertexCoords[f.vi[2] - 1] + sphere.vertexCoords[f.vi[0] - 1]) / 2;
      Normalize(v3);
      v3 *= size_coef;
      sphere.vertexCoords.push_back(v3);
      v3_id = sphere.vertexCoords.size();
      edge_div[Edge(f.vi[2], f.vi[0])] = v3_id;
    }

    new_faces.push_back(Face({ f.vi[0], v1_id, v3_id }));
    new_faces.push_back(Face({ f.vi[1], v2_id, v1_id }));
    new_faces.push_back(Face({ f.vi[2], v3_id, v2_id }));
    new_faces.push_back(Face({ v1_id, v2_id, v3_id }));
  }
  sphere.faces = new_faces;
  ++smooth_level;
  vertex_num = sphere.vertexCoords.size();
}
