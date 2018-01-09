#include "PressureEngine.h"


namespace Pressure {

	void PressureEngine::init() {
		// Initialize GLFW.
		if (!glfwInit()) {
			std::cout << "GLFW Failed to initialize!" << std::endl;
			__debugbreak();
		}

		window = std::make_unique<Window>(PRESSURE_WINDOW_WIDTH, PRESSURE_WINDOW_HEIGHT, PRESSURE_WINDOW_TITLE, PRESSURE_WINDOW_FULLSCREEN, PRESSURE_WINDOW_VSYNC);
		
		// Initialize GLEW.
		unsigned int err = glewInit();
		if (GLEW_OK != err) {
			std::cout << "GLEW Failed to initialize!" << std::endl;
			__debugbreak();
		}

#ifdef PRESSURE_DEBUG
		// Enable OpenGL debugging callback.
		enableErrorCallbacks();
#endif

		loader = std::make_unique<Loader>();
		camera = std::make_unique<Camera>();
		renderer = std::make_unique<MasterRenderer>(*window, *loader, *camera);
		guiRenderer = std::make_unique<GuiRenderer>(*loader);
		ParticleMaster::init(*loader, window->getWindow());

		initialized = true;
	}

	void PressureEngine::tick() {
		glfwPollEvents();
		camera->tick();

		if (window->resized) {
			renderer->updateProjectionMatrix();
			window->resized = false;
		}

		renderer->tick();
		ParticleMaster::tick(*camera);
	}

	void PressureEngine::process(Entity& entity) {
		renderer->processEntity(entity);
	}

	void PressureEngine::process(std::vector<Entity>& entities) {
		for (auto& entity : entities) {
			process(entity);
		}
	}

	void PressureEngine::process(Water& water) {
		renderer->processWater(water);
	}

	void PressureEngine::process(std::vector<Water>& water) {
		for (auto& w : water) {
			process(w);
		}
	}

	void PressureEngine::process(Light& light) {
		lights.push_back(light);
	}

	void PressureEngine::process(std::vector<Light>& light) {
		lights.insert(std::end(lights), std::begin(light), std::end(light));
	}

	void PressureEngine::process(GuiTexture& gui) {
		guis.push_back(gui);
	}

	void PressureEngine::process(std::vector<GuiTexture>& gui) {
		guis.insert(std::end(guis), std::begin(gui), std::end(gui));
	}

	void PressureEngine::render() {
		if (lights.size() > 0)
			renderer->renderShadowMap(lights[0]);
		renderer->render(lights, *camera);
		guiRenderer->render(guis);
		window->swapBuffers();
		lights.clear();
		guis.clear();
	}

	RawModel* PressureEngine::loadObjModel(const char* fileName) {
		return OBJLoader::loadObjModel(fileName, *loader);
	}

	ModelTexture PressureEngine::loadTexture(const char* filePath) {
		return ModelTexture(loader->loadTexture(filePath));
	}

	Water* PressureEngine::generateWater(const Vector3f& position) const {
		return new Water(position, *loader);
	}

	void PressureEngine::enableErrorCallbacks() {
		// Enable GLFW debugging callback.
		glfwSetErrorCallback(Callbacks::glfw_error_callback);

		// Enable GLEW debugging callback.
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		if (glDebugMessageCallback) {
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(opengl_error_callback, nullptr);
			unsigned int unusedIds = 0;
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
		}
		else
			std::cout << "glDebugMessageCallback Not Available! Disabling OpenGL Error Handling." << std::endl;
	}

	void PressureEngine::terminate() {
		loader->cleanUp();
		renderer->cleanUp();
		ParticleMaster::cleanUp();
		glfwTerminate();
	}

}