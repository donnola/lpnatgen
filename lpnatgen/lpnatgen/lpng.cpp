#define _USE_MATH_DEFINES

#include "lpng.h"
#include "lpng_math.h"
#include <filesystem>
#include <stack>


std::vector<lpng::Mesh> lpng::GenerateObject::GetModel()
{
  PolygonDecomposition();
  return model;
}


void lpng::GenerateObject::Generate()
{
  //fast_lpng_rand(1780338363);
  seed = get_lpng_seed();
  GenerateMesh();
  PolygonDecomposition();
  GenerateNormals();
  GenerateTextureCoords();
}


void lpng::GenerateObject::GenerateNormals()
{
  for (Mesh& mesh : model)
  {
    std::vector<float3> normals = CalculateObjNormals(mesh);

    mesh.vertexNormals = std::move(normals);
    for (Face& f : mesh.faces)
    {
      f.vni = f.vi;
    }
  }
}


void lpng::GenerateObject::GenerateTextureCoords()
{
  for (Mesh& mesh : model)
  {
    mesh.vertexTextCoords.emplace_back(0.f, 0.f);
    mesh.vertexTextCoords.emplace_back(0.9, 0.f);
    mesh.vertexTextCoords.emplace_back(0.f, 0.9);

    for (Face& f : mesh.faces)
    {
      for (size_t i = 0; i < 3; ++i)
      {
        f.vti = {1, 2, 3};
      }
    }
  }
}


void lpng::GenerateObject::PolygonDecomposition()
{
  for (Mesh& mesh : model)
  {
    DecomposeObj(mesh);
  }
}


void lpng::GenerateObject::SaveModel(std::string file_name, std::string save_path) const
{

}


void lpng::GenerateObject::SaveModel() const
{
  std::string file_name = "test_object";
  std::string save_path = std::filesystem::current_path().string();
}


void lpng::GenerateObject::AddObject(Mesh& mesh)
{
  model.push_back(std::move(mesh));
}


void lpng::GenerateObjectPlant::RelaxBranch(Branch& branch, size_t meshId)
{
  if (branch.level == 0)
  {
    BranchRing& p_ring = branch.rings.front();
    BranchRing ring = BranchRing();
    float seg_len = branch.length / 5.f;
    ring.curLength = p_ring.curLength - seg_len;
    ring.rad = p_ring.rad * 1.1;
    ring.vecIn = {0,1,0};
    ring.vecOut = p_ring.vecOut;
    ring.center = p_ring.center - p_ring.vecIn * seg_len;
    p_ring.vecIn = p_ring.vecOut;
    Mesh& mesh = model[meshId];
    ring.facesIds = Extrude(mesh, p_ring.facesIds, -seg_len * p_ring.vecIn);
    p_ring.facesIds.clear();
    ring.vertexesIds = GetVertexesIds(mesh, ring.facesIds);
    ScaleVertexes(mesh, float3(ring.rad / p_ring.rad, 1.f, ring.rad / p_ring.rad), ring.vertexesIds);
    branch.rings.insert(branch.rings.begin(), std::move(ring));
  }
  for (size_t i = 0; i < branch.rings.size(); ++i)
  {
    BranchRing& ring = branch.rings[i];
    float3 normal = i == 0 ? Normalized(ring.vecIn) : Normalized(ring.vecIn + ring.vecOut);
    float angle = Angle(float3(0, 1, 0), normal);
    if (angle > FLT_EPSILON)
    {
      Quat q(Cross(float3(0, 1, 0), normal), angle);
      RotateVertexes(model[meshId], ring.vertexesIds, q, ring.center);
    }
  }
}


void lpng::GenerateObjectPlant::GenerateBranch(Branch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut)
{
  Mesh mesh;
  mesh.matType = MaterialTypes::WOOD;
  float seg_angle = 2 * M_PI / branch.edgeBase;
  float last_seg_len = branch.length / 7;
  // create first ring
  BranchRing ring;
  ring.center = pointStart;
  ring.rad = branch.baseRad;
  ring.curLength = 0;
  ring.vecIn = vecIn;
  ring.vecOut = vecOut == float3() ? GenOutVec(vecIn, 0, 15) : vecOut;

  for (size_t i = 0; i < branch.edgeBase; ++i)
  {
    float3 vertex = ring.center + float3(sin(i * seg_angle) * ring.rad, 0, cos(i * seg_angle) * ring.rad);
    mesh.vertexCoords.push_back(vertex);
  }
  mesh.vertexCoords.push_back(ring.center);
  int rootId = mesh.vertexCoords.size();
  for (int i = 0; i < rootId - 1; ++i)
  {
    mesh.faces.push_back(Face({ i + 1, int((i + 1) % branch.edgeBase) + 1, rootId }));
    ring.facesIds.push_back(mesh.faces.size() - 1);
  }
  ring.vertexesIds = GetVertexesIds(mesh, ring.facesIds);
  branch.rings.push_back(std::move(ring));

  while (branch.rings.back().curLength < branch.length)
  {
    BranchRing& p_ring = branch.rings.back();
    ring = BranchRing();
    float seg_len = std::max((branch.length - p_ring.curLength) / 5 * 2, last_seg_len);
    ring.curLength = p_ring.curLength + seg_len;
    ring.rad = (branch.baseRad - branch.lastRad) * (1 - std::min(ring.curLength / branch.length, 1.f)) + branch.lastRad;

    ring.vecIn = p_ring.vecOut;
    ring.vecOut = GenOutVec(Normalized(ring.vecIn + float3(0, 1, 0) * upCoef), 0, 15);
    ring.center = p_ring.center + ring.vecIn * seg_len;

    if (branch.rings.size() == 1)
      ring.facesIds = ExtrudeWithCap(mesh, p_ring.facesIds, ring.vecIn * seg_len);
    else
    {
      ring.facesIds = Extrude(mesh, p_ring.facesIds, ring.vecIn * seg_len);
      p_ring.facesIds.clear();
    }
    ring.vertexesIds = GetVertexesIds(mesh, ring.facesIds);
    ScaleVertexes(mesh, float3(ring.rad / p_ring.rad, 1.f, ring.rad / p_ring.rad), ring.vertexesIds);
    branch.rings.push_back(std::move(ring));
  }

  branches.push_back(std::move(branch));
  model.push_back(std::move(mesh));
}


int lpng::GenerateObjectPlant::SelectWeightedBranch()
{
  size_t weight_sum = 0;
  for (Branch& branch : branches)
  {
    weight_sum += branch.weight;
  }
  if (weight_sum == 0)
    return -1;
  size_t w = fast_lpng_rand(0, weight_sum);
  size_t w_cum = 0;
  for (size_t i = 0; i < branches.size(); ++i)
  {
    w_cum += branches[i].weight;
    if (w_cum >= w)
    {
      return i;
    }
  }
}


void lpng::GenerateObjectPlant::InitBranch(const size_t parent_id, Branch& branch, float3& point_start, float3& vec_in)
{
  Branch& parent = branches[parent_id];
  branch.level = parent.level + 1;
  float c_start = fast_lpng_rand(branchMinCoefStart * 10000, branchMaxCoefStart * 10000) / 10000.f;
  float l_start = parent.length * c_start;
  float c_len = fast_lpng_rand(branchMinCoefLen * 1000, branchMaxCoefLen * 1000) / 1000.f;
  branch.length = (parent.length - l_start) * c_len;
  branch.baseRad = (parent.rings.front().rad - parent.rings.back().rad) * (1.f - c_start) + parent.rings.back().rad;
  branch.baseRad *= 0.9;
  branch.lastRad = std::min(branch.baseRad / 2, finalBranchRad);
  branch.edgeBase = branchBase;
  float weight_r = parent.weight / parent.baseRad * branch.baseRad * 0.8;
  float weight_l = parent.weight / parent.length * branch.length * 0.8;
  branch.weight = size_t((weight_r + weight_l) / 2);
  if (branch.weight > 0)
    branch.freeDirections = GetNDirections(branchBase);
  point_start = parent.rings.front().center;
  for (size_t i = 0; i < parent.rings.size(); ++i)
  {
    const BranchRing& ring = parent.rings[i];
    if (ring.curLength < l_start)
      continue;
    float d = ring.curLength - l_start;
    point_start = ring.center - Normalized(ring.vecIn) * d;
    vec_in = GenOutVec(parent.freeDirections, Normalized(ring.vecIn), 40, 60);
    if (parent.freeDirections.size() == 0)
      parent.weight = 0;
    break;
  }
}


inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::float3& v)
{
  return out << v.x << ' ' << v.y << ' ' << v.z << "\n";
}


inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::float2& v)
{
  return out << v.x << ' ' << v.y << "\n";
}


inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::Vertex& v)
{
  return out << v.vi << '/' << v.vti << '/' << v.vni;
}


std::ostream& lpng::operator<<(std::ostream& out, const lpng::Face& f)
{
  out << 'f';
  for (int i = 0; i < f.vi.size(); ++i)
  {
    Vertex v(f.vi[i], f.vti[i], f.vni[i]);
    out << ' ' << v;
  }
  return out << "\n";
}
