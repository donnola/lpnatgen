#define _USE_MATH_DEFINES

#include "lpng_fir.h"


void lpng::GenerateObjectFir::GenerateMesh()
{
  Branch mainBranch;
  mainBranch.length = firParams.height;
  mainBranch.baseRad = firParams.firstRad;
  mainBranch.lastRad = firParams.finalRad;
  mainBranch.edgeBase = firParams.edgeBase;
  GenerateBranch(mainBranch, { 0, 0, 0 }, { 0, 1, 0 });
  TreeRoot(branches.front(), 0);
  RelaxBranch(branches.front(), 0);
  GenerateCrown();
}


lpng::float3 lpng::GenerateObjectFir::FindRingCenter(float heightStart, int& ceilRingId)
{
  for (size_t j = 0; j < branches[0].rings.size(); ++j)
  {
    const BranchRing& ring = branches[0].rings[j];
    if (ring.curLength < heightStart)
      continue;
    ceilRingId = j;
    float d = ring.curLength - heightStart;
    return ring.center - Normalized(ring.vecIn) * d;
  }
  ceilRingId = branches[0].rings.size() - 1;
  float3 center = branches[0].rings.back().center + (heightStart - branches[0].rings.back().curLength) * Normalized(branches[0].rings.back().vecIn);
  return center;
}


void lpng::GenerateObjectFir::GenerateCrownPart(float heightStart, float baseRad, float last_rad, float height)
{
  Crown crown;
  crown.baseRad = baseRad;
  crown.lastRad = last_rad;
  crown.edgeBase = firParams.crownBase;
  crown.length = height;
  int ceil_id = 0;
  float3 point_start = FindRingCenter(heightStart, ceil_id);
  Mesh mesh;
  mesh.matType = MaterialTypes::CROWN;
  CrownRing ring;
  ring.center = point_start;
  ring.rad = crown.baseRad;
  ring.curLength = 0;
  ring.vecIn = branches[0].rings[ceil_id].vecIn;
  ring.vecOut = branches[0].rings[ceil_id].vecIn;
  float seg_angle = 2 * M_PI / crown.edgeBase;
  for (size_t i = 0; i < crown.edgeBase; ++i)
  {
    float3 vertex = ring.center + float3(sin(i * seg_angle) * ring.rad, 0, cos(i * seg_angle) * ring.rad);
    mesh.vertexCoords.push_back(vertex);
  }
  mesh.vertexCoords.push_back(ring.center);
  int rootId = mesh.vertexCoords.size();
  for (int i = 0; i < rootId - 1; ++i)
  {
    mesh.faces.push_back(Face({ i + 1, int((i + 1) % crown.edgeBase) + 1, rootId }));
    ring.facesIds.push_back(mesh.faces.size() - 1);
  }
  ring.verticesIds = GetVerticesIds(mesh, ring.facesIds);
  crown.rings.push_back(std::move(ring));
  float last_seg_len = crown.length / 5.f;
  while (crown.rings.back().curLength < crown.length)
  {
    CrownRing& p_ring = crown.rings.back();
    ring = CrownRing();
    float seg_len = std::max((crown.length - p_ring.curLength) / 3.f, last_seg_len);
    ring.curLength = p_ring.curLength + seg_len;
    ring.rad = (crown.baseRad - crown.lastRad) * (1 - std::min(ring.curLength / crown.length, 1.f)) + crown.lastRad;
    ring.rad *= exp(-float(crown.rings.size()) / 6.f);
    ring.center = FindRingCenter(heightStart + ring.curLength, ceil_id);
    ring.vecIn = Normalized(ring.center - p_ring.center);
    ring.vecOut = Normalized(ring.center - p_ring.center);
    p_ring.vecOut = ring.vecIn;

    if (crown.rings.size() == 1)
      ring.facesIds = ExtrudeWithCap(mesh, p_ring.facesIds, ring.vecIn * seg_len);
    else
    {
      ring.facesIds = Extrude(mesh, p_ring.facesIds, ring.vecIn * seg_len);
      p_ring.facesIds.clear();
    }
    ring.verticesIds = GetVerticesIds(mesh, ring.facesIds);
    ScaleVertices(mesh, float3(ring.rad / p_ring.rad, 1.f, ring.rad / p_ring.rad), ring.verticesIds);
    crown.rings.push_back(std::move(ring));
  }
  crowns.push_back(std::move(crown));
  model.push_back(std::move(mesh));
}


void lpng::GenerateObjectFir::GenerateCrown()
{
  crownCount = fast_lpng_rand(firParams.crownCountMin, firParams.crownCountMax+1);
  std::vector<float> crown_start_k(crownCount);
  crown_start_k[0] = fast_lpng_rand(firParams.crownMinCoefStart * 1000.f, firParams.crownMaxCoefStart * 1000.f) / 1000.f;
  float d_k = (0.8 - crown_start_k[0]) / float(crownCount - 1);
  for (int i = 1; i < crownCount; ++i)
  {
    crown_start_k[i] = crown_start_k[i-1] + d_k;
  }
  std::sort(crown_start_k.begin(), crown_start_k.end());
  std::vector<float> height_start(crownCount);
  std::vector<float> crown_rads(crownCount);
  float tree_rad_dif = branches[0].rings.front().rad - branches[0].rings.back().rad;
  for (int i = 0; i < crownCount; ++i)
  {
    float rad_coef = fast_lpng_rand(firParams.baseCrownRadMinCoefStart * 100.f, firParams.baseCrownRadMaxCoefStart * 100.f) / 100.f;
    float rad = (tree_rad_dif) * (1.f - crown_start_k[i]) + branches[0].rings.back().rad;
    crown_rads[i] = rad * rad_coef;
    height_start[i] = crown_start_k[i] * firParams.height;
  }
  sort(crown_rads.begin(), crown_rads.end(), std::greater<int>());
  for (int i = 0; i < crownCount; ++i)
  {
    float height_k = fast_lpng_rand(1800, 2000) / 1000.f;
    float height = i + 1 < crownCount ? (height_start[i + 1] - height_start[i]) * height_k : (firParams.height - height_start[i]) * height_k;
    float last_rad = i + 1 == crownCount ? 0.05 : crown_rads[i + 1] / 5.f;
    GenerateCrownPart(height_start[i], crown_rads[i], last_rad, height);
  }
  for (size_t i = 0; i < crowns.size(); ++i)
  {
    RelaxCrown(crowns[i], i + 1);
  }
}


void lpng::GenerateObjectFir::RelaxCrown(const Crown& crown, size_t meshId)
{
  for (size_t i = 1; i < crown.rings.size(); ++i)
  {
    const CrownRing& ring = crown.rings[i];
    float3 normal = i == 0 ? Normalized(ring.vecIn) : Normalized(ring.vecIn + ring.vecOut);
    float angle = Angle(float3(0, 1, 0), normal);
    if (angle > FLT_EPSILON)
    {
      Quat q(Cross(float3(0, 1, 0), normal), angle);
      RotateVertices(model[meshId], ring.verticesIds, q, ring.center);
    }
    
  }
}