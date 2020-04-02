#ifndef ECS_HPP
#define ECS_HPP

/* This is derived from:
 * https://austinmorlan.com/posts/entity_component_system/ */

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

#include "defs.hpp"

namespace ecs
{
using Entity = std::uint32_t;
const Entity MAX_ENTITIES = DEF_MAX_ENTITIES;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = DEF_MAX_COMPONENTS;

using Signature = std::bitset<MAX_COMPONENTS>;

class EntityManager
{
public:
	EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
			mAvaliableEntities.push(entity);
		}
	}

	Entity createEntity()
	{
		assert(mLivingEntityCount < MAX_ENTITIES &&
		       "Too many entities in existence.");

		// Take an ID from the front of the queue.
		Entity id = mAvaliableEntities.front();
		mAvaliableEntities.pop();
		mLivingEntityCount++;

		return id;
	}

	void destroyEntity(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Invalidate the destroyed entity's signature.
		mSignatures[entity].reset();

		// Put the destroyed ID at the back of the queue.
		mAvaliableEntities.push(entity);
		mLivingEntityCount--;
	}

	void setSignature(Entity entity, Signature signature)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Put this entity's signature into the array.
		mSignatures[entity] = signature;
	}

	Signature getSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		return mSignatures[entity];
	}

private:
	std::queue<Entity> mAvaliableEntities{};

	std::array<Signature, MAX_ENTITIES> mSignatures{};

	std::uint32_t mLivingEntityCount{};
};

// According to the guide I need a pure virtual class... Not sure why :|
/* Their comments:
 * The one instance of virtual inheritance in the entire implementation.
 * An interface is needed so that the ComponentManager (seen later)
 * can tell a generic ComponentArray that an entity has been destroyed
 * and that it needs to update its array mappings. */
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray
{
public:
	void insertData(Entity entity, T component)
	{
		assert(mEntityToIndexMap.find(entity) ==
			   mEntityToIndexMap.end() &&
		       "Component added to same entity more than once.");

		// Put the new entry at the end and update the maps.
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		mSize++;
	}

	void removeData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) !=
			   mEntityToIndexMap.end() &&
		       "Removing non-existent component.");

		// Copy element at end into deleted element's place to maintain
		// density.
		size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
		size_t indexOfLastElement = mSize - 1;
		mComponentArray[indexOfRemovedEntity] =
		    mComponentArray[indexOfLastElement];

		// Update map to point to the moved spot.
		Entity entityOfLastElement =
		    mIndexToEntityMap[indexOfLastElement];
		mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);

		mSize--;
	}

	T &getData(Entity entity)
	{
		assert(mEntityToIndexMap.find(entity) !=
			   mEntityToIndexMap.end() &&
		       "Retrieving non-existent component.");

		return mComponentArray[mEntityToIndexMap[entity]];
	}

	void entityDestroyed(Entity entity) override
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
			// Remove the entity's component if it existed.
			removeData(entity);
		}
	}

private:
	std::array<T, MAX_ENTITIES> mComponentArray;

	std::unordered_map<Entity, size_t> mEntityToIndexMap;
	std::unordered_map<size_t, Entity> mIndexToEntityMap;

	size_t mSize;
};

class ComponentManager
{
public:
	template <typename T>
	void registerComponent()
	{
		const char *typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) ==
			   mComponentTypes.end() &&
		       "Registering component type more than once.");

		// Add this component type to the component type map.
		mComponentTypes.insert({typeName, mNextComponentType});

		// Create a ComponentArray pointer and add it to the component
		// arrays map.
		mComponentArrays.insert(
		    {typeName, std::make_shared<ComponentArray<T>>()});

		// Increment the value so that the next component registered
		// will be different.
		mNextComponentType++;
	}

	template <typename T>
	ComponentType getComponentType()
	{
		const char *typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) !=
			   mComponentTypes.end() &&
		       "Component not registered before use.");

		// Return this component's type - used for signature creation.
		return mComponentTypes[typeName];
	}

	template <typename T>
	void addComponent(Entity entity, T component)
	{
		// Add a component to the array for an entity.
		getComponentArray<T>()->insertData(entity, component);
	}

	template <typename T>
	void removeComponent(Entity entity)
	{
		// Remove a component from the array for an entity.
		getComponentArray<T>()->removeData(entity);
	}

	template <typename T>
	T &getComponent(Entity entity)
	{
		// Get a reference to a component from the array for an entity.
		return getComponentArray<T>()->getData(entity);
	}

	void entityDestroyed(Entity entity)
	{
		// Notify each component array that an entity has been
		// destroyed. If it has a component for that entity, it will
		// remove it.
		for (auto const &pair : mComponentArrays) {
			auto const &component = pair.second;

			component->entityDestroyed(entity);
		}
	}

private:
	// Map from type string pointer to a component type.
	std::unordered_map<const char *, ComponentType> mComponentTypes{};

	// Map from type string pointer to a component array.
	std::unordered_map<const char *, std::shared_ptr<IComponentArray>>
	    mComponentArrays;

	// The component type to be assigned to the next registered component -
	// starting at 0.
	ComponentType mNextComponentType{};

	// Convenience function to get the statically casted pointer to the
	// ComponentArray of type T.
	template <typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		const char *typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) !=
			   mComponentTypes.end() &&
		       "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(
		    mComponentArrays[typeName]);
	}
};

class System
{
public:
	std::set<Entity> mEntities;
};

class SystemManager
{
public:
	template <typename T>
	std::shared_ptr<T> registerSystem()
	{
		const char *typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() &&
		       "Registering system more than once.");

		// Create a pointer to the system and return it so it can be
		// used externally.
		auto system = std::make_shared<T>();
		mSystems.insert({typeName, system});
		return system;
	}

	template <typename T>
	void setSignature(Signature signature)
	{
		const char *typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() &&
		       "System used before registered.");

		// Set the signature for this item.
		mSignatures.insert({typeName, signature});
	}

	void entityDestroyed(Entity entity)
	{
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed.
		for (auto const &pair : mSystems) {
			auto const &system = pair.second;

			system->mEntities.erase(entity);
		}
	}

	void entitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// Notify each system that an entity's signature changed.
		for (auto const &pair : mSystems) {
			auto const &type = pair.first;
			auto const &system = pair.second;
			auto const &systemSignature = mSignatures[type];

			if ((entitySignature & systemSignature) ==
			    systemSignature) {
				system->mEntities.insert(entity);
			} else {
				system->mEntities.erase(entity);
			}
		}
	}

private:
	std::unordered_map<const char *, Signature> mSignatures{};
	std::unordered_map<const char *, std::shared_ptr<System>> mSystems{};
};

class Coordinator
{
public:
	void init()
	{
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();
	}

	// Entity methods
	Entity createEntity()
	{
		return mEntityManager->createEntity();
	}

	void destroyEntity(Entity entity)
	{
		mEntityManager->destroyEntity(entity);

		mComponentManager->entityDestroyed(entity);

		mSystemManager->entityDestroyed(entity);
	}

	// Component methods
	template <typename T>
	void registerComponent()
	{
		mComponentManager->registerComponent<T>();
	}

	template <typename T>
	void addComponent(Entity entity, T component)
	{
		mComponentManager->addComponent<T>(entity, component);

		auto signature = mEntityManager->getSignature(entity);
		signature.set(mComponentManager->getComponentType<T>(), true);
		mEntityManager->setSignature(entity, signature);

		mSystemManager->entitySignatureChanged(entity, signature);
	}

	template <typename T>
	void removeComponent(Entity entity)
	{
		mComponentManager->removeComponent<T>(entity);

		auto signature = mEntityManager->getSignature(entity);
		signature.set(mComponentManager->getComponentType<T>(), false);
		mEntityManager->setSignature(entity, signature);

		mSystemManager->entitySignatureChanged(entity, signature);
	}

	template <typename T>
	T &getComponent(Entity entity)
	{
		return mComponentManager->getComponent<T>(entity);
	}

	template <typename T>
	ComponentType getComponentType()
	{
		return mComponentManager->getComponentType<T>();
	}

	// System methods
	template <typename T>
	std::shared_ptr<T> registerSystem()
	{
		return mSystemManager->registerSystem<T>();
	}

	template <typename T>
	void setSystemSignature(Signature signature)
	{
		mSystemManager->setSignature<T>(signature);
	}

private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;
};
} // namespace ecs

#endif
