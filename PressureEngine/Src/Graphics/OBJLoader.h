#pragma once
#include <vector>
#include <list>
#include <string>
#include "Models\RawModel.h"
#include "Loader.h"
#include "../Math/Math.h"

namespace Pressure {

	class OBJLoader {

	public:
		static RawModel* loadObjModel(const char* fileName, Loader& loader);

	private:
		static void processFaces(std::string& data, unsigned int lineStart, std::list<int>& indices, std::vector<Vector2f>& uvs,
			std::vector<Vector3f>& normals, std::vector<float>& textureArray, std::vector<float>& normalsArray);
	};

}