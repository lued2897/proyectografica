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

#include "stb_image.h"


// Functions
bool Start();
bool Update();
bool Intro();

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

float angCofre = 0.0f;

glm::vec3 position(0.0f,0.0f, 0.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     trdpersonOffset = 1.5f;
float     scaleV = 0.025f;
float     rotateCharacter = 0.0f;
float	  door_offset = 0.0f;
float	  door_rotation = 0.0f;

//Radio para colisiones
float cameraRadius = 0.4f;  // whatever you want

//variables para efecto bajo el agua
float		water_level = 5.0f;
float		fog_density = 0.04f;
float		depth_attnuation = 0.06;
glm::vec3	water_color = glm::vec3(0.0f, 0.25f, 0.45f);


bool draw_bolsa = true;
bool draw_tenedor = true;
bool draw_cuchara = true;
bool draw_tapa = true;
bool draw_lata = true;
bool draw_popote = true;
bool draw_plato = true;
bool draw_botella_vidrio = true;
bool draw_botella_plastico = true;
bool draw_cigarro = true;

bool draw_colliders = false;

bool girocofre = false;

//Generación de algas aleatorias
const int NUM_ALGAS = 200;
std::vector<glm::vec3> algaPositions;

// Shaders
Shader *mLightsShader;
Shader *proceduralShader;
Shader *wavesShader;

Shader *cubemapShader;
Shader *dynamicShader;

Shader* algaShader; // Shader Animación de algas

Shader* fullscreenShader; //UI

Shader* fresnelShader;


// ---------------------------------- Pariculas ----------------------------------
Shader* particlesShader;
// Partículas
Particles particlesSystem(700); // creamos 200 partículas

// Carga la información del modelo
Model* particleModel;
// ---------------------------------- Pariculas ----------------------------------

// Carga la información del modelo
Model	*terrain;
Model	*decor;
Model   *boat;
Model   *moon;
Model   *gridMesh;

//modelos estaticos playa
Model* beach_terrain;


Model	*chest;
Model	*burbuja1;
Model	*burbuja2;

Model* alga3d;
Model* algaMesh;

//colisiones
Model* bounding_boxes;
Model* bounding_boxes_playa;
Model* bounding_boxes_agua;


// letreros o carteles
Model* cartelMar1;
Model* cartelMarmapa;
Model* cartelPlaya1;
Model* cartelPlayamapa;

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
Model* cofre_sup;
Model* cofre_inf;
Model* monkey;
Model* flecha;

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
AnimatedModel	*delfin2;
AnimatedModel	*cangrejo;
Model			*erizo;

AnimatedModel* diver;
AnimatedModel* diver_walk;
AnimatedModel* diver_idle;


float tradius = 10.0f;
float theta = 0.0f;
float alpha = 0.0f;

// Cubemap
CubeMap *mainCubeMap;
CubeMap* beachCubeMap;

// Light gLight;
std::vector<Light> gLights;

//Luces superficie
std::vector<Light> Lights_playa;

// Materiales
Material material01;

float proceduralTime = 0.0f;
float wavesTime = 0.0f;
glm::vec4 WHITE(0.8f, 0.8f, 0.8, 1.0f);

// Audio/
ISoundEngine *SoundEngine = createIrrKlangDevice();

//Cancion segun la escena
ISound* currentMusic = nullptr; // canción actual

// selección de cámara
int    activeCamera = 0; // activamos la primera cámara

//para imprimir imagen en pantalla completa
unsigned int quadVAO, quadVBO;
unsigned int textTexture;
unsigned int texPressR;
unsigned int texCamera;
unsigned int texWASD;
unsigned int texTrash;


float fullscreenQuad[] = {
	// pos      // tex coords (invertido en Y)
	-1.0f,  1.0f,  0.0f, 0.0f,   // top-left → texCoord (0,0)
	-1.0f, -1.0f,  0.0f, 1.0f,   // bottom-left → texCoord (0,1)
	 1.0f, -1.0f,  1.0f, 1.0f,   // bottom-right → texCoord (1,1)

	-1.0f,  1.0f,  0.0f, 0.0f,   // top-left
	 1.0f, -1.0f,  1.0f, 1.0f,   // bottom-right
	 1.0f,  1.0f,  1.0f, 0.0f    // top-right → texCoord (1,0)
};

//colisiones
bool sphereAABB(const glm::vec3& center, float radius, const AABB& box)
{
	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(center.x, box.minExtent.x, box.maxExtent.x);
	closestPoint.y = glm::clamp(center.y, box.minExtent.y, box.maxExtent.y);
	closestPoint.z = glm::clamp(center.z, box.minExtent.z, box.maxExtent.z);

	glm::vec3 diff = closestPoint - center;
	float dist2 = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	return dist2 < radius * radius;
}


bool nearTrash(glm::vec3 cameraPos, glm::vec3 transform) {
	if (length(glm::vec2(cameraPos.x, cameraPos.z) - glm::vec2(transform.x, transform.z)) < 2.5f) {
		//std::cout << "Tenedor" << std::endl;
		return true;
	}
	else {
		return false;
	}
}

void loadTexture(const char* path, unsigned int& texID) {
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, n;
	unsigned char* data = stbi_load(path, &w, &h, &n, 0);

	if (data) {
		GLenum format = (n == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded " << path << " " << w << "x" << h << "\n";
	}
	else {
		std::cout << "Failed loading " << path << "\n";
	}

	stbi_image_free(data);
}



// Entrada a función principal
int main()
{
	if (!Start())
		return -1;

	lastFrame = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		if (!Intro())
			break;
	}
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;

}
//movimiento Trebol
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
//movimiento Anillo Sinusoidal 
glm::vec3 anilloSinusoidal(glm::vec3 center, float t, float radius, float amplitude, float n)
{
	float x = center.x + radius * cos(t);
	float z = center.z + radius * sin(t);
	float y = center.y + amplitude * sin(n * t);

	return glm::vec3(x, y, z);
}
//Calculo de orientación de la cara
glm::mat4 orientAlongPath(const glm::vec3& current, const glm::vec3& next)
{
	glm::vec3 forward = glm::normalize(next - current); // dirección de avance

	// Up “global”
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Evitar que forward sea casi paralelo a up
	if (fabs(glm::dot(forward, up)) > 0.99f)
		up = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 right = glm::normalize(glm::cross(up, forward));
	up = glm::normalize(glm::cross(forward, right));

	glm::mat4 R(1.0f);
	// Columnas de la matriz (convención de GLM)
	R[0] = glm::vec4(right, 0.0f);
	R[1] = glm::vec4(up, 0.0f);
	R[2] = glm::vec4(forward, 0.0f);  // forward, si tu delfín mira hacia +Z
	return R;
}

//Definición arreglo de peces.
	struct FishPath {
		glm::vec3 center;
		float radius;
		float amplitude;
		float n;             // número de ondas
		float speed;         // qué tan rápido recorre el anillo
		float directionSign; // +1 o -1 (sentido)
		float phase;         // desfase inicial
		float time;          // tiempo acumulado
	};
	const int NUM_PECES = 30;
	std::vector<FishPath> gFishes;
	const float PI = 3.14159265359f;

//Definición arreglo de medusas
	struct MedusaPath {
		glm::vec3 center;
		float radius;
		float amplitude;
		float n;             // número de ondas
		float speed;         // velocidad
		float directionSign; // +1 o -1 (sentido del recorrido)
		float phase;         // desfase inicial
		float time;          // tiempo acumulado
	};
	const int NUM_MEDUSAS = 10;
	std::vector<MedusaPath> gMedusas;

//Definición arreglo de pulos
	struct PulpoPath {
		glm::vec3 center;
		float radius;
		float amplitude;
		float n;             // número de ondas
		float speed;         // velocidad
		float directionSign; // +1 o -1 (sentido)
		float phase;         // desfase inicial
		float time;          // tiempo acumulado
	};
	const int NUM_PULPOS = 10;
	std::vector<PulpoPath> gPulpos;

//Definición arreglo de calamares
	struct CalamarPath {
		glm::vec3 center;
		float radius;
		float amplitude;
		float n;             // número de ondas
		float speed;         // velocidad
		float directionSign; // +1 o -1 (sentido)
		float phase;         // desfase inicial
		float time;          // tiempo acumulado
	};
	const int NUM_CALAMARES = 10;
	std::vector<CalamarPath> gCalamares;

//Definición arreglo de Mantarayas
	struct MantarayaPath {
		glm::vec3 center;
		float radius;
		float amplitude;
		float n;             // número de ondas
		float speed;         // velocidad
		float directionSign; // +1 o -1 (sentido)
		float phase;         // desfase inicial
		float time;          // tiempo acumulado
	};
	const int NUM_MANTARAYAS = 6;
	std::vector<MantarayaPath> gMantarayas;

//Definición arreglo de caballitos
	struct CaballitoPath {
		glm::vec3 center;
		float radius;
		float amplitude;
		float n;             // número de ondas
		float speed;         // velocidad
		float directionSign; // +1 o -1 (sentido)
		float phase;         // desfase inicial
		float time;          // tiempo acumulado
	};
	const int NUM_CABALLITOS = 20;
	std::vector<CaballitoPath> gCaballitos;

//Definición arreglo de cangrejos
	struct CangrejoPath {
		glm::vec3 center;   // centro del recorrido (en el piso)
		glm::vec3 dir;      // dirección en XZ donde se mueven
		float length;       // mitad del recorrido (de -length a +length)
		float speed;        // qué tan rápido “camina”
		float phase;        // desfase inicial
		float time;         // tiempo acumulado
	};
	const int NUM_CANGREJOS = 8;
	std::vector<CangrejoPath> gCangrejos;
	std::vector<CangrejoPath> gCangrejos2;

//Definición arreglo de tortugas
	struct TortugaPath {
		glm::vec3 center;   // centro del "trébol"
		float a;            // parámetro 1 de la curva (radio principal)
		float b;            // parámetro 2 de la curva
		float speed;        // velocidad sobre la trayectoria
		float directionSign;// +1 o -1 (sentido)
		float phase;        // desfase inicial
		float time;         // tiempo acumulado
	};
	const int NUM_TORTUGAS = 5;
	std::vector<TortugaPath> gTortugas;

	// Corrección fija del modelo de tortuga:
	// - La acostamos (-90° en X, como cuando la dibujabas estática)
	// - La giramos en Y para que mire hacia adelante (+Z local)
	glm::mat4 turtleCorrection = [] {
		glm::mat4 m(1.0f);

		// Igual que antes: acostar la tortuga
		m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// Si todavía la ves mirando mal, cambia 90.0f por -90.0f o 180.0f.
		//m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//m = glm::rotate(m, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		return m;
		}();

// Estrellas extra en el fondo
	const int NUM_ESTRELLAS_EXTRA = 100;
	std::vector<glm::vec3> gEstrellasExtra;

// Erizos extra 
	const int NUM_ERIZOS = 100;
	std::vector<glm::vec3> gErizos;

// Algas 3D extra fuera del área central
	const int NUM_ALGAS3D_EXTRA = 100;
	std::vector<glm::vec3> gAlgas3DExtra;

	// ================== BASURA: varios objetos ==================
	struct TrashInstance {
		Model* model;        // modelo a dibujar (bolsa, tenedor, etc.)
		glm::vec3 position;  // posición en el mundo
		float rotateX;       // rotación en X
		glm::vec3 scale;     // escala
		bool active;         // true = se dibuja, false = ya se "recogió"
	};
	const int NUM_TRASH_PER_TYPE = 15;   // 10 por tipo
	std::vector<TrashInstance> gTrash;   // todos los objetos de basura
	// == basura para la PLAYA ===
	const int NUM_TRASH_BEACH_PER_TYPE = 10;
	std::vector<TrashInstance> gTrashBeach;



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
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Vida Marina", NULL, NULL);
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
	fresnelShader = new Shader("shaders/11_fresnel.vs", "shaders/11_fresnel.fs");

	//algaShader = new Shader("shaders/13_algaAnimation.vs", "shaders/13_algaAnimation.fs"); // Shader Animación de algas
	algaShader = new Shader("shaders/13_algaAnimation.vs", "shaders/10_fragment_skinning-IT.fs");

	//shader de UI
	fullscreenShader = new Shader("shaders/fullscreen.vs", "shaders/fullscreen.fs");

	// Máximo número de huesos: 100
	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

	// Dibujar en malla de alambre
	// glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	/*house = new Model("models/IllumModels/House03.fbx");*/
	//house = new Model("models/rocks_w_corals.fbx");
	terrain = new Model("models/scene_static_models.fbx");
	//colisiones PLAYA
	bounding_boxes_agua = new Model("models/bounding_boxes_terrain.fbx");
	bounding_boxes_agua->calculateAABB();
	// ---------------------------------- Terreno playa ----------------------------------
	std::cout << "Cargando terreno... " << std::endl;
	beach_terrain = new Model("models/beach_terrain.fbx");
	bounding_boxes_playa = new Model("models/bounding_boxes_playa.fbx");
	bounding_boxes_playa->calculateAABB();


	decor = new Model("models/decor.fbx");
	//boat = new Model("models/boat.fbx");
	moon = new Model("models/IllumModels/moon.fbx");
	gridMesh = new Model("models/plano_mar.fbx");
	//std::cout << "Cofre lalo" << std::endl;
	//chest = new Model("models/untitled.fbx");
	//std::cout << "Fin Cofre lalo" << std::endl;

	// ---------------------------------- Pariculas ----------------------------------
	particlesShader = new Shader("shaders/13_particles.vs", "shaders/13_particles.fs");
	
	particleModel = new Model("models/Burbuja1Low.fbx");
	// ---------------------------------- Pariculas ----------------------------------

	
	{//basura
		std::cout << "Cargando modelos... " << std::endl;
		//std::cout << "Carga basura" << std::endl;
		bolsa = new Model("models/bolsaBasuraMejorada.fbx");
			bolsa->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			bolsa->material.diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
			bolsa->material.specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
			bolsa->material.alphaIndex = 40; //plastico negro
		//std::cout << "1" << std::endl;
		tenedor = new Model("models/FORK.fbx");
			tenedor->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			tenedor->material.diffuse = glm::vec4(0.55f, 0.55f, 0.55f, 1.0f);
			tenedor->material.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
			tenedor->material.alphaIndex = 40; //plastico blanco
		//std::cout << "2" << std::endl;
		cuchara = new Model("models/CucharaYaPorfavor.fbx");
			cuchara->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cuchara->material.diffuse = glm::vec4(0.55f, 0.55f, 0.55f, 1.0f);
			cuchara->material.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
			cuchara->material.alphaIndex = 40; //plastico blanco
		//std::cout << "3" << std::endl;
		tapa = new Model("models/Tapa_dorada.fbx");
			tapa->material.ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
			tapa->material.diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
			tapa->material.specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
			tapa->material.alphaIndex = 25; //oro
		//std::cout << "4" << std::endl;
		lata = new Model("models/lata.fbx");
			lata->material.ambient = glm::vec4(0.1745f, 	0.01175f, 	0.01175f, 1.0f);
			lata->material.specular = glm::vec4(0.527811f, 	0.426959f, 	0.426959f, 1.0f);
			lata->material.alphaIndex = 17; //metal rojo
		//std::cout << "5" << std::endl;
		popote = new Model("models/popote.fbx");
			popote->material.ambient = glm::vec4(0.0f, 	0.0f, 	0.0f, 1.0f);
			popote->material.diffuse = glm::vec4(0.5f, 	0.0f, 	0.0f, 1.0f);
			popote->material.specular = glm::vec4(0.7f, 	0.6f, 	0.6f, 1.0f);
			popote->material.alphaIndex = 40; //plastico rojo
		//std::cout << "6" << std::endl;
		cigarro = new Model("models/Cigarro.fbx");
			cigarro->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cigarro->material.diffuse = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
			cigarro->material.specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
			cigarro->material.alphaIndex = 120; //papel blanco
		//std::cout << "7" << std::endl;
		plato = new Model("models/PLATE.fbx");
			plato->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			plato->material.diffuse = glm::vec4(0.55f, 0.55f, 0.55f, 1.0f);
			plato->material.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
			plato->material.alphaIndex = 40; //plastico blanco
		//std::cout << "8" << std::endl;
		botella_vidrio = new Model("models/botelladevino.fbx");
			botella_vidrio->material.ambient = glm::vec4(0.074f, 0.231f, 0.070f, 1.0f);
			botella_vidrio->material.specular = glm::vec4(0.843f, 0.941f, 0.839f, 1.0f);
			botella_vidrio->material.alphaIndex = 20; //cristal verde
		//std::cout << "9" << std::endl;
		botella_plastico = new Model("models/botellaplastico.fbx");
			botella_plastico->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			botella_plastico->material.diffuse = glm::vec4(0.55f, 0.55f, 0.55f, 1.0f);
			botella_plastico->material.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
			botella_plastico->material.alphaIndex = 40; //plastico blanco
		//std::cout << "10" << std::endl;
		cofre_inf = new Model("models/cofre_inf.fbx");
			cofre_inf->material.ambient = glm::vec4(0.254f, 0.149f, 0.105f, 1.0f);
			cofre_inf->material.specular = glm::vec4(0.701f, 0.545f, 0.478f, 1.0f);
			cofre_inf->material.alphaIndex = 120; //madera
		//std::cout << "11" << std::endl;
		cofre_sup = new Model("models/cofre_sup.fbx");
			cofre_sup->material.ambient = glm::vec4(0.254f, 0.149f, 0.105f, 1.0f);
			cofre_sup->material.specular = glm::vec4(0.701f, 0.545f, 0.478f, 1.0f);
			cofre_sup->material.alphaIndex = 120; //madera
		//std::cout << "12" << std::endl;
		monkey = new Model("models/monkey.fbx");
			monkey->material.ambient = glm::vec4(0.24725f, 	0.1995f, 	0.0745f,1.0f);
			monkey->material.diffuse = glm::vec4(0.75164f, 	0.60648f, 	0.22648f, 1.0f);
			monkey->material.specular = glm::vec4(0.628281f, 	0.555802f, 	0.366065f, 1.0f);
			monkey->material.alphaIndex = 25; //oro
		//std::cout << "Termina basura" << std::endl;
		flecha = new Model("models/Flecha.fbx");
			popote->material.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			popote->material.diffuse = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
			popote->material.specular = glm::vec4(0.7f, 0.6f, 0.6f, 1.0f);
			popote->material.alphaIndex = 40; //plastico rojo
		//cofre = new Model("models/cofreahorasi.fbx");
		cartelMar1 = new Model("models/Cartelmar1.fbx");
		cartelMarmapa = new Model("models/Cartelmarmapa.fbx");
		cartelPlaya1 = new Model("models/cartelplaya1.fbx");
		cartelPlayamapa = new Model("models/Cartelplayamapa.fbx");
	}
	// ================== Crear basura aleatoria Submarina ==================
	{
		// ---- 1) Helper: comprobar si una esfera en 'pos' choca con los AABB del terreno submarino
		auto collidesWithWaterObstacles = [&](const glm::vec3& pos, float radius) -> bool
			{
				// Recorremos todos los AABB calculados para bounding_boxes_agua
				for (const auto& box : bounding_boxes_agua->aabbs) {
					// Usa la MISMA función que usa  la cámara:
					if (sphereAABB(pos, radius, box)){
						return true;
					}
				}
				return false;
			};

		// ---- 2) Función para añadir instancias de basura evitando obstáculos
		auto addTrashInstances = [&](Model* model, float rotX, glm::vec3 scale, float collisionRadius) {
			for (int i = 0; i < NUM_TRASH_PER_TYPE; ++i) {

				glm::vec3 pos;
				bool found = false;

				// Intentamos varias veces hasta encontrar un lugar libre
				const int MAX_TRIES = 50;
				for (int attempt = 0; attempt < MAX_TRIES; ++attempt) {
					float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

					float x = -50.0f + rx * 100.0f; // [-50, 50]
					float z = -50.0f + rz * 100.0f; // [-50, 50]

					glm::vec3 candidatePos = glm::vec3(x, 0.3f, z);

					// Si NO choca con ningún obstáculo submarino, aceptamos esta posición
					if (!collidesWithWaterObstacles(candidatePos, collisionRadius)) {
						pos = candidatePos;
						found = true;
						break;
					}
				}

				// Si no encontramos buen lugar en MAX_TRIES, mejor no instanciamos esa pieza
				if (!found) continue;

				TrashInstance t;
				t.model = model;
				t.position = pos;         // posición ya libre de colisión
				t.rotateX = rotX;
				t.scale = scale;
				t.active = true;

				gTrash.push_back(t);
			}
			};

		// ---- 3) Crear cada tipo de basura con un radio aproximado (para la prueba de colisión)
		//        Ajusta los radios según el tamaño real de cada modelo.
		addTrashInstances(bolsa, -90.0f, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		addTrashInstances(tenedor, -90.0f, glm::vec3(0.055f, 0.055f, 0.055f), 0.4f);
		addTrashInstances(cuchara, -90.0f, glm::vec3(0.105f, 0.105f, 0.105f), 0.4f);
		addTrashInstances(lata, 0.0f, glm::vec3(0.105f, 0.105f, 0.105f), 0.5f);
		addTrashInstances(popote, 0.0f, glm::vec3(0.025f, 0.025f, 0.02f), 0.3f);
		addTrashInstances(plato, -90.0f, glm::vec3(0.30f, 0.30f, 0.30f), 0.7f);
		addTrashInstances(botella_vidrio, -90.0f, glm::vec3(0.10f, 0.10f, 0.10f), 0.6f);
		addTrashInstances(botella_plastico, -90.0f, glm::vec3(0.10f, 0.10f, 0.10f), 0.6f);
		addTrashInstances(cigarro, 0.0f, glm::vec3(0.1f, 0.1f, 0.1f), 0.3f);
		addTrashInstances(tapa, 0.0f, glm::vec3(0.001f, 0.001f, 0.001f), 0.2f);
	}

	// ================== Crear basura aleatoria PLAYA ==================
	{
		auto addTrashInstancesBeach = [&](Model* model, float rotX, glm::vec3 scale) {
			for (int i = 0; i < NUM_TRASH_BEACH_PER_TYPE; ++i) {
				float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				// x ∈ [-55,55], z ∈ [-12,72]
				float z = -50.0f + rx * 100.0f; // [-50, 100]
				float x = -10.0f + rz * 80.0f; // [-10, 70]

				TrashInstance t2;
				t2.model = model;
				t2.position = glm::vec3(x, 0.22f, z); // y fijo
				t2.rotateX = rotX;
				t2.scale = scale;
				t2.active = true;

				gTrashBeach.push_back(t2);
			}
			};

		// Escalas/rotaciones 
		addTrashInstancesBeach(bolsa, -90.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		addTrashInstancesBeach(tenedor, -90.0f, glm::vec3(0.055f, 0.055f, 0.055f));
		addTrashInstancesBeach(cuchara, -90.0f, glm::vec3(0.105f, 0.105f, 0.105f));
		addTrashInstancesBeach(lata, 0.0f, glm::vec3(0.105f, 0.105f, 0.105f));
		addTrashInstancesBeach(popote, 0.0f, glm::vec3(0.025f, 0.025f, 0.02f));
		addTrashInstancesBeach(plato, -90.0f, glm::vec3(0.30f, 0.30f, 0.30f));
		addTrashInstancesBeach(botella_vidrio, -90.0f, glm::vec3(0.10f, 0.10f, 0.10f));
		addTrashInstancesBeach(botella_plastico, -90.0f, glm::vec3(0.10f, 0.10f, 0.10f));
		addTrashInstancesBeach(cigarro, 0.0f, glm::vec3(0.1f, 0.1f, 0.1f));
		addTrashInstancesBeach(tapa, 0.0f, glm::vec3(0.001f, 0.001f, 0.001f));
	}

	
	{//Animales
		std::cout << "Cargando animaciones... " << std::endl;
		//std::cout << "Carga animales" << std::endl;
		pez = new AnimatedModel("models/pezBien.fbx");
		//std::cout << "1" << std::endl;
		tortuga = new Model("models/tortuga.fbx");
		//std::cout << "2" << std::endl;
		medusa = new AnimatedModel("models/jellyfish.fbx");
		//std::cout << "3" << std::endl;
		pulpo = new AnimatedModel("models/pulpoAnimado.fbx");
		//std::cout << "4" << std::endl;
		cangrejo = new AnimatedModel("models/Crab.fbx");
		//std::cout << "5" << std::endl;
		calamar = new AnimatedModel("models/calamarAnimado.fbx");
		//std::cout << "6" << std::endl;
		estrella = new Model("models/star.fbx");
		//std::cout << "7" << std::endl;
		mantaraya = new AnimatedModel("models/ray_merged.fbx");
		//std::cout << "8" << std::endl;
		caballito = new AnimatedModel("models/caballito de mar.fbx");
		//std::cout << "9" << std::endl;
		delfin = new AnimatedModel("models/DolphinFinal_Animate2.fbx");
		//std::cout << "10" << std::endl;
		erizo = new Model("models/Erizo_mar.fbx");
		//std::cout << "11" << std::endl;
		diver = new AnimatedModel("models/diver_swim.fbx");
		//std::cout << "12" << std::endl;
		diver_walk = new AnimatedModel("models/diver_walk.fbx");
		//std::cout << "13" << std::endl;
		diver_idle = new AnimatedModel("models/diver_idle.fbx");
		//std::cout << "14" << std::endl;

		delfin2 = new AnimatedModel("models/DolphinFinal_Animate3.fbx");
		//std::cout << "Termina animales" << std::endl;

	}

	{//etc
		burbuja1 = new Model("models/Burbuja1.fbx");
		burbuja2 = new Model("models/Burbuja2.fbx");
		alga3d = new Model("models/Alga3D.fbx");
		algaMesh = new Model("models/Alga2D.fbx");
	}

	character01 = new AnimatedModel("models/pezBien.fbx");
	//std::cout << "Termina carga personaje" << std::endl;

	// Cubemap
	vector<std::string> faces
	{
		"textures/cubemap/01/px.png",
		"textures/cubemap/01/nx.png",
		"textures/cubemap/01/py.png",
		"textures/cubemap/01/ny.png",
		"textures/cubemap/01/pz.png",
		"textures/cubemap/01/nz.png"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	vector<std::string> faces_beach
	{
		"textures/cubemap/02/px.png",
		"textures/cubemap/02/nx.png",
		"textures/cubemap/02/py.png",
		"textures/cubemap/02/ny.png",
		"textures/cubemap/02/pz.png",
		"textures/cubemap/02/nz.png"
	};
	beachCubeMap = new CubeMap();
	beachCubeMap->loadCubemap(faces_beach);

	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f;
	camera3rd.Position -= trdpersonOffset * forwardView;
	camera3rd.Front = forwardView;

	// Lights configuration
	
	Light light01;
	light01.Position = glm::vec3(10.0f, 50.0f, 10.0f);
	light01.Color = glm::vec4(0.5f, 0.7f, 0.9f, 1.0f);
	light01.Direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));  // angled sunlight
	light01.Power = glm::vec4(180.0f, 180.0f, 180.0f, 1.0f); // strong!
	light01.distance = 12.0f;
	gLights.push_back(light01);

	Light light02;
	light02.Position = glm::vec3(0.0f, 50.0f, 1.0f);
	light02.Color = glm::vec4(0.721f, 0.498f, 0.882f, 1.0f);
	light02.Direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));  // angled sunlight
	light02.Power = glm::vec4(12.0f, 12.0f, 12.0f, 1.0f); 
	light02.distance = 4.0f;
	gLights.push_back(light02);

	/*Light light02;
	light02.Position = glm::vec3(-20.0f, 10.0f, -20.0f);
	light02.Color = glm::vec4(0.2f, 0.1f, 0.5f, 1.0f);
	gLights.push_back(light02);*/


	//Luces superficie
	Light sun;
	sun.Position = glm::vec3(-70.0f, 60.0f, 90.0f);   // arbitrary, since it's directional
	sun.Direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));  // angled sunlight
	sun.Color = glm::vec4(1.0f, 0.95f, 0.85f, 1.0f);   // warm sunlight
	sun.Power = glm::vec4(180.0f, 180.0f, 180.0f, 1.0f); // strong!
	sun.alphaIndex = 32;    // sharp specular on wet sand
	sun.distance = 10.0f;       // large so falloff is low
	Lights_playa.push_back(sun);

	Light skylight;
	skylight.Position = glm::vec3(0.0f, 60.0f, 0.0f);
	skylight.Direction = glm::vec3(0, -1, 0);
	skylight.Color = glm::vec4(0.4f, 0.55f, 0.75f, 1.0f);  // bluish
	skylight.Power = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);
	skylight.alphaIndex = 4;     // skylight is diffuse
	skylight.distance = 4.0f;
	Lights_playa.push_back(skylight);

	/*Light light04;
	light02.Position = glm::vec3(5.0f, 10.0f, 5.0f);
	light02.Color = glm::vec4(0.3f, 0.4f, 0.05f, 1.0f);
	Lights_playa.push_back(light04);*/



	/*/Light light03;
	light03.Position = glm::vec3(5.0f, 2.0f, -5.0f);
	light03.Color = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);
	gLights.push_back(light03);

	Light light04;
	light04.Position = glm::vec3(-5.0f, 2.0f, -5.0f);
	light04.Color = glm::vec4(0.2f, 0.2f, 0.0f, 1.0f);
	gLights.push_back(light04);*/
	

	// Reproduce la canción en loop
	SoundEngine = createIrrKlangDevice();
	SoundEngine->setSoundVolume(0.4f); // 50% volumen

	// ------------------ Posiciones aleatorias para las algas ------------------
	srand(1234); // semilla fija para que siempre salgan igual (si quieres)

	algaPositions.reserve(NUM_ALGAS);
	for (int i = 0; i < NUM_ALGAS; ++i)
	{
		float x = -60.0f + static_cast<float>(rand()) / RAND_MAX * 120.0f; // [-60, 60]
		float z = -60.0f + static_cast<float>(rand()) / RAND_MAX * 120.0f; // [-60, 60]
		float y = 0.0f;                                                     // piso

		algaPositions.push_back(glm::vec3(x, y, z));
	}
	// ================== Inicializar peces en anillos sinusoidales ==================
	{
		gFishes.clear();
		gFishes.resize(NUM_PECES);

		for (int i = 0; i < NUM_PECES; ++i) {
			FishPath& F = gFishes[i];

			// ----- Centro aleatorio -----
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // [0,1]
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -50.0f + rx * 100.0f;  // [-50, 50]
			float cz = -50.0f + rz * 100.0f;  // [-50, 50]
			float cy = 5.0f + ry * 25.0f;   // [5, 30]

			F.center = glm::vec3(cx, cy, cz);

			// ----- Radio aleatorio entre 6 y 20 -----
			float minRadius = 6.0f;
			float maxRadius = 20.0f;
			float r01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			F.radius = minRadius + r01 * (maxRadius - minRadius);

			// Amplitud vertical
			F.amplitude = 2.0f + 0.5f * (i % 3);

			// Número de ondas
			F.n = 1.0f + float(i % 4);

			// Velocidad
			F.speed = 0.2f + 0.05f * float(i % 5);

			// Sentido (horario/antihorario)
			F.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial
			F.phase = (2.0f * PI / 5.0f) * float(i % 5);

			// Tiempo
			F.time = 0.0f;
		}
	}
	// ================== Inicializar medusas en anillos sinusoidales ==================
	{
		gMedusas.clear();
		gMedusas.resize(NUM_MEDUSAS);

		// Centros aleatorios para las medusas
		for (int i = 0; i < NUM_MEDUSAS; ++i) {
			MedusaPath& M = gMedusas[i];

			// Centro aleatorio: x,z ∈ [-50,50], y ∈ [7,30] (un poco más altas que muchos peces)
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -50.0f + rx * 100.0f;  // [-50, 50]
			float cz = -50.0f + rz * 100.0f;  // [-50, 50]
			float cy = 7.0f + ry * 20.0f;   // [7, 30]

			M.center = glm::vec3(cx, cy, cz);

			// Radios variables entre 6 y 18 (más contenidos)
			float minRadius = 6.0f;
			float maxRadius = 18.0f;
			float r01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			M.radius = minRadius + r01 * (maxRadius - minRadius);

			// Amplitud vertical (no tan grande, para que floten suave)
			M.amplitude = 1.5f + 0.5f * (i % 3); // ~1.5, 2.0, 2.5

			// Número de ondas
			M.n = 1.0f + float(i % 3); // 1,2,3

			// Velocidad (más lenta que peces)
			M.speed = 0.03f + 0.01f * float(i % 4); // 0.03 .. 0.06 aprox

			// Sentido horario / antihorario
			M.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial para que no vayan pegadas
			const float PI = 3.14159265359f;
			M.phase = (2.0f * PI / 10.0f) * float(i); // se reparte en el círculo

			// Tiempo inicial
			M.time = 0.0f;
		}

	}

	// ================== Inicializar pulpos en anillos sinusoidales ==================
	{
		gPulpos.clear();
		gPulpos.resize(NUM_PULPOS);

		for (int i = 0; i < NUM_PULPOS; ++i) {
			PulpoPath& P = gPulpos[i];

			// Centro aleatorio: x,z ∈ [-50,50], y ∈ [7,20]
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -50.0f + rx * 100.0f;  // [-50, 50]
			float cz = -50.0f + rz * 100.0f;  // [-50, 50]
			float cy = 7.0f + ry * 13.0f;   // [7, 20]

			P.center = glm::vec3(cx, cy, cz);

			// Radio moderado, para que no se vayan lejísimos
			float minRadius = 6.0f;
			float maxRadius = 18.0f;
			float r01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			P.radius = minRadius + r01 * (maxRadius - minRadius);

			// Amplitud vertical (suaves, no mucho brinco)
			P.amplitude = 1.0f + 0.5f * (i % 3); // ~1.0, 1.5, 2.0

			// Número de ondas
			P.n = 1.0f + float(i % 3); // 1,2,3

			// Velocidad 0.03 .. 0.06 aprox
			P.speed = 0.03f + 0.01f * float(i % 4); // 0.03 .. 0.06 aprox

			// Sentido horario / antihorario
			P.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial (separarlos en el anillo)
			const float PI = 3.14159265359f;
			P.phase = (2.0f * PI / NUM_PULPOS) * float(i);

			// Tiempo acumulado
			P.time = 0.0f;
		}
	}

	// ================== Inicializar calamares en anillos sinusoidales ==================
	{
		gCalamares.clear();
		gCalamares.resize(NUM_CALAMARES);

		for (int i = 0; i < NUM_CALAMARES; ++i) {
			CalamarPath& C = gCalamares[i];

			// Centro aleatorio: x,z ∈ [-20,20], y ∈ [15, 25]
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -10.0f + rx * 20.0f;  // [-10, 10]
			float cz = -10.0f + rz * 20.0f;  // [-10, 10]
			float cy = 7.0f + ry * 17.0f;   // [7, 25]

			C.center = glm::vec3(cx, cy, cz);

			// Radio grande para que SIEMPRE naden fuera de x,z ∈ [-50,50]
			float minRadius = 60.0f;
			float maxRadius = 100.0f;  // puedes subir/bajar este máximo si quieres
			float r01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			C.radius = minRadius + r01 * (maxRadius - minRadius);

			// Amplitud vertical
			C.amplitude = 1.0f + 0.7f * (i % 3); // ~1.0, 1.7, 2.4

			// Número de ondas
			C.n = 1.0f + float(i % 4); // 1,2,3,4

			// Velocidad más lenta para radios grandes: ~0.05 .. 0.15 aprox
			float s01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			C.speed = 0.05f + s01 * 0.10f;     // [0.05, 0.15]

			// Sentido horario / antihorario
			C.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial para separarlos
			const float PI = 3.14159265359f;
			C.phase = (2.0f * PI / NUM_CALAMARES) * float(i);

			// Tiempo inicial
			C.time = 0.0f;
		}
	}
	// ================== Inicializar mantarayas en anillos sinusoidales ==================
	{
		gMantarayas.clear();
		gMantarayas.resize(NUM_MANTARAYAS);

		for (int i = 0; i < NUM_MANTARAYAS; ++i) {
			MantarayaPath& M = gMantarayas[i];

			// Centro aleatorio: x,z ∈ [-50,50], y ∈ [7,20]
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -50.0f + rx * 100.0f;  // [-50, 50]
			float cz = -50.0f + rz * 100.0f;  // [-50, 50]
			float cy = 7.0f + ry * 13.0f;   // [7, 20]

			M.center = glm::vec3(cx, cy, cz);

			// Radio similar a peces/pulpos: 6 .. 25 aprox
			float minRadius = 6.0f;
			float maxRadius = 25.0f;
			float r01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			M.radius = minRadius + r01 * (maxRadius - minRadius);

			// Amplitud vertical (flotan suave)
			M.amplitude = 1.0f + 0.5f * (i % 3); // ~1.0, 1.5, 2.0

			// Número de ondas
			M.n = 1.0f + float(i % 3); // 1,2,3

			// Velocidad media: 0.1 .. 0.35 aprox
			float s01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			M.speed = 0.1f + s01 * 0.25f;

			// Sentido horario / antihorario
			M.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial para separarlas
			const float PI = 3.14159265359f;
			M.phase = (2.0f * PI / NUM_MANTARAYAS) * float(i);

			// Tiempo inicial
			M.time = 0.0f;
		}
	}
	// ================== Inicializar caballitos en anillos sinusoidales ==================
	{
		gCaballitos.clear();
		gCaballitos.resize(NUM_CABALLITOS);

		for (int i = 0; i < NUM_CABALLITOS; ++i) {
			CaballitoPath& C = gCaballitos[i];

			// Centro aleatorio: x,z ∈ [-50,50], y ∈ [5,20]
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -50.0f + rx * 100.0f;  // [-50, 50]
			float cz = -50.0f + rz * 100.0f;  // [-50, 50]
			float cy = 2.0f + ry * 13.0f;   // [2, 15]

			C.center = glm::vec3(cx, cy, cz);

			// Radio similar a peces: 6 .. 25
			float minRadius = 6.0f;
			float maxRadius = 25.0f;
			float r01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			C.radius = minRadius + r01 * (maxRadius - minRadius);

			// Amplitud vertical (sube/baja un poquito)
			C.amplitude = 1.0f + 0.5f * (i % 3); // ~1.0, 1.5, 2.0

			// Número de ondas alrededor del anillo
			C.n = 1.0f + float(i % 3); // 1,2,3

			// Velocidad MUY baja: ~0.02 .. 0.08 aprox
			float s01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			C.speed = 0.02f + s01 * 0.06f;   // [0.02, 0.08]

			// Sentido horario / antihorario
			C.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial (separarlos en la trayectoria)
			const float PI = 3.14159265359f;
			C.phase = (2.0f * PI / NUM_CABALLITOS) * float(i);

			// Tiempo inicial
			C.time = 0.0f;
		}
	}
	// ================== Inicializar cangrejos (movimiento lineal) ==================
	{
		gCangrejos.clear();
		gCangrejos.resize(NUM_CANGREJOS); // asegúrate que NUM_CANGREJOS == 7

		for (int i = 0; i < NUM_CANGREJOS; ++i) {
			CangrejoPath& C = gCangrejos[i];

			// Valores manuales por cangrejo
			switch (i) {
			case 0:
				C.center = glm::vec3(-35.0f, 0.003f, -20.0f);
				C.dir = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));   // mueve en X
				C.length = 6.0f;
				C.speed = 0.05f;   // lento
				C.phase = 0.0f;
				break;

			case 1:
				C.center = glm::vec3(-10.0f, 0.003f, -25.0f);
				C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));   // mueve en Z
				C.length = 5.0f;
				C.speed = 0.06f;
				C.phase = 1.0f;    // desfase para no ir igual que el 0
				break;

			case 2:
				C.center = glm::vec3(15.0f, 0.005f, -15.0f);
				C.dir = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f));   // diagonal
				C.length = 4.0f;
				C.speed = 0.07f;
				C.phase = 2.0f;
				break;

			case 3:
				C.center = glm::vec3(20.0f, 0.005f, 0.0f);
				C.dir = glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f));  // diagonal
				C.length = 7.0f;
				C.speed = 0.08f;
				C.phase = 0.5f;
				break;

			case 4:
				C.center = glm::vec3(-25.0f, 0.005f, 15.0f);
				C.dir = glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f));  // diagonal
				C.length = 5.5f;
				C.speed = 0.06f;
				C.phase = 1.5f;
				break;

			case 5:
				C.center = glm::vec3(5.0f, 0.005f, 25.0f);
				C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));  // en Z
				C.length = 6.5f;
				C.speed = 0.09f;
				C.phase = 2.5f;
				break;

			case 6:
				C.center = glm::vec3(40.0f, 0.005f, 10.0f);
				C.dir = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.3f));  // leve diagonal
				C.length = 7.0f;
				C.speed = 0.1f;
				C.phase = 3.0f;
				break;
			case 7:
				C.center = glm::vec3(0.0f, 0.005f, 0.0f);
				C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));  // en Z
				C.length = 6.5f;
				C.speed = 0.09f;
				C.phase = 2.5f;
				break;
			}

			// Tiempo inicial
			C.time = 0.0f;
		}
		//CANGREJOS SUPERFICIE
		{
			gCangrejos2.clear();
			gCangrejos2.resize(NUM_CANGREJOS); // asegúrate que NUM_CANGREJOS == 7

			for (int i = 0; i < NUM_CANGREJOS; ++i) {
				CangrejoPath& C = gCangrejos2[i];

				// Valores manuales por cangrejo
				switch (i) {
				case 0:
					C.center = glm::vec3(-10.0f, 0.25f, -20.0f);
					C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));   // mueve en X
					C.length = 6.0f;
					C.speed = 0.05f;   // lento
					C.phase = 0.0f;
					break;

				case 1:
					C.center = glm::vec3(10.0f, 0.25f, -25.0f);
					C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));   // mueve en Z
					C.length = 5.0f;
					C.speed = 0.06f;
					C.phase = 1.0f;    // desfase para no ir igual que el 0
					break;

				case 2:
					C.center = glm::vec3(35.0f, 0.25f, -15.0f);
					C.dir = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f));   // diagonal
					C.length = 4.0f;
					C.speed = 0.07f;
					C.phase = 2.0f;
					break;

				case 3:
					C.center = glm::vec3(40.0f, 0.25f, 0.0f);
					C.dir = glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f));  // diagonal
					C.length = 7.0f;
					C.speed = 0.08f;
					C.phase = 0.5f;
					break;

				case 4:
					C.center = glm::vec3(-5.0f, 0.25f, 15.0f);
					C.dir = glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f));  // diagonal
					C.length = 5.5f;
					C.speed = 0.06f;
					C.phase = 1.5f;
					break;

				case 5:
					C.center = glm::vec3(25.0f, 0.25f, 25.0f);
					C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));  // en Z
					C.length = 6.5f;
					C.speed = 0.09f;
					C.phase = 2.5f;
					break;

				case 6:
					C.center = glm::vec3(60.0f, 0.25f, 10.0f);
					C.dir = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.3f));  // leve diagonal
					C.length = 7.0f;
					C.speed = 0.1f;
					C.phase = 3.0f;
					break;
				case 7:
					C.center = glm::vec3(20.0f, 0.25f, 0.0f);
					C.dir = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));  // en Z
					C.length = 6.5f;
					C.speed = 0.09f;
					C.phase = 2.5f;
					break;
				}
			}
		}
	}
	// ================== Inicializar tortugas (trayectoria tipo trébol) ==================
	{
		gTortugas.clear();
		gTortugas.resize(NUM_TORTUGAS);

		for (int i = 0; i < NUM_TORTUGAS; ++i) {
			TortugaPath& T = gTortugas[i];

			// Centro aleatorio: x,z ∈ [-50,50], y ∈ [25,35]
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float ry = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float cx = -50.0f + rx * 100.0f;  // [-50, 50]
			float cz = -50.0f + rz * 100.0f;  // [-50, 50]
			float cy = 25.0f + ry * 10.0f;   // [25, 35]

			T.center = glm::vec3(cx, cy, cz);

			// Parámetros del trébol (tamaño de la figura)
			float r01a = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float r01b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			T.a = 4.0f + r01a * 4.0f;  // ~[4, 8]
			T.b = 5.0f + r01b * 5.0f;  // ~[5,10]

			// Velocidad lenta 
			float s01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			T.speed = 0.05f + s01 * 0.05f;   // [0.05, 0.10]

			// Sentido horario / antihorario
			T.directionSign = (i % 2 == 0) ? 1.0f : -1.0f;

			// Fase inicial para que no vayan pegadas
			const float PI = 3.14159265359f;
			T.phase = (2.0f * PI / NUM_TORTUGAS) * float(i);

			// Tiempo inicial
			T.time = 0.0f;
		}
	}
	// ================== Inicializar posiciones de estrellas extra ==================
	{
		gEstrellasExtra.clear();
		gEstrellasExtra.reserve(NUM_ESTRELLAS_EXTRA);

		for (int i = 0; i < NUM_ESTRELLAS_EXTRA; ++i) {
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float x = -60.0f + rx * 120.0f;  // [-60, 60]
			float z = -60.0f + rz * 120.0f;  // [-60, 60]
			float y = 0.05f;                  // altura fija

			gEstrellasExtra.emplace_back(x, y, z);
		}
	}
	// ================== Inicializar posiciones de erizos ==================
	{
		gErizos.clear();
		gErizos.reserve(NUM_ERIZOS);

		for (int i = 0; i < NUM_ERIZOS; ++i) {
			float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			float x = -60.0f + rx * 120.0f;  // [-60, 60]
			float z = -60.0f + rz * 120.0f;  // [-60, 60]
			float y = 0.03f;                 // altura fija

			gErizos.emplace_back(x, y, z);
		}
	}

	// ================== Inicializar posiciones de algas3d extra ==================
	{
		gAlgas3DExtra.clear();
		gAlgas3DExtra.reserve(NUM_ALGAS3D_EXTRA);

		for (int i = 0; i < NUM_ALGAS3D_EXTRA; ++i) {
			float x, z;
			float y = 0.0f; // y = 0

			// Generar una posición fuera del área x,z ∈ [-60,60]
			// Usamos un rango más grande y rechazamos las que caen en el cuadrado central.
			while (true) {
				float rx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float rz = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				// Rango amplio, por ejemplo [-150, 150]
				x = -150.0f + rx * 300.0f; // [-150, 150]
				z = -150.0f + rz * 300.0f; // [-150, 150]

				// Condición: fuera del cuadrado [-60,60] x [-60,60]
				if (std::abs(x) > 60.0f || std::abs(z) > 60.0f) {
					break;
				}
			}

			gAlgas3DExtra.emplace_back(x, y, z);
		}
	}



	//glGenTextures(1, &textTexture);
	//glBindTexture(GL_TEXTURE_2D, textTexture);

	//// Configura parámetros de textura
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//// Carga la imagen
	//int width, height, nrChannels;
	//unsigned char* data = stbi_load("textures/pressR.png", &width, &height, &nrChannels, 0);
	//if (data) {
	//	GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
	//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//	std::cout << "Se cargó pressR.png con tamaño: " << width << "x" << height << std::endl;
	//}
	//else {
	//	std::cout << "Error al cargar pressR.png" << std::endl;
	//}

	//stbi_image_free(data);
	loadTexture("textures/camera.png", texCamera);
	loadTexture("textures/wasd.png", texWASD);
	loadTexture("textures/basura.png", texTrash);
	loadTexture("textures/pressR.png", texPressR);

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenQuad), fullscreenQuad, GL_STATIC_DRAW);

	// posición
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	// tex coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);


	return true;
}
//final Start

bool ismoving(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS or 
		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS or
		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS or
		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		return true;
	}
	else {
		return false;
	}
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



void prepareTrash(Model* object, glm::vec3 translate, float rotatex, glm::vec3 scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, translate); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(rotatex), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, scale);	// it's a bit too big for our scene, so scale it down
	mLightsShader->setMat4("model", model);

	//Luces
	
	std::vector<Light> Lights;
	if (submarino) {
		Lights = gLights;
	}
	else {
		Lights = Lights_playa;
	}
	for (size_t i = 0; i < Lights.size(); ++i) {
		SetLightUniformVec3(mLightsShader, "Position", i, Lights[i].Position);
		SetLightUniformVec3(mLightsShader, "Direction", i, Lights[i].Direction);
		SetLightUniformVec4(mLightsShader, "Color", i, Lights[i].Color);
		SetLightUniformVec4(mLightsShader, "Power", i, Lights[i].Power);
		SetLightUniformInt(mLightsShader, "alphaIndex", i, object->material.alphaIndex);
		SetLightUniformFloat(mLightsShader, "distance", i, Lights[i].distance);
	}

	// Aplicamos propiedades materiales
	mLightsShader->setVec4("MaterialAmbientColor", object->material.ambient);
	mLightsShader->setVec4("MaterialDiffuseColor", object->material.diffuse);
	mLightsShader->setVec4("MaterialSpecularColor", object->material.specular);
	mLightsShader->setFloat("transparency", object->material.transparency);

}

bool Intro() {
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	elapsedTime += deltaTime;

	processInput(window);

	// Renderizado R - G - B - A
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection;
	glm::mat4 view;
	projection = glm::perspective(glm::radians(camera.Zoom),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	view = camera.GetViewMatrix();


	if (elapsedTime < 1.0f) {
		textTexture = texWASD;
	}
	else if (elapsedTime < 2.0f) {
		textTexture = texCamera;
	}
	else if (elapsedTime < 3.0f) {
		textTexture = texTrash;
	}
	else {
		return false;
		elapsedTime = 0.0f;
	}

	
	// Mostrar imagen de texto como overlay en pantalla completa
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fullscreenShader->use();
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textTexture);
	fullscreenShader->setInt("screenTexture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glDisable(GL_BLEND);
	glUseProgram(0);

	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
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

		
		if (submarino) {
			// Ajuste: posición un poco más abajo y enfrente de la cámara
			glm::vec3 offset = glm::vec3(0.0f, -0.5f, 0.0f); // bajamos Y a 0.3, enfrente 0.5
			model = glm::translate(model, position + offset);

			// Ajuste: rotar hacia donde mira la cámara, pero mostrando la espalda
			float angle = atan2(forwardView.x, forwardView.z);
			model = glm::rotate(model, glm::radians(rotateCharacter + 180.0f), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::rotate(model, angle + glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			// Mantener la escala que ya tenías
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, diver->gBones);

			diver->UpdateAnimation(deltaTime);
			diver->Draw(*dynamicShader);
			glUseProgram(0);
		}
		else {
			// Ajuste: posición un poco más abajo y enfrente de la cámara
			glm::vec3 offset = glm::vec3(0.0f, -0.5f, 0.0f); // bajamos Y a 0.3, enfrente 0.5
			model = glm::translate(model, position + offset);

			// Ajuste: rotar hacia donde mira la cámara, pero mostrando la espalda
			float angle = atan2(forwardView.x, forwardView.z);
			model = glm::rotate(model, glm::radians(rotateCharacter + 180.0f), glm::vec3(0.0, 1.0f, 0.0f));
			model = glm::rotate(model, angle + glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			// Mantener la escala que ya tenías
			model = glm::scale(model, glm::vec3(0.0065f, 0.0065f, 0.0065f));

			dynamicShader->setMat4("model", model);

			dynamicShader->setFloat("waterLevel", 0.0f); // adjust if needed
			dynamicShader->setFloat("fogDensity", 0.00f);
			dynamicShader->setFloat("depthAttenuation", 0.0f);
			dynamicShader->setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
			dynamicShader->setFloat("caustic_intensity", 0.0f);

			//std::cout << "mamada 1" << std::endl;
			if (ismoving(window)) {
				//std::cout << "mamada 2" << std::endl;
				dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, diver_walk->gBones);
				diver_walk->UpdateAnimation(deltaTime);
				diver_walk->Draw(*dynamicShader);
			}
			else {
				//std::cout << "mamada 3" << std::endl;
				dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, diver_idle->gBones);
				diver_idle->UpdateAnimation(deltaTime);
				diver_idle->Draw(*dynamicShader);
			}
			
			glUseProgram(0);
		}
		

	}

	
	
	//--------------------ESCENA AGUA---------------------
	if (submarino) {
		// Cubemap (fondo)
		{
			mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
		}
		bounding_boxes = bounding_boxes_agua;
		//bounding_boxes->aabbs = bounding_boxes_agua->aabbs;
		{
			


			mLightsShader->use();

			// Activamos para objetos transparentes
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			mLightsShader->setMat4("projection", projection);
			mLightsShader->setMat4("view", view);

			glm::vec3 translate_temp;
			float rotatex_temp;
			glm::vec3 scale_temp;

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
			mLightsShader->setFloat("depthAttenuation", 0.0f);
			mLightsShader->setVec3("fogColor", glm::vec3(0.0f, 0.25f, 0.45f));
			mLightsShader->setFloat("caustic_intensity", 1.0f);



			terrain->Draw(*mLightsShader);
			decor->Draw(*mLightsShader);
			if(draw_colliders)
				bounding_boxes->Draw(*mLightsShader);
			//boat->Draw(*mLightsShader);

			// ================== DIBUJAR CARTELES ESTATICOS ==================
			{
				glm::mat4 modelCartel;
				float scale = 0.10f; // Puse esta escala por quenlos modelos son muy grandes.

				// --- Cartel 1
				modelCartel = glm::mat4(1.0f);
				modelCartel = glm::translate(modelCartel, glm::vec3(6.0f, 0.0f, 7.5f));
				modelCartel = glm::rotate(modelCartel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelCartel = glm::scale(modelCartel, glm::vec3(scale));
				mLightsShader->setMat4("model", modelCartel);

				cartelMar1->Draw(*mLightsShader);

				// --- Cartel Mapa
				modelCartel = glm::mat4(1.0f);
				modelCartel = glm::translate(modelCartel, glm::vec3(6.0f, 0.0f, 10.0f));
				modelCartel = glm::rotate(modelCartel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelCartel = glm::scale(modelCartel, glm::vec3(scale));
				mLightsShader->setMat4("model", modelCartel);

				cartelMarmapa->Draw(*mLightsShader);

				float alturaPlaya = 0.35f;
				float rotacionPlaya = 90.0f; // Rotación de 90 grados para que miren en la dirección que quieres

				// --- Cartel Playa 1
				modelCartel = glm::mat4(1.0f);
				// Cambiando Z de 5.0f a 20.0f para que esté enfrente de Z=10.0f
				modelCartel = glm::translate(modelCartel, glm::vec3(0.0f, alturaPlaya, 20.0f));
				modelCartel = glm::rotate(modelCartel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelCartel = glm::rotate(modelCartel, glm::radians(rotacionPlaya), glm::vec3(0.0f, 0.0f, 1.0f));
				modelCartel = glm::scale(modelCartel, glm::vec3(10.0f, 10.0f, 10.0f));
				mLightsShader->setMat4("model", modelCartel);
				cartelPlaya1->Draw(*mLightsShader);

				// --- Cartel Playa Mapa
				modelCartel = glm::mat4(1.0f);
				// Cambiando Z de 5.0f a 20.0f
				modelCartel = glm::translate(modelCartel, glm::vec3(5.0f, alturaPlaya, 20.0f));
				modelCartel = glm::rotate(modelCartel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				modelCartel = glm::rotate(modelCartel, glm::radians(rotacionPlaya), glm::vec3(0.0f, 0.0f, 1.0f));
				modelCartel = glm::scale(modelCartel, glm::vec3(scale));
				mLightsShader->setMat4("model", modelCartel);
				cartelPlayamapa->Draw(*mLightsShader);

			}

			// ================== DIBUJAR TODA LA BASURA ==================
			{
				for (TrashInstance& t : gTrash) {
					if (!t.active)
						continue; // ya fue recogida

					// Configuramos transformaciones y materiales base
					prepareTrash(t.model, t.position, t.rotateX, t.scale);

					bool isNear = nearTrash(camera.Position, t.position);

					if (isNear) {
						// Pintar en blanco
						mLightsShader->setVec4("MaterialAmbientColor", WHITE);
						mLightsShader->setVec4("MaterialDiffuseColor", WHITE);
						mLightsShader->setVec4("MaterialSpecularColor", WHITE);
						mLightsShader->setFloat("transparency", 1.0f);

						// Si se presiona R, "recogemos" este objeto
						if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
							t.active = false;
							continue; // no lo dibujes este frame
						}
					}
					// Dibujar el modelo si sigue activo
					t.model->Draw(*mLightsShader);
				}
				// ================== DIBUJAR FLECHAS SOBRE BASURA ACTIVA ==================
				for (const auto& t : gTrash) {
					if (!t.active) continue; // solo basura que aún no se recoge

					glm::vec3 arrowPos = t.position + glm::vec3(0.0f, 3.0f, 0.0f); // y = +3 por encima

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, arrowPos);

					// Orientación de la flecha: ejemplo, apuntando hacia abajo
					// (ajusta el eje/ángulo según cómo venga el modelo Flecha.fbx)
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

					// Escala de la flecha (ajusta a ojo)
					model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));

					mLightsShader->setMat4("model", model);

					// Usamos el mismo material que estrella/erizo/basura
					mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
					mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
					mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
					mLightsShader->setFloat("transparency", material01.transparency);

					flecha->Draw(*mLightsShader);
				}

			}


			// ================= DIBUJAR VARIAS ESTRELLAS ===========================
			{
				for (const glm::vec3& pos : gEstrellasExtra) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, pos);
					// misma orientación que la estrella original (acostada sobre el piso)
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

					mLightsShader->setMat4("model", model);
					mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
					mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
					mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
					mLightsShader->setFloat("transparency", material01.transparency);

					estrella->Draw(*mLightsShader);
				}
			}
			//glUseProgram(0);

			// ================= DIBUJAR VARIOS ERIZOS ===========================
			{
				for (const glm::vec3& pos : gErizos) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, pos);
					model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

					mLightsShader->setMat4("model", model);
					mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
					mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
					mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
					mLightsShader->setFloat("transparency", material01.transparency);

					erizo->Draw(*mLightsShader);
				}
			}

			// ================== ALGAS 3D EXTRA ==================
			{
				// mLightsShader->use();
				// mLightsShader->setMat4("projection", projection);
				// mLightsShader->setMat4("view", view);

				for (const glm::vec3& pos : gAlgas3DExtra) {
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, pos);
					 model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1,0,0));
					model = glm::scale(model, glm::vec3(2.0f)); // cambia si la ves muy grande/pequeña

					mLightsShader->setMat4("model", model);

					// Mismo material que estrella/erizo:
					mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
					mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
					mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
					mLightsShader->setFloat("transparency", material01.transparency);

					alga3d->Draw(*mLightsShader);
				}
			}


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

			for (size_t i = 0; i < gLights.size(); ++i) {
				SetLightUniformVec3(proceduralShader, "Position", i, gLights[i].Position);
				SetLightUniformVec3(proceduralShader, "Direction", i, gLights[i].Direction);
				SetLightUniformVec4(proceduralShader, "Color", i, gLights[i].Color);
				SetLightUniformVec4(proceduralShader, "Power", i, gLights[i].Power);
				SetLightUniformInt(proceduralShader, "alphaIndex", i, gLights[i].alphaIndex);
				SetLightUniformFloat(proceduralShader, "distance", i, gLights[i].distance);
			}

			proceduralShader->setInt("numLights", (int)gLights.size());

			proceduralShader->setVec4("MaterialAmbientColor", material01.ambient);
			proceduralShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			proceduralShader->setVec4("MaterialSpecularColor", material01.specular);
			proceduralShader->setFloat("transparency", material01.transparency);

			proceduralShader->setVec3("cameraPos", camera.Position);
			proceduralShader->setFloat("waterLevel", 0.0f);
			proceduralShader->setFloat("fogDensity", 0.005f);
			proceduralShader->setFloat("depthAttenuation", 0.0f);
			proceduralShader->setVec3("fogColor", glm::vec3(0.0f, 0.25f, 0.45f));
			proceduralShader->setFloat("caustic_intensity", 1.0f);

			// ================== TORTUGAS EN TRAYECTORIA TIPO TRÉBOL ==================
			{
				proceduralShader->use();
				proceduralShader->setMat4("projection", projection);
				proceduralShader->setMat4("view", view);

				// Luces, material, agua, fog, etc. (esto ya lo tienes configurado igual que antes)

				// IMPORTANTE: DESACTIVAR la animación de vértices tipo trébol
				proceduralShader->setInt("useTrefoil", 0);
				proceduralShader->setFloat("time", 0.0f);
				proceduralShader->setFloat("radius", 0.0f);
				proceduralShader->setFloat("height", 0.0f);

				for (int i = 0; i < NUM_TORTUGAS; ++i) {
					TortugaPath& T = gTortugas[i];

					// Avanzar el tiempo de cada tortuga
					T.time += deltaTime * T.speed;

					// t con dirección y fase
					float t = T.directionSign * T.time + T.phase;

					// Posición actual y siguiente sobre la curva de trébol (CPU)
					glm::vec3 posNow = trebol(T.center, t, T.a, T.b);
					glm::vec3 posNext = trebol(T.center, t + 0.05f * T.directionSign,
						T.a, T.b);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					// Orientación según la trayectoria
					glm::mat4 R = orientAlongPath(posNow, posNext);

					// Combinamos orientación del camino + corrección fija de la tortuga
					model *= R * turtleCorrection;

					// Escala del modelo de tortuga
					model = glm::scale(model, glm::vec3(1.0f));

					proceduralShader->setMat4("model", model);
					tortuga->Draw(*proceduralShader);  // Modelo estático, sin gBones
				}
			}

		}

		glUseProgram(0);


		// ================================================ DIBUJAR OBJETOS ANIMADOS =========================================

		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

			dynamicShader->setInt("numLights", (int)gLights.size());
			for (size_t i = 0; i < gLights.size(); ++i) {
				SetLightUniformVec3(dynamicShader, "Position", i, gLights[i].Position);
				SetLightUniformVec3(dynamicShader, "Direction", i, gLights[i].Direction);
				SetLightUniformVec4(dynamicShader, "Color", i, gLights[i].Color);
				SetLightUniformVec4(dynamicShader, "Power", i, gLights[i].Power);
				SetLightUniformInt(dynamicShader, "alphaIndex", i, gLights[i].alphaIndex);
				SetLightUniformFloat(dynamicShader, "distance", i, gLights[i].distance);
			}

			mLightsShader->setVec3("eye", camera.Position);

			// Aplicamos propiedades materiales
			dynamicShader->setVec4("MaterialAmbientColor", material01.ambient);
			dynamicShader->setVec4("MaterialDiffuseColor", material01.diffuse);
			dynamicShader->setVec4("MaterialSpecularColor", material01.specular);
			dynamicShader->setFloat("transparency", material01.transparency);

			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character01->gBones);

			// parametros para efecto de agua
			dynamicShader->setVec3("cameraPos", camera.Position);
			dynamicShader->setFloat("time", glfwGetTime());
			dynamicShader->setFloat("waterLevel", 0.0f); // adjust if needed
			dynamicShader->setFloat("fogDensity", 0.03f);
			dynamicShader->setFloat("depthAttenuation", 0.0f);
			dynamicShader->setVec3("fogColor", glm::vec3(0.0f, 0.25f, 0.45f));
			dynamicShader->setFloat("caustic_intensity", 1.0f);		

			// ================== PECES EN ANILLOS SINUSOIDALES ===========================
			{
				pez->UpdateAnimation(deltaTime);
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				// Ya estan estableciadas las luces, material,  waterLevel, fog, etc. para dynamicShader)

				for (int i = 0; i < NUM_PECES; ++i) {
					FishPath& F = gFishes[i];

					// Avanzar el tiempo de cada pez
					F.time += deltaTime * F.speed;

					// t con dirección y fase
					float t = F.directionSign * F.time + F.phase;

					// Posición actual y siguiente (para dirección)
					glm::vec3 posNow = anilloSinusoidal(F.center, t, F.radius, F.amplitude, F.n);
					glm::vec3 posNext = anilloSinusoidal(F.center, t + 0.05f * F.directionSign,
						F.radius, F.amplitude, F.n);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					// Orientar para que mire en la dirección de su movimiento
					glm::mat4 R = orientAlongPath(posNow, posNext);
					model *= R;

					// Corregir orientación del pez (gira 180° sobre Y)
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

					// Escala del pez (ajusta si está muy grande o pequeño)
					model = glm::scale(model, glm::vec3(0.002f)); // ejemplo

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, pez->gBones);
					pez->Draw(*dynamicShader);
				}
			}

			// ================== MEDUSAS EN ANILLOS SINUSOIDALES ==================
			{
				medusa->UpdateAnimation(deltaTime);
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				// Ya estan estableciadas las luces, material,  waterLevel, fog, etc. para dynamicShader)

				for (int i = 0; i < NUM_MEDUSAS; ++i) {
					MedusaPath& M = gMedusas[i];

					// Avanzar el tiempo de cada medusa
					M.time += deltaTime * M.speed;

					// t con dirección y fase
					float t = M.directionSign * M.time + M.phase;

					// Posición actual y siguiente en el anillo
					glm::vec3 posNow = anilloSinusoidal(M.center, t,
						M.radius, M.amplitude, M.n);
					glm::vec3 posNext = anilloSinusoidal(M.center,
						t + 0.05f * M.directionSign,
						M.radius, M.amplitude, M.n);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					// Orientación según la trayectoria
					glm::mat4 R = orientAlongPath(posNow, posNext);
					model *= R;

					// Escala (ajusta según tu modelo)
					model = glm::scale(model, glm::vec3(0.002f)); // ejemplo

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, medusa->gBones);
					medusa->Draw(*dynamicShader);
				}

			}

			// ================== PULPOS EN ANILLOS SINUSOIDALES ==================
			{
				pulpo->UpdateAnimation(deltaTime);
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				// Ya estan estableciadas las luces, material,  waterLevel, fog, etc. para dynamicShader)

				for (int i = 0; i < NUM_PULPOS; ++i) {
					PulpoPath& P = gPulpos[i];

					// Avanzar el tiempo de cada pulpo
					P.time += deltaTime * P.speed;

					// t con dirección y fase
					float t = P.directionSign * P.time + P.phase;

					// Posición actual y siguiente en la trayectoria
					glm::vec3 posNow = anilloSinusoidal(P.center, t,
						P.radius, P.amplitude, P.n);
					glm::vec3 posNext = anilloSinusoidal(P.center,
						t + 0.05f * P.directionSign,
						P.radius, P.amplitude, P.n);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					// Orientación para que mire en la dirección de su movimiento
					glm::mat4 R = orientAlongPath(posNow, posNext);
					model *= R;

					// Escala del pulpo (ajusta a tu escena)
					model = glm::scale(model, glm::vec3(0.003f)); 

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, pulpo->gBones);
					pulpo->Draw(*dynamicShader);
				}
			}

			// ================== CALAMARES EN ANILLOS SINUSOIDALES ==================
			{
				calamar->UpdateAnimation(deltaTime);
				// ================== CALAMARES EN ANILLOS SINUSOIDALES ==================
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				// Ya estan estableciadas las luces, material,  waterLevel, fog, etc. para dynamicShader)

				for (int i = 0; i < NUM_CALAMARES; ++i) {
					CalamarPath& C = gCalamares[i];

					// Avanzar el tiempo (son más rápidos)
					C.time += deltaTime * C.speed;

					// t con dirección y fase
					float t = C.directionSign * C.time + C.phase;

					// Posición actual y siguiente
					glm::vec3 posNow = anilloSinusoidal(C.center, t,
						C.radius, C.amplitude, C.n);
					glm::vec3 posNext = anilloSinusoidal(C.center,
						t + 0.05f * C.directionSign,
						C.radius, C.amplitude, C.n);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					glm::mat4 R = orientAlongPath(posNow, posNext);
					model *= R;

					// 1) Acostar al calamar (rotar sobre X)
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

					model = glm::scale(model, glm::vec3(0.003f));

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, calamar->gBones);
					calamar->Draw(*dynamicShader);
				}
			}

			// ================== MANTARAYAS EN ANILLOS SINUSOIDALES ==================
			{
				mantaraya->UpdateAnimation(deltaTime);
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				// Ya estan estableciadas las luces, material,  waterLevel, fog, etc. para dynamicShader)

				for (int i = 0; i < NUM_MANTARAYAS; ++i) {
					MantarayaPath& M = gMantarayas[i];

					// Avanzar el tiempo de cada mantarraya
					M.time += deltaTime * M.speed;

					// t con dirección y fase
					float t = M.directionSign * M.time + M.phase;

					// Posición actual y siguiente en el anillo
					glm::vec3 posNow = anilloSinusoidal(M.center, t,
						M.radius, M.amplitude, M.n);
					glm::vec3 posNext = anilloSinusoidal(M.center,
						t + 0.05f * M.directionSign,
						M.radius, M.amplitude, M.n);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					glm::mat4 R = orientAlongPath(posNow, posNext);
					model *= R;

					//Girar 180° en Y para que la cabeza apunte en la dirección de avance
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

					// Escala grande para que se vea
					model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, mantaraya->gBones);
					mantaraya->Draw(*dynamicShader);
				}
			}
			// ================== CABALLITOS EN ANILLOS SINUSOIDALES ==================
			{
				caballito->UpdateAnimation(deltaTime);
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				// Ya estan estableciadas las luces, material,  waterLevel, fog, etc. para dynamicShader)

				for (int i = 0; i < NUM_CABALLITOS; ++i) {
					CaballitoPath& C = gCaballitos[i];

					// Avanzar el tiempo de cada caballito
					C.time += deltaTime * C.speed * 0.5f; // aún más lento

					// t con dirección y fase
					float t = C.directionSign * C.time + C.phase;

					// Posición actual y siguiente en la curva
					glm::vec3 posNow = anilloSinusoidal(C.center, t,
						C.radius, C.amplitude, C.n);
					glm::vec3 posNext = anilloSinusoidal(C.center,
						t + 0.05f * C.directionSign,
						C.radius, C.amplitude, C.n);

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);

					// Orientación para que mire en la dirección de su movimiento
					glm::mat4 R = orientAlongPath(posNow, posNext);
					model *= R;

					// gira 90° en Y después de orientarlo:
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

					// Escala del caballito 
					model = glm::scale(model, glm::vec3(1.05f, 1.0f, 1.05f));

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, caballito->gBones);
					caballito->Draw(*dynamicShader);
				}
			}

			// ===== DELFINES EN ANILLO SINUSOIDAL ===================================================
			{
				delfin->UpdateAnimation(deltaTime);
				static float delfinTime = 0.0f;
				delfinTime += deltaTime * 0.5f;   // velocidad de recorrido sobre la curva
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);
				// -------- Delfín 1: sentido "normal" --------
				{
					static float delfinTime = 0.0f;
					delfinTime += deltaTime * 0.5f;

					glm::vec3 centroDelfin1 = glm::vec3(-10.0f, 5.0f, -10.0f);
					float radius1 = 8.0f;
					float amplitude1 = 2.0f;
					float n1 = 2.0f;

					float t1 = delfinTime;

					glm::vec3 posNow1 = anilloSinusoidal(centroDelfin1, t1, radius1, amplitude1, n1);
					glm::vec3 posNext1 = anilloSinusoidal(centroDelfin1, t1 + 0.05f, radius1, amplitude1, n1);

					glm::mat4 model1 = glm::mat4(1.0f);
					model1 = glm::translate(model1, posNow1);

					glm::mat4 R1 = orientAlongPath(posNow1, posNext1);
					model1 *= R1;

					model1 = glm::scale(model1, glm::vec3(0.001f));
					dynamicShader->setMat4("model", model1);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin->gBones);
					delfin->Draw(*dynamicShader);
				}
				// -------- Delfín 2: mismo recorrido pero al revés --------
				{
					glm::vec3 centroDelfin2 = glm::vec3(-10.0f, 7.5f, -10.0f); // un poco más alto
					float radius2 = 10.0f;  // anillo más grande
					float amplitude2 = 1.5f;   // un poco menos de oscilación vertical
					float n2 = 2.0f;

					float t2 = -delfinTime;    // recorre la curva al revés

					glm::vec3 posNow2 = anilloSinusoidal(centroDelfin2, t2, radius2, amplitude2, n2);
					glm::vec3 posNext2 = anilloSinusoidal(centroDelfin2, t2 - 0.05f, radius2, amplitude2, n2);
					//       ojo: t2 - 0.05f para que mire en la dirección de su movimiento inverso

					glm::mat4 model2 = glm::mat4(1.0f);
					model2 = glm::translate(model2, posNow2);

					glm::mat4 R2 = orientAlongPath(posNow2, posNext2);
					model2 *= R2;

					model2 = glm::scale(model2, glm::vec3(0.001f));
					dynamicShader->setMat4("model", model2);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin->gBones);
					delfin->Draw(*dynamicShader);
				}
				// -------- Delfín 3 --------
				{
					glm::vec3 centroDelfin2 = glm::vec3(0.0f, 10.0f, 30.0f); // un poco más alto
					float radius2 = 10.0f;  // anillo más grande
					float amplitude2 = 1.5f;   // un poco menos de oscilación vertical
					float n2 = 2.0f;

					float t2 = -delfinTime;    // recorre la curva al revés

					glm::vec3 posNow2 = anilloSinusoidal(centroDelfin2, t2, radius2, amplitude2, n2);
					glm::vec3 posNext2 = anilloSinusoidal(centroDelfin2, t2 - 0.05f, radius2, amplitude2, n2);
					//                                ^ ojo: t2 - 0.05f para que mire en la dirección de su movimiento inverso

					glm::mat4 model2 = glm::mat4(1.0f);
					model2 = glm::translate(model2, posNow2);

					glm::mat4 R2 = orientAlongPath(posNow2, posNext2);
					model2 *= R2;

					model2 = glm::scale(model2, glm::vec3(0.001f));
					dynamicShader->setMat4("model", model2);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin->gBones);
					delfin->Draw(*dynamicShader);
				}
				// -------- Delfín 4 --------
				{
					static float delfinTime = 0.0f;
					delfinTime += deltaTime * 0.5f;

					glm::vec3 centroDelfin1 = glm::vec3(0.0f, 8.0f, 30.0f);
					float radius1 = 8.0f;
					float amplitude1 = 2.0f;
					float n1 = 2.0f;

					float t1 = delfinTime;

					glm::vec3 posNow1 = anilloSinusoidal(centroDelfin1, t1, radius1, amplitude1, n1);
					glm::vec3 posNext1 = anilloSinusoidal(centroDelfin1, t1 + 0.05f, radius1, amplitude1, n1);

					glm::mat4 model1 = glm::mat4(1.0f);
					model1 = glm::translate(model1, posNow1);

					glm::mat4 R1 = orientAlongPath(posNow1, posNext1);
					model1 *= R1;

					model1 = glm::scale(model1, glm::vec3(0.001f));
					dynamicShader->setMat4("model", model1);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin->gBones);
					delfin->Draw(*dynamicShader);
				}
				// ===== DELFINES VUELTA EN ANILLO SINUSOIDAL =====
				delfin2->UpdateAnimation(deltaTime);
				static float delfinTime2 = 0.0f;
				delfinTime2 += deltaTime * 0.3f;   // velocidad de recorrido sobre la curva
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);
				// -------- Delfín 1: sentido "normal" --------
				{
					static float delfinTime2 = 0.0f;
					delfinTime2 += deltaTime * 0.5f;

					glm::vec3 centroDelfin1 = glm::vec3(20.0f, 10.0f, -5.0f);
					float radius1 = 6.0f;
					float amplitude1 = 1.0f;
					float n1 = 1.0f;

					float t1 = delfinTime2;

					glm::vec3 posNow1 = anilloSinusoidal(centroDelfin1, t1, radius1, amplitude1, n1);
					glm::vec3 posNext1 = anilloSinusoidal(centroDelfin1, t1 + 0.05f, radius1, amplitude1, n1);

					glm::mat4 model1 = glm::mat4(1.0f);
					model1 = glm::translate(model1, posNow1);

					glm::mat4 R1 = orientAlongPath(posNow1, posNext1);
					model1 *= R1;

					model1 = glm::scale(model1, glm::vec3(0.001f));
					dynamicShader->setMat4("model", model1);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin2->gBones);
					delfin2->Draw(*dynamicShader);
				}
				// -------- Delfín 2: mismo recorrido pero al revés --------
				{
					glm::vec3 centroDelfin2 = glm::vec3(20.0f, 8.0f, -5.0f); // un poco más alto
					float radius2 = 9.0f;  // anillo más grande
					float amplitude2 = 1.0f;   // un poco menos de oscilación vertical
					float n2 = 1.0f;

					float t2 = -delfinTime2;    // recorre la curva al revés

					glm::vec3 posNow2 = anilloSinusoidal(centroDelfin2, t2, radius2, amplitude2, n2);
					glm::vec3 posNext2 = anilloSinusoidal(centroDelfin2, t2 - 0.05f, radius2, amplitude2, n2);
					//                                ^ ojo: t2 - 0.05f para que mire en la dirección de su movimiento inverso

					glm::mat4 model2 = glm::mat4(1.0f);
					model2 = glm::translate(model2, posNow2);

					glm::mat4 R2 = orientAlongPath(posNow2, posNext2);
					model2 *= R2;

					model2 = glm::scale(model2, glm::vec3(0.001f));
					dynamicShader->setMat4("model", model2);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, delfin2->gBones);
					delfin2->Draw(*dynamicShader);
				}
			}
		
			// ================== CANGREJOS CAMINANDO DE LADO A LADO ==================
			{
				cangrejo->UpdateAnimation(deltaTime);
				dynamicShader->use();
				dynamicShader->setMat4("projection", projection);
				dynamicShader->setMat4("view", view);

				for (int i = 0; i < NUM_CANGREJOS; ++i) {
					CangrejoPath& C = gCangrejos[i];

					// Avanzar tiempo
					C.time += deltaTime * C.speed;
					float t = C.time + C.phase;

					// Movimiento de ida y vuelta
					float factor = sinf(t);                  // [-1,1]
					glm::vec3 offset = C.dir * (factor * C.length);

					glm::vec3 posNow = C.center + offset;

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, posNow);
					model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));

					// Escala
					model = glm::scale(model, glm::vec3(0.008f));

					dynamicShader->setMat4("model", model);
					dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, cangrejo->gBones);
					cangrejo->Draw(*dynamicShader);
				}
			}


			// ================== DIBUJAR ALGAS 2D con iluminación =====================
			{
				algaShader->use();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				// Matrices de proyección y vista (iguales para todas)
				algaShader->setMat4("projection", projection);
				algaShader->setMat4("view", view);

				// --------- Iluminación (igual que con dynamicShader) ----------
				algaShader->setInt("numLights", (int)gLights.size());
				for (size_t i = 0; i < gLights.size(); ++i) {
					SetLightUniformVec3(algaShader, "Position", i, gLights[i].Position);
					SetLightUniformVec3(algaShader, "Direction", i, gLights[i].Direction);
					SetLightUniformVec4(algaShader, "Color", i, gLights[i].Color);
					SetLightUniformVec4(algaShader, "Power", i, gLights[i].Power);
					SetLightUniformInt(algaShader, "alphaIndex", i, gLights[i].alphaIndex);
					SetLightUniformFloat(algaShader, "distance", i, gLights[i].distance);
				}

				// Material
				algaShader->setVec4("MaterialAmbientColor", material01.ambient);
				algaShader->setVec4("MaterialDiffuseColor", material01.diffuse);
				algaShader->setVec4("MaterialSpecularColor", material01.specular);
				algaShader->setFloat("transparency", material01.transparency);

				// Agua / niebla
				algaShader->setVec3("cameraPos", camera.Position);
				algaShader->setFloat("waterLevel", 0.0f);
				algaShader->setFloat("fogDensity", 0.03f);
				algaShader->setFloat("depthAttenuation", 0.0f);
				algaShader->setVec3("fogColor", glm::vec3(0.0f, 0.25f, 0.45f));
				algaShader->setFloat("caustic_intensity", 1.0f);

				// Tiempo para la animación de las algas
				wavesTime += 0.05f;
				algaShader->setFloat("time", wavesTime);

				// --------- Dibujar las 50 algas ---------
				for (int i = 0; i < NUM_ALGAS; ++i)
				{
					glm::mat4 modelAlga = glm::mat4(1.0f);

					// Posición en el piso dentro del cuadrado [-60,60]x[-60,60]
					modelAlga = glm::translate(modelAlga, algaPositions[i]);

					// Poner el plano “de pie”, igual que antes
					modelAlga = glm::rotate(modelAlga,
						glm::radians(-90.0f),
						glm::vec3(1.0f, 0.0f, 0.0f));

					// Escala (ajusta si las ves muy grandes/pequeñas)
					modelAlga = glm::scale(modelAlga, glm::vec3(0.6f));

					algaShader->setMat4("model", modelAlga);
					algaMesh->Draw(*algaShader);
				}
			}
			
			//  ================ DIBUJAR BURBUJAS =========================================
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

			proceduralTime += 0.0001;
		}

		textTexture = texPressR;
		// Mostrar imagen de texto como overlay en pantalla completa
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		fullscreenShader->use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textTexture);
		fullscreenShader->setInt("screenTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);


		PlaySceneMusic("sounds/SusurrosdelMar.mp3"); // música submarina

	}
	//--------------------ESCENA PLAYA------------------------
	else {
		{
			beachCubeMap->drawCubeMap(*cubemapShader, projection, view);
		}
		//pruebas fresnel
		{
			// Activamos el shader de Phong
			fresnelShader->use();

			// Activamos para objetos transparentes
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Aplicamos transformaciones de proyección y cámara (si las hubiera)
			fresnelShader->setMat4("projection", projection);
			fresnelShader->setMat4("view", view);

			// Aplicamos transformaciones del modelo
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			fresnelShader->setMat4("model", model);

			fresnelShader->setVec3("cameraPosition", camera.Position);
			fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.333f); // keep
			fresnelShader->setFloat("_Bias", 0.02f);   // less base reflection
			fresnelShader->setFloat("_Scale", 0.7f);   // strong fresnel edge
			fresnelShader->setFloat("_Power", 5.0f);   // fast falloff
			fresnelShader->setFloat("time", wavesTime);
			wavesTime += 0.01;

			gridMesh->Draw(*fresnelShader);


		}

		glUseProgram(0);

		bounding_boxes = bounding_boxes_playa;
		//bounding_boxes->aabbs = bounding_boxes_playa->aabbs;
		mLightsShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mLightsShader->setMat4("projection", projection);
		mLightsShader->setMat4("view", view);

		glm::vec3 translate_temp;
		float rotatex_temp;
		glm::vec3 scale_temp;

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		mLightsShader->setMat4("model", model);

		// Configuramos propiedades de fuentes de luz
		mLightsShader->setInt("numLights", (int)Lights_playa.size());
		for (size_t i = 0; i < Lights_playa.size(); ++i) {
			SetLightUniformVec3(mLightsShader, "Position", i, Lights_playa[i].Position);
			SetLightUniformVec3(mLightsShader, "Direction", i, Lights_playa[i].Direction);
			SetLightUniformVec4(mLightsShader, "Color", i, Lights_playa[i].Color);
			SetLightUniformVec4(mLightsShader, "Power", i, Lights_playa[i].Power);
			SetLightUniformInt(mLightsShader, "alphaIndex", i, 20);
			SetLightUniformFloat(mLightsShader, "distance", i, Lights_playa[i].distance);
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

		//PAREMETROS AGUA, PONER EN 0 PARA ESCENAS EN LA PLAYA
		mLightsShader->setFloat("waterLevel", 0.0f); // adjust if needed
		mLightsShader->setFloat("fogDensity", 0.00f);
		mLightsShader->setFloat("depthAttenuation", 0.0f);
		mLightsShader->setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f)); 
		mLightsShader->setFloat("caustic_intensity", 0.0f);

		beach_terrain->Draw(*mLightsShader);
		if (draw_colliders)
			bounding_boxes->Draw(*mLightsShader);

		// ================== DIBUJAR BASURA ALEATORIA EN LA PLAYA ==================
		{
			for (TrashInstance& t2 : gTrashBeach) {
				if (!t2.active)
					continue; // ya fue recogida

				// Transformaciones + material original
				prepareTrash(t2.model, t2.position, t2.rotateX, t2.scale);

				bool isNear = nearTrash(camera.Position, t2.position);

				if (isNear) {
					// Pintar en blanco si estamos cerca
					mLightsShader->setVec4("MaterialAmbientColor", WHITE);
					mLightsShader->setVec4("MaterialDiffuseColor", WHITE);
					mLightsShader->setVec4("MaterialSpecularColor", WHITE);
					mLightsShader->setFloat("transparency", 1.0f);

					// Si se presiona R, "recogemos" este objeto
					if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
						t2.active = false;
						continue; // ya no lo dibuja
					}
				}
				// Dibujar el modelo si sigue activo
				t2.model->Draw(*mLightsShader);

				// ================== DIBUJAR FLECHAS SOBRE BASURA ACTIVA ==================
				for (const auto& t : gTrash) {
					if (!t2.active) continue; // solo basura que aún no se recoge

					glm::vec3 arrowPos = t2.position + glm::vec3(0.0f, 2.0f, 0.0f); // y = +3 por encima

					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, arrowPos);

					// Orientación de la flecha: ejemplo, apuntando hacia abajo
					// (ajusta el eje/ángulo según cómo venga el modelo Flecha.fbx)
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

					// Escala de la flecha (ajusta a ojo)
					model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));

					mLightsShader->setMat4("model", model);

					// Usamos el mismo material que estrella/erizo/basura
					mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
					mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
					mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
					mLightsShader->setFloat("transparency", material01.transparency);

					flecha->Draw(*mLightsShader);
				}

			}
		

			//Animación del cofre
				glm::vec3 translate_temp;
				float rotatex_temp;
				glm::vec3 scale_temp;

				//COFRE
				translate_temp = glm::vec3(-8.0f, 0.55f, -5.0f);
				rotatex_temp = -90.0f;
				scale_temp = glm::vec3(0.3f, 0.3f, 0.3f);
				prepareTrash(cofre_inf, translate_temp, rotatex_temp, scale_temp);
				cofre_inf->Draw(*mLightsShader);


				model = glm::mat4(1.0f);
				model = glm::translate(model,glm::vec3(-8.0f, 0.55f, -5.0f )); // translate it down so it's at the center of the scene

				model = glm::rotate(model, glm::radians(-angCofre), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
				mLightsShader->setMat4("model", model);
				if (nearTrash(camera.Position, translate_temp)) {
					if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
						girocofre = true;	
				}
				if (girocofre and angCofre < 90.0f) {
					angCofre += 0.1;
				}
				
				cofre_sup->Draw(*mLightsShader);

				//chango
				translate_temp = glm::vec3(-8.0f, 0.55f, -5.0f);
				rotatex_temp = -90.0f;
				scale_temp = glm::vec3(0.3f, 0.3f, 0.3f);
				prepareTrash(monkey, translate_temp, rotatex_temp, scale_temp);
				monkey->Draw(*mLightsShader);

			
		}
		glUseProgram(0);

		// ================== CANGREJOS CAMINANDO DE LADO A LADO ==================
		{
			cangrejo->UpdateAnimation(deltaTime);
			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);
			dynamicShader->setFloat("waterLevel", 0.0f); // adjust if needed
			dynamicShader->setFloat("fogDensity", 0.00f);
			dynamicShader->setFloat("depthAttenuation", 0.0f);
			dynamicShader->setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
			dynamicShader->setFloat("caustic_intensity", 0.0f);
			for (size_t i = 0; i < Lights_playa.size(); ++i) {
				SetLightUniformVec3(dynamicShader, "Position", i, Lights_playa[i].Position);
				SetLightUniformVec3(dynamicShader, "Direction", i, Lights_playa[i].Direction);
				SetLightUniformVec4(dynamicShader, "Color", i, Lights_playa[i].Color);
				SetLightUniformVec4(dynamicShader, "Power", i, Lights_playa[i].Power);
				SetLightUniformInt(dynamicShader, "alphaIndex", i, Lights_playa[i].alphaIndex);
				SetLightUniformFloat(dynamicShader, "distance", i, Lights_playa[i].distance);
			}

			for (int i = 0; i < NUM_CANGREJOS; ++i) {
				CangrejoPath& C = gCangrejos2[i];

				// Avanzar tiempo
				C.time += deltaTime * C.speed;
				float t = C.time + C.phase;

				// Movimiento de ida y vuelta
				float factor = sinf(t);                  // [-1,1]
				glm::vec3 offset = C.dir * (factor * C.length);

				glm::vec3 posNow = C.center + offset;

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, posNow);
				model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));

				// Escala
				model = glm::scale(model, glm::vec3(0.008f));

				dynamicShader->setMat4("model", model);
				dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, cangrejo->gBones);
				cangrejo->Draw(*dynamicShader);
			}
		}

		
		textTexture = texPressR;
		// Mostrar imagen de texto como overlay en pantalla completa
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		fullscreenShader->use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textTexture);
		fullscreenShader->setInt("screenTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		PlaySceneMusic("sounds/OlasdeSal.mp3"); // música playa


	}
	 
	
	// glfw: swap buffers 
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

bool TryMove(const glm::vec3& offset, const std::vector<AABB>& colliders, float radius)
{
	glm::vec3 newPos = camera.Position + offset;

	// collision check
	for (const AABB& box : colliders)
	{
		if (sphereAABB(newPos, radius, box))
			return false; // BLOCKED
	}

	// safe → apply move
	camera.Position = newPos;
	return true;
}

glm::vec3 ComputeMovement(Camera_Movement direction, float deltaTime)
{
	float velocity = camera.MovementSpeed * deltaTime;
	glm::vec3 move(0.0f);

	if (direction == FORWARD) {
		move.x += camera.Front.x * velocity;
		move.z += camera.Front.z * velocity;
	}
	if (direction == BACKWARD) {
		move.x -= camera.Front.x * velocity;
		move.z -= camera.Front.z * velocity;
	}
	if (direction == LEFT) {
		move -= camera.Right * velocity;
	}
	if (direction == RIGHT) {
		move += camera.Right * velocity;
	}

	return move;
}

// Procesamos entradas del teclado
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glm::vec3 off = ComputeMovement(FORWARD, deltaTime);
		TryMove(off*2.0f, bounding_boxes->aabbs, cameraRadius);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glm::vec3 off = ComputeMovement(BACKWARD, deltaTime);
		TryMove(off, bounding_boxes->aabbs, cameraRadius);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 off = ComputeMovement(LEFT, deltaTime);
		TryMove(off, bounding_boxes->aabbs, cameraRadius);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 off = ComputeMovement(RIGHT, deltaTime);
		TryMove(off, bounding_boxes->aabbs, cameraRadius);
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		draw_colliders = true;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		draw_colliders = false;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		if (submarino == false) {
			submarino = true;
			camera.Position = glm::vec3(0.0f, 2.0f, 10.0f);
		}		
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		if (submarino == true) {
			submarino = false;
			camera.Position = glm::vec3(0.0f, 2.0f, 10.0f);
		}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		door_rotation += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		door_rotation -= 0.01f;

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



