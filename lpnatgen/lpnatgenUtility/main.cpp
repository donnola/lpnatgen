#define _CRT_SECURE_NO_WARNINGS
#include <lpnatgen.h>
#include <iostream>
#include "raylib.h"
#define NUM_MODELS 1 

static Mesh GenMesh(const std::vector<lpng::Mesh> &model);

int main(void)
{
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "lpnatgenUtility : object generation");

  // Define the camera to look into our 3d world
  Camera camera = { 0 };
  camera.position = { 10.0f, 10.0f, 10.0f }; // Camera position
  camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
  camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
  camera.fovy = 45.0f;                                // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

  Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };
  Vector2 modelScreenPosition = { 0.0f, 0.0f };

  DisableCursor();                    // Limit cursor to relative movement inside the window

  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  lpng::GenerateObject* m = new lpng::GenerateObjectStone();
  m->Generate();
  std::vector<lpng::Mesh> generatedModel = m->GetModel();
  Model model = LoadModelFromMesh(GenMesh(generatedModel));

  // Main game loop
  while (!WindowShouldClose())        // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    // Calculate cube screen space position (with a little offset to be in top)
    modelScreenPosition = GetWorldToScreen({ modelPosition.x, modelPosition.y, modelPosition.z }, camera);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawModel(model, modelPosition, 1.0f, RED);
    DrawModelWires(model, modelPosition, 1.0f, DARKPURPLE);

    DrawGrid(10, 1.0f);

    EndMode3D();

    DrawText(TextFormat("Cube position in screen space coordinates: [%i, %i]", (int)modelScreenPosition.x, (int)modelScreenPosition.y), 10, 10, 20, LIME);

    EndDrawing();
  }

  UnloadModel(model);
  delete m;

  CloseWindow();

  return 0;
}

static Mesh GenMesh(const std::vector<lpng::Mesh>& model)
{
  int triangleCount = 0;
  for (const lpng::Mesh& m : model)
  {
    triangleCount += m.faces.size();
  }
  Mesh mesh = { 0 };
  mesh.triangleCount = triangleCount;
  mesh.vertexCount = mesh.triangleCount * 3;
  mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
  mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

  int nextVertId = 0;
  for (const lpng::Mesh& obj : model)
  {
    for (const lpng::Face& f : obj.faces)
    {
      for (int i = 0; i < 3; i++)
      {
        lpng::Vertex v = f[i];
        lpng::float3 vc = obj.vertexCoords[v.vi - 1];
        lpng::float3 vn = obj.vertexNormals[v.vni - 1];
        lpng::float2 vt = obj.vertexTextCoords[v.vti - 1];
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
  }
  UploadMesh(&mesh, false);

  return mesh;
}