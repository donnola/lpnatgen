#define _USE_MATH_DEFINES

#include "lpng_tree.h"
#include "lpng_primitive.h"
#include <algorithm>


void lpng::GenerateObjectTree::ClearTree()
{
  branches.clear();
  model.clear();
  crownClusters.clear();
  seed = get_lpng_seed();
  ++buildId;
}


void lpng::GenerateObjectTree::GenerateMesh()
{
  Branch mainBranch;
  mainBranch.weight = 10000;
  mainBranch.length = treeParams.height;
  mainBranch.baseRad = treeParams.firstRad;
  mainBranch.lastRad = std::min(mainBranch.baseRad / 2, treeParams.finalRad);
  mainBranch.freeDirections = GetNDirections(treeParams.edgeBase);
  mainBranch.edgeBase = treeParams.edgeBase;
  GenerateBranch(mainBranch, { 0, 0, 0 }, {0, 1, 0});
  int children_count = fast_lpng_rand(treeParams.branchMinCount, treeParams.branchMaxCount + 1);
  while (branches.size() < children_count + 1)
  {
    int parent_id = SelectWeightedBranch();
    if (parent_id < 0)
      break;
    Branch branch;
    float3 point_start;
    float3 vec_in;
    InitBranch(parent_id, branch, point_start, vec_in);
    GenerateBranch(branch, point_start, vec_in, vec_in);
    branches[parent_id].childsIds.push_back(branches.size() - 1);
  }
  for (size_t i = 0; i < branches.size(); ++i)
  {
    RelaxBranch(branches[i], i);
  }
  CalculateQuality();
  if (buildId < rebuildParams.rebuildNum)
  {   
    bool nead_rebuild_tree = false;
    bool is_balanced = Magnitude(float2(quality.C.x, quality.C.z)) <= rebuildParams.balance;
    bool is_centered = Magnitude(float2(quality.mean.x, quality.mean.z)) <= rebuildParams.centered;
    bool is_spreading = quality.D > rebuildParams.disp.x && quality.D < rebuildParams.disp.y;
    nead_rebuild_tree = !is_balanced || !is_centered || !is_spreading || branches[0].childsIds.size() < children_count / 3;
    if (nead_rebuild_tree)
    {
      ClearTree();
      GenerateMesh();
      return;
    }
  }
  GenerateCrown();
}


void lpng::GenerateObjectTree::CalculateQuality()
{
  size_t i = 0;
  float weight = 0;
  for (const Branch& branch : branches)
  {
    ++i;
    weight += branch.rings[0].rad;
    quality.C += branch.rings.back().center * branch.rings[0].rad;
    quality.mean += branch.rings.back().center;
  }
  quality.C /= weight;
  quality.mean /= i;
  for (const Branch& branch : branches)
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
  for (int i = 0; i < branches.size(); ++i)
  {
    free_branch_ids.emplace_back(i, branches[i].level);
  }

  std::sort(free_branch_ids.begin(), free_branch_ids.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b)
    { return a.second < b.second; });

  while (free_branch_ids.size() > 0)
  {
    CrownCluster cluster;
    int id = free_branch_ids.front().first;
    float3 c = branches[id].rings.back().center - Normalized(branches[id].rings.back().vecIn) * branches[id].length * 0.2;
    cluster.rad = std::max(branches[id].baseRad * 5, branches[id].length * 0.4f);
    cluster.rad = std::max(cluster.rad, branches[0].baseRad * 2.5f);
    cluster.deltaRad = cluster.rad / 4.f;
    cluster.mainBranchId = id;
    cluster.center = c;
    std::unordered_set<int> selected_ids;
    for (const auto& i : free_branch_ids)
    {
      float3 p = branches[i.first].rings.back().center;
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
      float3 p = branches[i].rings.back().center - cluster.center;
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
    double v_r = Magnitude(v);
    if (v_r < cluster.rad)
    {
      v *= cluster.rad / v_r;
      v_r = cluster.rad;
    }
    float k = fast_lpng_rand(1150, 1300) / 1000.f;
    cluster.rad = v_r * k;
    v *= k;
    std::unordered_set<size_t> points;
    Mesh crown = sphere->GetSphere();
    for (int i : cluster.branchIds)
    {
      if (points.size() == treeParams.crownVertexNum)
        break;
      float3 p = branches[i].rings.back().center;
      int neares_p = 0;
      double min_dist_sq = MagnitudeSq(p - crown.vertexCoords[0]);
      for (int j = 1; j < crown.vertexCoords.size(); ++j)
      {
        double dist_sq = MagnitudeSq(p - crown.vertexCoords[j]);
        if (MagnitudeSq(p - crown.vertexCoords[j]) < min_dist_sq)
        {
          min_dist_sq = dist_sq;
          neares_p = j;
        }
      }
      points.insert(neares_p);
    }
    while (points.size() < treeParams.crownVertexNum)
    {
      points.insert(fast_lpng_rand(0, int(sphere->GetVertexCount())));
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
  int quant_id = int(vertexes.size() * 0.6);
  float3 quantile = vertexes[quant_id];
  for (float3& v : crown.vertexCoords)
  {
    float k;
    float n = v.y - quantile.y;
    if (n > 0)
    {
      k = fast_lpng_rand(800, 1001) / 1000.f; // TODO : var
      float n_k = fast_lpng_rand(1400, 1450) / 1000.f; // TODO : var
      v.y = quantile.y + n * n_k;
    }
    if (n <= 0)
    {
      k = fast_lpng_rand(950, 1200) / 1000.f; // TODO : var
    }
    v.x = c.x + (v.x - c.x) * k;
    v.z = c.z + (v.z - c.z) * k;
  }
}
