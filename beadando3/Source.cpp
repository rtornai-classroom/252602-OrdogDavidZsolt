#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

using namespace glm;
using namespace std;

enum eVertexArrayObject { VAOCube, VAOSphere, VAOCount };
enum eBufferObject { VBOCube, VBOSphere, BOCount };
enum eProgram { MainProgram, ProgramCount };
enum eTexture { SunTex, TextureCount };

#include "common.cpp"

char windowTitle[] = "Ördög Dávid - 3.";

float camRadius = 9.0f;
float camAlpha = 0.0f;
float camHeight = 0.0f;

bool lightingEnabled = true;
vec3 lightColor = vec3(0.2f, 0.7f, 0.0f); 
float lightRadius = 18.0f;

int sphereVertexCount = 0;
float sunAlpha = 0.0f;
bool keys[1024];

void initScene() {
    ShaderInfo shaders[] = {
            { GL_VERTEX_SHADER, "vertexShader.glsl" },
            { GL_FRAGMENT_SHADER, "fragmentShader.glsl" },
            { GL_NONE, nullptr }
    };
    program[MainProgram] = LoadShaders(shaders);
    glUseProgram(program[MainProgram]);

    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glBindVertexArray(VAO[VAOCube]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOCube]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    vector<float> sVerts;
    float r = 0.25f;    //gömb sugár
    int stacks = 30, sectors = 30;
    for (int i = 0; i <= stacks; ++i) {
        float phi = pi<float>() / 2 - i * pi<float>() / stacks;
        for (int j = 0; j <= sectors; ++j) {
            float theta = j * 2 * pi<float>() / sectors;
            float x = r * cos(phi) * cos(theta);
            float y = r * cos(phi) * sin(theta);
            float z = r * sin(phi);

            sVerts.push_back(x); sVerts.push_back(y); sVerts.push_back(z);
            sVerts.push_back(x / r); sVerts.push_back(y / r); sVerts.push_back(z / r);
            sVerts.push_back((float)j / sectors); sVerts.push_back((float)i / stacks);
        }
    }

    vector<unsigned int> indices;
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j <= sectors; j++) {
            indices.push_back(i * (sectors + 1) + j);
            indices.push_back((i + 1) * (sectors + 1) + j);
        }
    }

    glBindVertexArray(VAO[VAOSphere]);
    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOSphere]);
    glBufferData(GL_ARRAY_BUFFER, sVerts.size() * sizeof(float), sVerts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    sphereVertexCount = (int)indices.size();

    texture[SunTex] = SOIL_load_OGL_texture(
        "sun.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    if (texture[SunTex] == 0) {
        cerr << "SOIL hiba: " << SOIL_last_result() << endl;
        glGenTextures(1, &texture[SunTex]);
        glBindTexture(GL_TEXTURE_2D, texture[SunTex]);
        unsigned char yellow[] = { 255, 255, 0 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, yellow);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, texture[SunTex]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program[MainProgram]);

    static double lastTime = 0.0;
    float deltaTime = (float)(currentTime - lastTime);
    lastTime = currentTime;

    float rotSpeed = 2.5f * deltaTime;
    float moveSpeed = 6.0f * deltaTime;

    //kamera mozgatás
    if (keys[GLFW_KEY_LEFT])  camAlpha -= rotSpeed;
    if (keys[GLFW_KEY_RIGHT]) camAlpha += rotSpeed;
    if (keys[GLFW_KEY_UP])    camHeight += moveSpeed;
    if (keys[GLFW_KEY_DOWN])  camHeight -= moveSpeed;

    sunAlpha += 1.0f * deltaTime;

    //hengerfelület
    vec3 eye = vec3(camRadius * cos(camAlpha), camRadius * sin(camAlpha), camHeight);

    //up vektor és origo
    matView = lookAt(eye, vec3(0, 0, 0), vec3(0, 0, 1));

    //perspektív 55 fok
    matProjection = perspective(radians(55.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(program[MainProgram], "matView"), 1, GL_FALSE, value_ptr(matView));
    glUniformMatrix4fv(glGetUniformLocation(program[MainProgram], "matProjection"), 1, GL_FALSE, value_ptr(matProjection));
    glUniform3fv(glGetUniformLocation(program[MainProgram], "viewPos"), 1, value_ptr(eye));
    glUniform1i(glGetUniformLocation(program[MainProgram], "lightingEnabled"), lightingEnabled);

     vec3 lPos = vec3(lightRadius * cos(sunAlpha), lightRadius * sin(sunAlpha), 0.0f);

    glUniform3fv(glGetUniformLocation(program[MainProgram], "lightPos"), 1, value_ptr(lPos));
    glUniform3fv(glGetUniformLocation(program[MainProgram], "lightColor"), 1, value_ptr(lightColor));

    glBindVertexArray(VAO[VAOCube]);
    glUniform1i(glGetUniformLocation(program[MainProgram], "isSun"), 0);

    //kockák elhelyezése
    vec3 cubeOffsets[] = {
        vec3(0, 0, 0),
        vec3(0, 0, 2),
        vec3(0, 0, -2)
    };

    for (int i = 0; i < 3; ++i) {
        matModel = translate(mat4(1.0f), cubeOffsets[i]);
        glUniformMatrix4fv(glGetUniformLocation(program[MainProgram], "matModel"), 1, GL_FALSE, value_ptr(matModel));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(VAO[VAOSphere]);
    glUniform1i(glGetUniformLocation(program[MainProgram], "isSun"), 1);
    matModel = translate(mat4(1.0f), lPos);
    glUniformMatrix4fv(glGetUniformLocation(program[MainProgram], "matModel"), 1, GL_FALSE, value_ptr(matModel));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[SunTex]);
    glDrawElements(GL_TRIANGLE_STRIP, sphereVertexCount, GL_UNSIGNED_INT, 0);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (action == GLFW_PRESS) keys[key] = true;
    else if (action == GLFW_RELEASE) keys[key] = false;
    if (key == GLFW_KEY_L && action == GLFW_PRESS) lightingEnabled = !lightingEnabled;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width; windowHeight = height;
    glViewport(0, 0, width, height);
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}

int main() {
    init(3, 3, GLFW_OPENGL_CORE_PROFILE);
    glfwSetKeyCallback(window, keyCallback);
    setlocale(LC_ALL, "");

    cout << "Irányítás:" << endl;
    cout << "Bal / Jobb nyíl  : z tengely körüli forgatás" << endl;
    cout << "Fel / Le nyíl    : z tengely mentén mozgás" << endl;
    cout << "L             : fény be/ki" << endl;
    cout << "ESC           : exit" << endl << endl;
    initScene();
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cleanUpScene(EXIT_SUCCESS);
    return 0;
}
