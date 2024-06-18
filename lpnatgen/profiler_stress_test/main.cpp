#include "lpng_tree.h"
#include "lpng_test.h"
#include "lpng_stone.h"
#include "lpng_bush.h"
#include "lpng_fir.h"
#include "lpng_rand.h"
#include <iostream>

static enum ObjectType
{
  TEST = 0,
  STONE = 1,
  TREE = 2,
  BUSH = 3,
  FIR = 4,
  MAX_TYPE = 5
};

static lpng::TreeParams treeParams;
static lpng::FirParams firParams;
static lpng::TreeRebuildParams treeRebuildParams;
static lpng::BushParams bushParams;
static int stoneVertexCount = 50;

static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);
static void SetModel(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);

void test(ObjectType type)
{
  std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  int model_count = 100000;
  float vertex_count = 0;
  float triangle_count = 0;
  long long models_gen_time = 0;
  for (int i = 0; i < model_count; ++i)
  {
    SetModel(type, modelPtr);
    auto begin = std::chrono::steady_clock::now();
    modelPtr->Generate();
    auto end = std::chrono::steady_clock::now();
    models_gen_time += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    const std::vector<lpng::Mesh>& generatedModel = modelPtr->GetModel();
    for (const lpng::Mesh& m : generatedModel)
    {
      vertex_count += m.vertexCoords.size();
      triangle_count += m.faces.size();
    }  
  }
  float time = float(models_gen_time) / float(model_count);
  vertex_count /= float(model_count);
  triangle_count /= float(model_count);
  std::cout << "model " << type << "\n";
  std::cout << "time " << time << "\n";
  std::cout << "vertex_count " << vertex_count << "\n";
  std::cout << "triangle_count " << triangle_count << "\n";
  std::cout << "====================================" << "\n";
}

int main(void)
{
  fast_lpng_rand(int(std::time(nullptr)));
  for (int i = STONE; i < MAX_TYPE; ++i)
  {
    test(ObjectType(i));
  }
  //std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  //int model_type = TREE;
  //for (int i = 0; i < 10000000; ++i)
  //{
  //  SetModelParams(model_type, modelPtr);
  //  modelPtr->Generate();
  //} 
}

static void SetModel(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  bool res = false;
  lpng::float3 model_size;

  switch (type)
  {

  case STONE:
  {
    lpng::GenerateObjectStone* stone_ptr = new lpng::GenerateObjectStone();
    model_size = { 4, 4, 4 };
    model_ptr.reset(stone_ptr);
    res = true;
    break;
  }

  case BUSH:
  {
    lpng::GenerateObjectBush* bush_ptr = new lpng::GenerateObjectBush();
    model_size.x = 2 * bushParams.firstRad;
    model_size.z = 2 * bushParams.firstRad;
    model_size.y = bushParams.height;
    model_ptr.reset(bush_ptr);
    res = true;
    break;
  }

  case TREE:
  {
    lpng::GenerateObjectTree* tree_ptr = new lpng::GenerateObjectTree();
    model_size.x = 2 * treeParams.firstRad;
    model_size.z = 2 * treeParams.firstRad;
    model_size.y = treeParams.height;
    model_ptr.reset(tree_ptr);
    res = true;
    break;
  }
  case FIR:
  {
    lpng::GenerateObjectFir* fir_ptr = new lpng::GenerateObjectFir();
    model_size.x = 3 * firParams.firstRad;
    model_size.z = 3 * firParams.firstRad;
    model_size.y = firParams.height * 1.5;
    model_ptr.reset(fir_ptr);
    res = true;
    break;
  }
  default:
    break;
  }
  if (res)
  {
    model_ptr->SetSize(model_size);
    model_ptr->SetModelSmoothness(false);
  }
}

static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  bool res = false;
  lpng::float3 model_size;

  switch (type)
  {

  case STONE:
  {
    lpng::GenerateObjectStone* stone_ptr = new lpng::GenerateObjectStone();
    float x_k = float(fast_lpng_rand(1700, 2800)) / 1000.f;
    float y = float(fast_lpng_rand(200, 600)) / 1000.f;
    model_size = { y * x_k, y, y * x_k };
    stone_ptr->SetVertexCount(stoneVertexCount);
    model_ptr.reset(stone_ptr);
    res = true;
    break;
  }

  case BUSH:
  {
    lpng::GenerateObjectBush* bush_ptr = new lpng::GenerateObjectBush();
    bushParams.height = float(fast_lpng_rand(500, 1500)) / 1000.f;
    model_size.x = 2 * bushParams.firstRad;
    model_size.z = 2 * bushParams.firstRad;
    model_size.y = bushParams.height;
    bush_ptr->SetBushParams(bushParams);
    model_ptr.reset(bush_ptr);
    res = true;
    break;
  }

  case TREE:
  {
    lpng::GenerateObjectTree* tree_ptr = new lpng::GenerateObjectTree();
    treeParams.height = float(fast_lpng_rand(6000, 10000)) / 1000.f;
    treeParams.firstRad = float(fast_lpng_rand(250, 400)) / 1000.f;
    model_size.x = 2 * treeParams.firstRad;
    model_size.z = 2 * treeParams.firstRad;
    model_size.y = treeParams.height;
    tree_ptr->SetTreeParams(treeParams);
    model_ptr.reset(tree_ptr);
    res = true;
    break;
  }
  case FIR:
  {
    lpng::GenerateObjectFir* fir_ptr = new lpng::GenerateObjectFir();
    firParams.height = float(fast_lpng_rand(6000, 10000)) / 1000.f;
    firParams.firstRad = float(fast_lpng_rand(200, 500)) / 1000.f;
    firParams.baseCrownRadMinCoefStart = 5.f;
    firParams.baseCrownRadMaxCoefStart = 7.f;
    model_size.x = 3 * firParams.firstRad;
    model_size.z = 3 * firParams.firstRad;
    model_size.y = firParams.height * 1.5;
    fir_ptr->SetFirParams(firParams);
    model_ptr.reset(fir_ptr);
    res = true;
    break;
  }
  default:
    break;
  }
  if (res)
  {
    model_ptr->SetSize(model_size);
    model_ptr->SetModelSmoothness(false);
  }
  return res;
}