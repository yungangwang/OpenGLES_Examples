#include "camera.h"

namespace es
{
	Camera* Camera::create()
	{
		Camera* camera = new (std::nothrow) Camera();
		camera->initDefault();
		camera->setDepth(0.0f);

		return camera;
	}

	Camera* Camera::createPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		Camera* camera = new (std::nothrow) Camera();
		if (camera)
		{
			camera->initPerspective(fov, aspectRatio, nearPlane, farPlane);
			return camera;
		}
		delete(camera);
		camera = nullptr;
		return nullptr;
	}

	Camera* Camera::createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
	{
		Camera* camera = new (std::nothrow) Camera();
		if (camera)
		{
			camera->initOrthoGraphic(zoomX, zoomY, nearPlane, farPlane);
			return camera;
		}
		delete(camera);
		camera = nullptr;
		return nullptr;
	}

	Camera* Camera::getDefaultCamera()
	{
		return World::getWorld()->getDefaultCamera();
	}

	Camera::Camera()
	{
		mRotation = glm::vec3(0.0f, 0.0f, 0.0f);

		front = glm::vec3(0.0f, 0.0f, -1.0f);
		right = glm::vec3(1.0f, 0.0f, 0.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	Camera::~Camera()
	{
	
	}

	void Camera::lookAt(const glm::vec3& target, const glm::vec3& up)
	{

	}

	const glm::mat4& Camera::getProjectionMatrix() const
	{
		return projection;
	}

	glm::mat4 Camera::getViewMatrix()
	{
		return view;
	}

	const glm::mat4& Camera::getViewProjectionMatrix() const
	{
		return projection * view;
	}

	bool Camera::initDefault()
	{
		initPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
		setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		return true;
	}

	bool Camera::initPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		this->fov = fov;
		this->aspectRatio = aspectRatio;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
		viewDirty = true;
		frustumDirty = true;
		type = Camera::Type::Perspective;

		return true;
	}

	bool Camera::initOrthoGraphic(float zoomX, float zoomY, float nearPlane, float farPlane)
	{
		this->zoom[0] = zoomX;
		this->zoom[1] = zoomY;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		projection = glm::ortho(0.0f, zoomX, 0.0f, zoomY, nearPlane, farPlane);
		viewDirty = true;
		frustumDirty = true;
		type = Camera::Type::Orthographic;

		return true;
	}

	void Camera::setDepth(int8_t depth)
	{
		if (this->depth != depth)
		{
			this->depth = depth;
		}
	}

	void Camera::update(float deltaTime)
	{
		if (viewDirty)
		{
			glm::vec3 camFront;
			camFront.x = -cos(glm::radians(mRotation.x)) * sin(glm::radians(mRotation.y));
			camFront.y = sin(glm::radians(mRotation.x));
			camFront.z = cos(glm::radians(mRotation.x)) * cos(glm::radians(mRotation.y));
			
			front = -glm::normalize(camFront);
			right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
			up = glm::normalize(glm::cross(right, front));

			float moveSpeed = deltaTime * movementSpeed;

			if (keys.forward)
				translate(front * moveSpeed);
			if (keys.backward)
				translate(-front * moveSpeed);
			if (keys.leftward)
				translate(-right * moveSpeed);
			if (keys.rightward)
				translate(right * moveSpeed);
			
			updateViewMatrix();

			viewDirty = false;
		}
	}

	void Camera::translate(const glm::vec3& deltaPosition)
	{
		this->mPosition += deltaPosition;
		viewDirty = true;
	}

	void Camera::rotate(const glm::vec3& deltaEuler)
	{
		this->mRotation += deltaEuler;
		this->mRotation.x = glm::clamp(this->mRotation.x, -90.0f, 90.0f);
		viewDirty = true;
	}

	void Camera::setPosition(const glm::vec3& position)
	{
		this->mPosition = position;
		viewDirty = true;
	}

	void Camera::setRotation(const glm::vec3& euler)
	{
		this->mRotation = euler;
		this->mRotation.x = glm::clamp(this->mRotation.x, -90.0f, 90.0f);
		viewDirty = true;
	}

	const glm::vec3& Camera::getFrontVector() const
	{
		return front;
	}

	const glm::vec3& Camera::getRightVector() const
	{
		return right;
	}

	const glm::vec3& Camera::getUpVector() const
	{
		return up;
	}

	bool Camera::moving() const
	{
		return keys.leftward || keys.rightward || keys.forward || keys.backward;
	}

	void Camera::updateViewMatrix()
	{
		view = glm::mat4(1.0f);
		view = glm::rotate(view, glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		view = glm::translate(view, -mPosition);
	}
}