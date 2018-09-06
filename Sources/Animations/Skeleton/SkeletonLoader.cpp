#include "SkeletonLoader.hpp"

#include "Animations/MeshAnimated.hpp"

namespace acid
{
	SkeletonLoader::SkeletonLoader(const std::shared_ptr<Metadata> &libraryControllers, const std::vector<std::string> &boneOrder) :
		m_armatureData(nullptr),
		m_boneOrder(boneOrder),
		m_jointCount(0),
		m_headJoint(nullptr)
	{
		m_armatureData = libraryControllers->FindChild("visual_scene")->FindChildWithAttribute("node", "id", "Armature");
		auto headNode = m_armatureData->FindChild("node");
		m_headJoint = LoadJointData(headNode, true);
	}

	std::shared_ptr<JointData> SkeletonLoader::LoadJointData(const std::shared_ptr<Metadata> &jointNode, const bool &isRoot)
	{
		auto joint = ExtractMainJointData(jointNode, isRoot);

		for (auto &childNode : jointNode->FindChildren("node"))
		{
			joint->AddChild(LoadJointData(childNode, false));
		}

		return joint;
	}

	std::shared_ptr<JointData> SkeletonLoader::ExtractMainJointData(const std::shared_ptr<Metadata> &jointNode, const bool &isRoot)
	{
		std::string nameId = jointNode->FindAttribute("id");
		auto index = GetBoneIndex(nameId);
		auto matrixData = String::Split(jointNode->FindChild("matrix")->GetValue(), " ");

		Matrix4 transform = Matrix4();

		for (uint32_t i = 0; i < matrixData.size(); i++)
		{
			transform.m_linear[i] = String::From<float>(matrixData[i]);
		}

		transform = transform.Transpose();

		if (isRoot)
		{
			// Because in Blender z is up, but the engine is y up.
			transform = MeshAnimated::CORRECTION * transform;
		}

		m_jointCount++;
		return std::make_shared<JointData>(*index, nameId, transform);
	}

	std::optional<uint32_t> SkeletonLoader::GetBoneIndex(const std::string &name)
	{
		for (uint32_t i = 0; i < m_boneOrder.size(); i++)
		{
			if (m_boneOrder[i] == name)
			{
				return i;
			}
		}

		return {};
	}
}
