#pragma once

#include "Core/Defines.h"
#include "Util/Datafile.h"
#include <EVK.h>

/// @brief forward declarations
namespace Cosmos { class Entity; }

namespace Cosmos
{
    struct COSMOS_API VisibilityComponent
    {
        /// @brief constructor
        VisibilityComponent(bool visible = true);

        /// @brief saves the transform component of an entity into a datafile
        static void Save(Entity* entity, Datafile& datafile);

        /// @brief loads the transform component of an entity from a datafile
        static void Load(Entity* entity, Datafile& datafile);

    public:
        bool visible;
    };

    struct COSMOS_API TransformComponent
    {
    public:

        /// @brief constructor
        TransformComponent(float3 translation = { 0.0f, 0.0f, 0.0f }, float3 rotation = { 0.0f, 0.0f, 0.0f }, float3 scale = { 1.0f, 1.0f, 1.0f });

        /// @brief saves the transform component of an entity into a datafile
        static void Save(Entity* entity, Datafile& datafile);

        /// @brief loads the transform component of an entity from a datafile
        static void Load(Entity* entity, Datafile& datafile);

    public:
        float3 translation;
        float3 rotation;
        float3 scale;
    };

    struct COSMOS_API SpriteComponent
    {
    public:

        /// @brief constructor
        SpriteComponent(uint32_t id);

        /// @brief destructor
        ~SpriteComponent();

        /// @brief saves the transform component of an entity into a datafile
        static void Save(Entity* entity, Datafile& datafile);

        /// @brief loads the transform component of an entity from a datafile
        static void Load(Entity* entity, Datafile& datafile);

    public:
        evkSprite* sprite = nullptr;
        bool visible = true;
        uint32_t id = 0;
    };
}