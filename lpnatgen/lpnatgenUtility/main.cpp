#define _CRT_SECURE_NO_WARNINGS
#include <lpnatgen_tree.h>
#include <lpnatgen_test.h>
#include <lpnatgen_stone.h>
#include <iostream>
#include <memory>
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
static void InputBox(std::vector<InputBoxDesc>& inputBoxes, const int inputState, const int activeInputBoxId, const Font& font, const size_t framesCounter);


int main(void)
{
  const int screenWidth = 1000;
  const int screenHeight = 560;

  InitWindow(screenWidth, screenHeight, "lpnatgenUtility : object generation");

  // Define the camera to look into our 3d world
  Camera camera = { 0 };
  camera.position = { 10.0f, 10.0f, 10.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };
  Vector2 modelScreenPosition = { 0.0f, 0.0f };

  DisableCursor();

  SetTargetFPS(60);

  Font fonts = LoadFont("resources/fonts/alpha_beta.png");

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

  std::unique_ptr<lpng::GenerateObject> model_ptr{ std::make_unique<lpng::GenerateObjectTest>() };
  model_ptr->Generate();
  std::vector<lpng::Mesh> generatedModel = model_ptr->GetModel();
  Model model = LoadModelFromMesh(GenMesh(generatedModel));


  Font font = LoadFont("resources/fonts/romulus.png");
  size_t framesCounter = 0;

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

    modelScreenPosition = GetWorldToScreen({ modelPosition.x, modelPosition.y, modelPosition.z }, camera);

    BeginDrawing();

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

    if (modelTypeEditMode || fileDialogState.windowActive) GuiLock();

    if (GuiButton(btnCreateBounds, createModelText.c_str()))
    {
      bool isNewModel = GenerateObjectWithType(modelTypeActive, model_ptr);
      if (!isNewModel)
        break;
      model_ptr->Generate();
      generatedModel = model_ptr->GetModel();
      UnloadModel(model);
      model = LoadModelFromMesh(GenMesh(generatedModel));
    }
    if (GuiButton(btnSaveBounds, saveModelText.c_str())) model_ptr->SaveModel();
    InputBox(inputBoxes, inputState, activeInputBoxId, font, framesCounter);
    if (GuiButton(Rectangle{ 270, 180, 30, 30 }, GuiIconText(ICON_FILE_OPEN, ""))) fileDialogState.windowActive = true;

    GuiUnlock();
    GuiWindowFileDialog(&fileDialogState);

    DrawText("PRESS TAB TO ENABLE CURSOR", 10, 420, 16, MAROON);
    if (GuiDropdownBox({ 12, 20, 140, 30 }, "TEST;STONE;TREE", &modelTypeActive, modelTypeEditMode)) modelTypeEditMode = !modelTypeEditMode; //;BUSH;TREE
    EndDrawing();
  }

  UnloadModel(model);
  model_ptr.reset();

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


static void InputBox(std::vector<InputBoxDesc>& inputBoxes, const int inputState, const int activeInputBoxId, const Font& font, const size_t framesCounter)
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