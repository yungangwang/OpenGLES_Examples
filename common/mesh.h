#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <object.h>
#include <buffer.h>

namespace es
{
	// vertex attribute structure
	struct Vertex
	{
		glm::vec3 vPosition;
		glm::vec2 vTexcoord;
		glm::vec3 vNormal;
		glm::vec3 vTangent;
		glm::vec3 vBitangent;
	};

	class Mesh : public Object
	{
	public:
		Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		template<typename... T>
		static std::shared_ptr<Mesh> createWithData(T&&... args)
		{
			return std::make_shared<Mesh>(std::forward<T>(args)...);
		}

		void render();
	private:
		std::string mName;

		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;

		std::unique_ptr<VertexArray> mVAO = nullptr;
		std::unique_ptr<VertexBuffer> mVBO = nullptr;
		std::unique_ptr<ElementBuffer> mEBO = nullptr;
	};
}

#endif