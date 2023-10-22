#pragma once

#include "lpmath.h"

#include <vector>
#include <ostream>

namespace lpng
{
  struct Vertex
  {
    int vi;
    int vti;
    int vni;

    Vertex(int a, int b, int c) : vi(a), vti(b), vni(c) {}
    Vertex(int a, int b) : vi(a), vti(b), vni(-1) {}
    Vertex(int a) : vi(a), vti(-1), vni(-1) {}
  };

  using Face = std::vector<Vertex>;

  struct Object
  {
    std::vector<Face> faces;
    std::vector<float3> vertexCoords;
    std::vector<float3> vertexTextCoords;
    std::vector<float3> vertexNormals;
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
