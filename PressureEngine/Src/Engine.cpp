#include <chrono>
#include <vector>
#include "Engine.h"
#include "Callbacks.h"
#include "Graphics\OBJLoader.h"
#include "Input\Input.h"
#include "Graphics\Particles\ParticleMaster.h"
#include "Graphics\Particles\ParticleTexture.h"

namespace Pressure {

	Engine::Engine() {
		init();
		loop();
		terminate();
	}

	void Engine::init() {
		if (!glfwInit()) {
			std::cout << "GLFW Failed to initialize!" << std::endl;
			// TODO: Handle this error.
		}

		glfwSetErrorCallback(Callbacks::error_callback);

		window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, false, false);

		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cout << "GLEW Failed to initialize!" << std::endl;
			// TODO: Handle this error.
		}

		loader = new Loader();
		camera = new Camera();
		renderer = new MasterRenderer(*window, *loader, *camera);
		ParticleMaster::init(*loader, window->getWindow());

		RawModel* model = OBJLoader::loadObjModel("ball", *loader);
		ModelTexture* texture = new ModelTexture(loader->loadTexture("default.png"));
		texture->setShineDamper(10);
		texture->setReflectivity(1);
		TexturedModel* texturedModel = new TexturedModel(model, texture);
		entity = new Entity(*texturedModel, Vector3f(-25, -50, 0), Vector3f(0, 0, 0), 50.f);
		entity->setRotationSpeed(0, 0.5f, 0);
		lights = new std::vector<Light>;
		lights->emplace_back(Vector3f(150000, 170000, 200000), Vector3f(1));
		//lights->emplace_back(Vector3f(-10, 10, -10), Vector3f(1, 0, 0), Vector3f(0.4, 0.4, 0.4));

		ParticleTexture particleTexture(loader->loadTexture("particleAtlas.png"), 4);
		particleSystem = new ParticleSystem(particleTexture, 2, 0.03f, 0.00f, 2 * 60);
		
		water = new Water(Vector3f(-16, 0, -16), *loader);
		water2 = new Water(Vector3f(-48, 10, -16), *loader);

	}

	void Engine::loop() {
		long lastTime = Math::getTimeNano();
		double ns = 1000000000. / PRESSURE_TICKRATE;
		double delta = 0;
		long timer = Math::getTimeMillis();
		int frames = 0;
		long now;

		while (!glfwWindowShouldClose(window->getWindow())) {
			now = Math::getTimeNano();
			delta += (now - lastTime) / ns;
			lastTime = now;

			while (delta >= 1) {
				tick();
				delta--;
			}

			if (!glfwWindowShouldClose(window->getWindow()))
				render();
			frames++;

#if 1
			if (Math::getTimeMillis() - timer > 1000) {
				timer += 1000;
				std::cout << "FPS: " << frames << std::endl;
				frames = 0;
			}
#endif
		}
	}

	void Engine::tick() {
		glfwPollEvents();
		camera->tick();

		if (Keyboard::isPressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window->getWindow(), GLFW_TRUE);

		if (window->resized) {
			renderer->updateProjectionMatrix();
			window->resized = false;
		}

		entity->tick();
		renderer->tick();

		ParticleMaster::tick(*camera);
		particleSystem->generateParticles(Vector3f(5, .5f, 0));
	}

	void Engine::render() {
		renderer->processEntity(*entity);
		renderer->processWater(*water);
		//renderer->processWater(*water2);

		renderer->renderShadowMap((*lights)[0]);
		renderer->render(*lights, *camera);

		//ParticleMaster::renderParticles(*camera);

		glfwSwapBuffers(window->getWindow());
	}

	void Engine::terminate() {
		loader->cleanUp();
		renderer->cleanUp();
		ParticleMaster::cleanUp();

		delete window;
		delete loader;
		delete renderer;
		delete entity;
		delete camera;
		delete lights;
		delete water;
		delete water2;

		glfwTerminate();
	}

}
