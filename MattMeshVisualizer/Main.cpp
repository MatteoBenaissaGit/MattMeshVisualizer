#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"imgui_impl_opengl3_loader.h"

#include"Model.h"


const unsigned int width = 800;
const unsigned int height = 800;


int main()
{
	glfwInit();

	// version of OpenGL = 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "MattMeshViewer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");

	// light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.7f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//cam
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	//model
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

	float cameraInputSpeed = 0.2f;
	float cameraFOV = 45.0f;

	float lightIntensity = 0.5f;
	float imguiLightColor[3] = { 1.0f, 1.0f, 1.0f};
	int lightType = 0; //0 = direct light, 1 = point, 2 = spot

	// Exporting variables to shaders
	glUseProgram(shaderProgram.ID);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), size);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "imguiColor"), imguiColor[0], imguiColor[1], imguiColor[2], 1.0f);

	glUniform1f(glGetUniformLocation(shaderProgram.ID, "lightIntensity"), lightIntensity);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "imguiLightColor"), imguiLightColor[0], imguiLightColor[1], imguiLightColor[2], 1.0f);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightType"), lightType);


	while (!glfwWindowShouldClose(window))
	{
		// cleat
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (io.WantCaptureMouse == false) 
		{
			camera.Inputs(window, cameraInputSpeed);
		}
		camera.updateMatrix(cameraFOV, 0.1f, 100.0f);

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

		// ImGUI Camera Parameters
		ImGui::Begin("Camera Parameters");
		ImGui::SliderFloat("Camera Speed", &cameraInputSpeed, 0.0f, 2.0f);
		ImGui::SliderFloat("Camera FOV", &cameraFOV, 1.0f, 100.0f);
		ImGui::End();

		// ImGUI Light Parameters
		ImGui::Begin("Light Parameters");
		ImGui::SliderFloat("Light Intensity", &lightIntensity, 0.0f, 1.0f);
		ImGui::ColorEdit3("Light Color", imguiLightColor);
		if (ImGui::Button("Change light type")) {
			ImGui::OpenPopup("LightTypeMenu");
		}

		if (ImGui::BeginPopup("LightTypeMenu")) {
			if (ImGui::MenuItem("Direct light")) {
				lightType = 0;
			}
			if (ImGui::MenuItem("Point light")) {
				lightType = 1;
			}
			if (ImGui::MenuItem("Spot light")) {
				lightType = 2;
			}
			ImGui::EndPopup();
		}
		ImGui::End();


		//top bar
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Options")) {
				if (ImGui::MenuItem("Take screenshot")) {
				}
				if (ImGui::MenuItem("Load model")) {
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// Exporting variables to shaders
		glUseProgram(shaderProgram.ID);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), size);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "imguiColor"), imguiColor[0], imguiColor[1], imguiColor[2], 1.0f);

		glUniform1f(glGetUniformLocation(shaderProgram.ID, "lightIntensity"), lightIntensity);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "imguiLightColor"), imguiLightColor[0], imguiLightColor[1], imguiLightColor[2], 1.0f);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightType"), lightType);

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

	// end
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}