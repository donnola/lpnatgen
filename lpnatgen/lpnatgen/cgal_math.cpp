#include "cgal_math.h"
#include <algorithm>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Surface_mesh.h>


lpng::Mesh lpng::BuildMeshWithPoints(const std::vector<float3>& input_points)
{
  std::vector<Facet> facets;
  std::vector<Point_3> points;
  for (const float3 &p : input_points)
    points.emplace_back(p.x, p.y, p.z);

  CGALMesh m;
  Construct construct(m, points.begin(), points.end());
  CGAL::advancing_front_surface_reconstruction(points.begin(), points.end(), construct, double(500), double(1.57));
  
  Mesh mesh;
  std::vector<float3> vertexCoords;
  for (CGALMesh::Vertex_index vi : m.vertices())
  {
    Point_3 pt = m.point(vi);
    mesh.vertexCoords.emplace_back(pt.x(), pt.y(), pt.z());
  }
  for (CGALMesh::Face_index face_index : m.faces())
  {
    CGALMesh::Surface_mesh::Halfedge_index hf = m.halfedge(face_index);
    std::vector<Vertex> face;
    for (CGALMesh::Surface_mesh::Halfedge_index hi : halfedges_around_face(hf, m))
    {
      CGALMesh::Surface_mesh::Vertex_index vi = target(hi, m);
      face.push_back(vi.id() + 1);
    }
    mesh.faces.push_back(std::move(face));
  }
  //FixMeshOrientation(mesh);
  return mesh;
}
