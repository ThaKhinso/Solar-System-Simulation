
#include "raylib.h"
#include "raymath.h"
#include <string>

Model LoadModelObb(char* filename) {
	char* tofix = new char[strlen(filename) + 1];
	strcpy(tofix, filename);
	int length = strlen(filename);
	tofix[length - 1] = 'j';
	tofix[length - 2] = 'b';
	tofix[length - 3] = 'o';
	printf("Loading model %s\n", tofix);
	return LoadModel(tofix);
    
}

struct Planet {
	Model model;
	Texture2D texture;
	Vector3 position;
	float radius;
	float rotationAnglePerSecond;
	float orbitSpeed;
	float rotationSpeed;
    uint64_t mass;

};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Solar Systems");
    Camera camera = { 0 };
    camera.position = { 50.0f, 50.0f, 50.0f }; // Camera position
    camera.target = { 0.0f, 10.0f, 0.0f };     // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type

    Model Earth = LoadModel("resources/earth/Earth.obj");                 // Load model
    Texture2D texture = LoadTexture("resources/earth/Maps/ColorMap.jpg"); // Load model texture
    Earth.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture
    Vector3 position = { 0,0,0 };
	float angle = 0.0f;

    Planet earth = {
        Earth,
        texture,
        position,
        20000,
        0.000729,
        5,
        .2f,
		5.972 * pow(10, 24)
    };

    float rotationRadius = 30.0f; // Radius of the circular path
    float rotationSpeed = 1.0f; // Speed of rotation
    float rotationAngle = 0.0f;

    //Model Moon = LoadModel("resources/moon/Moon.obj");
	/*Texture2D MoonTexture = LoadTexture("resources/moon/Maps/ColorMap.jpg");*/
    Vector3 moonPosition = { position.x + 100, 0, 0 };

	BoundingBox box = GetModelBoundingBox(Earth);
	
	Model Sun = LoadModel("resources/earth/untitled.obj");                 // Load model
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    DisableCursor();
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
		float dt = GetFrameTime();
		angle += earth.rotationAnglePerSecond / 60;
		rotationAngle += rotationSpeed * dt;
		Earth.transform = MatrixRotateY(angle);

        Vector3 rotatingSpherePosition;
        rotatingSpherePosition.x = (moonPosition.x - position.x) / 2.0f + rotationRadius * cosf(rotationAngle);
        rotatingSpherePosition.y = position.y; // Keep it on the same horizontal plane
        rotatingSpherePosition.z = position.z + rotationRadius * sinf(rotationAngle);
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FREE);
        if (IsKeyPressed('Z')) camera.target = { 0.0f, 0.0f, 0.0f };
        ClearBackground(BLACK);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            BeginMode3D(camera);
                DrawModel(Earth, position, 1, WHITE);
				DrawModel(Earth, rotatingSpherePosition, 1, WHITE);

            EndMode3D();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
