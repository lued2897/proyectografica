/*
* 
* 09 - Animación
*/

#include <iostream>
#include <stdlib.h>

// GLAD: Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator
// https://glad.dav1d.de/
#include <glad/glad.h>

// GLFW: https://www.glfw.org/
#include <GLFW/glfw3.h>

// GLM: OpenGL Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model loading classes
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>
#include <particles.h>// ---------------------------------- Pariculas ----------------------------------

#include <irrKlang.h>
using namespace irrklang;

// Functions
bool Start();
bool Update();

// Definición de callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Gobals
GLFWwindow* window;

// Tamaño en pixeles de la ventana
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Definición de cámara (posición en XYZ)
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
Camera camera3rd(glm::vec3(0.0f, 0.0f, 0.0f));

// Controladores para el movimiento del mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool submarino = true;

// Variables para la velocidad de reproducción
// de la animación
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;

glm::vec3 position(0.0f,0.0f, 0.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     trdpersonOffset = 1.5f;
float     scaleV = 0.025f;
float     rotateCharacter = 0.0f;
float	  door_offset = 0.0f;
float	  door_rotation = 0.0f;

//variables para efecto bajo el agua
float		water_level = 5.0f;
float		fog_density = 0.04f;
float		depth_attnuation = 0.06;
glm::vec3	water_color = glm::vec3(0.0f, 0.25f, 0.45f);


// Shaders
Shader *mLightsShader;
Shader *proceduralShader;
Shader *wavesShader;

Shader *cubemapShader;
Shader *dynamicShader;

// ---------------------------------- Pariculas ----------------------------------
Shader* particlesShader;
// Partículas
Particles particlesSystem(500); // creamos 200 partículas

// Carga la información del modelo
Model* particleModel;
// ---------------------------------- Pariculas ----------------------------------

// Carga la información del modelo
Model	*terrain;
Model	*decor;
Model   *boat;
Model   *moon;
Model   *gridMesh;


Model	*chest;
Model	*burbuja1;
Model	*burbuja2;


// 
//basura
Model *bolsa;
Model *tenedor;
Model *cuchara;
Model *tapa;
Model *lata;
Model *popote;
Model *cigarro;
Model *plato;
Model *botella_vidrio;
Model *botella_plastico;
Model *cofre;

// Modelos animados
AnimatedModel   *character01;


//animales
AnimatedModel	*pez;
Model			*tortuga;
AnimatedModel	*medusa;
AnimatedModel	*pulpo;
AnimatedModel	*calamar;
Model			*estrella;
AnimatedModel	*mantaraya;
AnimatedModel	*caballito;
AnimatedModel	*delfin;
AnimatedModel	*cangrejo;
Model			*erizo;


float tradius = 10.0f;
float theta = 0.0f;
float alpha = 0.0f;

// Cubemap
CubeMap *mainCubeMap;

// Light gLight;
std::vector<Light> gLights;

// Materiales
Material material01;

float proceduralTime = 0.0f;
float wavesTime = 0.0f;

// Audio
ISoundEngine *SoundEngine = createIrrKlangDevice();

//Cancion segun la escena
ISound* currentMusic = nullptr; // canción actual

// selección de cámara
int    activeCamera = 0; // activamos la primera cámara


// Entrada a función principal
int main()
{
	if (!Start())
		return -1;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;

}

glm::vec3 trebol(glm::vec3 translate,float time, float radius, float height) {
	float t = time;

	// Nudo de trébol
	float x = -1 * (sin(t) + 2.0 * sin(2.0 * t));
	float y = -1 * (cos(t) - 2.0 * cos(2.0 * t));
	//float z = -sin(3.0 * t);

	translate[0] += radius * x;
	translate[2] += radius * y;
	//PosL.z += height * z;
	translate[1] += height;
	
	return translate;
}


//para cambiar la cancion

std::string currentSceneMusic = "";
void PlaySceneMusic(const char* filename) {
	if (currentSceneMusic == filename) return; // ya está sonando, no reiniciar
	if (currentMusic) {
		currentMusic->stop(); // Detiene la canción anterior
		currentMusic->drop(); // Libera recursos
	}
	currentMusic = SoundEngine->play2D(filename, true, false, true);
	currentSceneMusic = filename;
	if (!currentMusic) {
		std::cout << "Error: no se pudo abrir la cancion de esta escena: " << filename << std::endl;
	}
}

bool Start() {
	// Inicialización de GLFW

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creación de la ventana con GLFW
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Ocultar el cursor mientras se rota la escena
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: Cargar todos los apuntadores
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Activación de buffer de profundidad
	glEnable(GL_DEPTH_TEST);

	// Compilación y enlace de shaders
	mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
	proceduralShader = new Shader("shaders/12_ProceduralAnimation.vs", "shaders/12_ProceduralAnimation.fs");
	wavesShader = new Shader("shaders/13_wavesAnimation.vs", "shaders/13_wavesAnimation.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
	dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");

	// Máximo número de huesos: 100
	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

	// Dibujar en malla de alambre
	// glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	/*house = new Model("models/IllumModels/House03.fbx");*/
	//house = new Model("models/rocks_w_corals.fbx");
	terrain = new Model("models/scene_static_models.fbx");
	decor = new Model("models/decor.fbx");
	//boat = new Model("models/boat.fbx");
	moon = new Model("models/IllumModels/moon.fbx");
	gridMesh = new Model("models/IllumModels/grid.fbx");
	std::cout << "Cofre lalo" << std::endl;
	//chest = new Model("models/untitled.fbx");
	std::cout << "Fin Cofre lalo" << std::endl;

	// ---------------------------------- Pariculas ----------------------------------
	particlesShader = new Shader("shaders/13_particles.vs", "shaders/13_particles.fs");
	
	particleModel = new Model("models/Burbuja1.fbx");
	// ---------------------------------- Pariculas ----------------------------------

	
	{//basura
		std::cout << "Carga basura" << std::endl;
		bolsa = new Model("models/bolsaBasuraMejorada.fbx");
		std::cout << "1" << std::endl;
		tenedor = new Model("models/FORK.fbx");
		std::cout << "2" << std::endl;
		cuchara = new Model("models/CucharaYaPorfavor.fbx");
		std::cout << "3" << std::endl;
		tapa = new Model("models/Tapa_dorada.fbx");
		std::cout << "4" << std::endl;
		lata = new Model("models/lata.fbx");
		std::cout << "5" << std::endl;
		popote = new Model("models/popote.fbx");
		std::cout << "6" << std::endl;
		cigarro = new Model("models/Cigarro.fbx");
		std::cout << "7" << std::endl;
		plato = new Model("models/PLATE.fbx");
		std::cout << "8" << std::endl;
		botella_vidrio = new Model("models/botelladevino.fbx");
		std::cout << "9" << std::endl;
		botella_plastico = new Model("models/botellaplastico.fbx");
		std::cout << "10" << std::endl;
		std::cout << "Termina basura" << std::endl;
		//cofre = new Model("models/cofreahorasi.fbx");
	}
	
	{//Animales
		std::cout << "Carga animales" << std::endl;
		pez = new AnimatedModel("models/pezBien.fbx");
		std::cout << "1" << std::endl;
		tortuga = new Model("models/tortuga.fbx");
		std::cout << "2" << std::endl;
		medusa = new AnimatedModel("models/jellyfish.fbx");
		std::cout << "3" << std::endl;
		pulpo = new AnimatedModel("models/pulpoAnimado.fbx");
		std::cout << "4" << std::endl;
		cangrejo = new AnimatedModel("models/Crab.fbx");
		std::cout << "5" << std::endl;
		calamar = new AnimatedModel("models/calamarAnimado.fbx");
		std::cout << "6" << std::endl;
		estrella = new Model("models/star.fbx");
		std::cout << "7" << std::endl;
		mantaraya = new AnimatedModel("models/ray_merged.fbx");
		std::cout << "8" << std::endl;
		caballito = new AnimatedModel("models/caballito de mar.fbx");
		std::cout << "9" << std::endl;
		delfin = new AnimatedModel("models/DolphinFinal_Animate2.fbx");
		std::cout << "10" << std::endl;
		erizo = new Model("models/Erizo_mar.fbx");
		std::cout << "Termina animales" << std::endl;

	}

	{//etc
		burbuja1 = new Model("models/Burbuja1.fbx");
		burbuja2 = new Model("models/Burbuja2.fbx");
	}

	character01 = new AnimatedModel("models/pezBien.fbx");
	std::cout << "Termina carga personaje" << std::endl;

	// Cubemap
	vector<std::string> faces
	{
		"textures/cubemap/01/posx.png",
		"textures/cubemap/01/negx.png",
		"textures/cubemap/01/posy.png",
		"textures/cubemap/01/negy.png",
		"textures/cubemap/01/posz.png",
		"textures/cubemap/01/negz.png"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f;
	camera3rd.Position -= trdpersonOffset * forwardView;
	camera3rd.Front = forwardView;

	// Lights configuration
	
	Light light01;
	light01.Position = glm::vec3(5.0f, 2.0f, 5.0f);
	light01.Color = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	gLights.push_back(light01);

	Light light02;
	light02.Position = glm::vec3(-5.0f, 2.0f, 5.0f);
	light02.Color = glm::vec4(0.0f, 0.2f, 0.0f, 1.0f);
	gLights.push_back(light02);

	Light light03;
	light03.Position = glm::vec3(5.0f, 2.0f, -5.0f);
	light03.Color = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);
	gLights.push_back(light03);

	Light light04;
	light04.Position = glm::vec3(-5.0f, 2.0f, -5.0f);
	light04.Color = glm::vec4(0.2f, 0.2f, 0.0f, 1.0f);
	gLights.push_back(light04);
	

	// Reproduce la canción en loop
	SoundEngine = createIrrKlangDevice();
	SoundEngine->setSoundVolume(0.4f); // 50% volumen

	return true;
}


void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec3(uniformName.c_str(), value);
}


bool Update() {
	// Cálculo del framerate
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// ---------------------------------- Pariculas ----------------------------------
	elapsedTime += deltaTime;
	if (elapsedTime > 1.0f / 30.0f) {
		elapsedTime = 0.0f;
	
		particlesSystem.UpdatePhysics(deltaTime);
	}
	// ---------------------------------- Pariculas ----------------------------------
	
	// Procesa la entrada del teclado o mouse
	processInput(window);

	// Renderizado R - G - B - A
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection;
	glm::mat4 view;

	if (activeCamera == 0) {
		// Primera persona
		projection = glm::perspective(glm::radians(camera.Zoom),
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera.GetViewMatrix();
	}
	else if (activeCamera == 1) {
		projection = glm::perspective(glm::radians(camera3rd.Zoom),
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera3rd.GetViewMatrix();

		dynamicShader->use();
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);

		// Ajuste: posición un poco más abajo y enfrente de la cámara
		glm::vec3 offset = glm::vec3(0.0f, 0.3f, 0.5f); // bajamos Y a 0.3, enfrente 0.5
		model = glm::translate(model, position + offset);

		// Ajuste: rotar hacia donde mira la cámara, pero mostrando la espalda
		float angle = atan2(forwardView.x, forwardView.z);
		model = glm::rotate(model, angle + glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Mantener la escala que ya tenías
		model = glm::scale(model, glm::vec3(0.0007f, 0.0007f, 0.0007f));

		dynamicShader->setMat4("model", model);

		character01->UpdateAnimation(deltaTime);
		character01->Draw(*dynamicShader);

	}

	
	// Cubemap (fondo)
	{
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}
	
	if (submarino) {

		{
			{
				// Activación del shader de las partículas
				particlesShader->use();
				particlesShader->setMat4("projection", projection);
				particlesShader->setMat4("view", view);

				// Activamos para objetos transparentes
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glm::mat4 model;

				for (int psc = 0; psc < particlesSystem.particles.size(); psc++) {
					Particle p_i = particlesSystem.particles.at(psc);

					// Aplicamos transformaciones del modelo
					model = glm::mat4(1.0f);
					model = glm::translate(model, p_i.position); // translate it down so it's at the center of the scene
					model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down

					particlesShader->setMat4("model", model);

					// Dibujamos el modelo
					particleModel->Draw(*particlesShader);
				}

			}


			mLightsShader->use();

			// Activamos para objetos transparentes
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			mLightsShader->setMat4("projection", projection);
			mLightsShader->setMat4("view", view);

			// Aplicamos transformaciones del modelo
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);

			// Configuramos propiedades de fuentes de luz
			mLightsShader->setInt("numLights", (int)gLights.size());
			for (size_t i = 0; i < gLights.size(); ++i) {
				SetLightUniformVec3(mLightsShader, "Position", i, gLights[i].Position);
				SetLightUniformVec3(mLightsShader, "Direction", i, gLights[i].Direction);
				SetLightUniformVec4(mLightsShader, "Color", i, gLights[i].Color);
				SetLightUniformVec4(mLightsShader, "Power", i, gLights[i].Power);
				SetLightUniformInt(mLightsShader, "alphaIndex", i, gLights[i].alphaIndex);
				SetLightUniformFloat(mLightsShader, "distance", i, gLights[i].distance);
			}

			mLightsShader->setVec3("eye", camera.Position);

			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);

			// parametros para efecto de agua
			mLightsShader->setVec3("cameraPos", camera.Position);
			mLightsShader->setFloat("time", glfwGetTime());
			mLightsShader->setFloat("waterLevel", 0.0f); // adjust if needed
			mLightsShader->setFloat("fogDensity", 0.03f);
			mLightsShader->setFloat("depthAttenuation", 0.006f);
			mLightsShader->setVec3("fogColor", glm::vec3(0.0f, 0.25f, 0.45f));



			terrain->Draw(*mLightsShader);
			decor->Draw(*mLightsShader);
			//boat->Draw(*mLightsShader);
			//glUseProgram(0);
			
			//chest->Draw(*mLightsShader);
			/*Model* bolsa;
			Model* tenedor;
			//Model* cuchara;
			Model* tapa;
			Model* lata;
			Model* popote;
			Model* cigarro;
			Model* plato;
			//Model* botella_vidrio;
			Model* botella_plastico;*/


			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-5.0f, 0.5f, 4.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			bolsa->Draw(*mLightsShader);
			//glUseProgram(0);


			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-5.0f, 0.15f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			tenedor->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			cuchara->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			tapa->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			lata->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.1f, -10.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			popote->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.2f, 10.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			plato->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -10.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", 0.9f);
			botella_vidrio->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -5.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", 0.8f);
			botella_plastico->Draw(*mLightsShader);
			//glUseProgram(0);

			////mLightsShader->use();
			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(3.0f, 0.0f, -3.0f)); // translate it down so it's at the center of the scene
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
			//mLightsShader->setMat4("model", model);
			//// Aplicamos propiedades materiales
			//mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			//mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			//mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			//mLightsShader->setFloat("transparency", material01.transparency);
			//tortuga->Draw(*mLightsShader);
			////glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.0f, -0.1f, 3.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			estrella->Draw(*mLightsShader);
			//glUseProgram(0);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.15f, -5.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			cigarro->Draw(*mLightsShader);

			////mLightsShader->use();
			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(-9.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(2.0f, 0.9f, 0.8f));	// it's a bit too big for our scene, so scale it down
			//mLightsShader->setMat4("model", model);
			//// Aplicamos propiedades materiales
			//mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			//mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			//mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			//mLightsShader->setFloat("transparency", material01.transparency);
			//cofre->Draw(*mLightsShader);

			//mLightsShader->use();
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-12.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
			mLightsShader->setMat4("model", model);
			// Aplicamos propiedades materiales
			mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
			mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
			mLightsShader->setFloat("transparency", material01.transparency);
			erizo->Draw(*mLightsShader);
			glUseProgram(0);

		}

		


		// Actividad 5.2

		{
			// Activamos el shader de Phong
			proceduralShader->use();

			// Activamos para objetos transparentes
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Aplicamos transformaciones de proyección y cámara (si las hubiera)
			proceduralShader->setMat4("projection", projection);
			proceduralShader->setMat4("view", view);

			// Aplicamos transformaciones del modelo
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
			proceduralShader->setMat4("model", model);

			proceduralShader->setFloat("time", proceduralTime);
			proceduralShader->setFloat("radius", 20.0f);
			proceduralShader->setFloat("height", 20.0f);

			//moon->Draw(*proceduralShader);
			tortuga->Draw(*proceduralShader);

			

		}

		glUseProgram(0);


		// Actividad 5.3
		//
		//{
		//	// Activamos el shader de Phong
		//	wavesShader->use();

		//	// Activamos para objetos transparentes
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//	// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		//	wavesShader->setMat4("projection", projection);
		//	wavesShader->setMat4("view", view);

		//	// Aplicamos transformaciones del modelo
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		//	wavesShader->setMat4("model", model);

		//	wavesShader->setFloat("time", wavesTime);
		//	wavesShader->setFloat("radius", 5.0f);
		//	wavesShader->setFloat("height", 5.0f);

		//	gridMesh->Draw(*wavesShader);
		//	wavesTime += 0.01;

		//}

		//glUseProgram(0);
		//

		// Objeto animado

		{	
			character01->UpdateAnimation(deltaTime);
			//pez->UpdateAnimation(deltaTime);
			//tortuga->UpdateAnimation(deltaTime);
			
			
			
			
			//cangrejo->UpdateAnimation(deltaTime);


			// Activación del shader del personaje
			dynamicShader->use();

			// Aplicamos transformaciones de proyección y cámara (si las hubiera)
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);

			// Aplicamos transformaciones del modelo
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f)); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);

			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character01->gBones);

			// parametros para efecto de agua
			dynamicShader->setVec3("cameraPos", camera.Position);
			dynamicShader->setFloat("time", glfwGetTime());
			dynamicShader->setFloat("waterLevel", 0.0f); // adjust if needed
			dynamicShader->setFloat("fogDensity", 0.03f);
			dynamicShader->setFloat("depthAttenuation", 0.006f);
			dynamicShader->setVec3("fogColor", glm::vec3(0.0f, 0.25f, 0.45f));



			// Dibujamos el modelo
			//character01->Draw(*dynamicShader);

			/*AnimatedModel* pez;
			AnimatedModel* tortuga;
			AnimatedModel* medusa;
			AnimatedModel* pulpo;
			AnimatedModel* calamar;
			AnimatedModel* estrella;
			AnimatedModel* mantaraya;
			AnimatedModel* caballito;
			AnimatedModel* delfin;
			AnimatedModel* cangrejo;*/
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character01->gBones);
			//character01->Draw(*dynamicShader);
			glUseProgram(0);


			caballito->UpdateAnimation(deltaTime);
			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			glm::vec3 translate = trebol(glm::vec3(-3.0f, 2.0f, 0.0f), proceduralTime, 1.0, 1.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.001f, 1.001f, 1.001f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, caballito->gBones);
			caballito->Draw(*dynamicShader);
			glUseProgram(0);

			pez->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 5.0f);
			dynamicShader->setFloat("height", 10.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(0.0f, 1.0f, 0.0f), proceduralTime, 10.0, 10.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, pez->gBones);
			pez->Draw(*dynamicShader);
			glUseProgram(0);

			cangrejo->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 2.0f);
			dynamicShader->setFloat("height", 1.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(0.0f, 0.0f, 0.0f), proceduralTime, 1.0, 0.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, cangrejo->gBones);
			cangrejo->Draw(*dynamicShader);
			glUseProgram(0);

			medusa->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 25.0f);
			dynamicShader->setFloat("height", 15.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(-3.0f, 2.0f, 0.0f), proceduralTime, 15.0, 8.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, medusa->gBones);
			medusa->Draw(*dynamicShader);
			glUseProgram(0);

			pulpo->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 8.0f);
			dynamicShader->setFloat("height", 10.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(-3.0f, 2.0f, 0.0f), proceduralTime, 12.0, 6.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter + 180.0f), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, pulpo->gBones);
			pulpo->Draw(*dynamicShader);
			glUseProgram(0);

			calamar->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 35.0f);
			dynamicShader->setFloat("height", 7.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);


			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(-3.0f, 2.0f, 0.0f), proceduralTime, 20.0, 16.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter + 180.0f), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, calamar->gBones);
			calamar->Draw(*dynamicShader);
			glUseProgram(0);

			mantaraya->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 40.0f);
			dynamicShader->setFloat("height", 5.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(-3.0f, 2.0f, 0.0f), proceduralTime, 12.0, 16.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter + 180.0f), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, mantaraya->gBones);
			mantaraya->Draw(*dynamicShader);
			glUseProgram(0);

			delfin->UpdateAnimation(deltaTime);

			/*dynamicShader->setFloat("time", proceduralTime);
			dynamicShader->setFloat("radius", 20.0f);
			dynamicShader->setFloat("height", 3.0f);*/

			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			model = glm::mat4(1.0f);
			translate = trebol(glm::vec3(-3.0f, 2.0f, 0.0f), proceduralTime, 5.0, 7.0);
			model = glm::translate(model, translate); // translate it down so it's at the center of the scene
			model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	// it's a bit too big for our scene, so scale it down
			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin->gBones);
			delfin->Draw(*dynamicShader);
			glUseProgram(0);

			proceduralTime += 0.0001;
		}


		PlaySceneMusic("sounds/SusurrosdelMar.mp3"); // música submarina

	}
	else {
		PlaySceneMusic("sounds/OlasdeSal.mp3"); // música playa
	}
	 

	// glfw: swap buffers 
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// Procesamos entradas del teclado
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime*2.0);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		submarino = true;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		submarino = false;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		door_rotation += 0.1f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		door_rotation -= 0.1f;

	// Character movement
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {

		position = position + scaleV * forwardView;
		camera3rd.Front = forwardView;
		camera3rd.ProcessKeyboard(FORWARD, deltaTime);
		camera3rd.Position = position;
		camera3rd.Position.y += 1.7f;
		camera3rd.Position -= trdpersonOffset * forwardView;

	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position = position - scaleV * forwardView;
		camera3rd.Front = forwardView;
		camera3rd.ProcessKeyboard(BACKWARD, deltaTime);
		camera3rd.Position = position;
		camera3rd.Position.y += 1.7f;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		rotateCharacter += 0.5f;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		forwardView = glm::vec3(viewVector);
		forwardView = glm::normalize(forwardView);

		camera3rd.Front = forwardView;
		camera3rd.Position = position;
		camera3rd.Position.y += 1.7f;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		rotateCharacter -= 0.5f;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		forwardView = glm::vec3(viewVector);
		forwardView = glm::normalize(forwardView);

		camera3rd.Front = forwardView;
		camera3rd.Position = position;
		camera3rd.Position.y += 1.7f;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}

	// Detectar transición de teclas F1 y F2
	static bool f1Last = false;
	static bool f2Last = false;

	bool f1Now = glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS;
	bool f2Now = glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS;

	if (f1Now && !f1Last) {
		activeCamera = 0; // primera persona
		std::cout << "Cámara: Primera persona" << std::endl;
	}
	if (f2Now && !f2Last) {
		activeCamera = 1; // tercera persona
		std::cout << "Cámara: Tercera persona" << std::endl;
	}

	f1Last = f1Now;
	f2Last = f2Now;

	// Sincronizar la posición del jugador con la cámara activa
	position = camera.Position;
	forwardView = camera.Front;

	// Actualizar cámara de tercera persona para que siga al jugador
	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f; // altura de la cámara
	camera3rd.Position -= trdpersonOffset * forwardView; // offset hacia atrás
	camera3rd.Front = forwardView;
}

// glfw: Actualizamos el puerto de vista si hay cambios del tamaño
// de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: Callback del movimiento y eventos del mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; 

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: Complemento para el movimiento y eventos del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}
