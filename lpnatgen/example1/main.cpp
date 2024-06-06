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

static Mesh GenMesh(const lpng::Mesh& model);
static void GenModels(std::vector<Model>& models, const std::vector<lpng::Mesh>& generated_model);
static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);
static Mesh GenLandscape(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map);
static bool SetModelsPos(float cell_size, const std::vector<std::vector<float>>& height_map, std::vector<std::vector<bool>>& free, Vector3& p);
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
  float ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
  SetShaderValue(shadowShader, ambientLocShadow, ambient, SHADER_UNIFORM_VEC4);
  int lightVPLocShadow = GetShaderLocation(shadowShader, "lightVP");
  int shadowMapLocShadow = GetShaderLocation(shadowShader, "shadowMap");
  int shadowMapResolution = SHADOWMAP_RESOLUTION;
  SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "shadowMapResolution"), &shadowMapResolution, SHADER_UNIFORM_INT);
  float fogDensity = 0.03f;
  int fogDensityLocShadow = GetShaderLocation(shadowShader, "fogDensity");
  SetShaderValue(shadowShader, fogDensityLocShadow, &fogDensity, SHADER_UNIFORM_FLOAT);

  Shader fogShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/fog.fs");
  fogShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(fogShader, "viewPos");
  fogShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(fogShader, "matModel");
  int ambientLoc = GetShaderLocation(fogShader, "ambient");
  float ambientColor[] = { 0.2f, 0.23f, 0.27f, 1.0f };
  SetShaderValue(fogShader, ambientLoc, ambientColor, SHADER_UNIFORM_VEC4);
  int fogDensityLoc = GetShaderLocation(fogShader, "fogDensity");
  SetShaderValue(fogShader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

  CreateLight(LIGHT_DIRECTIONAL, Vector3Zero(), Vector3{ 0, -1, 0 }, Color{ 80, 20, 0, 255 }, shadowShader);
  CreateLight(LIGHT_DIRECTIONAL, Vector3Zero(), Vector3{ 0, 1, 1 }, Color{ 54, 70, 128, 255 }, shadowShader);

  CreateLight(LIGHT_DIRECTIONAL, Vector3Zero(), Vector3{ -1, -3, -2 }, Color{ 224, 180, 135, 255 }, fogShader);
  CreateLight(LIGHT_DIRECTIONAL, Vector3Zero(), Vector3{ 0, -1, 0 }, Color{ 80, 20, 0, 255 }, fogShader);
  CreateLight(LIGHT_DIRECTIONAL, Vector3Zero(), Vector3{ 0, 1, 3 }, Color{ 54, 70, 128, 255 }, fogShader);

  // LOAD MODEL
  int models_count = 500;
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
  
  // GENERATE LANDSCAPE
  int size_x = 100;
  int size_y = 100;
  float cell_size = 1.f;
  Vector3 water_pos = { 0, -2.5, 0 };
  Model water = LoadModelFromMesh(GenMeshPlane(float(size_x) * cell_size, float(size_y) * cell_size, 4, 3));
  std::vector<std::vector<float>> height_map;
  Model landscape = LoadModelFromMesh(GenLandscape(size_x, size_y, cell_size, height_map));
  std::vector<std::vector<bool>> free(height_map.size(), std::vector<bool>(height_map.front().size(), true));
  for (int i = 0; i < size_x; ++i)
  {
    for (int j = 0; j < size_y; ++j)
    {
      if (height_map[i][j] <= water_pos.y + 1.0)
      {
        free[i][j] = false;
      }
    }
  }
  std::vector<Vector3> models_pos;
  for (int i = 0; i < models.size(); ++i)
  {
    Vector3 p;
    bool success = SetModelsPos(cell_size, height_map, free, p);
    if (success)
    {
      models_pos.push_back(p);
    }
    else
    {
      for (int j = 0; j < models[i].size(); ++j)
      {
        UnloadModel(models[i][j]);
      }
      models_pos.erase(models_pos.begin() + i);
      --i;
    }
  }
  
  // SET MODEL SHADERS
  landscape.materials[0].shader = shadowShader;
  water.materials[0].shader = fogShader;
  for (int i = 0; i < models.size(); ++i)
  {
    for (int j = 0; j < models[i].size(); ++j)
    {
      models[i][j].materials[0].shader = shadowShader;
    }
  }

  RenderTexture2D shadowMap = LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
  Camera3D lightCam = { 0 };
  lightCam.position = Vector3Scale(lightDir, -200.0f);
  lightCam.target = Vector3Scale(lightDir, 200.0f);
  // Use an orthographic projection for directional lights
  lightCam.projection = CAMERA_ORTHOGRAPHIC;
  lightCam.up = { 0.0f, 1.0f, 0.0f };
  lightCam.fovy = 180.0f;
  
  while (!WindowShouldClose())
  {  
    UpdateCamera(&camera, CAMERA_FREE);
    Vector3 cameraPos = camera.position;
    SetShaderValue(shadowShader, shadowShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(fogShader, fogShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
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
          DrawModel(models[i][j], models_pos[i], 1.0f, WHITE);
        }
      }
      EndMode3D();
      EndTextureMode();

      Matrix lightViewProj = MatrixMultiply(lightView, lightProj);
      ClearBackground(SKYBLUE);
      SetShaderValueMatrix(shadowShader, lightVPLocShadow, lightViewProj);
      rlEnableShader(shadowShader.id);
      int slot = 10; // Can be anything 0 to 15, but 0 will probably be taken up
      rlActiveTextureSlot(10);
      rlEnableTexture(shadowMap.depth.id);
      rlSetUniform(shadowMapLocShadow, &slot, SHADER_UNIFORM_INT, 1);
      BeginMode3D(camera);
      DrawModel(water, water_pos, 1.0f, DARKBLUE);
      DrawModel(landscape, {0, 0, 0}, 1.0f, { 15, 75, 25, 255 });
      for (int i = 0; i < models.size(); ++i)
      {
        for (int j = 0; j < models[i].size(); ++j)
        {
          DrawModel(models[i][j], models_pos[i], 1.0f, WHITE);
        }
      }
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
  UnloadShader(fogShader);
  UnloadModel(water);
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


static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  bool res = false;
  lpng::float3 model_size;

  switch (type)
  {
    
  case STONE:
  {
    lpng::GenerateObjectStone* stone_ptr = new lpng::GenerateObjectStone();
    float x_k = float(fast_lpng_rand(700, 1200)) / 1000.f;
    float y = float(fast_lpng_rand(900, 1500)) / 1000.f;
    model_size = {y * x_k, y, y * x_k};
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
    treeParams.height = float(fast_lpng_rand(4000, 6000)) / 1000.f;
    treeParams.firstRad = float(fast_lpng_rand(200, 400)) / 1000.f;
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
    firParams.height = float(fast_lpng_rand(4000, 6000)) / 1000.f;
    firParams.firstRad = float(fast_lpng_rand(200, 400)) / 1000.f;
    model_size.x = 2 * firParams.firstRad;
    model_size.z = 2 * firParams.firstRad;
    model_size.y = firParams.height;
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


static Mesh GenLandscape(int size_x, int size_y, float cell_size, std::vector<std::vector<float>>& height_map)
{
  Mesh landscape = { 0 };
  landscape.triangleCount = size_x * size_y * 2;
  landscape.vertexCount = landscape.triangleCount * 3;
  landscape.vertices = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));
  landscape.normals = (float*)MemAlloc(landscape.vertexCount * 3 * sizeof(float));
  int x_offset = fast_lpng_rand(0, 150);
  int y_offset = fast_lpng_rand(0, 150);
  for (int x = 0; x < size_x; ++x)
  {
    std::vector<float> noise;
    for (int y = 0; y < size_y; ++y)
    {
      float n = db::perlin(double(x + x_offset) / 12.0, double(y + y_offset) / 12.0);
      float v = db::lerp(0.f, 12.f, n);
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


static bool SetModelsPos(float cell_size, const std::vector<std::vector<float>>& height_map, std::vector<std::vector<bool>>& free, Vector3& p)
{
  int k = 0;
  int size_x = height_map.size();
  int size_y = height_map.front().size();
  while (k <= 10)
  {
    int x = fast_lpng_rand(4, size_x - 3);
    int y = fast_lpng_rand(4, size_y - 3);
    if (free[x][y])
    {
      for (int i = -1; i < 2; ++i)
      {
        for (int j = -1; j < 2; ++j)
        {
          free[std::clamp(x + i, 0, size_x - 1)][std::clamp(y + j, 0, size_y - 1)] = false;
        }
      }
      p = { float(x) * cell_size - float(size_x) * cell_size / 2.f, height_map[x][y], float(y) * cell_size - float(size_y) * cell_size / 2.f };
      return true;
    }
  }
  return false;
}


RenderTexture2D LoadShadowmapRenderTexture(int width, int height)
{
  RenderTexture2D target = { 0 };

  target.id = rlLoadFramebuffer(width, height); // Load an empty framebuffer
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