#include "Skeleton.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"
#include "Shaders/Program.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	glm::mat4 model_matrix;
	for (const auto& bone : bones) {
		const glm::vec3 pos = bone.second.translation;
		model_matrix = glm::translate(glm::mat4(), pos);
		model_matrix = glm::scale(model_matrix, glm::vec3(0.1f, 0.1f, 0.1f));
		GLUtils::defaultProgram->setUniform("model", model_matrix);
		Render::cube();
	}
}

void Skeleton::initBones() 
{
	for(int bone = EBoneID::HIP_CENTER; bone != EBoneID::COUNT; ++bone) {
		EBoneID boneID = static_cast<EBoneID>(bone);
		bones.insert(std::make_pair(boneID, Bone(boneID, glm::vec3(), glm::quat(), glm::vec3(1,1,1))));
	}
}
