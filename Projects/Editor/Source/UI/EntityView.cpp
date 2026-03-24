#include "EntityView.h"

namespace Cosmos
{
	EntityView::EntityView(ApplicationBase* app)
		: WidgetBase("Entity View Window", true), mApp(app)
	{
	}

	EntityView::~EntityView()
	{
	}

	void EntityView::Update()
	{
		if (ImGui::Begin(ICON_LC_CIRCUIT_BOARD " Entity View"))
		{
			static const ImGuiTableFlags flags = ImGuiTableFlags_BordersH | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY;
            if (ImGui::BeginTable("EntitiesTable", 4, flags, ImVec2(0, ImGui::GetContentRegionAvail().y)))
            {
                ImGui::TableSetupColumn("##list", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (auto& entry : mApp->GetWorld()->GetEntitiesLibraryRef().GetAllRef())
                {
                    Entity* entity = entry.second;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    if (ImGui::Button(" " ICON_LC_SETTINGS " ")) {
                        mShowSettingsWindow = true;
                        mEntitySettings = entity;
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", entity->GetID());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("Generic");

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", entity->GetName());
                }

                ImGui::EndTable();
            }
		}
		ImGui::End();

        ShowEntitySettingsPopup(mEntitySettings);
	}

	void EntityView::Render()
	{
	}

    void EntityView::ShowEntitySettingsPopup(Entity* entity)
    {
        if (!entity) return;

        ImGui::PushID(entity);

        if (ImGui::Begin("Entity Config", &mShowSettingsWindow))
        {
            ImGui::SeparatorText("Config Components");

            ImGui::Text("ID: %d", entity->GetID());
            ImGui::Text("Name: %s", entity->GetName());

            if (entity->HasComponent<VisibilityComponent>())
            {
                ImGui::Checkbox("Visible", &entity->GetComponent<VisibilityComponent>()->visible);
            }

            ImGui::SeparatorText(ICON_LC_AXIS_3D " Transform");

            if (entity->HasComponent<TransformComponent>())
            {
                float3& translationRef = entity->GetComponent<TransformComponent>()->translation;
                float3& rotationRef = entity->GetComponent<TransformComponent>()->rotation;
                float3& scaleRef = entity->GetComponent<TransformComponent>()->scale;

                Widget::Float3Controller("Translation", &translationRef.xyz.x, &translationRef.xyz.y, &translationRef.xyz.z);
                Widget::Float3Controller("Rotation", &rotationRef.xyz.x, &rotationRef.xyz.y, &rotationRef.xyz.z);
                Widget::Float3Controller("Scale", &scaleRef.xyz.x, &scaleRef.xyz.y, &scaleRef.xyz.z);
            }

            ImGui::End();
        }
        ImGui::PopID();
    }
}