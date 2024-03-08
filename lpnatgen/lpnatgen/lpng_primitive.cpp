#include "lpng_primitive.h"
#include <algorithm>
#include <unordered_map>


lpng::Sphere* lpng::Sphere::p_instance = nullptr;

lpng::Sphere::Sphere()
{
  float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
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

  sphere.faces.push_back(Face{ 3, 2, 1 });
  sphere.faces.push_back(Face{ 2, 3, 4 });
  sphere.faces.push_back(Face{ 6, 5, 4 });
  sphere.faces.push_back(Face{ 5, 9, 4 });
  sphere.faces.push_back(Face{ 8, 7, 1 });
  sphere.faces.push_back(Face{ 7, 10, 1 });
  sphere.faces.push_back(Face{ 12, 11, 5 });
  sphere.faces.push_back(Face{ 11, 12, 7 });
  sphere.faces.push_back(Face{ 10, 6, 3 });
  sphere.faces.push_back(Face{ 6, 10, 12 });
  sphere.faces.push_back(Face{ 9, 8, 2 });
  sphere.faces.push_back(Face{ 8, 9, 11 });
  sphere.faces.push_back(Face{ 3, 6, 4 });
  sphere.faces.push_back(Face{ 9, 2, 4 });
  sphere.faces.push_back(Face{ 10, 3, 1 });
  sphere.faces.push_back(Face{ 2, 8, 1 });
  sphere.faces.push_back(Face{ 12, 10, 7 });
  sphere.faces.push_back(Face{ 8, 11, 7 });
  sphere.faces.push_back(Face{ 6, 12, 5 });
  sphere.faces.push_back(Face{ 11, 9, 5 });

  for (int i = 1; i < 3; ++i)
  {
    RaiseSmoothness();
  }
}


void lpng::Sphere::RaiseToMinSmoothness(int min_point_count)
{
  while (vertex_num < min_point_count)
  {
    RaiseSmoothness();
  }
}


void lpng::Sphere::RaiseSmoothness()
{
  std::unordered_map<Edge, size_t, EdgeHash, EdgeEqual> edge_div;
  std::vector<Face> new_faces;
  for (const Face& f : sphere.faces)
  {
    float3 v1;
    size_t v1_id;
    float3 v2;
    size_t v2_id;
    float3 v3;
    size_t v3_id;

    if (edge_div.find(Edge(f[0].vi, f[1].vi)) != edge_div.end())
    {
      v1_id = edge_div[Edge(f[0].vi, f[1].vi)];
      v1 = sphere.vertexCoords[v1_id - 1];
    }
    else
    {
      v1 = (sphere.vertexCoords[f[0].vi - 1] + sphere.vertexCoords[f[1].vi - 1]) / 2;
      Normalize(v1);
      v1 *= size_coef;
      sphere.vertexCoords.push_back(v1);
      v1_id = sphere.vertexCoords.size();
      edge_div[Edge(f[0].vi, f[1].vi)] = v1_id;
    }


    if (edge_div.find(Edge(f[1].vi, f[2].vi)) != edge_div.end())
    {
      v2_id = edge_div[Edge(f[1].vi, f[2].vi)];
      v2 = sphere.vertexCoords[v2_id - 1];
    }
    else
    {
      v2 = (sphere.vertexCoords[f[1].vi - 1] + sphere.vertexCoords[f[2].vi - 1]) / 2;
      Normalize(v2);
      v2 *= size_coef;
      sphere.vertexCoords.push_back(v2);
      v2_id = sphere.vertexCoords.size();
      edge_div[Edge(f[1].vi, f[2].vi)] = v2_id;
    }
    

    if (edge_div.find(Edge(f[2].vi, f[0].vi)) != edge_div.end())
    {
      v3_id = edge_div[Edge(f[2].vi, f[0].vi)];
      v3 = sphere.vertexCoords[v3_id - 1];
    }
    else
    {
      v3 = (sphere.vertexCoords[f[2].vi - 1] + sphere.vertexCoords[f[0].vi - 1]) / 2;
      Normalize(v3);
      v3 *= size_coef;
      sphere.vertexCoords.push_back(v3);
      v3_id = sphere.vertexCoords.size();
      edge_div[Edge(f[2].vi, f[0].vi)] = v3_id;
    }

    new_faces.push_back(Face{ f[0].vi, v1_id, v3_id });
    new_faces.push_back(Face{ f[1].vi, v2_id, v1_id });
    new_faces.push_back(Face{ f[2].vi, v3_id, v2_id });
    new_faces.push_back(Face{ v1_id, v2_id, v3_id });
  }
  sphere.faces = new_faces;
  ++smooth_level;
  vertex_num = sphere.vertexCoords.size();
}

lpng::Mesh lpng::GenerateMeshFromSphere(const std::set<size_t>& points_ids)
{
  Sphere* sphere = Sphere::GetInstance();
  Mesh mesh = sphere->GetSphere();
  for (int i = 0; i < mesh.vertexCoords.size(); ++i)
  {
    if (auto it = points_ids.find(i); it == points_ids.end())
    {
      std::vector<size_t> faces = FindFacesInMesh(mesh, i+1);
      std::sort(faces.begin(), faces.end(), std::greater<size_t>());
      std::vector<Edge> edges;
      for (size_t j : faces)
      {
        const Face& f = mesh.faces[j];
        int v_id = std::find_if(f.begin(), f.end(), [&](Vertex v) { return (i + 1) == v.vi; }) - f.begin();
        Edge e(f[(v_id + 1) % f.size()].vi, f[(v_id + 2) % f.size()].vi);
        if (edges.empty())
          edges.push_back(e);
        else
        {
          auto e_it1 = std::find_if(edges.begin(), edges.end(), [&](Edge edge) { return e.first == edge.second; });
          auto e_it2 = std::find_if(edges.begin(), edges.end(), [&](Edge edge) { return e.second == edge.first; });
          if (e_it1 != edges.end())
            edges.insert(e_it1 + 1, e);
          else if (e_it2 != edges.end())
            edges.insert(e_it2, e);
          else
            edges.push_back(e);
        }
      }
      for (int j : faces)
      {
        mesh.faces.erase(mesh.faces.begin() + j);
      }
      while (edges.size() > 3)
      {
        float min_angle = 7;
        int e = 0;
        for (int e1 = 0; e1 < edges.size(); ++e1)
        {
          int e2 = (e1 + 1) % edges.size();
          float3 a = mesh.vertexCoords[edges[e1].first - 1] - mesh.vertexCoords[edges[e1].second - 1];
          float3 b = mesh.vertexCoords[edges[e2].second - 1] - mesh.vertexCoords[edges[e2].first - 1];
          float angle = Angle(a, b);
          if (angle < min_angle)
          {
            min_angle = angle;
            e = e1;
          }
        }
        int e2 = (e + 1) % edges.size();
        Edge new_edge(edges[e].first, edges[e2].second);
        mesh.faces.push_back(Face{ edges[e].first, edges[e].second, edges[e2].second });
        if (e2 < e)
          std::swap(e, e2);
        edges.erase(edges.begin() + e2);
        edges.erase(edges.begin() + e);
        edges.insert(edges.begin() + e, new_edge);
      }
      mesh.faces.push_back(Face{ edges[0].first, edges[0].second, edges[1].second });
      edges.clear();
    }
  }

  return mesh;
}