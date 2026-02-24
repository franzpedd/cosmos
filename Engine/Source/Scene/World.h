#pragma once

#include "Core/Defines.h"
#include "Util/ID.h"
#include "Util/Library.h"
#include <vecmath/vecmath.h>

/// @brief forward declarations
namespace Cosmos { class Entity; }
namespace Cosmos { class Renderer; }

namespace Cosmos
{
    class COSMOS_API World
    {
    public:

        /// @brief constructor
        World(Renderer* renderer, const char* name);

        /// @brief destructor
        ~World();

        /// @brief returns the entities library reference for this world
        inline const Library<ID, Entity*>& GetEntitiesLibraryRef() const { return mEntities; }

        /// @brief returns the world name
        inline const char* GetName() const { return mName; }

        /// @brief sets a new name for the world
        inline void SetName(const char* name)
        {
            strncpy(mName, name ? name : "World Name", sizeof(mName) - 1);
            mName[sizeof(mName) - 1] = '\0';
        }

    public:

        /// @brief process all pending world events
        void Update(float timestep);

        /// @brief process al pending world renders
        void Render(float timestep);

        /// @brief delete all entities on the world
        void Destroy();

    public:

        /// @brief attempts to find the entity by it's id, returns NULL if not found
        Entity* FindEntity(ID id);

        /// @brief creates a new entity on the world at a given position, returns it's id or 0 if failed
        Entity* CreateEntity(const char* name, const float3& pos = {0.0f, 0.0f, 0.0f});

        /// @brief destroys an entity by it's id
        void DestroyEntity(ID id);

        /// @brief inserts an existing entity into the world, returns false if failed
        bool InsertEntity(Entity* entity);

        /// @brief removes the entity from the world without deleting it
        Entity* ExtractEntity(ID id);

        /// @brief transfer an entity from this world to another, returns false on failure
        bool TransferEntity(ID id, World& another);

    private:

        Renderer* mRenderer = nullptr;
        char mName[32];
        Library<ID, Entity*> mEntities = { };
    };
}