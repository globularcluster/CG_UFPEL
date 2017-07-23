// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* g_pWindow;
unsigned int g_nWidth = 1024, g_nHeight = 715;

// Include AntTweakBar
#include <AntTweakBar.h>
TwBar *g_pToolBar;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/spline.hpp>
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>

#include <../build/Mesh.hpp>
#include <../build/ModelManager.hpp>
#include <../build/Camera.h>

void togglePolygonMode() {
	GLint polygonMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygonMode);

	polygonMode[0] == GL_LINE ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void WindowSizeCallBack(GLFWwindow *pWindow, int nWidth, int nHeight) {

	g_nWidth = nWidth;
	g_nHeight = nHeight;
	glViewport(0, 0, g_nWidth, g_nHeight);
	TwWindowSize(g_nWidth, g_nHeight);
}


ModelManager::mesh_enum model_select_add;
std::string modelsLoaded;
std::string camerasLoaded;
bool toggleModelAdd;
bool toggleCameraAdd;
float cam_x = 0.0f;
float cam_y = 0.0f;
float cam_z = 5.0f;
std::vector<Camera> cameras;


void TW_CALL TransCB(void *data) {

	ModelManager *m = (ModelManager*)data;
	m->addModel(model_select_add);

	// UPDATE ENUM "MODELS LOADED"
	if (modelsLoaded.empty())
		modelsLoaded += "1-" + m->mesh_list[model_select_add];
	else {
		int i = m->getModels()->size();
		modelsLoaded += "," + std::to_string(i) + "-" + m->mesh_list[model_select_add];
	}

	// UPDATE BAR
	toggleModelAdd = true;
}

void TW_CALL AddCameraCB(void *data) {

	Camera c = Camera(g_pWindow, vec3(cam_x, cam_y, cam_z));

	// UPDATE ENUM "MODELS LOADED"
	if (cameras.empty())
		camerasLoaded += "camera-1";
	else {
		int i = cameras.size();
		camerasLoaded += ",camera-" + std::to_string(i+1);
	}

	cameras.push_back(c);

	// UPDATE BAR
	toggleCameraAdd = true;
}

glm::vec3 catmull_rom_spline(const std::vector<glm::vec3>& cp, float t)
{
	// indices of the relevant control points
	int i0 = glm::clamp<int>(t - 1, 0, cp.size() - 1);
	int i1 = glm::clamp<int>(t, 0, cp.size() - 1);
	int i2 = glm::clamp<int>(t + 1, 0, cp.size() - 1);
	int i3 = glm::clamp<int>(t + 2, 0, cp.size() - 1);

	// parameter on the local curve interval
	float local_t = glm::fract(t);

	return glm::catmullRom(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
}

void animation1(double &delta, Model &model) {

	static int n = 1;
	static float x = 0.001f;
	static double dt = 1000 / 60;
	float accumulator = delta;

	if (n == 1000) {
		x = -x;
		n = 1;
	}
	n++;

	double timeNow = glfwGetTime();

	if (timeNow - delta >= 0.0001f) {
		delta += 0.0001f;
		glm::mat4 trans_mat;
		trans_mat = glm::translate((double)x, (double)x, (double)0);
		model.translate(trans_mat);
	}


	//double trans_x = x * dt;

	//glm::mat4 trans_mat;
	//while (accumulator >= dt) {
	//	trans_mat = glm::translate(trans_x, trans_x, (double)0);
	//	accumulator -= dt;
	//}

	//model.translate(trans_mat);

}

void animation3(double &delta, Model &model) {

	static int n = 1;
	static float x = 0.001f;
	static double dt = 1000 / 60;
	float accumulator = delta;

	vec3 v0(0, 0, 0);
	vec3 v1(1, 1, 0);
	vec3 v2(2, 1, 0);
	vec3 v3(3, 0, 0);

	static float t = 0.01f;

	if (n == 1000) {
		x = -x;
		n = 1;
	}
	n++;

	double timeNow = glfwGetTime();

	if (timeNow - delta >= 0.01f) {
		delta += 0.01f;

		//vec3 r0 = pow((1 - t), 3) * v0;
		//vec3 r1 = ((3 * t) * (pow(1 - t, 2))) * v1;
		//vec3 r2 = ((3 * pow(t, 2)) * (1 - t)) * v2;
		//vec3 r3 = pow(t, 3) * v3;

		//vec3 res = r0 + r1 + r2 + r3;

		std::vector<vec3> poin;
		poin.push_back(v0);
		poin.push_back(v1);
		poin.push_back(v2);
		poin.push_back(v3);

		vec3 res = catmull_rom_spline(poin, t);

		t += 0.01f;
		if (t >= 1)
			t = 0.01f;

		glm::mat4 trans_mat;
		trans_mat = glm::translate((double)res.x, (double)res.y, (double)res.z);

		model.translate(trans_mat);
	}
}

void animation2(double delta, Model &model) {

	static int n = 1;
	static float x = -180.0f;

	float rota = x * 700 / 1000 * delta;

	glm::mat4 rot = model.getModelMatrix();

	rot = translate(rot, vec3(3.0f, 0.f, 0.f));
	rot = rotate(rot, rota, vec3(0.0f, 0.0f, 1.0f));
	rot = translate(rot, vec3(-3.0, 0.f, 0.f));

	model.rotation(rot);

	static float x2 = 200.0f;
	if (n == 1000) {
		x2 = -x2;
		n = 1;
	}
	n++;

	double trans_x = x2 / 100 * delta;
	rot = glm::translate(trans_x, (double)0, (double)0);
	model.translate(rot);
}


int initWindow() {


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	g_pWindow = glfwCreateWindow(g_nWidth, g_nHeight, "CG UFPel", NULL, NULL);
	if (g_pWindow == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(g_pWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	check_gl_error();//OpenGL error from GLEW



	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetMouseButtonCallback(g_pWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCursorPosCallback(g_pWindow, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(g_pWindow, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(g_pWindow, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(g_pWindow, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetWindowSizeCallback(g_pWindow, WindowSizeCallBack);


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(g_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(g_pWindow, g_nWidth / 2, g_nHeight / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
}

int main(void)
{
	int nUseMouse = 0;
	modelsLoaded = "";
	camerasLoaded.clear();
	toggleModelAdd = false;

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// CONFIGURA JANELA E TOOLBAR
	initWindow();


	// MODEL MANAGER
	ModelManager MM = ModelManager();
	Camera cam = Camera(g_pWindow);
	cameras.push_back(cam);
	camerasLoaded += "camera-1";


#pragma region GUI
	// Initialize the GUI

	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(g_nWidth, g_nHeight);

	//create the toolbar
	g_pToolBar = TwNewBar("Main");
	TwDefine(" Main label='Main TweakBar' position='2 2' valueswidth=fit size='220 450' alpha=0");


	// TRANSLATION
	double trans_x = 0.0;
	TwAddVarRW(g_pToolBar, "trans_x", TW_TYPE_DOUBLE, &trans_x, "label='x' group='Translate' ");
	double trans_y = 0.0;
	TwAddVarRW(g_pToolBar, "trans_y", TW_TYPE_DOUBLE, &trans_y, "label='y' group='Translate' ");
	double trans_z = 0.0;
	TwAddVarRW(g_pToolBar, "trans_z", TW_TYPE_DOUBLE, &trans_z, "label='z' group='Translate' ");
	TwDefine(" Main/Translate group=Transform opened=false ");

	// SCALE
	float scale = 1.0f;
	TwAddVarRW(g_pToolBar, "scale", TW_TYPE_FLOAT, &scale, "label='Factor' min=0.1 step=0.1 group='Scale' ");
	TwDefine(" Main/Scale group=Transform opened=false ");

	// ROTATION
	float rotate_x = 0.0f;
	TwAddVarRW(g_pToolBar, "rotate_x", TW_TYPE_FLOAT, &rotate_x, "label='Rotate X' group='Rotation' ");
	float rotate_y = 0.0f;
	TwAddVarRW(g_pToolBar, "rotate_y", TW_TYPE_FLOAT, &rotate_y, "label='Rotate Y' group='Rotation' ");
	float rotate_z = 0.0f;
	TwAddVarRW(g_pToolBar, "rotate_z", TW_TYPE_FLOAT, &rotate_z, "label='Rotate Z' group='Rotation' ");
	TwDefine(" Main/Rotation group=Transform opened=false ");

	// ROTATION IN POINT
	float point_r_x = 0.0f;
	TwAddVarRW(g_pToolBar, "point_r_x", TW_TYPE_FLOAT, &point_r_x, "label='Point X' group='Around a point' ");
	float point_r_y = 0.0f;
	TwAddVarRW(g_pToolBar, "point_r_y", TW_TYPE_FLOAT, &point_r_y, "label='Point Y' group='Around a point' ");
	float point_r_z = 0.0f;
	TwAddVarRW(g_pToolBar, "point_r_z", TW_TYPE_FLOAT, &point_r_z, "label='Point Z' group='Around a point' ");
	TwDefine(" Main/'Around a point' group=Rotation opened=false ");

	float shear_x = 0.0f;
	TwAddVarRW(g_pToolBar, "shear_x", TW_TYPE_FLOAT, &shear_x, "label='Shear X' group='Shear' ");
	float shear_y = 0.0f;
	TwAddVarRW(g_pToolBar, "shear_y", TW_TYPE_FLOAT, &shear_y, "label='Shear Y' group='Shear' ");
	float shear_z = 0.0f;
	TwAddVarRW(g_pToolBar, "shear_z", TW_TYPE_FLOAT, &shear_z, "label='Shear Z' group='Shear' ");
	TwDefine(" Main/Shear group=Transform opened=false ");

	int anim = 0;
	TwType animationType;
	animationType = TwDefineEnumFromString("AnimationType", "Animation 1, Animation 2");
	TwAddVarRW(g_pToolBar, "AnimationComb", animationType, &anim, " label='Select' group='Animation' ");
	bool animation = false;
	TwAddVarRW(g_pToolBar, "animation", TW_TYPE_BOOLCPP, &animation, " label='Animation' group='Animation' ");
	TwDefine(" Main/Animation group=Transform opened=false ");

	//bool spline = false;
	//TwAddVarRW(g_pToolBar, "spline", TW_TYPE_BOOLCPP, &spline, "label='Spline', group='Transform' ");

	// TRANFORM!
	bool trans = false;
	TwAddVarRW(g_pToolBar, "Transform!", TW_TYPE_BOOLCPP, &trans, " group='Transform' ");

	int model_select_trans = NULL;
	TwType modelSelecType;
	modelSelecType = TwDefineEnumFromString("ModelSelecType", modelsLoaded.c_str());
	TwAddVarRW(g_pToolBar, "ModelSelecType", modelSelecType, &model_select_trans, "label='Select model' group='Transform'");

	int cam_select = NULL;
	TwType camSelectType;
	camSelectType = TwDefineEnumFromString("CamSelectType", camerasLoaded.c_str());
	TwAddVarRW(g_pToolBar, "CamSelectType", camSelectType, &cam_select, "label='Select Camera' group='Camera' ");

	float fov = 45.0f;
	TwAddVarRW(g_pToolBar, "fov", TW_TYPE_FLOAT, &fov, "label='FoV' max=179 min=10 group='Adjust' ");
	float zNear = 0.1f;
	TwAddVarRW(g_pToolBar, "zNear", TW_TYPE_FLOAT, &zNear, "label='zNear' step=0.1 max=100 min=0.1 group='Adjust' ");
	float zFar = 100.0f;
	TwAddVarRW(g_pToolBar, "zFar", TW_TYPE_FLOAT, &zFar, "label='zFar' step=0.1 max=200 min=0.5 group='Adjust' ");
	TwDefine(" Main/'Adjust' group='Camera' opened=false ");

	TwAddVarRW(g_pToolBar, "cam_x", TW_TYPE_FLOAT, &cam_x, "label='Cam X' group='Add Camera' ");
	TwAddVarRW(g_pToolBar, "cam_y", TW_TYPE_FLOAT, &cam_y, "label='Cam Y' group='Add Camera' ");
	TwAddVarRW(g_pToolBar, "cam_z", TW_TYPE_FLOAT, &cam_z, "label='Cam Z' group='Add Camera' ");
	TwDefine(" Main/'Add Camera' group='Camera' opened=false ");
	TwAddButton(g_pToolBar, "Add Cam", AddCameraCB, &camerasLoaded, " group='Add Camera' ");
	
	bool rot_around_c = false;
	TwAddVarRW(g_pToolBar, "rot_around_c", TW_TYPE_BOOLCPP, &rot_around_c, " label='Rotate?' group='Rotate Camera' ");
	float rot_camera_x = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_camera_x", TW_TYPE_FLOAT, &rot_camera_x, "label='Rotate X' step=0.01 group='Rotate Camera' ");
	float rot_camera_y = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_camera_y", TW_TYPE_FLOAT, &rot_camera_y, "label='Rotate Y' step=0.01 group='Rotate Camera' ");
	float rot_camera_z = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_camera_z", TW_TYPE_FLOAT, &rot_camera_z, "label='Rotate Z' step=0.01 group='Rotate Camera' ");
	TwDefine(" Main/'Rotate Camera' group='Camera' opened=false ");
	
	float rot_cam_p_x = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_cam_p_x", TW_TYPE_FLOAT, &rot_cam_p_x, "label='Point X' group='Rotate Point' ");
	float rot_cam_p_y = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_cam_p_y", TW_TYPE_FLOAT, &rot_cam_p_y, "label='Point Y' group='Rotate Point' ");
	float rot_cam_p_z = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_cam_p_z", TW_TYPE_FLOAT, &rot_cam_p_z, "label='Point Z' group='Rotate Point' ");
	TwDefine(" Main/'Rotate Point' group='Rotate Camera' opened=false ");

	float rot_cam_axis_x = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_cam_axis_x", TW_TYPE_FLOAT, &rot_cam_axis_x, "label='Axis X' step=0.01 group='Rotate Axis' ");
	float rot_cam_axis_y = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_cam_axis_y", TW_TYPE_FLOAT, &rot_cam_axis_y, "label='Axis Y' step=0.01 group='Rotate Axis' ");
	float rot_cam_axis_z = 0.0f;
	TwAddVarRW(g_pToolBar, "rot_cam_axis_z", TW_TYPE_FLOAT, &rot_cam_axis_z, "label='Axis Z' step=0.01 group='Rotate Axis' ");
	TwDefine(" Main/'Rotate Axis' group='Rotate Camera' opened=false ");

	float look_x = 0.0f;
	TwAddVarRW(g_pToolBar, "look_x", TW_TYPE_FLOAT, &look_x, "label='Point X' step=0.1 min=-1 max=1  group='Look' ");
	float look_y = 0.0f;
	TwAddVarRW(g_pToolBar, "look_y", TW_TYPE_FLOAT, &look_y, "label='Point Y' step=0.1 min=0.1 max=1 group='Look' ");
	TwDefine(" Main/Look group='Camera' opened=false ");

	// create model
	TwEnumVal modelsEV[] = {
		{ ModelManager::cube, "cube" },
		{ ModelManager::goose, "goose" },
		{ ModelManager::suzanne, "suzanne" }
	};
	TwType modelType = TwDefineEnum("Models", modelsEV, 3);
	TwAddVarRW(g_pToolBar, "Select", modelType, &model_select_add, " group='New Model' ");
	TwAddButton(g_pToolBar, "Add", TransCB, &MM, " group='New Model' ");
	TwDefine(" Main/'New Model' opened=false ");

	TwDefine(" Main/'Transform' opened=false ");

#pragma endregion

#pragma region SHADER & BUFFER

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("../shaders/StandardShading.vertexshader", "../shaders/StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadDDS("../mesh/uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

#pragma endregion

	// For speed computation
	double lastTime = glfwGetTime();
	double lastTime2 = glfwGetTime();

	int nbFrames = 0;

	double oldTimeSinceStart = glfwGetTime();
	glm::mat4 trans_mat;


	do {
		check_gl_error();

		double timeSinceStart = glfwGetTime();
		double delta = 1 / (timeSinceStart - oldTimeSinceStart);
		oldTimeSinceStart = timeSinceStart;

		double currentTime = glfwGetTime();

		//use the control key to free the mouse
		if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
			nUseMouse = 0;
		else
			nUseMouse = 1;

		// TOGGLE WIREFRAME
		if (glfwGetKey(g_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (glfwGetKey(g_pWindow, GLFW_KEY_SPACE) == GLFW_RELEASE) {
				togglePolygonMode();
			}
		}

		if (glfwGetKey(g_pWindow, GLFW_KEY_M) == GLFW_PRESS) {
			if (glfwGetKey(g_pWindow, GLFW_KEY_M) == GLFW_RELEASE) {

			}
		}

		// utilizado para atualizar Bar com modelo adicionado
		if (toggleModelAdd) {
			toggleModelAdd = false;

			TwRemoveVar(g_pToolBar, "ModelSelecType");
			modelSelecType = TwDefineEnumFromString("ModelSelecType", modelsLoaded.c_str());
			TwAddVarRW(g_pToolBar, "ModelSelecType", modelSelecType, &model_select_trans, "label='Select model' group='Transform'");
		}

		if (toggleCameraAdd) {
			toggleCameraAdd = false;

			TwRemoveVar(g_pToolBar, "CamSelectType");
			camSelectType = TwDefineEnumFromString("CamSelectType", camerasLoaded.c_str());
			TwAddVarRW(g_pToolBar, "CamSelectType", camSelectType, &cam_select, "label='Select Camera' group='Camera' ");
		}

		if (animation == true) {

			if (MM.getModels()->size()) {
				if (anim == 0) {
					animation3(lastTime2, MM.getModels()->at(model_select_trans));
				}
				else if (anim == 1) {
					animation2(delta, MM.getModels()->at(model_select_trans));
				}

			}
		}
		else
			animation = false;

		if (trans) {
			trans = false;
			auto m = MM.getModels();

			// se houver modelos na cena
			if (m->size()) {

				if (trans_x || trans_y || trans_z) {	// translação
					trans_mat = glm::translate(trans_x, trans_y, trans_z);
					m->at(model_select_trans).translate(trans_mat);
				}
				if (scale > 0) {						// escala
					trans_mat = glm::scale(scale, scale, scale);
					m->at(model_select_trans).scale(trans_mat);
				}

				if (rotate_x || rotate_y || rotate_z) { // rotação em relação a eixo

					if (point_r_x || point_r_y || point_r_z) { // rotação em relação a um ponto

						glm::mat4 rot = m->at(model_select_trans).getModelMatrix();

						rot = translate(rot, vec3(point_r_x, point_r_y, point_r_z));
						rot = rotate(rot, rotate_x, vec3(1.0f, 0.0f, 0.0f));
						rot = rotate(rot, rotate_y, vec3(0.0f, 1.0f, 0.0f));
						rot = rotate(rot, rotate_z, vec3(0.0f, 0.0f, 1.0f));
						rot = translate(rot, vec3(-point_r_x, -point_r_y, -point_r_z));

						m->at(model_select_trans).rotation(rot);
					}
					else {

						vec3 EulerAng(radians(rotate_x), radians(rotate_y), radians(rotate_z));
						quat myQuat = quat(EulerAng);

						//quat myQuat = glm::angleAxis(degrees(90.0f), vec3(0.0f, 0.0f, 1.0f));

						glm::mat4 rot = glm::toMat4(myQuat);

						m->at(model_select_trans).rotation(rot);
					}
				}

				if (shear_x || shear_y || shear_z) {		// shear

					trans_mat = m->at(model_select_trans).getModelMatrix();
					trans_mat = shearX3D(trans_mat, shear_x, 0.0f);
					trans_mat = shearY3D(trans_mat, shear_y, 0.0f);
					trans_mat = shearZ3D(trans_mat, shear_z, 0.0f);

					m->at(model_select_trans).shear(trans_mat);

				}

			}
		}

		// Measure speed
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame - frames: %d\n", 1000.0 / double(nbFrames), nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		cameras[cam_select].computeMatricesFromInputs(nUseMouse);

		if (rot_around_c)
			cameras[cam_select].rotate(rot_camera_x, rot_camera_y, rot_camera_z, rot_cam_p_x, rot_cam_p_y, rot_cam_p_z);
		if (look_x != 0 || look_y != 0)
			cameras[cam_select].look(look_x, look_y);
		if (rot_cam_axis_x != 0 || rot_cam_axis_y != 0 || rot_cam_axis_z != 0)
			cameras[cam_select].pitch(rot_cam_axis_x, rot_cam_axis_y, rot_cam_axis_z);
		if (fov != 45.0) {
			cameras[cam_select].setFov(fov);
		}
		if (zNear != 0.1f || zFar != 100.0f)
			cameras[cam_select].setClipping(zNear, zFar);


		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);


		// DRAW MODELS
		MM.draw(cameras[cam_select], ModelMatrixID, ViewMatrixID, MatrixID);

		// Draw tweak bars
		TwDraw();

		// Swap buffers
		glfwSwapBuffers(g_pWindow);
		glfwPollEvents();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(g_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(g_pWindow) == 0);

	// Cleanup VBO and shader

	MM.deleteMeshs();

	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Terminate AntTweakBar and GLFW
	TwTerminate();
	glfwTerminate();

	return 0;
}

