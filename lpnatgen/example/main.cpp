#define _CRT_SECURE_NO_WARNINGS
#include "lpng_tree.h"
#include "lpng_test.h"
#include "lpng_stone.h"
#include "lpng_bush.h"
#include "lpng_rand.h"
#include "raylib.h"
#define DB_PERLIN_IMPL
#include "db_perlin.hpp"

#include <chrono>


static enum ObjectType
{
  TEST = 0,
  STONE = 1,
  TREE = 2,
  BUSH = 3,
  MAX_TYPE = 4
};

static lpng::TreeParams treeParams;
static lpng::TreeRebuildParams treeRebuildParams;
static lpng::BushParams bushParams;
static int stoneVertexCount = 40;

static Texture2D modelBaseTex;
static Texture2D modelWoodTex;
static Texture2D modelCrownTex;
static Texture2D modelStoneTex;

static Vector2 mousePoint = { 0.0f, 0.0f };

static void SetModelsPos(int count_x, int count_y, float cell_size, const std::vector<std::vector<float>>& height_map, std::vector<Vector3>& pos);
static Mesh GenLandscape(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map);
static void GenModels(std::vector<Model>& models, const std::vector<lpng::Mesh>& generated_model);
static Mesh GenMesh(const lpng::Mesh& model);
static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);


int main(void)
{
  const int screenWidth = 1400;
  const int screenHeight = 800;
  
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Example : lpng object generation");
  SetTargetFPS(60);
  DisableCursor();
  fast_lpng_rand(int(std::time(0)));

  Camera3D camera = { 0 };
  camera.position = { 40.0f, 40.0f, 40.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };    
  camera.fovy = 45.0f;                              
  camera.projection = CAMERA_PERSPECTIVE;  

  std::filesystem::path dir_path = std::filesystem::current_path();
  std::filesystem::create_directories(dir_path / "resources");
  lpng::ModelMaterial::CreateModelTexture(dir_path / "resources");
  modelBaseTex = LoadTexture("resources/obj.tga");
  modelWoodTex = LoadTexture("resources/wood.tga");
  modelCrownTex = LoadTexture("resources/crown.tga");
  modelStoneTex = LoadTexture("resources/stone.tga");

  // LOAD MODEL
  int models_count = 225;
  std::vector<std::vector<Model>> models;
  std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  std::vector<Model> model;
  long long models_gen_time = 0;
  for (int i = 0; i < models_count; ++i)
  {
    int model_type = fast_lpng_rand(STONE, MAX_TYPE);
    SetModelParams(model_type, modelPtr);
    auto begin = std::chrono::steady_clock::now();
    modelPtr->Generate();
    auto end = std::chrono::steady_clock::now();
    models_gen_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::vector<lpng::Mesh> generatedModel = modelPtr->GetModel();
    GenModels(model, generatedModel);
    models.emplace_back(std::move(model));
  }
  modelPtr.reset();

  std::vector<Vector3> models_pos;
  int size_x = 100;
  int size_y = 100;
  float cell_size = 1.f;
  std::vector<std::vector<float>> height_map;
  Model landscape = LoadModelFromMesh(GenLandscape(size_x, size_y, cell_size, height_map));
  SetModelsPos(std::sqrt(models_count), std::sqrt(models_count), cell_size, height_map, models_pos);
  while (!WindowShouldClose())
  {  
    UpdateCamera(&camera, CAMERA_FREE);

    BeginDrawing();
    {
      ClearBackground(SKYBLUE);
      BeginMode3D(camera);
      DrawModel(landscape, {0, 0, 0}, 1.0f, { 0, 55, 25, 255 });
      DrawModelWires(landscape, { 0, 0, 0 }, 1.0f, BLACK);
      for (int i = 0; i < models.size(); ++i)
      {
        for (int j = 0; j < models[i].size(); ++j)
        {
          DrawModel(models[i][j], models_pos[i], 1.0f, WHITE);
          DrawModelWires(models[i][j], models_pos[i], 1.0f, BLACK);
        }
      }
      EndMode3D();
      DrawText(TextFormat("MODELS GEN TIME: %lld ms", models_gen_time), 10, 710, 22, MAROON);
    }
    EndDrawing();
  }

  for (int i = 0; i < models.size(); ++i)
  {
    for (int j = 0; j < models[i].size(); ++j)
    {
      UnloadModel(models[i][j]);
    }
  }
  UnloadModel(landscape);
  UnloadTexture(modelBaseTex);
  UnloadTexture(modelWoodTex);
  UnloadTexture(modelCrownTex);
  UnloadTexture(modelStoneTex);
  CloseWindow();

  return 0;
}


static Mesh GenMesh(const lpng::Mesh& model)
{
  int triangleCount = model.faces.size();
  Mesh mesh = { 0 };
  mesh.triangleCount = triangleCount;
  mesh.vertexCount = mesh.triangleCount * 3;
  mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
  mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

  int nextVertId = 0;
  for (const lpng::Face& f : model.faces)
  {
    for (int i = 0; i < 3; i++)
    {
      lpng::float3 vc = model.vertexCoords[f.vi[i] - 1];
      lpng::float3 vn = model.vertexNormals[f.vni[i] - 1];
      lpng::float2 vt = model.vertexTextCoords[f.vti[i] - 1];
      mesh.vertices[nextVertId * 3] = vc.x;
      mesh.vertices[nextVertId * 3 + 1] = vc.y;
      mesh.vertices[nextVertId * 3 + 2] = vc.z;
      mesh.normals[nextVertId * 3] = vn.x;
      mesh.normals[nextVertId * 3 + 1] = vn.y;
      mesh.normals[nextVertId * 3 + 2] = vn.z;
      mesh.texcoords[nextVertId * 2] = vt.x;
      mesh.texcoords[nextVertId * 2 + 1] = vt.y;
      ++nextVertId;
    }
  }
  UploadMesh(&mesh, false);
  return mesh;
}


static void GenModels(std::vector<Model>& models, const std::vector<lpng::Mesh>& generated_model)
{
  for (const lpng::Mesh& mesh : generated_model)
  {
    models.emplace_back(LoadModelFromMesh(GenMesh(mesh)));
    if (mesh.matType == lpng::MaterialTypes::NONE)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelBaseTex;
    }
    else if (mesh.matType == lpng::MaterialTypes::WOOD)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelWoodTex;
    }
    else if (mesh.matType == lpng::MaterialTypes::CROWN)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelCrownTex;
    }
    else if (mesh.matType == lpng::MaterialTypes::STONE)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelStoneTex;
    }
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
    float x = float(fast_lpng_rand(700, 900)) / 1000.f;
    float y = float(fast_lpng_rand(900, 1200)) / 1000.f;
    model_size = {x, y, x};
    stone_ptr->SetVertexCount(stoneVertexCount);
    model_ptr.reset(stone_ptr);
    res = true;
    break;
  }

  case BUSH:
  {
    lpng::GenerateObjectBush* bush_ptr = new lpng::GenerateObjectBush();
    bushParams.height = float(fast_lpng_rand(800, 1500)) / 1000.f;
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
    treeParams.height = float(fast_lpng_rand(4000, 7000)) / 1000.f;
    treeParams.firstRad = treeParams.height / 10.f;
    model_size.x = 2 * treeParams.firstRad;
    model_size.z = 2 * treeParams.firstRad;
    model_size.y = treeParams.height;
    tree_ptr->SetTreeParams(treeParams);
    model_ptr.reset(tree_ptr);
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


static Mesh GenLandscape(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map)
{
  Mesh landscape = { 0 };
  landscape.triangleCount = size_x * size_y * 2;
  landscape.vertexCount = landscape.triangleCount * 3;
  landscape.vertices = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));

  for (int x = 0; x < size_x; ++x)
  {
    std::vector<float> noise;
    for (int y = 0; y < size_y; ++y)
    {
      float n = db::perlin(double(x) / 16.0, double(y) / 16.0);
      float v = db::lerp(0.f, 8.f, n);
      noise.push_back(v);
    }
    height_map.emplace_back(std::move(noise));
  }
  int sq_id = 0;
  for (int x = 0; x < size_x - 1; ++x)
  {
    for (int y = 0; y < size_y - 1; ++y)
    {
      landscape.vertices[sq_id * 18] = float(x) * cell_size - float(size_x) * cell_size / 2.f;
      landscape.vertices[sq_id * 18 + 1] = height_map[x][y];
      landscape.vertices[sq_id * 18 + 2] = float(y) * cell_size - float(size_y) * cell_size / 2.f;
      
      landscape.vertices[sq_id * 18 + 3] = float(x) * cell_size - float(size_x) * cell_size / 2.f;
      landscape.vertices[sq_id * 18 + 4] = height_map[x][y+1];
      landscape.vertices[sq_id * 18 + 5] = float(y + 1) * cell_size - float(size_y) * cell_size / 2.f;
    
      landscape.vertices[sq_id * 18 + 6] = float(x + 1) * cell_size - float(size_x) * cell_size / 2.f;
      landscape.vertices[sq_id * 18 + 7] = height_map[x + 1][y];
      landscape.vertices[sq_id * 18 + 8] = float(y) * cell_size - float(size_y) * cell_size / 2.f;
    
      landscape.vertices[sq_id * 18 + 9] = float(x + 1) * cell_size - float(size_x) * cell_size / 2.f;
      landscape.vertices[sq_id * 18 + 10] = height_map[x + 1][y + 1];
      landscape.vertices[sq_id * 18 + 11] = float(y + 1) * cell_size - float(size_y) * cell_size / 2.f;

      landscape.vertices[sq_id * 18 + 12] = float(x + 1) * cell_size - float(size_x) * cell_size / 2.f;
      landscape.vertices[sq_id * 18 + 13] = height_map[x + 1][y];
      landscape.vertices[sq_id * 18 + 14] = float(y) * cell_size - float(size_y) * cell_size / 2.f;

      landscape.vertices[sq_id * 18 + 15] = float(x) * cell_size - float(size_x) * cell_size / 2.f;
      landscape.vertices[sq_id * 18 + 16] = height_map[x][y + 1];
      landscape.vertices[sq_id * 18 + 17] = float(y + 1) * cell_size - float(size_y) * cell_size / 2.f;
      ++sq_id;
    }
  }
  UploadMesh(&landscape, false);
  return landscape;
}


static void SetModelsPos(int count_x, int count_y, float cell_size, const std::vector<std::vector<float>>& height_map, std::vector<Vector3>& pos)
{
  int size_x = height_map.front().size();
  int size_y = height_map.size();
  int step_x = (size_x - 10) / count_x;
  int step_y = (size_y - 10) / count_y;
  for (int i = 0; i < count_x; ++i)
  {
    int x = 5 + step_x * i;
    for (int j = 0; j < count_y; ++j)
    {
      int y = 5 + step_y * j;
      pos.emplace_back(float(x) * cell_size - float(size_x) * cell_size / 2.f, height_map[x][y], float(y) * cell_size - float(size_y) * cell_size / 2.f);
    }
  }
}