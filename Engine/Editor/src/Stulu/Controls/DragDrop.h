#pragma once
#include <Stulu/Scene/GameObject.h>
#include <Stulu/Scene/AssetsManager.h>
#include <imgui/imgui.h>

namespace Editor {
	namespace Controls {
        inline void DragDropGameObject(entt::entity gameObjectID) {
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("GAMEOBJECT", &gameObjectID, sizeof(entt::entity));
                ImGui::EndDragDropSource();
            }
        }
        inline entt::entity ReceiveDragDopGameObject() {
            entt::entity gameObjectID = entt::null;

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                    if (payload->DataSize == sizeof(entt::entity)) {
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
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_LIST")) {
                    size_t count = static_cast<size_t>(payload->DataSize / sizeof(entt::entity));
                    entt::entity* data = (entt::entity*)payload->Data;
                    gameobjects = std::vector<entt::entity>(data, data + count);

                    ImGui::EndDragDropTarget();
                    return true;
                }
                ImGui::EndDragDropTarget();
            }

            return false;
        }

        inline void DragDropAsset(const Stulu::UUID& asset, Stulu::AssetType type) {
            if (ImGui::BeginDragDropSource()) {
                std::string assetType = "ASSET_TYPE_" + std::to_string((uint32_t)type);
                ImGui::SetDragDropPayload(assetType.c_str(), &asset, sizeof(Stulu::UUID));
                ImGui::EndDragDropSource();
            }
        }
        inline Stulu::UUID ReceiveDragDopAsset(Stulu::AssetType type) {
            Stulu::UUID uuid;

            if (ImGui::BeginDragDropTarget()) {
                std::string assetType = "ASSET_TYPE_" + std::to_string((uint32_t)type);
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(assetType.c_str())) {
                    if (payload->DataSize == sizeof(Stulu::UUID)) {
                        uuid = *static_cast<Stulu::UUID*>(payload->Data);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            return uuid;
        }
	}
}