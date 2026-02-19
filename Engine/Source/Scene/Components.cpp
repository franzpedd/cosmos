#include "Components.h"

#include "Entity.h"

namespace Cosmos
{
    TransformComponent::TransformComponent(float3 translation, float3 rotation, float3 scale)
        : translation(translation), rotation(rotation), scale(scale)
    {
    }

    void TransformComponent::Save(Entity* entity, Datafile& datafile)
    {
		if (entity->HasComponent<TransformComponent>()) {
			std::string uuid = std::to_string(entity->GetID());
			auto* component = entity->GetComponent<TransformComponent>();
			auto& place = datafile[uuid]["Transform"];

			place["Translation"]["X"].SetDouble(component->translation.xyz.x);
			place["Translation"]["Y"].SetDouble(component->translation.xyz.y);
			place["Translation"]["Z"].SetDouble(component->translation.xyz.z);

			place["Rotation"]["X"].SetDouble(component->rotation.xyz.x);
			place["Rotation"]["Y"].SetDouble(component->rotation.xyz.y);
			place["Rotation"]["Z"].SetDouble(component->rotation.xyz.z);

			place["Scale"]["X"].SetDouble(component->scale.xyz.x);
			place["Scale"]["Y"].SetDouble(component->scale.xyz.y);
			place["Scale"]["Z"].SetDouble(component->scale.xyz.z);
		}
    }

	void TransformComponent::Load(Entity* entity, Datafile& datafile)
	{
		if (datafile.Exists("Transform")) {
			entity->AddComponent<TransformComponent>();
			auto* component = entity->GetComponent<TransformComponent>();

			auto& dataT = datafile["Transform"]["Translation"];
			component->translation = { (float)dataT["X"].GetDouble(), (float)dataT["Y"].GetDouble(), (float)dataT["Z"].GetDouble() };

			auto& dataR = datafile["Transform"]["Rotation"];
			component->rotation = { (float)dataR["X"].GetDouble(), (float)dataR["Y"].GetDouble(), (float)dataR["Z"].GetDouble() };

			auto& dataS = datafile["Transform"]["Scale"];
			component->scale = { (float)dataS["X"].GetDouble(), (float)dataS["Y"].GetDouble(), (float)dataS["Z"].GetDouble() };
		}
	}
}