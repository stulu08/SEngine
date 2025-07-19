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
			const ImGuiStyle& style = ImGui::GetStyle();
			const ImVec2 fontSize(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
			const float frameWidth = ImGui::CalcItemWidth() - 1.0f;

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImGuiID itemID = window->GetID(label.c_str());
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();

			const bool hasValue = registry && registry->IsValid(gameobjectID);
			bool changed = false;

			ImRect frameBB(cursorPos, cursorPos + ImVec2(frameWidth, fontSize.y + style.FramePadding.y * 2.0f));
			ImGui::ItemSize(frameBB, style.FramePadding.y);
			if (!ImGui::ItemAdd(frameBB, itemID))
				return false;

			ImVec2 closeButtonSize(fontSize.x + style.FramePadding.x, 0.0f);
			ImRect buttonBB(frameBB.Min, frameBB.Max - closeButtonSize);

			bool hovered, held;
			bool pressed = ImGui::ButtonBehavior(buttonBB, itemID, &hovered, &held);

			if (hasValue)
				DragDropGameObject(gameobjectID);

			entt::entity dragDropEntityID = ReceiveDragDopGameObject();
			if (dragDropEntityID != entt::null) {
				gameobjectID = dragDropEntityID;
				changed = true;
			}

			ImU32 frameColor = ImGui::GetColorU32(
				ImGui::GetCurrentContext()->ActiveId == itemID ? ImGuiCol_FrameBgActive :
				hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

			ImGui::RenderNavCursor(frameBB, itemID);
			ImGui::RenderFrame(frameBB.Min, frameBB.Max, frameColor, true, style.FrameRounding);


			float imageOffset = style.FramePadding.x + style.ItemInnerSpacing.x;
			ImVec2 iconPos = frameBB.Min + style.FramePadding;
			if (icon) {
				float aspect = (float)icon->getHeight() / (float)icon->getWidth();
				ImVec2 iconSize(fontSize.x * aspect, fontSize.y);
				ImGui::GetWindowDrawList()->AddImage(ImGui::StuluTextureToImGui(icon), iconPos, iconPos + iconSize, ImVec2(0, 1), ImVec2(1, 0));
				imageOffset += iconSize.x;
			}
			else {
				ImGui::GetWindowDrawList()->AddRect(iconPos, iconPos + fontSize, ImGui::GetColorU32(ImGuiCol_Border));
				imageOffset += fontSize.x;
			}


			ImVec2 textMin = frameBB.Min + ImVec2(imageOffset, 0.0f);
			ImVec2 textMax = frameBB.Max;
			if (hasValue)
				textMax.x -= (fontSize.x + style.FramePadding.x + style.ItemInnerSpacing.x);

			if (!hint.empty()) {
				ImGui::BeginDisabled();
				ImGui::RenderTextClipped(textMin, textMax, hint.c_str(), nullptr, nullptr, ImVec2(0.5f, 0.5f));
				ImGui::EndDisabled();
			}

			if (hasValue) {
				ImVec2 closeMin = ImVec2(textMax.x + style.ItemInnerSpacing.x, frameBB.Min.y);
				ImVec2 closeMax = frameBB.Max - ImVec2(style.FramePadding.x, 0.0f);
				ImVec2 available = closeMax - closeMin;
				ImVec2 closePos = closeMin + ImVec2((available.x - fontSize.x) * 0.5f, (available.y - fontSize.y) * 0.5f);

				if (ImGui::CloseButton(ImGui::GetID("##REMOVE"), closePos)) {
					gameobjectID = entt::null;
					changed = true;
				}
			}

			if (changed)
				ImGui::MarkItemEdited(itemID);

			return changed;
		}
		
		template<class T /* = AssetHandle<SharedAssetData> */>
		inline bool RenderAssetFrame(const std::string& label, Stulu::UUID& assetID, const std::string& hint, Stulu::Texture2D* icon, bool useIcon = true) {
			const ImGuiStyle& style = ImGui::GetStyle();
			const ImVec2 fontSize(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
			const float frameWidth = ImGui::CalcItemWidth() - 1.0f;

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImGuiID itemID = window->GetID(label.c_str());
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();

			const bool hasValue = Stulu::AssetsManager::GlobalInstance().TypeCheck<T>(assetID);
			const std::string typeStr = Stulu::AssetsManager::GlobalInstance().GetTypeNameT<T>();
			bool changed = false;

			ImRect frameBB(cursorPos, cursorPos + ImVec2(frameWidth, fontSize.y + style.FramePadding.y * 2.0f));
			ImGui::ItemSize(frameBB, style.FramePadding.y);
			if (!ImGui::ItemAdd(frameBB, itemID))
				return false;

			ImVec2 closeButtonSize(fontSize.x + style.FramePadding.x, 0.0f);
			ImRect buttonBB(frameBB.Min, frameBB.Max - closeButtonSize);

			bool hovered, held;
			bool pressed = ImGui::ButtonBehavior(buttonBB, itemID, &hovered, &held);

			if (hasValue)
				DragDropAsset(assetID, typeStr);

			if (Stulu::UUID uuid = ReceiveDragDopAsset(typeStr)) {
				assetID = uuid;
				changed = true;
			}

			ImU32 frameColor = ImGui::GetColorU32(
				ImGui::GetCurrentContext()->ActiveId == itemID ? ImGuiCol_FrameBgActive :
				hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

			ImGui::RenderNavCursor(frameBB, itemID);
			ImGui::RenderFrame(frameBB.Min, frameBB.Max, frameColor, true, style.FrameRounding);

			float imageOffset = style.FramePadding.x;
			if (useIcon) {
				ImVec2 iconPos = frameBB.Min + style.FramePadding;

				if (hasValue && icon) {
					float aspect = (float)icon->getHeight() / (float)icon->getWidth();
					ImVec2 iconSize(fontSize.x * aspect, fontSize.y);
					ImGui::GetWindowDrawList()->AddImage(ImGui::StuluTextureToImGui(icon), iconPos, iconPos + iconSize, ImVec2(0, 1), ImVec2(1, 0));
					imageOffset += iconSize.x;
				}
				else {
					ImGui::GetWindowDrawList()->AddRect(iconPos, iconPos + fontSize, ImGui::GetColorU32(ImGuiCol_Border));
					imageOffset += fontSize.x;
				}
				imageOffset += style.ItemInnerSpacing.x;
			}

			ImVec2 textMin = frameBB.Min + ImVec2(imageOffset, 0.0f);
			ImVec2 textMax = frameBB.Max;
			if (hasValue)
				textMax.x -= (fontSize.x + style.FramePadding.x + style.ItemInnerSpacing.x);

			if (!hint.empty()) {
				ImGui::BeginDisabled();
				ImGui::RenderTextClipped(textMin, textMax, hint.c_str(), nullptr, nullptr, ImVec2(0.5f, 0.5f));
				ImGui::EndDisabled();
			}

			if (hasValue) {
				ImVec2 closeMin = ImVec2(textMax.x + style.ItemInnerSpacing.x, frameBB.Min.y);
				ImVec2 closeMax = frameBB.Max - ImVec2(style.FramePadding.x, 0.0f);
				ImVec2 available = closeMax - closeMin;
				ImVec2 closePos = closeMin + ImVec2((available.x - fontSize.x) * 0.5f, (available.y - fontSize.y) * 0.5f);

				if (ImGui::CloseButton(ImGui::GetID("##REMOVE"), closePos)) {
					assetID = Stulu::UUID::null;
					changed = true;
				}
			}

			if (changed)
				ImGui::MarkItemEdited(itemID);

			return changed;
		}

		inline bool GameObject(const std::string& label, entt::entity& goID, Stulu::Registry* registry) {
			return LabeledBaseControl(label, [&]() {
				ImGui::PushItemWidth(-1);

				std::string hint = "";
				if (registry && registry->IsValid(goID)) {
					Stulu::GameObject gameObject(goID, registry);
					hint = gameObject.getComponent<Stulu::GameObjectBaseComponent>().name;
				}
				else {
					hint = "GameObject";
				}

				bool changed = GameObjectFrame(label, goID, hint, registry);

				ImGui::PopItemWidth();
				return changed;
				});
		}

		inline bool GameObject(const std::string& label, Stulu::GameObject& gameObject) {
			entt::entity goID = gameObject.GetID();
			bool changed = GameObject(label, goID, gameObject.GetRegistry());
			if (changed) {
				gameObject = Stulu::GameObject(goID, gameObject.GetRegistry());
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
					hint = asset->GetName();
				}
				else {
					hint = "No Material";
				}

				bool changed = RenderAssetFrame<Stulu::MaterialAsset>(label, materialID, hint, nullptr, false);

				ImGui::PopItemWidth();
				return changed;
				});
		}
		inline bool Mesh(const std::string& label, Stulu::UUID& meshID) {
			return LabeledBaseControl(label, [&]() {
				ImGui::PushItemWidth(-1);

				std::string hint = "";
				if (Stulu::AssetsManager::GlobalInstance().TypeCheck<Stulu::MeshAsset>(meshID)) {
					Stulu::MeshAsset asset = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::MeshAsset>(meshID);
					hint = asset->GetName();
				}
				else {
					hint = "No Mesh";
				}

				bool changed = RenderAssetFrame<Stulu::MeshAsset>(label, meshID, hint, nullptr, false);

				ImGui::PopItemWidth();
				return changed;
				});
		}
		template<class IntType, uint32_t kMinRes = 128, uint32_t kMaxRes = 8192>
		inline bool ResolutionCombo(const std::string& label, IntType& currentRes) {
			static constexpr int kNumEntries = static_cast<int>(Stulu::Math::log2(kMaxRes / kMinRes));

			static std::array<const char*, kNumEntries> items = []() {
				std::array<const char*, kNumEntries> temp{};
				static std::array<std::string, kNumEntries> labels{};
				for (int i = 0; i < kNumEntries; ++i) {
					uint32_t res = kMinRes << i;
					labels[i] = std::to_string(res);
					temp[i] = labels[i].c_str();
				}
				return temp;
				}();

			int currentIndex = std::clamp<int>((int)std::log2(static_cast<uint32_t>(currentRes) / kMinRes), 0, kNumEntries - 1);

			bool changed = LabeledBaseControl(label, [&]() {
				ImGui::PushItemWidth(-1);
				bool re = ImGui::Combo("##resolution_combo", &currentIndex, items.data(), kNumEntries);
				if (re) {
					uint32_t newRes = kMinRes << currentIndex;
					currentRes = static_cast<IntType>(newRes);
				}
				ImGui::PopItemWidth();
				return re;
				});

			return changed;
		}

	}
}