enum eVertexArrayObject {
	VAOVerticesData,
	VAOCount
};
enum eBufferObject {
	VBOVerticesData,
	BOCount
};
enum eProgram {
	QuadScreenProgram,
	ProgramCount
};
enum eTexture {
	NoTexture,
	TextureCount
};

#include <cmath>
#include <common.cpp>

// 50 pixel sugár OpenGL Normalized Device Coordinatesbe (-1.0 -> 1.0)
static float radius = 50.0f / 300.0f;
static array<vec2, 8> vertices = {
	vec2(-radius, -radius), vec2(radius, -radius), vec2(-radius,  radius),
	vec2(radius, -radius), vec2(radius,  radius), vec2(-radius,  radius),

	vec2(-0.333333f, 0.0f), vec2(0.333333f, 0.0f)
};

GLchar windowTitle[] = "Beadando 1";

void initShaderProgram() {
	ShaderInfo shader_info[] = {
		{ GL_FRAGMENT_SHADER,	"./fragmentShader.glsl" },
		{ GL_VERTEX_SHADER,		"./vertexShader.glsl" },
		{ GL_NONE,				nullptr }
	};
	program[QuadScreenProgram] = LoadShaders(shader_info);
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), vertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[VAOVerticesData]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glUseProgram(program[QuadScreenProgram]);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {}

void display(GLFWwindow* window, double currentTime) {
	static double lastTime = currentTime;
	float passedTime = (float)(currentTime - lastTime);
	lastTime = currentTime;

	static float center_x = 0.0f;
	static float center_y = 0.0f;

	static bool isBouncing2D = false;
	float speedPixels = 10.0f / 300.0f;

	// Jobbra-balra mozgás sebessége (X irány), kezdetben 10 pixel / 300 pixel = 0.0333 egység / frame
	static float dirX = speedPixels;
	static float dirY = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !isBouncing2D) {
		isBouncing2D = true;
		float angleRad = 25.0f * 3.1415926535f / 180.0f;	// 25 fokos szög átváltása radiánba (fok * PI / 180)
		float currentDirection = (dirX > 0) ? 1.0f : -1.0f;
		dirX = currentDirection * speedPixels * cos(angleRad);
		dirY = speedPixels * sin(angleRad);
	}

	static float lineY = 0.0f;	// Vonal magassága
	float lineSpeed = 1.2f;		// Vonal sebessége

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		lineY += lineSpeed * passedTime;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		lineY -= lineSpeed * passedTime;
	}

	if (lineY > 1.0f) lineY = 1.0f;
	if (lineY < -1.0f) lineY = -1.0f;

	float stepX = dirX * 60.0f * passedTime;	// Kör tervezett lépés (sebesség * 60FPS arány * eltelt idő)
	float stepY = dirY * 60.0f * passedTime;

	float next_center_x = center_x + stepX;
	float next_center_y = center_y + stepY;

	// Falak koordinátái
	float maxPos = 1.0f - radius; // Jobb / Felső fal
	float minPos = -1.0f + radius; // Bal / Alsó fal

	// X irányú pattogás vizsgálata
	if (next_center_x > maxPos) {
		center_x = maxPos - (next_center_x - maxPos);
		dirX = -dirX;
	}
	else if (next_center_x < minPos) {
		center_x = minPos + (minPos - next_center_x);
		dirX = -dirX;
	}
	else {
		center_x = next_center_x;
	}

	// Y irányú pattogás vizsgálata
	if (next_center_y > maxPos) {
		center_y = maxPos - (next_center_y - maxPos);
		dirY = -dirY;
	}
	else if (next_center_y < minPos) {
		center_y = minPos + (minPos - next_center_y);
		dirY = -dirY;
	}
	else {
		center_y = next_center_y;
	}

	// A kék szakaszon az a pont (closestX), ami a legközelebb van a körhöz
	float closestX = center_x;
	// Rászorítjuk ezt a pontot a szakasz két vége közé (-0.33 és +0.33)
	if (closestX < -0.333333f) closestX = -0.333333f;
	else if (closestX > 0.333333f) closestX = 0.333333f;

	float closestY = lineY;

	float distX = center_x - closestX;
	float distY = center_y - closestY;
	float distanceSquared = (distX * distX) + (distY * distY);

	int swapColors = 0;

	if (distanceSquared > (radius * radius)) {
		swapColors = 1;
	}

	glClear(GL_COLOR_BUFFER_BIT); // Letöröljük a képernyőt

	GLint centerLoc = glGetUniformLocation(program[QuadScreenProgram], "u_center");
	GLint typeLoc = glGetUniformLocation(program[QuadScreenProgram], "u_objectType");
	GLint swapLoc = glGetUniformLocation(program[QuadScreenProgram], "u_swapColors");

	glUniform1i(typeLoc, 1);             // 1 = vonal
	glUniform2f(centerLoc, 0.0f, lineY); // vonal magasságát
	glLineWidth(3.0f);                   // vonalvastagság 3 pixelre
	glDrawArrays(GL_LINES, 6, 2);        // a vertices 6. indexétől kezdve 2 pontból vonalat rajzol

	glUniform1i(typeLoc, 0);             // 0 = kör
	glUniform2f(centerLoc, center_x, center_y);		// kör (X, Y) középpontját
	glUniform1i(swapLoc, swapColors);    // volt-e metszés
	glDrawArrays(GL_TRIANGLES, 0, 6);    // a verticles 0. indextől kezdve 6 pontból 2 háromszöget rajzol
}

int main(void) {
	init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);
	glfwSetWindowSizeLimits(window, 600, 600, 600, 600);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // sárga háttér
	initShaderProgram();

	setlocale(LC_ALL, "");
	cout << "Billentyűgombok" << endl;
	cout << "ESC\t Kilépés" << endl;
	cout << "S\t Pattogás" << endl;
	cout << "up_arrow\t Kék csík fel" << endl;
	cout << "down_arrow\t Kék csík le" << endl << endl;

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
