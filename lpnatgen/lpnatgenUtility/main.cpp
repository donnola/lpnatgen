#define _CRT_SECURE_NO_WARNINGS
#include "lpng_tree.h"
#include "lpng_test.h"
#include "lpng_stone.h"
#include "lpng_rand.h"
#include <iostream>
#include <memory>
#include <ctime>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"


struct InputBoxDesc
{
  Rectangle box;
  std::string box_name;
  char text[1024];
};

enum ObjectTypes
{
  TEST = 0,
  STONE = 1,
  TREE = 2,
  //BUSH = 2,
  
};


static Mesh GenMesh(const std::vector<lpng::Mesh> &model);
static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);
static void InputBox(std::vector<InputBoxDesc>& inputBoxes, const int activeInputBoxId);


int main(void)
{
  const int screenWidth = 1400;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "lpngUtility : object generation");

  RenderTexture screen1 = LoadRenderTexture(screenWidth / 2, screenHeight);

  Camera camera = { 0 };
  camera.fovy = 45.0f;
  camera.up.y = 1.0f;
  camera.target.y = 0.0f;
  camera.position.x = 15.0f;
  camera.position.y = 15.0f;

  RenderTexture screen2 = LoadRenderTexture(screenWidth / 2, screenHeight);
  Rectangle splitScreenRect = { 0.0f, 0.0f, (float)screen1.texture.width, (float)-screen1.texture.height };
  Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };

  DisableCursor();

  SetTargetFPS(60);

  fast_lpng_rand(int(std::time(0)));

  int modelTypeActive = TEST;
  bool modelTypeEditMode = false;

  Rectangle btnCreateBounds = { 12, 100, 140, 30 };
  std::string createModelText = "Create model";

  Rectangle btnSaveBounds = { 12, 220, 140, 30 };
  std::string saveModelText = "Save model";

  std::vector<InputBoxDesc> inputBoxes{ 
    {Rectangle{ 120, 140, 140, 30 }, "Model name", ""},
    {Rectangle{ 120, 180, 140, 30 }, "Save path", ""}
  };

  char* textPathPtr = inputBoxes[1].text;

  int inputState = 0;
  bool inputAction = false;
  bool isInputActive = false;
  int activeInputBoxId = -1;

  Vector2 mousePoint = { 0.0f, 0.0f };

  std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  modelPtr->Generate();
  unsigned int modelSeed = modelPtr->GetModelSeed();
  std::vector<lpng::Mesh> generatedModel = modelPtr->GetModel();
  Model model = LoadModelFromMesh(GenMesh(generatedModel));

  GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
  
  while (!WindowShouldClose())
  {

    if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    if (IsKeyPressed(KEY_TAB))
    {
      if (IsCursorHidden()) EnableCursor();
      else DisableCursor();
    }

    mousePoint = GetMousePosition();

    if (fileDialogState.SelectFilePressed)
    {
      char pathToLoad[1024] = { 0 };
      if (IsFileExtension(fileDialogState.fileNameText, ".png"))
      {
        strcpy(pathToLoad, TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
      }

      fileDialogState.SelectFilePressed = false;
    }

    BeginTextureMode(screen1);
    ClearBackground({ 230, 245, 255, 255 });

    if (modelTypeEditMode || fileDialogState.windowActive) GuiLock();

    if (GuiButton(btnCreateBounds, createModelText.c_str()))
    {
      bool isNewModel = GenerateObjectWithType(modelTypeActive, modelPtr);
      if (!isNewModel)
        break;
      modelPtr->Generate();
      modelSeed = modelPtr->GetModelSeed();
      generatedModel = modelPtr->GetModel();
      UnloadModel(model);
      model = LoadModelFromMesh(GenMesh(generatedModel));
    }
    if (GuiButton(btnSaveBounds, saveModelText.c_str())) modelPtr->SaveModel();
    InputBox(inputBoxes, activeInputBoxId);
    if (GuiButton(Rectangle{ 270, 180, 30, 30 }, GuiIconText(ICON_FILE_OPEN, ""))) fileDialogState.windowActive = true;

    GuiUnlock();
    GuiWindowFileDialog(&fileDialogState);

    DrawText(TextFormat("MODEL SEED : %u", modelSeed), 10, 710, 22, MAROON);
    DrawText("PRESS TAB TO ENABLE CURSOR", 10, 740, 22, MAROON);
    if (GuiDropdownBox({ 12, 20, 140, 30 }, "TEST;STONE;TREE", &modelTypeActive, modelTypeEditMode)) modelTypeEditMode = !modelTypeEditMode; //;BUSH

    EndTextureMode();

    BeginTextureMode(screen2);
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    for (const lpng::Mesh& m : generatedModel)
    {
      for (const lpng::float3& v : m.vertexCoords)
      {
        DrawSphere({v.x, v.y, v.z}, 0.05f, DARKPURPLE);
      }
    }
    
    DrawModel(model, modelPosition, 1.0f, RED);
    DrawModelWires(model, modelPosition, 1.0f, DARKPURPLE);

    DrawGrid(10, 1.0f);
    EndMode3D();

    EndTextureMode();

    BeginDrawing();

    DrawTextureRec(screen1.texture, splitScreenRect, { 0, 0 }, WHITE);
    DrawTextureRec(screen2.texture, splitScreenRect, { screenWidth / 2.0f, 0 }, WHITE);
    DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);
    EndDrawing();
  }
  UnloadRenderTexture(screen1);
  UnloadRenderTexture(screen2);
  UnloadModel(model);
  modelPtr.reset();

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


static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  switch (type)
  {
  case TEST:
  {
    model_ptr.reset(new lpng::GenerateObjectTest());
    return true;
  }
    
  case STONE:
  {
    model_ptr.reset(new lpng::GenerateObjectStone());
    return true;
  }
    
  //case BUSH:
  //  return new lpng::GenerateObjectBush();
  case TREE:
  {
    model_ptr.reset(new lpng::GenerateObjectTree());
    return true;
  }
  default:
    return false;
  }
  return false;
}


static void InputBox(std::vector<InputBoxDesc>& inputBoxes, const int activeInputBoxId)
{
  for (int i = 0; i < inputBoxes.size(); ++i)
  {
    char* text = inputBoxes[i].text;
    const std::string& box_name = inputBoxes[i].box_name;
    const Rectangle& box = inputBoxes[i].box;
    DrawText(box_name.c_str(), 12, (int)box.y + 8, 16, BLACK);
    GuiTextBox(box, text, 1024, true);
  }
}