#pragma once
#include "BaseControls.h"
#include "DragDrop.h"
#include "Stulu/Style.h"
#include <Stulu.h>

namespace Editor {
    namespace Controls {
        inline bool MaterialEdit(Stulu::UUID& assetID) {

        }

        inline bool GameObjectFrame(const std::string& label, entt::entity& gameobjectID, const std::string& hint, const Stulu::Registry* registry) {
            const Stulu::Texture2D* icon = Resources::GetSceneIcon();
            const ImVec2 fontSize = ImVec2(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
            const float frameWidth = ImGui::GetContentRegionAvail().x - 1.0f;
            const ImVec2 origPos = ImGui::GetCursorPos();
            const ImVec2 origScreenPos = ImGui::GetCursorScreenPos();
            const ImVec2 spacing = ImGui::GetStyle().ItemInnerSpacing;
            const ImVec2 padding = ImGui::GetStyle().FramePadding;
            const bool hasValue = registry->IsValid(gameobjectID);
            bool changed = false;

            ImGui::SetCursorPosY(origPos.y - padding.y);
            // main frame
            const ImVec2 minFrame = ImGui::GetCursorScreenPos() - ImVec2(0, padding.y);
            const ImVec2 maxFrame = minFrame + ImVec2(frameWidth, fontSize.x + (2.0f * padding.y));
            if (ImGui::ItemHoverable({ minFrame, maxFrame }, 0, 0))
                ImGui::RenderFrame(minFrame, maxFrame, ImGui::GetColorU32(ImGuiCol_FrameBgHovered), true, ImGui::GetStyle().FrameRounding);
            else
                ImGui::RenderFrame(minFrame, maxFrame, ImGui::GetColorU32(ImGuiCol_FrameBg), true, ImGui::GetStyle().FrameRounding);

            // draw icon
            const ImVec2 ImagePos = ImVec2(origPos.x + padding.x, origPos.y - padding.y);
            float imageOffset = padding.x;
            if (icon) {
                if (hasValue && icon) {
                    const float aspect = (float)icon->getHeight() / (float)icon->getWidth();
                    const ImVec2 size = ImVec2(fontSize.x * aspect, fontSize.y);
                    ImGui::SetCursorPos(ImagePos);
                    ImGui::Image(icon, { size.x, size.y }, { 0,1 }, { 1,0 });
                    imageOffset += size.x;
                }
                // draw empty icon border
                else {
                    // empty frame
                    const ImVec2 min = origScreenPos + ImVec2(padding.x, -padding.y);
                    ImGui::GetWindowDrawList()->AddRect(min, min + fontSize, ImGui::GetColorU32(ImGuiCol_Border));
                    ImGui::Dummy(fontSize);
                    imageOffset += fontSize.x;
                }
                imageOffset += spacing.x;
            }

            // draw dummy around frame for drag drop
            const ImVec2 size = ImVec2(frameWidth - (2.0f * padding.x), fontSize.y);
            ImGui::SetCursorPos(ImagePos);
            ImGui::Dummy(size);
            {
                entt::entity id = ReceiveDragDopGameObject();
                if (id != entt::null) {
                    gameobjectID = id;
                    changed = true;
                }
            }
           

            const float paddingYOff = ((padding.y / 2.0f) + 1.0f);
            // draw text
            const ImVec2 textMin = minFrame + ImVec2(imageOffset, 0.0f);
            const ImVec2 textMax = maxFrame - (hasValue ? ImVec2(fontSize.x + padding.x + spacing.x, 0.0f) : ImVec2(0.0f, 0.0f));

            if (!hint.empty()) {
                ImGui::BeginDisabled();
                ImGui::RenderTextClipped(textMin, textMax, hint.c_str(), 0, 0, ImVec2(0.5f, 0.5f));
                ImGui::EndDisabled();
            }

            if (hasValue) {
                const ImVec2 min = ImVec2(textMax.x + spacing.x, minFrame.y);
                const ImVec2 max = maxFrame - ImVec2(padding.x, 0.0f);
                const ImVec2 available = max - min;
                const ImVec2 pos = min + ImVec2((available.x - fontSize.x) * 0.5f, (available.y - fontSize.y) * 0.5f);

                if (ImGui::CloseButton(ImGui::GetID("##REMOVE"), pos)) {
                    gameobjectID = entt::null;
                    changed = true;
                }
            }
            return changed;
        }
        
        template<class T /* = AssetHandle<SharedAssetData> */>
        inline bool RenderAssetFrame(const std::string& label, Stulu::UUID& assetID, const std::string& hint, Stulu::Texture2D* icon, bool useIcon = true) {
            const ImVec2 fontSize = ImVec2(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
            const float frameWidth = ImGui::GetContentRegionAvail().x - 1.0f;
            const ImVec2 origPos = ImGui::GetCursorPos();
            const ImVec2 origScreenPos = ImGui::GetCursorScreenPos();
            const ImVec2 spacing = ImGui::GetStyle().ItemInnerSpacing;
            const ImVec2 padding = ImGui::GetStyle().FramePadding;
            const bool hasValue = Stulu::AssetsManager::GlobalInstance().TypeCheck<T>(assetID);
            const std::string typeStr = Stulu::AssetsManager::GlobalInstance().GetTypeNameT<T>();
            bool changed = false;

            ImGui::SetCursorPosY(origPos.y - padding.y);
            // main frame
            const ImVec2 minFrame = ImGui::GetCursorScreenPos() - ImVec2(0, padding.y);
            const ImVec2 maxFrame = minFrame + ImVec2(frameWidth, fontSize.x + (2.0f * padding.y));
            if (ImGui::ItemHoverable({ minFrame, maxFrame }, 0, 0))
                ImGui::RenderFrame(minFrame, maxFrame, ImGui::GetColorU32(ImGuiCol_FrameBgHovered), true, ImGui::GetStyle().FrameRounding);
            else
                ImGui::RenderFrame(minFrame, maxFrame, ImGui::GetColorU32(ImGuiCol_FrameBg), true, ImGui::GetStyle().FrameRounding);
        
            // draw icon
            const ImVec2 ImagePos = ImVec2(origPos.x + padding.x, origPos.y - padding.y);
            float imageOffset = padding.x;
            if (useIcon) {
                if (hasValue && icon) {
                    const float aspect = (float)icon->getHeight() / (float)icon->getWidth();
                    const ImVec2 size = ImVec2(fontSize.x * aspect, fontSize.y);
                    ImGui::SetCursorPos(ImagePos);
                    ImGui::Image(icon, { size.x, size.y }, { 0,1 }, { 1,0 });
                    imageOffset += size.x;
                }
                // draw empty icon border
                else {
                    // empty frame
                    const ImVec2 min = origScreenPos + ImVec2(padding.x, -padding.y);
                    ImGui::GetWindowDrawList()->AddRect(min, min + fontSize, ImGui::GetColorU32(ImGuiCol_Border));
                    ImGui::Dummy(fontSize);
                    imageOffset += fontSize.x;
                }
                imageOffset += spacing.x;
            }

            // draw dummy around frame for drag drop
            const ImVec2 size = ImVec2(frameWidth - (2.0f * padding.x), fontSize.y);
            ImGui::SetCursorPos(ImagePos);
            ImGui::Dummy(size);
            if (Stulu::UUID uuid = ReceiveDragDopAsset(typeStr)) {
                assetID = uuid;
                changed = true;
            }

            const float paddingYOff = ((padding.y / 2.0f) + 1.0f);
            // draw text
            const ImVec2 textMin = minFrame + ImVec2(imageOffset, 0.0f);
            const ImVec2 textMax = maxFrame - (hasValue ? ImVec2(fontSize.x + padding.x + spacing.x, 0.0f) : ImVec2(0.0f, 0.0f));

            if (!hint.empty()) {
                ImGui::BeginDisabled();
                ImGui::RenderTextClipped(textMin, textMax, hint.c_str(), 0, 0, ImVec2(0.5f, 0.5f));
                ImGui::EndDisabled();
            }

            if (hasValue) {
                const ImVec2 min = ImVec2(textMax.x + spacing.x, minFrame.y);
                const ImVec2 max = maxFrame - ImVec2(padding.x, 0.0f);
                const ImVec2 available = max - min;
                const ImVec2 pos = min + ImVec2((available.x - fontSize.x) * 0.5f, (available.y - fontSize.y) * 0.5f);

                if (ImGui::CloseButton(ImGui::GetID("##REMOVE"), pos)) {
                    assetID = Stulu::UUID::null;
                    changed = true;
                }
            }
            return changed;
        }

        inline bool Texture2D(const std::string& label, Stulu::UUID& textureID) {
            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                
                Stulu::Texture2D* texture = nullptr;
                std::string hint = "";
                if (Stulu::AssetsManager::GlobalInstance().TypeCheck<Stulu::Texture2DAsset>(textureID)) {
                    Stulu::Texture2DAsset asset = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::Texture2DAsset>(textureID);
                    texture = *asset;
                    hint = std::filesystem::path(asset.Path()).stem().string();
                }
                else {
                    hint = "Texture2D";
                }
                
                bool changed = RenderAssetFrame<Stulu::Texture2DAsset>(label, textureID, hint, texture, true);

                ImGui::PopItemWidth();
                return changed;
            });
        }

        inline bool Material(const std::string& label, Stulu::UUID& materialID) {
            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);

                std::string hint = "";
                if (Stulu::AssetsManager::GlobalInstance().TypeCheck<Stulu::MaterialAsset>(materialID)) {
                    Stulu::MaterialAsset asset = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::MaterialAsset>(materialID);
                    hint = asset.Path();
                }
                else {
                    hint = "No material";
                }

                bool changed = RenderAssetFrame<Stulu::MaterialAsset>(label, materialID, hint, nullptr, false);

                ImGui::PopItemWidth();
                return changed;
                });
        }
        
    }
}