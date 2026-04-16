#include <vector>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum eVertexArrayObject {
    VAO_CONTROL_GEOMETRY,
    VAO_BEZIER_CURVE,
    VAO_COUNT
};

enum eVertexBufferObject {
    VBO_CONTROL_POINTS,
    VBO_CURVE_POINTS,
    VBO_COUNT
};

enum eProgram {
    SHADER_MAIN,
    PROGRAM_COUNT
};

enum eTexture {
    TEX_NONE,
    TEXTURE_COUNT
};

#include <common.cpp>
GLchar windowTitle[] = "Beadando_2";

int draggedPoint = -1;

std::vector<glm::vec3> controlPoints = {
    glm::vec3(-0.6f, -0.4f, 0.0f),
    glm::vec3(-0.2f,  0.6f, 0.0f),
    glm::vec3(0.2f,  0.6f, 0.0f),
    glm::vec3(0.6f, -0.4f, 0.0f)
};
std::vector<glm::vec3> bezierCurvePoints;

const glm::vec3 COLOR_CONTROL_POLYGON = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 COLOR_BEZIER_CURVE = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 COLOR_CONTROL_POINTS = glm::vec3(0.0f, 1.0f, 0.0f);
const float POINT_SIZE_PIXELS = 8.0f;
const float CURVE_LINE_WIDTH = 5.0f;
const int   CURVE_SEGMENTS_COUNT = 200;

long long calculateBinomial(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    if (k > n / 2) k = n - k;
    long long result = 1;
    for (int i = 1; i <= k; ++i) {
        result = result * (n - i + 1) / i;
    }
    return result;
}

void generateBezierCurve() {
    bezierCurvePoints.clear();
    int degree = (int)controlPoints.size() - 1;
    if (degree < 1) return;

    for (int i = 0; i <= CURVE_SEGMENTS_COUNT; ++i) {
        float t = (float)i / CURVE_SEGMENTS_COUNT;
        glm::vec3 pointOnCurve(0.0f);

        for (int j = 0; j <= degree; ++j) {
            float weight = (float)calculateBinomial(degree, j) * pow(t, j) * pow(1.0f - t, degree - j);
            pointOnCurve += controlPoints[j] * weight;
        }
        bezierCurvePoints.push_back(pointOnCurve);
    }
}

void syncGeometryToGPU() {
    generateBezierCurve();

    // Kontrollpontok és poligon szinkronizálása
    glBindVertexArray(VAO[VAO_CONTROL_GEOMETRY]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBO_CONTROL_POINTS]);
    glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), controlPoints.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Görbe pontjainak szinkronizálása
    glBindVertexArray(VAO[VAO_BEZIER_CURVE]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBO_CURVE_POINTS]);
    glBufferData(GL_ARRAY_BUFFER, bezierCurvePoints.size() * sizeof(glm::vec3), bezierCurvePoints.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

glm::vec3 convertCursorToWorldSpace(double xpos, double ypos) {
    float ndcX = (float)(2.0 * xpos) / windowWidth - 1.0f;
    float ndcY = 1.0f - (float)(2.0 * ypos) / windowHeight;
    glm::mat4 invVP = glm::inverse(matProjection * matModelView);
    glm::vec4 worldPos = invVP * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    return glm::vec3(worldPos.x, worldPos.y, 0.0f);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    glm::vec3 mouseWorldPos = convertCursorToWorldSpace(x, y);
    const float selectionThreshold = 0.025f;

    if (action == GLFW_PRESS) {
        int targetIndex = -1;
        for (int i = 0; i < (int)controlPoints.size(); ++i) {
            if (glm::distance(mouseWorldPos, controlPoints[i]) < selectionThreshold) {
                targetIndex = i;
                break;
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (targetIndex != -1) {
                draggedPoint = targetIndex;
            }
            else {
                controlPoints.push_back(mouseWorldPos);
                syncGeometryToGPU();
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (targetIndex != -1 && controlPoints.size() > 2) {
                controlPoints.erase(controlPoints.begin() + targetIndex);
                syncGeometryToGPU();
            }
        }
    }
    else if (action == GLFW_RELEASE) {
        draggedPoint = -1;
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (draggedPoint != -1) {
        controlPoints[draggedPoint] = convertCursorToWorldSpace(xpos, ypos);
        syncGeometryToGPU();
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width; windowHeight = height;
    glViewport(0, 0, width, height);

    float aspectRatio = (float)width / (float)height;
    if (width < height)
        matProjection = glm::ortho(-1.0f, 1.0f, -1.0f / aspectRatio, 1.0f / aspectRatio, -1.0f, 1.0f);
    else
        matProjection = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

    matModelView = glm::mat4(1.0f);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program[SHADER_MAIN]);
    GLuint projLoc = glGetUniformLocation(program[SHADER_MAIN], "matProjection");
    GLuint mvLoc = glGetUniformLocation(program[SHADER_MAIN], "matModelView");
    GLuint colorLoc = glGetUniformLocation(program[SHADER_MAIN], "objectColor");
    GLuint isPointLoc = glGetUniformLocation(program[SHADER_MAIN], "isPoint");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(matProjection));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(matModelView));

    //Kontrollpoligon
    glUniform1i(isPointLoc, 0);
    glUniform3fv(colorLoc, 1, glm::value_ptr(COLOR_CONTROL_POLYGON));
    glBindVertexArray(VAO[VAO_CONTROL_GEOMETRY]);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)controlPoints.size());

    //Bézier-görbe
    glLineWidth(CURVE_LINE_WIDTH);
    glUniform3fv(colorLoc, 1, glm::value_ptr(COLOR_BEZIER_CURVE));
    glBindVertexArray(VAO[VAO_BEZIER_CURVE]);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)bezierCurvePoints.size());
    glLineWidth(1.0f);

    //Kontrollpontok
    glPointSize(POINT_SIZE_PIXELS);
    glUniform1i(isPointLoc, 1);
    glUniform3fv(colorLoc, 1, glm::value_ptr(COLOR_CONTROL_POINTS));
    glBindVertexArray(VAO[VAO_CONTROL_GEOMETRY]);
    glDrawArrays(GL_POINTS, 0, (GLsizei)controlPoints.size());
}

int main() {
    setlocale(LC_ALL, "hungarian");
    cout << "Bal klikk: Új pont hozzáadása" << endl;
    cout << "Bal klikk húzás: Mozgatás" << endl;
    cout << "Jobb klikk: Pont törlése" << endl;
    cout << "ESC: Kilépés" << endl;
    init(3, 3, GLFW_OPENGL_CORE_PROFILE);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "vertexShader.glsl", 0 },
        { GL_FRAGMENT_SHADER, "fragmentShader.glsl", 0 },
        { GL_NONE, nullptr, 0 }
    };
    program[SHADER_MAIN] = LoadShaders(shaders);
    syncGeometryToGPU();
    framebufferSizeCallback(window, windowWidth, windowHeight);
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cleanUpScene(0);
    return 0;
}
