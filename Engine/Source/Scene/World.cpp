#include "World.h"

#include "Core/Renderer.h"
#include "Core/Logger.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace Cosmos
{
    World::World(Renderer* renderer, const char* name)
        : mRenderer(renderer)
    {
        SetName(name);
    }

    World::~World()
    {
        Destroy();
    }

    void World::Update(float timestep)
    {
        // nothing to do right now
    }

    void World::Render(float timestep)
    {
		for (auto& [id, entity] : mEntities)
		{
			if (id.GetValue() == id.GetNullID() || entity == nullptr) continue;
		}
    }

    void World::Destroy()
    {
        std::vector<ID> ids;
        ids.reserve(mEntities.Size());

        for (auto& [id, entity] : mEntities) { 
            ids.push_back(id); 
        }

        for (ID id : ids) { 
            DestroyEntity(id.GetValue());
            mRenderer->GetIDGenRef().Destroy(id); 
        }

        ids.clear();
        mEntities.Clear();
    }

    Entity* World::FindEntity(ID id)
    {
        return mEntities.Get(id).value_or(nullptr);
    }

    Entity* World::CreateEntity(const char* name, const float3& pos)
    {
        ID id = mRenderer->GetIDGenRef().Create();

        if (id.GetValue() == ID::GetNullID()) {
            LOG_TO_TERMINAL(Logger::Error, "Error while creating entity '%s'. The entity id created is the nullid.", name);
            return NULL;
        }
        
        if (mEntities.Contains(id)) {
            LOG_TO_TERMINAL(Logger::Error, "Error while creating entity '%s'. An entity id '%d' already exists on this world.", name, id.GetValue());
            return NULL;
        }

        Entity* ent = new Entity(name, id);

        // add components

        if (!mEntities.Insert(id, ent)) {
            LOG_TO_TERMINAL(Logger::Error, "Error while creating entity '%s'. Unable to insert entity on world entity library.", name);
            delete ent;
            return NULL;
        }

        return ent;
    }

    void World::DestroyEntity(ID id)
    {
        std::optional<Entity*> found = mEntities.Get(id);

        if (!found.has_value()) {
            LOG_TO_TERMINAL(Logger::Warn, "Entity with id '%d' was not found on world to be deleted.", id);
            return;
        }

        Entity* ent = found.value();
        if (!ent) return;

        // destroy components

        mEntities.Erase(id);
        mRenderer->GetIDGenRef().Destroy(id);
        delete ent;
    }

    bool World::InsertEntity(Entity* entity)
    {
        if (!entity) return false;

        if (mEntities.Contains(entity->GetID())) {
            LOG_TO_TERMINAL(Logger::Error, "An entity with the id '%' already exists on this world.", entity->GetID());
            return false;
        }

        return mEntities.Insert(entity->GetID(), entity);
    }

    Entity* World::ExtractEntity(ID id)
    {
        std::optional<Entity*> found = mEntities.Get(id);
        if (!found.has_value()) return nullptr;

        Entity* ent = found.value();
        mEntities.Erase(id);
        mRenderer->GetIDGenRef().Destroy(id);

        return ent;
    }

    bool World::TransferEntity(ID id, World& another)
    {
        Entity* ent = ExtractEntity(id);
        if (!ent) return false;

        return another.InsertEntity(ent);
    }
}