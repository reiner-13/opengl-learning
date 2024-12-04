#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glad/glad.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera.h>
#include <FastNoiseLite.h>

#include <iostream>

// Screen settings
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos(0.0f, 1.0f, -3.0f);

void processInput(const Uint8* keyboardState);
void handleMouseMotion(SDL_Event& e);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0) {
        std::cerr << "SDL_image could not initialize! SDL_Error: " << IMG_GetError() << std::endl;
        return -1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL + OpenGL", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Enable relative mouse mode to lock cursor
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Shader objectShader("object_shader.vert", "object_shader.frag");

    // Noise
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // You can choose other noise types like Simplex
    noise.SetFrequency(0.02f); // Adjust the frequency for scaling

    // Generate noise
    float noiseData[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            // Normalize coordinates to a range like 0.0 to 1.0
            float nx = static_cast<float>(x) / SCREEN_WIDTH;
            float ny = static_cast<float>(y) / SCREEN_HEIGHT;

            // Generate noise
            noiseData[y * SCREEN_WIDTH + x] = noise.GetNoise(nx, ny);
        }
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    // Upload the noise data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RED, GL_FLOAT, noiseData);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //delete[] noiseData;

    // Vertex data
    float vertices[] = {
    // Positions          // Texture Coords
    // Front face
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // Top-left

    // Back face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-left

    // Left face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-right
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-left

    // Right face
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-left

    // Top face
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // Top-left

    // Bottom face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-right
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f   // Top-left
};

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    unsigned int indices[] = {
    // Front face
    0, 1, 2,
    2, 3, 0,

    // Back face
    4, 5, 6,
    6, 7, 4,

    // Left face
    8, 9, 10,
    10, 11, 8,

    // Right face
    12, 13, 14,
    14, 15, 12,

    // Top face
    16, 17, 18,
    18, 19, 16,

    // Bottom face
    20, 21, 22,
    22, 23, 20
};

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Copy vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    objectShader.use();
    //objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    //objectShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    objectShader.setMat4("projection", projection);

    glm::vec2 scrollOffset(1.0f);
    const float scrollSpeed = 0.1f;
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
        processInput(keyboardState);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            } else if (e.type == SDL_MOUSEMOTION) {
                handleMouseMotion(e);
            }
        }

        // Render
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(0.8f, 0.7f, 0.2f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        objectShader.use();
        glm::mat4 view = camera.GetViewMatrix();
        objectShader.setMat4("view", view);

        objectShader.setVec2("scrollOffset", scrollOffset);

        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(*cubePositions); i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            objectShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    IMG_Quit();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void processInput(const Uint8* keyboardState) {
    if (keyboardState[SDL_SCANCODE_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keyboardState[SDL_SCANCODE_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keyboardState[SDL_SCANCODE_A]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keyboardState[SDL_SCANCODE_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keyboardState[SDL_SCANCODE_SPACE]) camera.ProcessKeyboard(UP, deltaTime);
    if (keyboardState[SDL_SCANCODE_LSHIFT]) camera.ProcessKeyboard(DOWN, deltaTime);
}

void handleMouseMotion(SDL_Event& e) {
    float xoffset = static_cast<float>(e.motion.xrel);
    float yoffset = -static_cast<float>(e.motion.yrel); // Reversed since y-coordinates go from bottom to top

    camera.ProcessMouseMovement(xoffset, yoffset);
}