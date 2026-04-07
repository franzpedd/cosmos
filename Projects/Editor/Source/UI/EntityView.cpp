#include "EntityView.h"

namespace Cosmos
{
    EntityView::EntityView(ApplicationBase* app)
        : WidgetBase("Entity View Window", true), mApp(app)
    {}

    EntityView::~EntityView()
    {}

    void EntityView::Update()
    {
        DisplayTest();
    }

    void EntityView::Render()
    {}

    void EntityView::DisplayTest()
    {
        if (ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginChild("##EntityList", ImGui::GetContentRegionAvail(), ImGuiChildFlags_FrameStyle))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

                for (auto& entry : mApp->GetWorld()->GetEntitiesLibraryRef().GetAllRef())
                {
                    if (!entry.second) continue;

                    ImGui::PushID(entry.second);
                    if (ImGui::TreeNodeEx(entry.second, ImGuiTreeNodeFlags_Framed, "%s", entry.second->GetName()))
                    {
                        ImGui::SeparatorText("General Info");

                        // ID
                        {
                            ;
                            ImGui::Text("ID:   ");
                            ImGui::SameLine();
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
                            ImGui::BeginDisabled();
                            ImGui::InputScalarN("##IDTag", ImGuiDataType_U32, &entry.second->GetID(), 1, NULL, NULL, "%u", ImGuiInputTextFlags_ReadOnly);
                            ImGui::EndDisabled();
                            ImGui::PopStyleVar();

                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                ImGui::SetTooltip("IDs cannot be changed");
                            }
                        }

                        ImGui::Separator();

                        // name
                        {
                            char buffer[128];
                            memset(buffer, 0, sizeof(buffer));
                            strncpy(buffer, entry.second->GetName(), sizeof(buffer));

                            ImGui::Text("Name: ");
                            ImGui::SameLine();
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
                            if (ImGui::InputText("##NameTag", buffer, sizeof(buffer))) {
                                entry.second->SetName(buffer);
                            }
                            ImGui::PopStyleVar();

                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                ImGui::SetTooltip("This name is displayed several places");
                            }
                        }

                        ImGui::Separator();

                        // visibile
                        {
                            if (entry.second->HasComponent<VisibilityComponent>()) {
                                ImGui::Checkbox("Visible", &entry.second->GetComponent<VisibilityComponent>()->visible);
                            }

                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                ImGui::SetTooltip("Will make such entity invisible/skipping it's rendering draw call");
                            }
                        }

                        ImGui::SeparatorText("2D Sprite");

                        // sprite
                        {
                            if (entry.second->HasComponent<SpriteComponent>())
                            {
                                SpriteComponent* sprite = entry.second->GetComponent<SpriteComponent>();

                                // transform
                                if (entry.second->HasComponent<TransformComponent>()) {

                                    float3& t = entry.second->GetComponent<TransformComponent>()->translation;
                                    float3& r = entry.second->GetComponent<TransformComponent>()->rotation;
                                    float3& s = entry.second->GetComponent<TransformComponent>()->scale;

                                    ImGui::Text("T:");
                                    ImGui::SameLine();
                                    if (Widget::Float3Control("T", &t.xyz.x, &t.xyz.y, &t.xyz.z)) {
                                        evk_sprite_set_translation(sprite->sprite, t);
                                    }

                                    ImGui::Text("R:");
                                    ImGui::SameLine();
                                    float3 radians = float3_fdegrees(&r);
                                    if (Widget::Float3Control("R", &radians.xyz.x, &radians.xyz.y, &radians.xyz.z)) {
                                        evk_sprite_set_rotation(sprite->sprite, r);
                                    }
                                    r = float3_fradians(&r);

                                    ImGui::Text("S:");
                                    ImGui::SameLine();
                                    if (Widget::Float2Control("S", &s.xyz.x, &s.xyz.y)) {
                                        evk_sprite_set_scale(sprite->sprite, float2{ s.xyz.x, s.xyz.y });
                                    }

                                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                        ImGui::SetTooltip("Controls the 2D position/rotation/scale of the sprite");
                                    }
                                }

                                ImGui::Separator();

                                // info
                                {
                                    ImGui::Text("EVK ID: %u", evk_sprite_get_id(sprite->sprite));
                                }

                                ImGui::Separator();

                                // albedo
                                {
                                    evkTexture2D* albedo = evk_sprite_get_albedo(sprite->sprite);
                                    constexpr ImVec2 imageSize = ImVec2(64, 64);
                                    float yPos = ImGui::GetCursorPosY() + (imageSize.y / 2) - (GUI::GetIconFontSize() / 2);

                                    char filename[256];
                                    filename_from_path(evk_texture2d_get_path(albedo), filename, sizeof(filename));

                                    ImGui::Image(mApp->GetGUI()->GetDescriptorImage(evk_texture2d_get_path(albedo)), imageSize);
                                    ImGui::SameLine();

                                    ImGui::SetCursorPosY(yPos);
                                    ImGui::Text("Albedo:");
                                    ImGui::SameLine();
                                    ImGui::SetCursorPosY(yPos);
                                    ImGui::SmallButton(filename);

                                    if (ImGui::BeginDragDropTarget()) {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXPLORER")) {
                                            evk_sprite_set_albedo(sprite->sprite, (const char*)payload->Data);
                                        }
                                        ImGui::EndDragDropTarget();
                                    }

                                    ImGui::SameLine();
                                    ImGui::SetCursorPosY(yPos);
                                    ImGui::TextDisabled(ICON_LC_INFO);

                                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                        ImGui::SetTooltip("Drag and drop an image from the Explorer");
                                    }
                                }
                            }
                        }

                        // billboard
                        {

                        }

                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                ImGui::PopStyleVar();

                ImGui::EndChild();
            }
        }

        ImGui::End();
    }
}