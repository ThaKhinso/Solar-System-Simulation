
#include "raylib.h"
#include "raymath.h"
#include <string>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

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
    const int screenWidth = 800;
    const int screenHeight = 600;

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

    Model moon = LoadModel("resources/earth/Earth.obj");
    Texture2D moonTexture = LoadTexture("resources/earth/moonTexture.jpg");
    moon.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = moonTexture;
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
    float rotationSpeed = 0.01f; // Speed of rotation
    float rotationAngle = 0.0f;
    float semiMajorAxis = 100.0f;
    float semiMinorAxis = 50.0f;

    //Model Moon = LoadModel("resources/moon/Moon.obj");
	/*Texture2D MoonTexture = LoadTexture("resources/moon/Ma ps/ColorMap.jpg");*/
    Vector3 moonPosition = { position.x + 100, 0, 0 };

	BoundingBox box = GetModelBoundingBox(Earth);
    moon.transform = MatrixScale(0.25, 0.25, 0.25);
	//Model Sun = LoadModel("resources/earth/untitled.obj");                 // Load model
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    DisableCursor();
    // Main game loop


    Model skybox = LoadModelFromMesh(GenMeshCube(1.f,1.f, 1.f));
    Texture2D skyboxRight = LoadTexture("resources/spaceSkyBox/space_rt.png"); // Right face
    Texture2D skyboxLeft = LoadTexture("resources/spaceSkyBox/space_lf.png");   // Left face
    Texture2D skyboxTop = LoadTexture("resources/spaceSkyBox/space_up.png");     // Top face
    Texture2D skyboxBottom = LoadTexture("resources/spaceSkyBox/space_dn.png"); // Bottom face
    Texture2D skyboxFront = LoadTexture("resources/spaceSkyBox/space_ft.png"); // Front face
    Texture2D skyboxBack = LoadTexture("resources/spaceSkyBox/space_bk.png");   // Back face

    skybox.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyboxFront;   // Front face
    skybox.materials[0].maps[MATERIAL_MAP_SPECULAR].texture = skyboxBack;  // Back face
    skybox.materials[0].maps[MATERIAL_MAP_NORMAL].texture = skyboxLeft;    // Left face
    skybox.materials[0].maps[MATERIAL_MAP_EMISSION].texture = skyboxRight; // Right face
    skybox.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyboxTop;    // Top face
    skybox.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyboxBottom; // Bottom face


    skybox.materials[0].shader = LoadShader(TextFormat("resources/shaders/glsl%i/skybox.vs", GLSL_VERSION),
       TextFormat("resources/shaders/glsl%i/skybox.fs", GLSL_VERSION));

    
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"),  { MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), (int[1]) { useHDR ? 1 : 0 }, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), (int[1]) { useHDR ? 1 : 0 }, SHADER_UNIFORM_INT);

    // Load cubemap shader and setup required shader locations
    Shader shdrCubemap = LoadShader(TextFormat("resources/shaders/glsl%i/cubemap.vs", GLSL_VERSION),
        TextFormat("resources/shaders/glsl%i/cubemap.fs", GLSL_VERSION));

    SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), (int[1]) { 0 }, SHADER_UNIFORM_INT);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
		float dt = GetFrameTime();
		angle += earth.rotationAnglePerSecond / 60;
        rotationAngle += rotationSpeed;
		Earth.transform = MatrixRotateY(angle);

        //Vector3 rotatingSpherePosition;
        //rotatingSpherePosition.x = (position.x - moonPosition.x) / 2.0f + rotationRadius * cosf(rotationAngle);
        //rotatingSpherePosition.y = position.y; // Keep it on the same horizontal plane
        //rotatingSpherePosition.z = position.z + rotationRadius * sinf(rotationAngle);
        Vector3 moonPosition = {
           semiMajorAxis * cosf(rotationAngle),  // X = radius * cos(angle)
           0,                     // Y = 0 (staying on the XY plane)
           semiMinorAxis * sinf(rotationAngle)   // Z = radius * sin(angle)
        };
         
         
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
            DrawModel(skybox,{ 0, 0, 0 }, 1000.0f, WHITE);
                DrawModel(Earth, position, 1, WHITE);
				DrawModel(moon, moonPosition, 1, WHITE);

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
