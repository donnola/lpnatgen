#define _CRT_SECURE_NO_WARNINGS
#include "lpng_tree.h"
#include "lpng_test.h"
#include "lpng_stone.h"
#include "lpng_bush.h"
#include "lpng_fir.h"
#include "lpng_rand.h"
#include "raylib.h"
#define DB_PERLIN_IMPL
#include "db_perlin.hpp"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "rlgl.h"

#include <chrono>
#include <random>

#define SHADOWMAP_RESOLUTION 8192

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
static int stoneVertexCount = 40;

static Texture2D modelBaseTex;
static Texture2D modelWoodTex;
static Texture2D modelCrownTex;
static Texture2D modelStoneTex;

static Vector2 mousePoint = { 0.0f, 0.0f };

static Vector3 GetModelsPos(int x, int y, float cell_size, const std::vector<std::vector<float>>& height_map, std::vector<std::vector<bool>>& free, int r);
static Mesh GenHeightMap(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map);
static Mesh GenLandscape(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map);
static void GenModels(std::vector<Model>& models, const std::vector<lpng::Mesh>& generated_model);
static Mesh GenMesh(const lpng::Mesh& model);
static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr, float C);
RenderTexture2D LoadShadowmapRenderTexture(int width, int height);
void UnloadShadowmapRenderTexture(RenderTexture2D target);


int main(void)
{
  const int screenWidth = 1400;
  const int screenHeight = 800;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Example : lpng object generation");
  SetTargetFPS(60);
  DisableCursor();
  fast_lpng_rand(int(std::time(nullptr)));

  Camera3D camera = { 0 };
  camera.position = { 40.0f, 40.0f, 40.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  std::filesystem::path dir_path = std::filesystem::current_path();
  lpng::ModelMaterial::CreateModelTexture(dir_path / "resources/textures");
  modelBaseTex = LoadTexture("resources/textures/obj.tga");
  modelWoodTex = LoadTexture("resources/textures/wood.tga");
  modelCrownTex = LoadTexture("resources/textures/crown.tga");
  modelStoneTex = LoadTexture("resources/textures/stone.tga");

  Shader shadowShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/shadowmap.fs");
  shadowShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shadowShader, "matModel");
  shadowShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shadowShader, "viewPos");
  Vector3 lightDir = Vector3Normalize({ -1, -3, -2 });
  Color lightColor = Color{ 224, 180, 135, 255 };
  Vector4 lightColorNormalized = ColorNormalize(lightColor);
  int lightDirLocShadow = GetShaderLocation(shadowShader, "lightDir");
  int lightColLocShadow = GetShaderLocation(shadowShader, "lightColor");
  SetShaderValue(shadowShader, lightDirLocShadow, &lightDir, SHADER_UNIFORM_VEC3);
  SetShaderValue(shadowShader, lightColLocShadow, &lightColorNormalized, SHADER_UNIFORM_VEC4);
  int ambientLocShadow = GetShaderLocation(shadowShader, "ambient");
  float ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
  SetShaderValue(shadowShader, ambientLocShadow, ambient, SHADER_UNIFORM_VEC4);
  int lightVPLocShadow = GetShaderLocation(shadowShader, "lightVP");
  int shadowMapLocShadow = GetShaderLocation(shadowShader, "shadowMap");
  int shadowMapResolution = SHADOWMAP_RESOLUTION;
  SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "shadowMapResolution"), &shadowMapResolution, SHADER_UNIFORM_INT);
  float fogDensity = 0.015f;
  int fogDensityLocShadow = GetShaderLocation(shadowShader, "fogDensity");
  SetShaderValue(shadowShader, fogDensityLocShadow, &fogDensity, SHADER_UNIFORM_FLOAT);

  CreateLight(LIGHT_POINT, Vector3{ -3, 6, -3 }, Vector3{ 0, 1, 1 }, Color{ 54, 70, 128, 255 }, shadowShader);

  // create double-sided box
  Model skybox = LoadModelFromMesh(GenMeshCube(1.0, 1.0, 1.0));

  // GENERATE LANDSCAPE
  int size_x = 100;
  int size_y = 100;
  float cell_size = 1.0f;
  Vector3 water_pos = { 0, -2.5, 0 };
  std::vector<std::vector<float>> height_map;
  GenHeightMap(size_x, size_y, cell_size, height_map);
  std::vector<std::vector<float>> landscape_height_map;
  Model landscape = LoadModelFromMesh(GenLandscape(size_x, size_y, cell_size, landscape_height_map));
  std::vector<std::vector<bool>> free(landscape_height_map.size(), std::vector<bool>(landscape_height_map.front().size(), true));

  std::vector<std::pair<int, int>> bypass_map;
  for (int i = 5; i < size_x; ++i)
  {
    for (int j = 5; j < size_y; ++j)
    {
      bypass_map.emplace_back(i, j);
    }
  }
  shuffle(bypass_map.begin(), bypass_map.end(), std::mt19937(std::random_device()()));

  // LOAD MODEL
  int models_count = 600;
  long long models_gen_time = 0;
  std::vector<Vector3> models_pos;
  std::vector<float> models_scale;
  std::vector<std::vector<Model>> models;
  std::vector<Model> model;
  std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  int id = 0;
  while (models.size() < models_count && id < bypass_map.size())
  {
    const std::pair<int, int>& coord = bypass_map[id];
    if (free[coord.first][coord.second])
    {
      float C = height_map[coord.first][coord.second];
      int model_type = C < 0.4 ? STONE : (C < 1.5 ? BUSH : (fast_lpng_rand(0, 2) == 0 ? TREE : FIR));
      int r = C < 0.4 ? fast_lpng_rand(1, 3) : (C < 1.5 ? fast_lpng_rand(3, 5) : fast_lpng_rand(5, 7));
      SetModelParams(model_type, modelPtr, C);
      auto begin = std::chrono::steady_clock::now();
      modelPtr->Generate();
      auto end = std::chrono::steady_clock::now();
      models_gen_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
      std::vector<lpng::Mesh> generatedModel = modelPtr->GetModel();
      GenModels(model, generatedModel);
      models.emplace_back(std::move(model));
      models_pos.push_back(GetModelsPos(coord.first, coord.second, cell_size, landscape_height_map, free, r));
      models_scale.push_back(0.2 + 2.0 * fabsf(db::perlin(double(coord.first) / 32.0, double(coord.second) / 32.0)));
    }
    ++id;
  }
  modelPtr.reset();

  // SET MODEL SHADERS
  skybox.materials[0].shader = shadowShader;
  landscape.materials[0].shader = shadowShader;
  for (int i = 0; i < models.size(); ++i)
  {
    for (int j = 0; j < models[i].size(); ++j)
    {
      models[i][j].materials[0].shader = shadowShader;
    }
  }

  RenderTexture2D shadowMap = LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
  Camera3D lightCam = { 0 };
  lightCam.position = Vector3Scale(lightDir, -100.0f);
  lightCam.target = Vector3Scale(lightDir, 100.0f);
  // Use an orthographic projection for directional lights
  lightCam.projection = CAMERA_ORTHOGRAPHIC;
  lightCam.up = { 0.0f, 1.0f, 0.0f };
  lightCam.fovy = 100.0f;

  while (!WindowShouldClose())
  {
    for (int i = 0; i < 5; i++)
      UpdateCamera(&camera, CAMERA_FREE);

    Vector3 cameraPos = camera.position;
    SetShaderValue(shadowShader, shadowShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
    BeginDrawing();
    {
      Matrix lightView;
      Matrix lightProj;
      BeginTextureMode(shadowMap);
      ClearBackground(WHITE);
      BeginMode3D(lightCam);
      lightView = rlGetMatrixModelview();
      lightProj = rlGetMatrixProjection();
      DrawModel(landscape, { 0, 0, 0 }, 1.0f, { 15, 75, 25, 255 });
      for (int i = 0; i < models.size(); ++i)
      {
        for (int j = 0; j < models[i].size(); ++j)
        {
          DrawModel(models[i][j], models_pos[i], models_scale[i], WHITE);
        }
      }
      EndMode3D();
      EndTextureMode();

      Matrix lightViewProj = MatrixMultiply(lightView, lightProj);
      ClearBackground(SKYBLUE);
      SetShaderValueMatrix(shadowShader, lightVPLocShadow, lightViewProj);
      rlEnableShader(shadowShader.id);
      int slot = 10;
      rlActiveTextureSlot(10);
      rlEnableTexture(shadowMap.depth.id);
      rlSetUniform(shadowMapLocShadow, &slot, SHADER_UNIFORM_INT, 1);
      BeginMode3D(camera);
      DrawModel(landscape, { 0, 0, 0 }, 1.0f, { 15, 75, 25, 255 });
      for (int i = 0; i < models.size(); ++i)
      {
        for (int j = 0; j < models[i].size(); ++j)
        {
          DrawModel(models[i][j], models_pos[i], models_scale[i], WHITE);
        }
      }
      DrawModel(skybox, { 0.0f, 0.0f, 0.0f }, -1000.0f, { 255, 255, 255, 255 });
      EndMode3D();
      DrawText(TextFormat("MODELS GEN TIME: %lld ms", models_gen_time), 10, 710, 22, MAROON);
      DrawText(TextFormat("MODEL GEN TIME: %lf ms", float(models_gen_time) / float(models.size())), 10, 732, 22, MAROON);
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
  UnloadShader(shadowShader);
  UnloadModel(landscape);
  UnloadTexture(modelBaseTex);
  UnloadTexture(modelWoodTex);
  UnloadTexture(modelCrownTex);
  UnloadTexture(modelStoneTex);
  UnloadShadowmapRenderTexture(shadowMap);
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


static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr, float C)
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
    bushParams.height = C;
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
    treeParams.height = std::clamp(C * 0.6f, 5.f, 8.f);
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
    firParams.height = std::clamp(C * 0.6f, 5.f, 8.f);
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

static Mesh GenHeightMap(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map)
{
  Mesh landscape = { 0 };
  landscape.triangleCount = size_x * size_y * 2;
  landscape.vertexCount = landscape.triangleCount * 3;
  landscape.vertices = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));
  landscape.normals = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));
  int x_offset = fast_lpng_rand(0, 200);
  int y_offset = fast_lpng_rand(0, 200);
  for (int x = 0; x < size_x; ++x)
  {
    std::vector<float> noise;
    for (int y = 0; y < size_y; ++y)
    {
      float n = db::perlin(double(x + x_offset) / 32.0, double(y + y_offset) / 32.0);
      float v = db::lerp(0.f, 8.f, n);
      v = std::clamp(v, -2.f, 2.f);
      noise.push_back((v + 2) * 3);
    }
    height_map.emplace_back(std::move(noise));
  }
  int sq_id = 0;
  for (int x = 0; x < size_x - 1; ++x)
  {
    for (int y = 0; y < size_y - 1; ++y)
    {
      float p1x = float(x) * cell_size - float(size_x) * cell_size / 2.f;
      float p1y = height_map[x][y];
      float p1z = float(y) * cell_size - float(size_y) * cell_size / 2.f;

      float p2y = height_map[x][y + 1];
      float p2z = float(y + 1) * cell_size - float(size_y) * cell_size / 2.f;

      float p3x = float(x + 1) * cell_size - float(size_x) * cell_size / 2.f;
      float p3y = height_map[x + 1][y];

      float p4y = height_map[x + 1][y + 1];

      lpng::float3 a1 = { p3x - p1x , p3y - p2y , p1z - p2z };
      lpng::float3 b1 = { p1x - p1x , p1y - p2y , p1z - p2z };
      lpng::float3 n1 = Normalized(Cross(a1, b1));

      lpng::float3 a2 = { p1x - p3x , p2y - p3y , p2z - p1z };
      lpng::float3 b2 = { p3x - p3x , p4y - p3y , p2z - p1z };
      lpng::float3 n2 = Normalized(Cross(a2, b2));

      landscape.vertices[sq_id * 18] = p1x;
      landscape.vertices[sq_id * 18 + 1] = p1y;
      landscape.vertices[sq_id * 18 + 2] = p1z;

      landscape.vertices[sq_id * 18 + 3] = p1x;
      landscape.vertices[sq_id * 18 + 4] = p2y;
      landscape.vertices[sq_id * 18 + 5] = p2z;

      landscape.vertices[sq_id * 18 + 6] = p3x;
      landscape.vertices[sq_id * 18 + 7] = p3y;
      landscape.vertices[sq_id * 18 + 8] = p1z;

      landscape.vertices[sq_id * 18 + 9] = p3x;
      landscape.vertices[sq_id * 18 + 10] = p4y;
      landscape.vertices[sq_id * 18 + 11] = p2z;

      landscape.normals[sq_id * 18 + 9] = n2.x;
      landscape.normals[sq_id * 18 + 10] = n2.y;
      landscape.normals[sq_id * 18 + 11] = n2.z;

      landscape.vertices[sq_id * 18 + 12] = p3x;
      landscape.vertices[sq_id * 18 + 13] = p3y;
      landscape.vertices[sq_id * 18 + 14] = p1z;

      landscape.vertices[sq_id * 18 + 15] = p1x;
      landscape.vertices[sq_id * 18 + 16] = p2y;
      landscape.vertices[sq_id * 18 + 17] = p2z;

      ++sq_id;
    }
  }
  UploadMesh(&landscape, false);
  return landscape;
}


static Mesh GenLandscape(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map)
{
  Mesh landscape = { 0 };
  landscape.triangleCount = size_x * size_y * 2;
  landscape.vertexCount = landscape.triangleCount * 3;
  landscape.vertices = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));
  landscape.normals = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));
  int x_offset = fast_lpng_rand(0, 200);
  int y_offset = fast_lpng_rand(0, 200);
  for (int x = 0; x < size_x; ++x)
  {
    std::vector<float> noise;
    for (int y = 0; y < size_y; ++y)
    {
      float n = ((db::perlin(double(x + x_offset) / 64.0, double(y + y_offset) / 64.0) +
        db::perlin(double(x + x_offset) / 16.0, double(y + y_offset) / 16.0) * 0.25)) / 1.25;
      float v = db::lerp(0.f, 2.f, n);
      noise.push_back(v);
    }
    height_map.emplace_back(std::move(noise));
  }
  int sq_id = 0;
  for (int x = 0; x < size_x - 1; ++x)
  {
    for (int y = 0; y < size_y - 1; ++y)
    {
      float p1x = float(x) * cell_size - float(size_x) * cell_size / 2.f;
      float p1y = height_map[x][y];
      float p1z = float(y) * cell_size - float(size_y) * cell_size / 2.f;

      float p2y = height_map[x][y + 1];
      float p2z = float(y + 1) * cell_size - float(size_y) * cell_size / 2.f;

      float p3x = float(x + 1) * cell_size - float(size_x) * cell_size / 2.f;
      float p3y = height_map[x + 1][y];

      float p4y = height_map[x + 1][y + 1];

      lpng::float3 a1 = { p3x - p1x , p3y - p2y , p1z - p2z };
      lpng::float3 b1 = { p1x - p1x , p1y - p2y , p1z - p2z };
      lpng::float3 n1 = Normalized(Cross(a1, b1));

      lpng::float3 a2 = { p1x - p3x , p2y - p3y , p2z - p1z };
      lpng::float3 b2 = { p3x - p3x , p4y - p3y , p2z - p1z };
      lpng::float3 n2 = Normalized(Cross(a2, b2));

      landscape.vertices[sq_id * 18] = p1x;
      landscape.vertices[sq_id * 18 + 1] = p1y;
      landscape.vertices[sq_id * 18 + 2] = p1z;

      landscape.normals[sq_id * 18] = n1.x;
      landscape.normals[sq_id * 18 + 1] = n1.y;
      landscape.normals[sq_id * 18 + 2] = n1.z;

      landscape.vertices[sq_id * 18 + 3] = p1x;
      landscape.vertices[sq_id * 18 + 4] = p2y;
      landscape.vertices[sq_id * 18 + 5] = p2z;

      landscape.normals[sq_id * 18 + 3] = n1.x;
      landscape.normals[sq_id * 18 + 4] = n1.y;
      landscape.normals[sq_id * 18 + 5] = n1.z;

      landscape.vertices[sq_id * 18 + 6] = p3x;
      landscape.vertices[sq_id * 18 + 7] = p3y;
      landscape.vertices[sq_id * 18 + 8] = p1z;

      landscape.normals[sq_id * 18 + 6] = n1.x;
      landscape.normals[sq_id * 18 + 7] = n1.y;
      landscape.normals[sq_id * 18 + 8] = n1.z;

      landscape.vertices[sq_id * 18 + 9] = p3x;
      landscape.vertices[sq_id * 18 + 10] = p4y;
      landscape.vertices[sq_id * 18 + 11] = p2z;

      landscape.normals[sq_id * 18 + 9] = n2.x;
      landscape.normals[sq_id * 18 + 10] = n2.y;
      landscape.normals[sq_id * 18 + 11] = n2.z;

      landscape.vertices[sq_id * 18 + 12] = p3x;
      landscape.vertices[sq_id * 18 + 13] = p3y;
      landscape.vertices[sq_id * 18 + 14] = p1z;

      landscape.normals[sq_id * 18 + 12] = n2.x;
      landscape.normals[sq_id * 18 + 13] = n2.y;
      landscape.normals[sq_id * 18 + 14] = n2.z;

      landscape.vertices[sq_id * 18 + 15] = p1x;
      landscape.vertices[sq_id * 18 + 16] = p2y;
      landscape.vertices[sq_id * 18 + 17] = p2z;

      landscape.normals[sq_id * 18 + 15] = n2.x;
      landscape.normals[sq_id * 18 + 16] = n2.y;
      landscape.normals[sq_id * 18 + 17] = n2.z;
      ++sq_id;
    }
  }
  UploadMesh(&landscape, false);
  return landscape;
}


static Vector3 GetModelsPos(int x, int y, float cell_size, const std::vector<std::vector<float>>& height_map, std::vector<std::vector<bool>>& free, int r)
{
  int size_x = height_map.size();
  int size_y = height_map.front().size();

  for (int i = -10; i < 11; ++i)
  {
    for (int j = -10; j < 11; ++j)
    {
      if (abs(i) + abs(j) <= r)
        free[std::clamp(x + i, 0, size_x - 1)][std::clamp(y + j, 0, size_y - 1)] = false;
    }
  }
  return { float(x) * cell_size - float(size_x) * cell_size / 2.f, height_map[x][y], float(y) * cell_size - float(size_y) * cell_size / 2.f };
}


RenderTexture2D LoadShadowmapRenderTexture(int width, int height)
{
  RenderTexture2D target = { 0 };

  target.id = rlLoadFramebuffer(width, height);
  target.texture.width = width;
  target.texture.height = height;

  if (target.id > 0)
  {
    rlEnableFramebuffer(target.id);

    target.depth.id = rlLoadTextureDepth(width, height, false);
    target.depth.width = width;
    target.depth.height = height;
    target.depth.format = 19;
    target.depth.mipmaps = 1;

    rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    if (rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

    rlDisableFramebuffer();
  }
  else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

  return target;
}


void UnloadShadowmapRenderTexture(RenderTexture2D target)
{
  if (target.id > 0)
  {
    rlUnloadFramebuffer(target.id);
  }
}