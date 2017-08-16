// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include <btBulletDynamicsCommon.h>

// Include GLEW
#include <GL/glew.h>
#include <gl\GLU.h>

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
#include <glm/gtx/random.hpp>

using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>

#include <Mesh.hpp>
#include <ModelManager.hpp>
#include "Camera.h"

#define CAMERA 1
#define BOLA 2
#define ALVO 3
#define POWERUP 4

void togglePolygonMode() {
	GLint polygonMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygonMode);

	polygonMode[0] == GL_LINE ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

ModelManager::mesh_enum model_select_add;
std::string modelsLoaded;
bool toggleModelAdd;
bool toggleCameraAdd;
float cam_x = 0.0f;
float cam_y = 0.0f;
float cam_z = 5.0f;
bool click = 0;
double mouse_x = 0;
double mouse_y = 0;
float power = 15;
float gravity = 9.81;
std::vector<btRigidBody*> rigidbodies;
std::vector<btRigidBody*> rigTargets;



void TW_CALL TransCB(void *data) {

	ModelManager *m = (ModelManager*)data;
	//m->addModel(model_select_add);

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

void MB_CB(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(g_pWindow, &mouse_x, &mouse_y);
		std::cout << "x: " << mouse_x << " y: " << mouse_y << std::endl;
		click = 1;
	}
}

vec3 calcRay(Camera cam, double mx, double my) {
	vec3 ray_wor;
	float x = (2.0f * mx) / g_nWidth - 1.0f;
	float y = 1.0f - (2.0f * my) / g_nHeight;
	float z = 1.0f;
	vec3 ray_nds = vec3(x, y, z);

	ray_nds.z = -1.0;
	vec4 ray_clip = vec4(ray_nds, 1.0);

	vec4 ray_eye = glm::inverse(cam.getProjectionMatrix()) * ray_clip;

	ray_eye.z = -1.0;
	ray_eye.w = 0.0;

	ray_wor = vec3(glm::inverse(cam.getViewMatrix()) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	return ray_wor;
}

void removeBola(btDiscreteDynamicsWorld* dynamicsWorld, ModelManager MM, int id) {

	for (int i = 0; i < rigidbodies.size(); i++) {
		if (rigidbodies[i]->getUserIndex() == id) {
			dynamicsWorld->removeRigidBody(rigidbodies[i]);
		}
	}

	int idx;
	for (int i = 0; i < MM.getTargets()->size(); i++) {
		if (MM.getTargets()->at(i).getId() == id) {
			idx = 0;
			break;
		}
	}
	MM.getTargets()->erase(MM.getTargets()->begin() + idx);

	/*for (int j = 0; j < ms->size(); j++) {
		if (ms->at(j).getId() == id) {
			
			Model m = ms->at(j);
					
		}
	}*/
}

void testCollisions(btDiscreteDynamicsWorld* dynamicsWorld, ModelManager MM) {
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				//const btVector3& ptA = pt.getPositionWorldOnA();
				//const btVector3& ptB = pt.getPositionWorldOnB();
				//const btVector3& normalOnB = pt.m_normalWorldOnB;
				int a = (int)obA->getUserPointer();
				int b = (int)obB->getUserPointer();

				if (a == ALVO) {
					if (b == BOLA) {
						std::cout << "remove bola!!" << std::endl;
						//MM.removeModel(obB->getUserIndex());
						//removeBola(dynamicsWorld, MM, obA->getUserIndex());

					}
				}

			}
		}
	}
}


int initWindow() {

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

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
	//glfwSetMouseButtonCallback(g_pWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetMouseButtonCallback(g_pWindow, MB_CB);
	glfwSetCursorPosCallback(g_pWindow, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(g_pWindow, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(g_pWindow, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(g_pWindow, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar

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
	toggleModelAdd = false;

	// CONFIGURA JANELA E TOOLBAR
	initWindow();

#pragma region GUI
	// Initialize the GUI

	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(g_nWidth, g_nHeight);

	//create the toolbar
	g_pToolBar = TwNewBar("Main");
	TwDefine(" Main label='Main TweakBar' position='2 2' valueswidth=fit size='2 2' alpha=0");

#pragma endregion

#pragma region FBO AND SHADER
	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders/StandardShadingRTT.vertexshader", "shaders/StandardShadingRTT.fragmentshader");
	
	// ---------------------------------------------
	// Render to Texture - specific code begins here
	// ---------------------------------------------

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The texture we're going to render to
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_nWidth, g_nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_nWidth, g_nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	//// Depth texture alternative : 
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;


	// The fullscreen quad's FBO
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint quad_programID = LoadShaders("shaders/NightVision.v.hlsl", "shaders/NightVision.f.hlsl");
	GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");

	GLuint sceneBufferID = glGetUniformLocation(quad_programID, "sceneBuffer");
	GLuint noiseTexID = glGetUniformLocation(quad_programID, "noiseTex");
	GLuint maskTexID = glGetUniformLocation(quad_programID, "maskTex");
	GLuint elapsID = glGetUniformLocation(quad_programID, "elapsedTime");
	GLuint lumID = glGetUniformLocation(quad_programID, "luminanceThreshold");
	GLuint cAmpID = glGetUniformLocation(quad_programID, "colorAmplification");

	GLuint noiseTex = loadBMP_custom("shaders/noise_text.bmp");
	GLuint maskTex = loadBMP_custom("shaders/mask_text.bmp");

#pragma endregion

#pragma region WORLD AND PHYSICS

	//// Build the broadphase
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// The world.
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -gravity, 0));


#pragma region paredes
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(0, 0, 0, 1),
			btVector3(0, -3, 0)
		));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(
			0,
			groundMotionState,
			groundShape,
			btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(0.9);
	dynamicsWorld->addRigidBody(groundRigidBody);

	
	btCollisionShape *parede1Shape = new btBoxShape(btVector3(0.1, 9999, 9999));
	btDefaultMotionState *parede1MS = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(-5, 0, 0)
	));
	btRigidBody::btRigidBodyConstructionInfo parede1RBCI(
		0,
		parede1MS,
		parede1Shape,
		btVector3(0, 0, 0)
	);
	btRigidBody *parede1RB = new btRigidBody(parede1RBCI);
	parede1RB->setRestitution(0.9);
	dynamicsWorld->addRigidBody(parede1RB);

	btCollisionShape *parede2Shape = new btBoxShape(btVector3(0.1, 9999, 9999));
	btDefaultMotionState *parede2MS = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(5, 0, 0)
	));
	btRigidBody::btRigidBodyConstructionInfo parede2RBCI(
		0,
		parede2MS,
		parede2Shape,
		btVector3(0, 0, 0)
	);
	btRigidBody *parede2RB = new btRigidBody(parede2RBCI);
	parede2RB->setRestitution(0.9);
	dynamicsWorld->addRigidBody(parede2RB);

#pragma endregion

	btCollisionShape *camShape = new btBoxShape(btVector3(20, 20, 0.1));
	btDefaultMotionState *camMS = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(0, 0, 6)
	));
	btRigidBody::btRigidBodyConstructionInfo camRBCI(
		0,
		camMS,
		camShape,
		btVector3(0, 0, 0)
	);
	btRigidBody *camRB = new btRigidBody(camRBCI);
	camRB->setRestitution(0.9);
	camRB->setUserPointer((void*)CAMERA);
	dynamicsWorld->addRigidBody(camRB);

	// MODEL MANAGER
	ModelManager MM = ModelManager();


	MM.addParede(ModelManager::mesh_enum::chao, vec3(0, -3, 0), 909, vec3(-90, 0, 0));
	MM.addParede(ModelManager::mesh_enum::parede, vec3(-5, 0, 0), 3125, vec3(-90, 0, 0));
	MM.addParede(ModelManager::mesh_enum::parede, vec3(5, 0, 0), 346, vec3(-90, 0, 0));

	srand(time(NULL));
	for (int i = 0; i < 25; i++) {

		vec3 pos(-3 + rand() % (3 + 3), -3 + rand() % (3 + 3), -10-rand()%150);

		MM.addTarget(ModelManager::mesh_enum::suzanne, pos, i);
		MM.getTargets()->at(i).scale(glm::scale(.5f, .5f, .5f));

		btCollisionShape *tShape = new btSphereShape(0.5);
		btDefaultMotionState *tMS = new btDefaultMotionState(btTransform(
			btQuaternion(0, 0, 0, 1),
			btVector3(pos.x, pos.y, pos.z)
		));
		btRigidBody::btRigidBodyConstructionInfo tRBCI(
			0,
			tMS,
			tShape,
			btVector3(0, 0, 0)
		);
		btRigidBody *tRB = new btRigidBody(tRBCI);
		tRB->setRestitution(0.9);
		tRB->setUserPointer((void*)ALVO);
		tRB->setUserIndex(i);
		
		rigTargets.push_back(tRB);
		dynamicsWorld->addRigidBody(tRB);

	}

	Camera cam = Camera(g_pWindow);

#pragma endregion

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	double oldTimeSinceStart = glfwGetTime();
	glm::mat4 trans_mat;
	bool nvShader = 1;
	int inc = 0;

	do {
		check_gl_error();

		double timeSinceStart = glfwGetTime();
		double deltaTime = (timeSinceStart - oldTimeSinceStart);
		oldTimeSinceStart = timeSinceStart;

		double currentTime = glfwGetTime();

		if (click) {
			click = 0;

			// esfera
			btCollisionShape* boxCollisionShape = new btSphereShape(0.2);

			vec3 p = cam.getPosition();
			btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
				btQuaternion(0, 0, 0, 1),
				btVector3(p.x, p.y , p.z - 5)
			));
			btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
				0.4,                  // mass, in kg. 0 -> Static object, will never move.
				motionstate,
				boxCollisionShape,  // collision shape of body
				btVector3(0, 0, 0)    // local inertia
			);
			btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

			
			vec3 ray = calcRay(cam, mouse_x, mouse_y);
			ray *= power;
			ray.y *= 2;

			rigidBody->setLinearVelocity(btVector3(ray.x, ray.y, ray.z));
			rigidBody->setRestitution(0.9);
			rigidBody->setUserPointer((void*) BOLA);
			rigidBody->setUserIndex(inc);
		

			dynamicsWorld->addRigidBody(rigidBody);
			rigidbodies.push_back(rigidBody);

			btTransform transf;
			rigidBody->getMotionState()->getWorldTransform(transf);

			vec3 v = glm::vec3(transf.getOrigin().getX(), (float)transf.getOrigin().getY(), (float)transf.getOrigin().getZ());

			MM.addModel(ModelManager::mesh_enum::esfera, v, inc);
			MM.getModels()->at(inc).scale(glm::scale(.2f,.2f,.2f));

			
			inc++;
		}

	
#pragma region KEYBOARD
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
				gravity += 3;
				dynamicsWorld->setGravity(btVector3(0, -gravity, 0));
			}
		}
		if (glfwGetKey(g_pWindow, GLFW_KEY_N) == GLFW_PRESS) {
			if (glfwGetKey(g_pWindow, GLFW_KEY_N) == GLFW_RELEASE) {
				gravity -= 3;
				dynamicsWorld->setGravity(btVector3(0, -gravity, 0));
			}
		}
		if (glfwGetKey(g_pWindow, GLFW_KEY_K) == GLFW_PRESS) {
			if (glfwGetKey(g_pWindow, GLFW_KEY_K) == GLFW_RELEASE) {
				power += 1;
			}
		}
		if (glfwGetKey(g_pWindow, GLFW_KEY_J) == GLFW_PRESS) {
			if (glfwGetKey(g_pWindow, GLFW_KEY_J) == GLFW_RELEASE) {
				power -= 1;
			}
		}
		if (glfwGetKey(g_pWindow, GLFW_KEY_G) == GLFW_PRESS) {
			if (glfwGetKey(g_pWindow, GLFW_KEY_G) == GLFW_RELEASE) {
				nvShader = !nvShader;
			}
		}

#pragma endregion

		
		// Measure speed
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame - frames: %d\n", 1000.0 / double(nbFrames), nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
		}

		if (nvShader) {
			// -------------------------
			// Render to our framebuffer
			// -------------------------
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			glViewport(0, 0, g_nWidth, g_nHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		}
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
		glm::vec3 lightPos = glm::vec3(0, 0, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


		dynamicsWorld->stepSimulation(deltaTime, 30);

		
		//testCollisions(dynamicsWorld, MM);


		for (int i = 0; i < rigidbodies.size(); i++) {
			btTransform trans;
			rigidbodies[i]->getMotionState()->getWorldTransform(trans);

			mat4 mat = glm::translate(trans.getOrigin().getX(), (float)trans.getOrigin().getY(), (float)trans.getOrigin().getZ());
			MM.getModels()->at(i).translate(mat);
		}

		// Compute the MVP matrix from keyboard and mouse input
		cam.computeMatricesFromInputs(nUseMouse);
		vec3 pos(0, 0, -1);
		pos *= deltaTime * 3;
		cam.setPosition(pos);
		
		// UPDATE RIGIDBODY DA CAMERA
		btTransform t;
		t = camRB->getCenterOfMassTransform();
		t.setOrigin(btVector3(cam.getPosition().x, cam.getPosition().y, cam.getPosition().z + 1	));
		camRB->setCenterOfMassTransform(t);



		// DRAW MODELS
		MM.draw(programID, cam);

		// Draw tweak bars
		TwDraw();

		if (nvShader) {
			// --------------------
			// Render to the screen
			// --------------------
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, g_nWidth, g_nHeight);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use our shader
			glUseProgram(quad_programID);

			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderedTexture);
			// Set our "renderedTexture" sampler to user Texture Unit 0
			glUniform1i(texID, 0);

			// Textures
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, noiseTex);
			glUniform1i(noiseTexID, 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, maskTex);
			glUniform1i(maskTexID, 2);

			// Uniforms
			glUniform1f(elapsID, (float)(glfwGetTime() * 1000));
			glUniform1f(lumID, (float)0.2);
			glUniform1f(cAmpID, (float)4.0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

			glDisableVertexAttribArray(0);
		}

		// Swap buffers
		glfwSwapBuffers(g_pWindow);
		glfwPollEvents();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(g_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(g_pWindow) == 0);

	// Cleanup VBO and shader

	MM.deleteMeshs();

	glDeleteProgram(programID);
	//glDeleteVertexArrays(1, &VertexArrayID);

	// Terminate AntTweakBar and GLFW
	TwTerminate();
	glfwTerminate();

	return 0;
}

