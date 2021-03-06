
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>,
#include <queue>

#include "filesystem.h"
#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static queue<int> filaMovimentos;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("../shaders/model_loading.vs", "../shaders/model_loading.frag");

	// load models
	// -----------
	//Model ourModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));
	Model ourModel("../resources/objects/nanosuit/nanosuit.obj");


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	bool inicializou = false;
	glm::mat4 model;
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		
		if (!inicializou) {
			inicializou = true;
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		}

		while (!filaMovimentos.empty()) {
			switch (filaMovimentos.front()) {
				case 4: model = glm::translate(model, glm::vec3(-0.1f, 0.0f, 0.0f)); break;
				case 6: model = glm::translate(model, glm::vec3( 0.1f, 0.0f, 0.0f)); break;
				case 2: model = glm::translate(model, glm::vec3( 0.0f, -0.1f, 0.0f)); break;
				case 8: model = glm::translate(model, glm::vec3( 0.0f, 0.1f, 0.0f)); break;
			}
			filaMovimentos.pop();
		}

		ourShader.setMat4("model", model);

		ourModel.Draw(ourShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//captura tecla numerica pressionada - utiliza para passar para o frag-shader para marcar malha selecionada
	if ((glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS))
		teclaOjetoSelecionado = 0;
	if ((glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS))
		teclaOjetoSelecionado = 1;
	if ((glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS))
		teclaOjetoSelecionado = 2;
	if ((glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS))
		teclaOjetoSelecionado = 3;
	if ((glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS))
		teclaOjetoSelecionado = 4;
	if ((glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS))
		teclaOjetoSelecionado = 5;
	if ((glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS))
		teclaOjetoSelecionado = 6;
	if ((glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS))
		teclaOjetoSelecionado = 7;
	if ((glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS))
		teclaOjetoSelecionado = 8;
	if ((glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS))
		teclaOjetoSelecionado = 9;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		filaMovimentos.push(6);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		filaMovimentos.push(4);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		filaMovimentos.push(8);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		filaMovimentos.push(2);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
