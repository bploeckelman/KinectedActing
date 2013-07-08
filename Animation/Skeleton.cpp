#include "Skeleton.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"
#include "Shaders/Program.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <map>


Skeleton::Skeleton()
	: bones()
{
	initBones();
}

Skeleton::~Skeleton()
{
	bones.clear();
}

void Skeleton::render() const
{
	const glm::vec3 scale(0.1f, 0.1f, 0.1f);

	glm::mat4 model_matrix;

	std::for_each(begin(bones), end(bones), [&](const std::pair<EBoneID, Bone>& pair) {
		const Bone& bone = pair.second;

		model_matrix = glm::translate(glm::mat4(), bone.translation);
		model_matrix = glm::scale(model_matrix, scale);
		GLUtils::defaultProgram->setUniform("model", model_matrix);

		Render::cube();
	});
}

void Skeleton::initBones() 
{
	for(int bone = EBoneID::HIP_CENTER; bone != EBoneID::COUNT; ++bone) {
		EBoneID boneID = static_cast<EBoneID>(bone);
		bones.insert(std::make_pair(boneID, Bone(boneID, glm::vec3(), glm::quat(), glm::vec3(1,1,1))));
	}
}
