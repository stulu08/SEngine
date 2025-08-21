#pragma once
#include <Stulu/Scene/GameObject.h>
#include <Stulu/Resources/AssetsManager.h>
#include <imgui/imgui.h>

namespace Editor {
	namespace Controls {
        // This handles hovering of Drag Drop sources, ImGui::ButtonBeahviour ignores this edge case
        inline bool DragDropHoverBtnPressed() {
            if (ImGui::IsDragDropActive()) {
                if (ImGui::IsItemHovered(
                    ImGuiHoveredFlags_AllowWhenBlockedByActiveItem |
                    ImGuiHoveredFlags_AllowWhenOverlapped |
                    ImGuiHoveredFlags_Stationary)) {

                    auto ctx = ImGui::GetCurrentContext();

                    const ImGuiID currentHoverId = ctx->HoverItemDelayId;
                    const ImGuiID lastHoveredID = ctx->HoverItemDelayIdPreviousFrame;
                    float& time = ctx->HoverItemDelayTimer;
                    
                    if (currentHoverId != lastHoveredID) {
                        time = 0.f;
                        return false;
                    }

                    const float DRAGDROP_HOLD_TO_OPEN_TIMER = 0.70f;
                    if (time >= DRAGDROP_HOLD_TO_OPEN_TIMER) {
                        time = 0.f;
                        return true;
                    }
                }
            }
            return false;
        }

        inline void DragDropGameObject(entt::entity gameObjectID) {
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("GAMEOBJECT", &gameObjectID, sizeof(entt::entity));
                ImGui::EndDragDropSource();
            }
        }
        inline entt::entity ReceiveDragDopGameObject() {
            entt::entity gameObjectID = entt::null;

            if (ImGui::BeginDragDropTarget()) {
                // also accept GAMEOBJECT_LIST with only one gameobject
                const ImGuiPayload* payload = ImGui::GetDragDropPayload();
                std::string type = std::string(payload->DataType);
                if (type.rfind("GAMEOBJECT", 0) == 0 && payload->DataSize == sizeof(entt::entity)) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str())) {
                        gameObjectID = *static_cast<entt::entity*>(payload->Data);
                    }
                }

                ImGui::EndDragDropTarget();
            }
            return gameObjectID;
        }

        inline void DragDropGameObjects(const std::vector<entt::entity>& gameobjects) {
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("GAMEOBJECT_LIST", gameobjects.data(), sizeof(entt::entity) * gameobjects.size());
                ImGui::EndDragDropSource();
            }
        }
        inline bool ReceiveDragDopGameObjects(std::vector<entt::entity>& gameobjects) {
            if (ImGui::BeginDragDropTarget()) {
                const ImGuiPayload* payload = ImGui::GetDragDropPayload();
                std::string type = std::string(payload->DataType);
                
                if (type.rfind("GAMEOBJECT", 0) == 0) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str())) {
                        size_t count = static_cast<size_t>(payload->DataSize / sizeof(entt::entity));

                        gameobjects.resize(count);
                        for (size_t i = 0; i < count; i++)
                            gameobjects[i] = ((entt::entity*)payload->Data)[i];

                        ImGui::EndDragDropTarget();
                        return true;
                    }
                }
                ImGui::EndDragDropTarget();
            }

            return false;
        }

        inline void DragDropAsset(const Stulu::UUID& asset, const std::string& type) {
            if (ImGui::BeginDragDropSource()) {
                std::string assetType = "ASSET_TYPE_" + type;
                ImGui::SetDragDropPayload(assetType.c_str(), &asset, sizeof(Stulu::UUID));
                ImGui::EndDragDropSource();
            }
        }
        inline Stulu::UUID ReceiveDragDopAsset(const std::string& type, bool anyAsset = false) {
            Stulu::UUID uuid = Stulu::UUID::null;

            if (ImGui::BeginDragDropTarget()) {
                if (!anyAsset) {
                    std::string assetType = "ASSET_TYPE_" + type;
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(assetType.c_str())) {
                        if (payload->DataSize == sizeof(Stulu::UUID)) {
                            uuid = *static_cast<Stulu::UUID*>(payload->Data);
                        }
                    }
                }
                else {
                    const ImGuiPayload* payload = ImGui::GetDragDropPayload();
                    std::string type = std::string(payload->DataType);
                    if (type.rfind("ASSET_TYPE_", 0) == 0) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str())) {
                            if (payload->DataSize == sizeof(Stulu::UUID)) {
                                uuid = *static_cast<Stulu::UUID*>(payload->Data);
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            return uuid;
        }
	}
}