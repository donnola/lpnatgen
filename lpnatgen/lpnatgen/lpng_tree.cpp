#define _USE_MATH_DEFINES

#include "lpng_tree.h"


lpng::float3 lpng::GenerateObjectTree::GenOutVec(const float3& vecIn, int angleDelta, int add) const
{
  float3 vec(int(fast_lpng_rand() % 101 - 50), 0, int(fast_lpng_rand() % 101 - 50));
  Normalize(vec);
  if (vec == float3())
    vec.y = 1;
  else
  {
    int angle = fast_lpng_rand() % angleDelta + add;
    if (angle == 0)
      vec = float3(0, 1, 0);
    else
      vec.y = 1 / tan(DEG2RAD(angle));
  }
  float angle = Angle(float3(0, 1, 0), vecIn);
  if (angle > FLT_EPSILON)
  {
    Quat q(Cross(float3(0, 1, 0), vecIn), angle);
    vec = QuatTransformVec(q, vec);
  }
  return Normalized(vec);
}


size_t lpng::GenerateObjectTree::SelectWeightedBranch() const
{
  size_t w = fast_lpng_rand() % weightCum;
  size_t w_sum = 0;
  for (size_t i = 0; i < tree.size(); ++i)
  {
    w_sum += tree[i].weight;
    if (w_sum >= w)
    {
      return i;
    }
  }
}


void lpng::GenerateObjectTree::InitBranch(const size_t parent_id, TreeBranch& branch, float3& point_start, float3& vec_in)
{
  TreeBranch& parent = tree[parent_id];
  float c_start = int(fast_lpng_rand() % 400 + 300) / 1000.f;
  float l_start = parent.length * c_start;
  float c_len = int(fast_lpng_rand() % 600 + 800) / 1000.f;
  branch.length = (parent.length - l_start) * c_len;
  branch.rad = (parent.rings.front().rad - parent.rings.back().rad) * (1.f - c_start) + parent.rings.back().rad;
  branch.rad *= 0.9;
  size_t weight = branch.length < treeHeight / 8.f ? 0 : size_t(parent.weight / parent.rad * branch.rad);
  branch.weight = weight;
  weightCum -= parent.weight / 5;
  parent.weight = parent.weight / 5 * 4;
  point_start = parent.rings.front().center;
  for (size_t i = 0; i < parent.rings.size(); ++i)
  {
    const TreeRing& ring = parent.rings[i];
    if (ring.curLength < l_start)
      continue;
    float d = ring.curLength - l_start;
    point_start = ring.center - Normalized(ring.vecIn) * d;
    vec_in = GenOutVec(Normalized(ring.vecIn), 25, 25);
    break;
  }
}


void lpng::GenerateObjectTree::GenerateMesh()
{
  Mesh branches;
  GenerateBranch(mainBranch, { 0, 0, 0 }, {0, 1, 0});
  while (tree.size() < branchCount)
  {
    size_t parent_id = SelectWeightedBranch();
    TreeBranch branch;
    float3 point_start;
    float3 vec_in;
    InitBranch(parent_id, branch, point_start, vec_in);
    GenerateBranch(branch, point_start, vec_in, vec_in);
  }
  for (size_t i = 0; i < tree.size(); ++i)
  {
    RelaxBranch(tree[i], i);
  }
}


void lpng::GenerateObjectTree::GenerateBranch(TreeBranch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut)
{
  Mesh mesh;
  mesh.matType = MaterialTypes::STONE;
  float seg_angle = 2 * M_PI / segNum;
  float last_seg_len = branch.length / 7;
  float last_rad = branch.rad / 8;
  // create first ring
  TreeRing ring;
  ring.center = pointStart;
  ring.rad = branch.rad;
  ring.curLength = 0;
  ring.vecIn = vecIn;
  ring.vecOut = vecOut == float3() ? GenOutVec(vecIn, 25) : vecOut;

  for (size_t i = 0; i < segNum; ++i)
  {
    float3 vertex = ring.center + float3(sin(i * seg_angle) * ring.rad, 0, cos(i * seg_angle) * ring.rad);
    mesh.vertexCoords.push_back(vertex);
  }
  mesh.vertexCoords.push_back(ring.center);
  int rootId = mesh.vertexCoords.size();
  for (size_t i = 0; i < rootId - 1; ++i)
  {
    mesh.faces.push_back(Face({ i + 1, (i + 1) % segNum + 1, rootId }));
    ring.facesIds.push_back(mesh.faces.size()-1);
  }
  ring.vertexesIds = GetVertexesIds(mesh, ring.facesIds);
  branch.rings.push_back(std::move(ring));

  while (branch.rings.back().curLength < branch.length)
  {
    TreeRing& p_ring = branch.rings.back();
    ring = TreeRing();
    float seg_len = std::max((branch.length - p_ring.curLength) / 5 * 2, last_seg_len);
    ring.curLength = p_ring.curLength + seg_len;
    ring.rad = (branch.rad - last_rad) * (1 - std::min(ring.curLength / branch.length, 1.f)) + last_rad;
    
    ring.vecIn = p_ring.vecOut;
    ring.vecOut = GenOutVec(Normalized(ring.vecIn + branch.rings.front().vecIn), 25);
    ring.center = p_ring.center + ring.vecIn * seg_len;
    
    if (branch.rings.size() == 1)
      ring.facesIds = ExtrudeWithCap(mesh, p_ring.facesIds, ring.vecIn * seg_len);
    else
      ring.facesIds = Extrude(mesh, p_ring.facesIds, ring.vecIn * seg_len);
    p_ring.facesIds.clear();
    ring.vertexesIds = GetVertexesIds(mesh, ring.facesIds);
    ScaleVertexes(mesh, float3(ring.rad / p_ring.rad, 1.f, ring.rad / p_ring.rad), ring.vertexesIds);
    branch.rings.push_back(std::move(ring));
  }

  tree.push_back(branch);
  weightCum += branch.weight;
  model.push_back(std::move(mesh));
}

void lpng::GenerateObjectTree::RelaxBranch(TreeBranch& branch, size_t meshId)
{
  for (size_t i = 0; i < branch.rings.size(); ++i)
  {
    TreeRing& ring = branch.rings[i];
    float3 normal = i == 0 ? Normalized(ring.vecIn) : Normalized(ring.vecIn + ring.vecOut);
    float angle = Angle(float3(0, 1, 0), normal);
    if (angle > FLT_EPSILON)
    {
      Quat q(Cross(float3(0, 1, 0), normal), angle);
      RotateVertexes(model[meshId], ring.vertexesIds, q, ring.center);
    }
  }
}