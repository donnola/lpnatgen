#include "desc.h"

inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::float3& v)
{
  return out << v.x << ' ' << v.y << ' ' << v.z << "\n";
}

inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::Vertex& v)
{
  return out << v.vi << '/' << v.vti << '/' << v.vni;
}

std::ostream& lpng::operator<<(std::ostream& out, const lpng::Face& f)
{
  out << 'f';
  for (const Vertex& v : f)
  {
    out << ' ' << v;
  }
  return out << "\n";
}
