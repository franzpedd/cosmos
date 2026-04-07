#include "World.h"

#include "Core/Renderer.h"
#include "Core/Logger.h"
#include "UI/GUI.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace Cosmos
{
    World::World(GUI* gui, Renderer* renderer, const char* name)
        : mGUI(gui), mRenderer(renderer)
    {
        SetName(name);
    }

    World::~World()
    {
        Destroy();
    }

    void World::Update(float timestep)
    {
        for (auto& [id, entity] : mEntities)
        {
            // sprite
            SpriteComponent* sprite = entity->GetComponent<SpriteComponent>();
            if (sprite) {
                if (sprite->sprite) {
                    evk_sprite_update(sprite->sprite, false);
                }
            }
        }
    }

    void World::Render(float timestep)
    {
		for (auto& [id, entity] : mEntities)
		{
			if (id.GetValue() == id.GetNullID() || entity == nullptr) continue;

            // visibility
            VisibilityComponent* visibility = entity->GetComponent<VisibilityComponent>();
            if (!visibility) continue;
            if (!visibility->visible) continue;

            // transform
            TransformComponent* transform = entity->GetComponent<TransformComponent>();
            if (!transform) continue;

            // sprite
            SpriteComponent* sprite = entity->GetComponent<SpriteComponent>();
            if (sprite) {
                if (sprite->sprite) {
                    evk_sprite_render(sprite->sprite);
                }
            }
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

    bool World::CreateEntity(const char* name, const float3& pos)
    {
        ID id = mRenderer->GetIDGenRef().Create();

        if (id.GetValue() == ID::GetNullID()) {
            LOG_TO_TERMINAL(Logger::Error, "Error while creating entity '%s'. The entity id created is the nullid.", name);
            return false;
        }
        
        if (mEntities.Contains(id)) {
            LOG_TO_TERMINAL(Logger::Error, "Error while creating entity '%s'. An entity id '%d' already exists on this world.", name, id.GetValue());
            return false;
        }

        Entity* ent = new Entity(name, id);

        // add components
        ent->AddComponent<VisibilityComponent>(true);
        ent->AddComponent<TransformComponent>(pos, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.5f, 0.5f, 1.0f });
        ent->AddComponent<SpriteComponent>(id);

        // modify components
        evkSprite* sprite = ent->GetComponent<SpriteComponent>()->sprite;
        evkTexture2D* albedo = evk_sprite_get_albedo(sprite);
        evk_sprite_set_translation(sprite, pos);  
        evk_sprite_set_scale(sprite, { 0.25f, 0.25f });
        mGUI->AddDescriptorImage(evk_texture2d_get_path(albedo), evk_texture2d_get_sampler(albedo), evk_texture2d_get_view(albedo));

        if (!mEntities.Insert(id, ent)) {
            LOG_TO_TERMINAL(Logger::Error, "Error while creating entity '%s'. Unable to insert entity on world entity library.", name);
            delete ent;
            return false;
        }

        return true;
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
        if (ent->HasComponent<VisibilityComponent>()) { ent->RemoveComponent<VisibilityComponent>(); }
        if (ent->HasComponent<SpriteComponent>()) { ent->RemoveComponent<SpriteComponent>(); }
        if (ent->HasComponent<TransformComponent>()) { ent->RemoveComponent<TransformComponent>(); }

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