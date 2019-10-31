﻿#include <examplebase.h>
#include <model.h>
#include <material.h>
#include <buffer.h>
#include <random>
#include <ctime>
using namespace es;

class Example final : public ExampleBase
{
public:
	const int row = 7;
	const int col = 7;
	std::vector<std::shared_ptr<Model>> spheres;

	struct Light
	{
		glm::vec3 color;
		glm::vec3 position;
	};
	std::array<Light, 4> lights;

	Example()
	{
		title = "basic pbr";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/27.basic_pbr/";
	}

	~Example()
	{
		spheres.swap(std::vector<std::shared_ptr<Model>>());
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 10.0f, 12.0f));
		mMainCamera->setRotation(glm::vec3(45.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::shared_ptr<Model> sphereTemplate = Model::createFromFile("sphere_template", modelsDirectory + "/sphere/sphere.obj",
			{
				shadersDirectory + "pbr.vert",
				shadersDirectory + "pbr.frag"
			},
			true
		);
		
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				std::shared_ptr<Model> sphere = Model::clone("sphere_" + std::to_string(i * col + j), sphereTemplate.get());
			
				glm::vec3 pos = glm::vec3(float(j - (row / 2.0f)) * 2.5f, 0.0f, float(i - (col / 2.0f)) * 2.5f);
				sphere->setPosition(pos);
				sphere->setScale(glm::vec3(0.035f));
				sphere->setUniform("roughness", glm::clamp((float)i / (float)(row - 1), 0.05f, 1.0f));
				sphere->setUniform("metallic", glm::clamp((float)j / (float)(row - 1), 0.1f, 1.0f));

				for (std::size_t i = 0; i < lights.size(); i++)
				{
					lights[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
					lights[i].position = glm::vec3(sin(glm::radians(i * 360.0f)) * 20.0f, cos(glm::radians(i * 360.0f)) * 20.0f, 0.0f);

					sphere->setUniform("lights[" + std::to_string(i) + "].color", lights[i].color);
					sphere->setUniform("lights[" + std::to_string(i) + "].position", lights[i].position);
				}

				spheres.push_back(sphere);
			}
		}
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		for (std::size_t i = 0; i < spheres.size(); i++)
		{
			for (std::size_t j = 0; j < lights.size(); j++)
			{
				lights[j].position = glm::vec3(sin(glm::radians(j * 360.0f)) * 20.0f, cos(glm::radians(j * 360.0f)) * 20.0f, 0.0f);
				spheres[i]->setUniform("lights[" + std::to_string(j) + "].position", lights[j].position);
			}
			spheres[i]->setUniform("viewPos", mMainCamera->getPosition());
			spheres[i]->render();
		}
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupSDL() ||
		!example->loadGLESFunctions() ||
		!example->setupImGui())
	{
		return 0;
	}
	example->prepare();
	example->renderLoop();
	delete(example);
	return 0;
}