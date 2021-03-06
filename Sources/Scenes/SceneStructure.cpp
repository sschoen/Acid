﻿#include "SceneStructure.hpp"

#include "Physics/Rigidbody.hpp"

namespace acid
{
	SceneStructure::SceneStructure() :
		m_mutex(std::mutex()),
		m_objects(std::vector<std::unique_ptr<Entity>>())
	{
	}

	Entity *SceneStructure::CreateEntity(const Transform &transform)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto entity = new Entity(transform);
		m_objects.emplace_back(entity);
		return entity;
	}

	Entity *SceneStructure::CreateEntity(const std::string &filename, const Transform &transform)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto entity = new Entity(filename, transform);
		m_objects.emplace_back(entity);
		return entity;
	}

	void SceneStructure::Add(Entity *object)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_objects.emplace_back(object);
	}

	void SceneStructure::Add(std::unique_ptr<Entity> object)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_objects.emplace_back(std::move(object));
	}

	bool SceneStructure::Remove(Entity *object)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto it = --m_objects.end(); it != m_objects.begin(); --it)
		{
			if ((*it).get() != object)
			{
				continue;
			}

			m_objects.erase(it);
			return true;
		}

		return false;
	}

	bool SceneStructure::Move(Entity *object, SceneStructure &structure)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto it = --m_objects.end(); it != m_objects.begin(); --it)
		{
			if ((*it).get() != object)
			{
				continue;
			}

			structure.Add(std::move(*it));
			m_objects.erase(it);
			return true;
		}

		return false;
	}

	void SceneStructure::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_objects.clear();
	}

	void SceneStructure::Update()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto it = m_objects.begin(); it != m_objects.end();)
		{
			if ((*it)->IsRemoved())
			{
				it = m_objects.erase(it);
				continue;
			}

			(*it)->Update();
			++it;
		}
	}

	std::vector<Entity *> SceneStructure::QueryAll()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto result = std::vector<Entity *>();

		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if ((*it)->IsRemoved())
			{
				continue;
			}

			result.emplace_back((*it).get());
		}

		return result;
	}

	std::vector<Entity *> SceneStructure::QueryFrustum(const Frustum &range)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto result = std::vector<Entity *>();

		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if ((*it)->IsRemoved())
			{
				continue;
			}

			auto rigidbody = (*it)->GetComponent<Rigidbody>();

			if (rigidbody == nullptr || rigidbody->InFrustum(range))
			{
				result.emplace_back((*it).get());
			}
		}

		return result;
	}

	/*std::vector<Entity *> SceneStructure::QuerySphere(const Vector3 &centre, const Vector3 &radius)
	{
		return std::vector<Entity *>();
	}*/

	/*std::vector<Entity *> SceneStructure::QueryCube(const Vector3 &min, const Vector3 &max)
	{
		return std::vector<Entity *>();
	}*/

	bool SceneStructure::Contains(Entity *object)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto &object2 : m_objects)
		{
			if (object2.get() == object)
			{
				return true;
			}
		}

		return false;
	}
}
