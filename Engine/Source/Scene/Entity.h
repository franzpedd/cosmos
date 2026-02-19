#pragma once

#include "Core/Defines.h"
#include "Scene/Components.h"
#include "Util/ID.h"

#include <any>
#include <unordered_map>
#include <typeindex>

namespace Cosmos
{
    class COSMOS_API Entity
    {
    public:

        /// @brief constructor
        Entity(const char* name, ID id) : mID(id) { SetName(name);  }

        /// @brief destructor
        ~Entity() = default;

        /// @brief returns the entity name
        inline const char* GetName() const { return mName; }

        /// @brief sets a new name for the entity
        inline void SetName(const char* name)
        {
            strncpy(mName, name ? name : "Entity Name", sizeof(mName) - 1);
            mName[sizeof(mName) - 1] = '\0';
        }

        /// @brief returns the entity id
        inline ID GetID() const { return mID; }

        /// @brief sets a new id for the entity
        inline void SetID(ID id) { mID = id; }

    public:

        /// @brief returns if the entity has a given component
        template<typename T>
        bool HasComponent()
        {
            return mComponents.find(typeid(T)) != mComponents.end();
        }

        /// @brief returns desired the component's memory address, nullptr otherwise
        template<typename T>
        T* GetComponent()
        {
            auto it = mComponents.find(typeid(T));
            if (it != mComponents.end()) {
                try {
                    return std::any_cast<T>(&it->second);
                }
                catch (const std::bad_any_cast&) {
                    return nullptr;
                }
            }
            return nullptr;
        }

        /// @brief adds a unique type of component to the entity
        template<typename T, typename... Args>
        void AddComponent()
        {
            if (HasComponent<T>()) return;

            // construct in-place with perfect forwarding
            auto [it, inserted] = mComponents.try_emplace(typeid(T), std::in_place_type<T>, std::forward<Args>(args)...);
        }

        /// @brief erases the component from the entity
        template<typename T>
        bool RemoveComponent()
        {
            auto it = mComponents.find(typeid(T));
            if (it != mComponents.end()) {
                mComponents.erase(it);
                return true;
            }
            return false;
        }
    private:

        char mName[32];
        ID mID;
        std::unordered_map<std::type_index, std::any> mComponents;
    };
}