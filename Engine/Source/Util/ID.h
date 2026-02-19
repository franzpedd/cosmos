#pragma once

#include "Core/Defines.h"
#include <mutex>
#include <unordered_set>

namespace Cosmos
{
    class COSMOS_API ID
    {
    public:

        /// @brief constructor
        constexpr ID(uint32_t val = 0) : value(val) {}

        /// @brief destructor
        ~ID() = default;

        /// @brief returns the null id
        static constexpr ID GetNullID() { return ID{ 0 }; }

    public:

        // @brief returns the id value
        inline uint32_t GetValue() const { return value; }

        /// @brief returns the id value
        inline operator uint32_t() { return value; }

        /// @brief compares if equals to other id
        inline bool operator==(const ID& other) const { return value == other.value; }

        /// @brief compares if different from other id
        inline bool operator!=(const ID& other) const { return value != other.value; }

    private:
        uint32_t value;
    };

    class COSMOS_API IDGen
    {
    public:

        /// @brief creates an ID
        inline ID Create()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            uint32_t newId = ++mCounter;

            // prevent overflow
            if (newId == 0 || newId > MAX_ID) throw std::runtime_error("IDGen overflow");

            mActiveIds.insert(newId);
            return ID{ newId };
        }

        /// @brief creates an ID with a specific value
        inline bool Create(uint32_t idValue)
        {
            if (idValue == 0 || idValue > MAX_ID) return false;

            std::lock_guard<std::mutex> lock(mMutex);
            if (!mActiveIds.insert(idValue).second) return false; // already exists

            // update counter to avoid reusing lower IDs
            if (idValue > mCounter) mCounter = idValue;
            return true;
        }

        /// @brief removes an id
        inline bool Destroy(ID id)
        {
            if (id.GetValue() == 0) return false;

            std::lock_guard<std::mutex> lock(mMutex);
            return mActiveIds.erase(id.GetValue()) > 0;
        }

        /// @brief checks if id is active
        inline bool IsValid(ID id) const
        {
            if (id.GetValue() == 0) return false;

            std::lock_guard<std::mutex> lock(mMutex);
            return mActiveIds.find(id.GetValue()) != mActiveIds.end();
        }

        /// @brief clear all id's in use
        inline void Reset()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mCounter = 0;
            mActiveIds.clear();
        }

    private:

        static constexpr uint32_t MAX_ID = std::numeric_limits<uint32_t>::max() - 1;
        uint32_t mCounter = 0;
        std::unordered_set<uint32_t> mActiveIds;
        mutable std::mutex mMutex;
    };
}

/// @brief hash specialization must be in the std namespace
namespace std
{
    /// @brief hash algorithm for ID class (a uint32_t fancy wrapper)
    template<>
    struct hash<Cosmos::ID>
    {
        size_t operator()(const Cosmos::ID& id) const noexcept
        {
            return hash<uint32_t>{}(id.GetValue());
        }
    };
}