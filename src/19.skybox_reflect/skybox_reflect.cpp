﻿#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Model> skybox;
	std::shared_ptr<Model> sphere;

	float LodBias = 0.0f;
	glm::vec3 lightPos = glm::vec3(0.0f, -5.0f, 5.0f);

	Example()
	{
		title = "skybox reflect";
		settings.vsync = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/19.skybox_reflect/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::shared_ptr<TextureCube> cubemap = TextureCube::createFromFiles({ texturesDirectory + "/skyboxes/sincity/right.tga", texturesDirectory + "/skyboxes/sincity/left.tga",
															 texturesDirectory + "/skyboxes/sincity/top.tga", texturesDirectory + "/skyboxes/sincity/bottom.tga",
															 texturesDirectory + "/skyboxes/sincity/front.tga", texturesDirectory + "/skyboxes/sincity/back.tga" }, 1, false);
		cubemap->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		cubemap->setMagFilter(GL_LINEAR);
		cubemap->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		cubemap->generateMipmaps();

		// create a cube model as skybox
		skybox = Model::createFromFile("skybox", modelsDirectory + "/cube/cube.obj", {
			shadersDirectory + "skybox.vert",
			shadersDirectory + "skybox.frag"
		});

		skybox->setTexture("cubemap", cubemap);
		skybox->setUniform("LodBias", LodBias);

		sphere = Model::createFromFile("sphere", modelsDirectory + "/sphere/sphere.dae", {
			shadersDirectory + "reflect.vert",
			shadersDirectory + "reflect.frag"
		});

		sphere->setTexture("skybox", cubemap);
		sphere->setUniform("LodBias", LodBias);
		sphere->setUniform("lightPos", lightPos);
	}

	virtual void render(float deltaTime) override
	{
		glEnable(GL_CULL_FACE);
		sphere->setUniform("viewPos", mMainCamera->getPosition());
		sphere->render();
		glDisable(GL_CULL_FACE);

		// change depth function so depth test passes when depth values are equal to content of depth buffer
		glDepthFunc(GL_LEQUAL);
		 //render skybox at last
		skybox->render();
		// set depth function back to default
		glDepthFunc(GL_LESS);
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
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