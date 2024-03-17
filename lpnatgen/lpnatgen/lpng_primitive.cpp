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
    Subdiv();
  }
}


void lpng::Sphere::RaiseToMinSubdiv(int min_point_count)
{
  while (vertex_num < min_point_count)
  {
    Subdiv();
  }
}


void lpng::Sphere::Subdiv()
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

lpng::Mesh lpng::GenerateMeshFromSphere(const std::unordered_set<size_t>& vertexes_ids)
{
  Sphere* sphere = Sphere::GetInstance();
  Mesh mesh = sphere->GetSphere();
  std::unordered_set<size_t> error_vertexes;
  for (int i = 0; i < mesh.vertexCoords.size(); ++i)
  {
    if (auto it = vertexes_ids.find(i); it == vertexes_ids.end())
    {
      std::vector<size_t> faces_ids = FindFacesInMesh(mesh, i+1);
      if (faces_ids.size() < 3)
      {
        error_vertexes.insert(i);
        continue;
      }
      std::sort(faces_ids.begin(), faces_ids.end(), std::greater<size_t>());
      std::vector<Edge> edges;
      for (size_t j : faces_ids)
      {
        const Face& f = mesh.faces[j];
        int v_id = std::find_if(f.begin(), f.end(), [&](Vertex v) { return (i + 1) == v.vi; }) - f.begin();
        edges.emplace_back(f[(v_id + 1) % f.size()].vi, f[(v_id + 2) % f.size()].vi);
      }
      std::vector<Edge> prev_edges = edges;
      if (!SortEdges(edges))
      {
        error_vertexes.insert(i);
        continue;
      } 
      std::vector<Face> new_faces;
      CupFromEdges(mesh, new_faces, edges);
      mesh.faces.insert(mesh.faces.end(), new_faces.begin(), new_faces.end());
      for (int j : faces_ids)
      {
        mesh.faces.erase(mesh.faces.begin() + j);
      }
    }
  }
  DeleteUnusedVertexes(mesh);
  return mesh;
}


bool lpng::SortEdges(std::vector<Edge>& edges)
{
  std::vector<Edge> sorted_edges;
  sorted_edges.push_back(edges.back());
  edges.pop_back();
  while (edges.size() > 0)
  {
    const Edge& e = sorted_edges.back();
    auto e_it = std::find_if(edges.begin(), edges.end(), [&](Edge edge) { return e.second == edge.first; });
    if (e_it != edges.end())
    {
      sorted_edges.push_back(*e_it);
      edges.erase(e_it);
    }
    else
      return false;
  }
  edges = std::move(sorted_edges);
  return true;
}


void lpng::CupFromEdges(const Mesh& mesh, std::vector<Face>& faces, std::vector<Edge> edges)
{
  if (edges.size() > 3)
  {
    float min_angle = 7;
    int e = 0;
    float3 normal;
    int f_id = FindFaceWithEdge(mesh.faces, edges[0]);
    if (f_id >= 0)
      normal = FaceNormal(mesh, mesh.faces[f_id]);
    else
    {
      f_id = FindFaceWithEdge(faces, edges[0]);
      normal = FaceNormal(mesh, faces[f_id]);
    }
    for (int e1 = 1; e1 < edges.size() - 1; ++e1)
    {
      float3 a = mesh.vertexCoords[edges[0].first - 1] - mesh.vertexCoords[edges[0].second - 1];
      float3 b = mesh.vertexCoords[edges[e1].second - 1] - mesh.vertexCoords[edges[0].second - 1];
      float3 new_normal = Normalized(Cross(b, a));
      float angle = Angle(new_normal, normal);
      if (angle < min_angle)
      {
        min_angle = angle;
        e = e1;
      }
    }
    faces.push_back(Face{ edges[0].first, edges[0].second, edges[e].second });
    if (e == 1)
    {
      Edge new_edge(edges[0].first, edges[e].second);
      edges.erase(edges.begin() + e);
      edges.erase(edges.begin());
      edges.insert(edges.begin(), new_edge);
      CupFromEdges(mesh, faces, edges);
    }
    else if (e == edges.size() - 2)
    {
      Edge new_edge(edges[e].second, edges[0].second);
      edges.erase(edges.begin() + e + 1);
      edges.erase(edges.begin());
      edges.insert(edges.begin(), new_edge);
      CupFromEdges(mesh, faces, edges);
    }
    else
    {
      Edge new_edge1(edges[e].second, edges[0].second);
      Edge new_edge2(edges[0].first, edges[e].second);
      std::vector<Edge> edges1;
      std::vector<Edge> edges2;
      edges1.insert(edges1.end(), edges.begin() + 1, edges.begin() + e + 1);
      edges1.push_back(new_edge1);
      edges2.insert(edges2.end(), edges.begin() + e + 1, edges.end());
      edges2.push_back(new_edge2);
      edges.erase(edges.begin());
      CupFromEdges(mesh, faces, edges1);
      CupFromEdges(mesh, faces, edges2);
    }
  }
  else
  {
    faces.push_back(Face{ edges[0].first, edges[0].second, edges[1].second });
    edges.clear();
  }
  return;
}
