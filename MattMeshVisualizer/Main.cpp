#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"imgui_impl_opengl3_loader.h"

#include"Model.h"


const unsigned int width = 800;
const unsigned int height = 800;


int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "MattMeshViewer"
	GLFWwindow* window = glfwCreateWindow(width, height, "MattMeshViewer", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);





	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);





	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Original code from the tutorial
	const char* modelPath = "models/map/scene.gltf";
	Model model(modelPath);


	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Variables to be changed in the ImGUI window
	bool showModel = true;
	float size = 1.0f;
	float imguiColor[3] = { 1.0f, 1.0f, 1.0f};
	float cameraInputSpeed = 1.0f;

	// Exporting variables to shaders
	glUseProgram(shaderProgram.ID);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), size);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), imguiColor[0], imguiColor[1], imguiColor[2], 1.0f);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (io.WantCaptureMouse == false) 
		{
			// Handles camera inputs
			camera.Inputs(window, cameraInputSpeed);
		}
		
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Draw a model
		if (showModel) 
		{
			model.Draw(shaderProgram, camera);
		}


		// ImGUI Model parameters
		ImGui::Begin("Model parameters");
		ImGui::Text(modelPath);
		ImGui::Checkbox("Show Model", &showModel);
		ImGui::SliderFloat("Model Size", &size, 0.5f, 5.0f);
		ImGui::ColorEdit3("Model Color", imguiColor);
		ImGui::End();

		// ImGUI Viewer Parameters
		ImGui::Begin("Viewer Parameters");
		ImGui::SliderFloat("Camera Speed", &cameraInputSpeed, 0.0f, 2.0f);
		ImGui::End();

		// Exporting variables to shaders
		glUseProgram(shaderProgram.ID);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), size);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "imguiColor"), imguiColor[0], imguiColor[1], imguiColor[2], imguiColor[3]);

		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}


	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// Delete all the objects we've created
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}