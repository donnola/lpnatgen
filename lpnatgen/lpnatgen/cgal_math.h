#pragma once
#include "lpng_math.h"
#include <CGAL/Surface_mesh.h>
#include <CGAL/Simple_cartesian.h>


typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3  Point_3;
typedef std::array<std::size_t, 3> Facet;
typedef CGAL::Surface_mesh<Point_3> CGALMesh;

struct Construct
{
  CGALMesh& mesh;
  template < typename PointIterator>
  Construct(CGALMesh& mesh, PointIterator b, PointIterator e)
    : mesh(mesh)
  {
    for (; b != e; ++b)
    {
      boost::graph_traits<CGALMesh>::vertex_descriptor v;
      v = add_vertex(mesh);
      mesh.point(v) = *b;
    }
  }
  Construct& operator=(const Facet f)
  {
    typedef boost::graph_traits<CGALMesh>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<CGALMesh>::vertices_size_type size_type;
    mesh.add_face(vertex_descriptor(static_cast<size_type>(f[0])),
      vertex_descriptor(static_cast<size_type>(f[1])),
      vertex_descriptor(static_cast<size_type>(f[2])));
    return *this;
  }
  Construct&
    operator*() { return *this; }
  Construct&
    operator++() { return *this; }
  Construct
    operator++(int) { return *this; }
};


namespace lpng
{
   Mesh BuildMeshWithPoints(const std::vector<float3>& points);
}
