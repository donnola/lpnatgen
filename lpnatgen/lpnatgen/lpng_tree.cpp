#define _USE_MATH_DEFINES

#include "lpng_tree.h"
#include "lpng_primitive.h"
#include <algorithm>


void lpng::GenerateObjectTree::ClearTree()
{
  tree.clear();
  model.clear();
  seed = get_lpng_seed();
  ++buildId;
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
    float angle = seg_angle * fast_lpng_rand(700, 1300) / 1000.f;
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
  branch.level = parent.level + 1;
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
    tree[parent_id].childsIds.push_back(tree.size() - 1);
  }
  for (size_t i = 0; i < tree.size(); ++i)
  {
    RelaxBranch(tree[i], i);
  }
  CalculateQuality();
  if (buildId < rebuildParams.rebuildNum || rebuildParams.rebuildNum < 0)
  {   
    bool nead_rebuild_tree = false;
    bool is_balanced = Magnitude(float2(quality.C.x, quality.C.z)) <= rebuildParams.balance;
    bool is_centered = Magnitude(float2(quality.mean.x, quality.mean.z)) <= rebuildParams.centered;
    bool is_spreading = quality.D > rebuildParams.disp.x && quality.D < rebuildParams.disp.y;
    nead_rebuild_tree = !is_balanced || !is_centered || !is_spreading || tree[0].childsIds.size() < params.branchCount / 3;
    if (nead_rebuild_tree)
    {
      ClearTree();
      GenerateMesh();
      return;
    }
  }
  GenerateCrown();
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
    mesh.faces.push_back(Face({ i + 1, int((i + 1) % params.edgeBase) + 1, rootId }));
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
  float weight = 0;
  for (const TreeBranch& branch : tree)
  {
    ++i;
    weight += branch.rings[0].rad;
    quality.C += branch.rings.back().center * branch.rings[0].rad;
    quality.mean += branch.rings.back().center;
  }
  quality.C /= weight;
  quality.mean /= i;
  for (const TreeBranch& branch : tree)
  {
    float3 t = branch.rings.back().center - quality.mean;
    quality.D3 += t * t;
    quality.D += MagnitudeSq(t);
  }
  quality.D3 /= i;
  quality.D /= i;
}


void lpng::GenerateObjectTree::InitClusters()
{
  crownClusters.clear();
  std::vector<std::pair<int, int>> free_branch_ids;
  for (int i = 0; i < tree.size(); ++i)
  {
    free_branch_ids.emplace_back(i, tree[i].level);
  }

  std::sort(free_branch_ids.begin(), free_branch_ids.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b)
    { return a.second < b.second; });

  while (free_branch_ids.size() > 0)
  {
    CrownCluster cluster;
    int id = free_branch_ids.front().first;
    float3 c = tree[id].rings.back().center - Normalized(tree[id].rings.back().vecIn) * tree[id].length * 0.2;
    cluster.rad = std::max(tree[id].rad * 5, tree[id].length * 0.4f);
    cluster.rad = std::max(cluster.rad, tree[0].rad * 2.5f);
    cluster.deltaRad = cluster.rad / 4.f;
    cluster.mainBranchId = id;
    cluster.center = c;
    std::unordered_set<int> selected_ids;
    for (const auto& i : free_branch_ids)
    {
      float3 p = tree[i.first].rings.back().center;
      if (Magnitude(p - c) + cluster.deltaRad <= cluster.rad)
      {
        selected_ids.insert(i.first);
      }
    }
    for (int i = 0; i < free_branch_ids.size(); ++i)
    {
      if (selected_ids.find(free_branch_ids[i].first) != selected_ids.end())
      {
        free_branch_ids.erase(free_branch_ids.begin() + i);
        --i;
      }
    }
    cluster.branchIds = std::move(selected_ids);
    crownClusters.push_back(cluster);
  }
}


void lpng::GenerateObjectTree::GenerateCrown()
{
  InitClusters();
  Sphere* sphere = Sphere::GetInstance();
  for (CrownCluster& cluster : crownClusters)
  {
    cluster.center;
    float3 v = Normalized(float3(1, 1, 1)) * cluster.rad;
    for (int i : cluster.branchIds)
    {
      float3 p = tree[i].rings.back().center - cluster.center;
      if (abs(p.x) > v.x)
      {
        v.x = abs(p.x);
      }
      if (abs(p.y) > v.y)
      {
        v.y = abs(p.y);
      }
      if (abs(p.z) > v.z)
      {
        v.z = abs(p.z);
      }
    }
    float v_r = Magnitude(v);
    if (v_r < cluster.rad)
    {
      v *= cluster.rad / v_r;
      v_r = cluster.rad;
    }
    float k = fast_lpng_rand(1150, 1300) / 1000.f;
    cluster.rad = v_r * k;
    v *= k;
    std::unordered_set<size_t> points;
    int points_count = 50;
    Mesh crown = sphere->GetSphere();
    for (int i : cluster.branchIds)
    {
      float3 p = tree[i].rings.back().center;
      int neares_p = 0;
      float min_dist_sq = MagnitudeSq(p - crown.vertexCoords[0]);
      for (int j = 1; j < crown.vertexCoords.size(); ++j)
      {
        float dist_sq = MagnitudeSq(p - crown.vertexCoords[j]);
        if (MagnitudeSq(p - crown.vertexCoords[j]) < min_dist_sq)
        {
          min_dist_sq = dist_sq;
          neares_p = j;
        }
      }
      points.insert(neares_p);
    }
    while (points.size() < points_count)
    {
      points.insert(fast_lpng_rand(0, sphere->GetVertexCount()));
    }
    FilterMeshWithPoints(crown, points);
    ModifyCrown(crown, cluster.center);
    ScaleObj(crown, v / sphere->GetSizeCoef());
    MoveObj(crown, cluster.center);
    crown.matType = MaterialTypes::CROWN;
    
    model.push_back(std::move(crown));
  }
}


void lpng::GenerateObjectTree::ModifyCrown(Mesh& crown, const float3& c)
{
  std::vector<float3> vertexes = crown.vertexCoords;
  std::sort(vertexes.begin(), vertexes.end(), [](const float3& a, const float3& b) { return a.y < b.y; });
  int quant_id = vertexes.size() * 0.6;
  float3 quantile = vertexes[quant_id];
  for (float3& v : crown.vertexCoords)
  {
    float n = v.y - quantile.y;
    if (n > 0)
    {
      float k = fast_lpng_rand(800, 1001) / 1000.f;
      v.x = c.x + (v.x - c.x) * k;
      v.z = c.z + (v.z - c.z) * k;
      float n_k = fast_lpng_rand(1400, 1450) / 1000.f;
      v.y = quantile.y + n * n_k;
    } 
    if (n <= 0)
    {
      float k = fast_lpng_rand(950, 1200) / 1000.f;
      v.x = c.x + (v.x - c.x) * k;
      v.z = c.z + (v.z - c.z) * k;
    }
  }
}
