#define _USE_MATH_DEFINES

#include "lpng_tree.h"


void lpng::GenerateObjectTree::ClearTree()
{
  tree.clear();
  model.clear();
  seed = get_lpng_seed();
  ++build_id;
}


lpng::float3 lpng::GenerateObjectTree::GenOutVec(const float3& vecIn, int fromAngle, int toAngle) const
{
  float3 vec(fast_lpng_rand(-50, 51), 0, fast_lpng_rand(-50, 51));
  Normalize(vec);
  if (vec == float3())
    vec.y = 1;
  else
  {
    int angle = fast_lpng_rand(fromAngle, toAngle);
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

lpng::float3 lpng::GenerateObjectTree::GenOutVec(std::vector<float2>& dirs, const float3& vecIn, int fromAngle, int toAngle) const
{
  int d_id = fast_lpng_rand(0, dirs.size());
  float2 dir = dirs[d_id];
  dirs.erase(dirs.begin() + d_id);
  float3 vec = float3(dir.x, 0, dir.y);
  Normalize(vec);
  int branch_angle = fast_lpng_rand(fromAngle, toAngle);
  if (branch_angle == 0)
    vec = float3(0, 1, 0);
  else
    vec.y = 1 / tan(DEG2RAD(branch_angle));
  float angle = Angle(float3(0, 1, 0), vecIn);
  if (angle > FLT_EPSILON)
  {
    Quat q(Cross(float3(0, 1, 0), vecIn), angle);
    vec = QuatTransformVec(q, vec);
  }
  return Normalized(vec);
}


std::vector<lpng::float2> lpng::GenerateObjectTree::GetNDirections(int n)
{
  std::vector<lpng::float2> dirs;
  float seg_angle = 2 * M_PI / n;
  float angle_cum = DEG2RAD(fast_lpng_rand(0, 360));
  for (int i = 0; i < n; ++i)
  {
    dirs.emplace_back(sin(angle_cum), cos(angle_cum));
    float angle = seg_angle * fast_lpng_rand(900, 1100) / 1000.f;
    angle_cum += angle;
  } 
  return dirs;
}


int lpng::GenerateObjectTree::SelectWeightedBranch()
{
  size_t weight_sum = 0;
  for (TreeBranch& branch : tree)
  {
    weight_sum += branch.weight;
  }
  if (weight_sum == 0)
    return -1;
  size_t w = fast_lpng_rand(0, weight_sum);
  size_t w_cum = 0;
  for (size_t i = 0; i < tree.size(); ++i)
  {
    w_cum += tree[i].weight;
    if (w_cum >= w)
    {
      return i;
    }
  }
}


void lpng::GenerateObjectTree::InitBranch(const size_t parent_id, TreeBranch& branch, float3& point_start, float3& vec_in)
{
  TreeBranch& parent = tree[parent_id];
  float c_start = fast_lpng_rand(2500, 8000) / 10000.f;
  float l_start = parent.length * c_start;
  float c_len = fast_lpng_rand(850, 1100) / 1000.f;
  branch.length = (parent.length - l_start) * c_len;
  branch.rad = (parent.rings.front().rad - parent.rings.back().rad) * (1.f - c_start) + parent.rings.back().rad;
  branch.rad *= 0.9;
  float weight_r = parent.weight / parent.rad * branch.rad;
  float weight_l = parent.weight / parent.length * branch.length;
  size_t weight = size_t((weight_r + weight_l) / 2);
  branch.weight = weight < 5000 ? 0 : weight;
  if (branch.weight > 0)
    branch.freeDirections = GetNDirections(params.edgeBase);
  point_start = parent.rings.front().center;
  for (size_t i = 0; i < parent.rings.size(); ++i)
  {
    const TreeRing& ring = parent.rings[i];
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


void lpng::GenerateObjectTree::GenerateMesh()
{
  Mesh branches;

  TreeBranch mainBranch;
  mainBranch.weight = 10000;
  mainBranch.length = params.height;
  mainBranch.rad = params.firstRad;
  mainBranch.freeDirections = GetNDirections(params.edgeBase);
  GenerateBranch(mainBranch, { 0, 0, 0 }, {0, 1, 0});
  while (tree.size() < params.branchCount)
  {
    int parent_id = SelectWeightedBranch();
    if (parent_id < 0)
      break;
    TreeBranch branch;
    float3 point_start;
    float3 vec_in;
    InitBranch(parent_id, branch, point_start, vec_in);
    GenerateBranch(branch, point_start, vec_in, vec_in);
    tree[parent_id].childs_id.push_back(tree.size() - 1);
  }
  for (size_t i = 0; i < tree.size(); ++i)
  {
    RelaxBranch(tree[i], i);
  }
  if (build_id < params.rebuildNum)
  {
    CalculateQuality();
    bool nead_rebuild_tree = false;
    bool is_balanced = MagnitudeSq(float2(quality.C.x, quality.C.z)) <= params.sqBalance;
    bool is_centered = MagnitudeSq(float2(quality.mean.x, quality.mean.z)) <= params.sqCentered;
    bool is_spreading = quality.D > params.disp.x && quality.D < params.disp.y;
    nead_rebuild_tree = !is_balanced || !is_centered || !is_spreading;
    if (nead_rebuild_tree)
    {
      ClearTree();
      GenerateMesh();
    }
  }
}


void lpng::GenerateObjectTree::GenerateBranch(TreeBranch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut)
{
  Mesh mesh;
  mesh.matType = MaterialTypes::WOOD;
  float seg_angle = 2 * M_PI / params.edgeBase;
  float last_seg_len = branch.length / 7;
  float last_rad = std::min(branch.rad / 8, params.lastRad);
  // create first ring
  TreeRing ring;
  ring.center = pointStart;
  ring.rad = branch.rad;
  ring.curLength = 0;
  ring.vecIn = vecIn;
  ring.vecOut = vecOut == float3() ? GenOutVec(vecIn, 0, 15) : vecOut;

  for (size_t i = 0; i < params.edgeBase; ++i)
  {
    float3 vertex = ring.center + float3(sin(i * seg_angle) * ring.rad, 0, cos(i * seg_angle) * ring.rad);
    mesh.vertexCoords.push_back(vertex);
  }
  mesh.vertexCoords.push_back(ring.center);
  int rootId = mesh.vertexCoords.size();
  for (int i = 0; i < rootId - 1; ++i)
  {
    mesh.faces.push_back(Face({ i + 1, (i + 1) % params.edgeBase + 1, rootId }));
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
    ring.vecOut = GenOutVec(Normalized(ring.vecIn + float3(0, 1, 0) * params.upCoef), 0, 15);
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

  tree.push_back(std::move(branch));
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


void lpng::GenerateObjectTree::CalculateQuality()
{
  size_t i = 0;
  for (const TreeBranch& branch : tree)
  {
    for (const TreeRing& ring : branch.rings)
    {
      ++i;
      quality.C += ring.center * ring.rad;
      quality.mean += ring.center;
    }
  }
  quality.C /= i;
  quality.mean /= i;
  for (const TreeBranch& branch : tree)
  {
    for (const TreeRing& ring : branch.rings)
    {
      float3 t = ring.center - quality.mean;
      quality.D3 += t * t;
      quality.D += MagnitudeSq(t);
    }
  }
  quality.D3 /= i;
  quality.D /= i;
}