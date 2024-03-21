#include "ComponentsRender.h"

#include "Editor/EditorApp.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace Stulu {

	template<typename T>
	void ComponentsRender::drawComponent(GameObject gameObject, T& component) {}
	template<>
	void ComponentsRender::drawComponent<GameObjectBaseComponent>(GameObject gameObject, GameObjectBaseComponent& component) {
		
		ImGui::Text(std::to_string((uint32_t)gameObject).c_str());
		drawStringControl("Name", gameObject.getComponent<GameObjectBaseComponent>().name);
		drawStringControl("Tag", gameObject.getComponent<GameObjectBaseComponent>().tag);
		
		ImGui::Separator();
	}
	template<>
	void ComponentsRender::drawComponent<TransformComponent>(GameObject gameObject, TransformComponent& transform) {
		
		bool updPhysx = false;
		updPhysx |= drawVector3Control("Position", transform.position);
		updPhysx |= drawVector3Control("Rotation", transform.eulerAnglesDegrees);
		drawVector3Control("Scale", transform.scale);
		transform.rotation = Math::EulerToQuaternion(glm::radians(transform.eulerAnglesDegrees));

		if (gameObject && updPhysx && getEditorLayer().isRuntime() && getEditorScene()->getData().enablePhsyics3D) {
			getEditorScene()->updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(gameObject);
		}
	}
	template<>
	void ComponentsRender::drawComponent<PostProcessingComponent>(GameObject gameObject, PostProcessingComponent& component) {
		auto& settings = component.data.settings;
		
		if (ImGui::CollapsingHeader("Gamma Correction")) {
			ImGui::PushID("GammaCorrection");
			auto& gammaCorrection = settings.gammaCorrection;

			drawControl("Enabled", gammaCorrection.enabled);

			if (!gammaCorrection.enabled)
				ImGui::BeginDisabled();

			drawControl("Gamma", gammaCorrection.gamma);
			drawControl("Exposure", gammaCorrection.exposure);

			if (!gammaCorrection.enabled)
				ImGui::EndDisabled();
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Bloom")) {
			ImGui::PushID("Bloom");
			auto& bloom = settings.bloom;

			drawControl("Enabled", bloom.enabled);

			if (!bloom.enabled)
				ImGui::BeginDisabled();

			drawControl("Clamp", bloom.clamp);
			drawFloatControl("Threshold", bloom.threshold, 0.0f, 2000.0f);
			drawFloatControl("Intensity", bloom.intensity, 0.0f, 2000.0f, 0.01f);
			drawFloatSliderControl("Knee", bloom.knee);
			drawUIntControl("Max Samples", bloom.maxSamples, 1);
			drawUIntSliderControl("Diffusion", bloom.diffusion, 1, bloom.maxSamples);
			drawControl("Resolution Scale", bloom.resolutionScale);

			if (!bloom.enabled)
				ImGui::EndDisabled();
			ImGui::PopID();
		}
	}
	template<>
	void ComponentsRender::drawComponent<CameraComponent>(GameObject gameObject, CameraComponent& component) {
		
		const char* const camTypes[] = { "Perspective","Orthographic" };
		if (ImGui::Combo("Type", (int*)&component.mode, camTypes, IM_ARRAYSIZE(camTypes)))
			component.updateMode();

		const char* const clerTypes[] = { "Color","Skybox" };
		ImGui::Combo("Clear Mode", (int*)&component.settings.clearType, clerTypes, IM_ARRAYSIZE(clerTypes));
		if (component.settings.clearType == CameraComponent::ClearType::Color)
			ImGui::ColorEdit4("Clear Color", glm::value_ptr(component.settings.clearColor));

		bool recalc = false;

		if (component.mode == CameraMode::Orthographic) {
			recalc |= drawFloatControl("Zoom", component.settings.zoom);
		}
		else if (component.mode == CameraMode::Perspective) {
			recalc |= drawFloatSliderControl("Fov", component.settings.fov, .0f, 180.0f);
		}
		recalc |= drawFloatControl("Far", component.settings.zFar, component.settings.zNear);

		recalc |= drawFloatControl("Near", component.settings.zNear, .001f, component.settings.zFar);

		recalc |= drawBoolControl("Is Render Target", component.settings.isRenderTarget);

		if (component.settings.isRenderTarget) {
			recalc |= drawFloatControl("Aspect Ratio", component.settings.aspectRatio);
			if (drawUIntControl("Width", component.settings.textureWidth)) {

				component.updateSize();
			}
			if (drawUIntControl("Height", component.settings.textureHeight)) {

				component.updateSize();
			}
			if(drawTextureEdit("Render Texture", component.renderTexture, false))
				component.updateSize();
		}
		else {
			if (drawIntControl("Depth", component.depth))
				component.depth = glm::max(component.depth, 0);
		}
		if (recalc)
			component.updateProjection();

		static bool showPreview = false;
		std::string popupName = std::string(gameObject.getComponent<GameObjectBaseComponent>().name + " Preview");
		if (ImGui::Button("Show Preview"))
			ImGui::OpenPopup(popupName.c_str());
		if (ImGui::BeginPopup(popupName.c_str())) {
			ImGui::Image(component.getTexture(), glm::vec2((float)component.settings.textureWidth, (float)component.settings.textureHeight) * 0.25f, glm::vec2(0, 1), glm::vec2(1, 0));
			//ImGui::Image(reinterpret_cast<void*>((uint64_t)component.getTexture()->getColorAttachmentRendereID()), ImVec2((float)component.settings.textureWidth, (float)component.settings.textureHeight) * 0.25f, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndPopup();
		}
		
	}
	template<>
	void ComponentsRender::drawComponent<SkyBoxComponent>(GameObject gameObject, SkyBoxComponent& component) {
		
		UUID uuid = UUID::null;
		if (component.texture) {
			if (AssetsManager::existsAndType(component.texture->uuid, AssetType::SkyBox))
				uuid = component.texture->uuid;
			else
				component.texture = nullptr;
		}
		else
			ImGui::Text("Drag texture");
		if (drawTextureEdit("Texture", uuid, true)) {
			if (uuid != UUID::null && AssetsManager::exists(uuid))
				component.texture = std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data));
			else
				component.texture = nullptr;
		}
		drawVector3Control("Rotation", component.rotation);
		drawComboControl("Map Type", (int&)component.mapType, "Environment Map\0Irradiance Map\0Prefilter Map");

	}
	template<>
	void ComponentsRender::drawComponent<SpriteRendererComponent>(GameObject gameObject, SpriteRendererComponent& component) {
		
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		drawVector2Control("Tiling", component.tiling);
		UUID uuid = UUID::null;
		if (component.texture) {
			Asset asset;
			if (AssetsManager::saveGet(component.texture->uuid, asset)) {
				if(asset.type == AssetType::Texture2D || asset.type == AssetType::RenderTexture)
					uuid = component.texture->uuid;
				else
					component.texture = nullptr;
			}
			else
				component.texture = nullptr;
		}
		else
			ImGui::Text("Drag texture");
		if (drawTextureEdit("Texture", uuid)) {
			if (uuid != UUID::null && AssetsManager::exists(uuid))
				component.texture = std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data);
			else
				component.texture = nullptr;
		}
	}
	template<>
	void ComponentsRender::drawComponent<CircleRendererComponent>(GameObject gameObject, CircleRendererComponent& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		drawFloatControl("Thickness", component.thickness, 0.0f, 1.0f, 0.025f);
		drawFloatControl("Fade", component.fade, .0f, 1.0f, 0.00025f);
	}
	template<>
	void ComponentsRender::drawComponent<NativeScriptComponent>(GameObject gameObject, NativeScriptComponent& component) {
		if(component.instance)
			component.instance->uiFunc();
	}
	template<>
	void ComponentsRender::drawComponent<ParticleSystemComponent>(GameObject gameObject, ParticleSystemComponent& component) {
		ParticleSystemData& data = component.getData();
		drawEnumComboControl("Emit Type", data.emitStart);
		drawUIntControl("Emit count", data.emitCount);

		drawBoolControl("Moving", data.moving);
		if (data.moving) {
			drawFloatControl("Speed", data.speed);
			drawFloatControl("Gravity", data.gravity);
			drawBoolControl("Random Velocity", data.randomVelcity);
			if (data.randomVelcity) {
				drawVector3Control("Min Velocity", data.minVelocity);
				drawVector3Control("Max Velocity", data.maxVelocity);
			}
			else
				drawVector3Control("Velocity", data.velocity);
		}

		ImGui::Checkbox("Dying", &data.imortal);
		if (data.imortal) {
			ImGui::Checkbox("Random Lifetime", &data.randomLifeTime);
			if (data.randomLifeTime) {
				drawFloatControl("Min Lifetime", data.randomLifeTimeMin);
				drawFloatControl("Max Lifetime", data.randomLifeTimeMax);
			}
			else
				drawFloatControl("Lifetime", data.lifeTime);
		}
		ImGui::Checkbox("Rotate", &data.rotate);
		if (data.rotate) {

			ImGui::Checkbox("Random Start Rotation", &data.randomStartRotation);
			if (!data.randomStartRotation)
				drawFloatControl("Start Rotation", data.startRotation);

			ImGui::Checkbox("Random Rotation Speed", &data.randomRotationSpeed);
			if (data.randomRotationSpeed)
				drawVector2Control("Random Rotation Speed Range", data.randomRotationRange);
			else
				drawFloatControl("Rotation Speed", data.rotationSpeed);
		}
		ImGui::Checkbox("Random Size", &data.randomSize);
		if (!data.randomSize) {
			drawVector3Control("Begin Size", data.endSize);
			drawVector3Control("End Size", data.beginSize);
		}
		else {
			drawVector3Control("Begin Size min", data.beginSizeMin);
			drawVector3Control("Begin Size max", data.beginSizeMax);
			drawVector3Control("End Size min", data.endSizeMin);
			drawVector3Control("End Size max", data.endSizeMax);
		}
		ImGui::ColorEdit4("Begin Color", glm::value_ptr(data.beginColor));
		ImGui::ColorEdit4("End Color", glm::value_ptr(data.endColor));
	}
	template<>
	void ComponentsRender::drawComponent<LightComponent>(GameObject gameObject, LightComponent& component) {
		
		const char* const lightTypes[] = { "Directional","Area","Spot" };
		ImGui::Combo("Type", (int*)&component.lightType, lightTypes, IM_ARRAYSIZE(lightTypes));
		ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
		if(drawFloatControl("Strength", component.strength))
			component.strength = glm::max(component.strength, .0f);
		if (component.lightType == LightComponent::Spot) {
			if (drawFloatControl("Cut off", component.spotLight_cutOff))
				component.spotLight_cutOff = glm::max(component.spotLight_cutOff, .0f);
			if (drawFloatControl("Outer cut off", component.spotLight_outerCutOff))
				component.spotLight_outerCutOff = glm::max(component.spotLight_outerCutOff, .0f);
		}
		else if (component.lightType == LightComponent::Area) {
			if (drawFloatControl("Radius", component.areaRadius))
				component.areaRadius = glm::max(component.areaRadius, .0f);
		}

	}
	template<>
	void ComponentsRender::drawComponent<MeshRendererComponent>(GameObject gameObject, MeshRendererComponent& component) {
		
		drawComboControl("CullMode", (int&)component.cullmode, {"Back","Front","Both"});

		UUID id = UUID::null;
		if (component.material && AssetsManager::exists(component.material->getUUID())) {
			ImGui::Text("Material: %s (Shader: %s)", component.material->getName().c_str(), component.material->getShader()->getName().c_str());
			id = component.material->getUUID();
		}
		else {
			ImGui::Text("Material: Using default Renderer Shader");
			if (drawMaterialEdit("Drag Material", id)) {
				component.material = std::any_cast<Ref<Material>>(AssetsManager::get(id).data);
				component.material->uploadData();
			}
			return;
		}
		if (drawMaterialEdit(component.material->getName(), id)) {
			component.material = std::any_cast<Ref<Material>>(AssetsManager::get(id).data);
			component.material->uploadData();
		}
	}
	template<>
	void ComponentsRender::drawComponent<MeshFilterComponent>(GameObject gameObject, MeshFilterComponent& component) {
		

		if (drawMeshEdit("Drag Mesh", component.mesh.uuid)) {
			if (AssetsManager::existsAndType(component.mesh.uuid, AssetType::Mesh)) {
				component.mesh = std::any_cast<MeshAsset>(AssetsManager::get(component.mesh.uuid).data);
			}
		}
		if (component.mesh.hasMesh) {
			Ref<Mesh>& mesh = component.mesh.mesh;
			if (mesh->getSubMeshCount()) {
				ImGui::Text("SubMesh Count: %d", (uint32_t)component.mesh.mesh->getSubMeshCount());
				for (uint32_t i = 0; i < mesh->getSubMeshCount(); i++) {
					SubMesh& m = mesh->getSubMesh(i);
					if (ImGui::TreeNodeEx((void*)((uint64_t)mesh->getSubMeshCount() + i + (uint32_t)gameObject), ImGuiTreeNodeFlags_OpenOnArrow, "Submesh %d", i)) {
						ImGui::Text("Vertices: %d", m.getVerticesCount());
						ImGui::Text("Indices: %d", m.getIndicesCount());
						ImGui::Text("Triangles: %d", m.getIndicesCount() / 3);
						ImGui::TreePop();
					}
				}
			}
			else {
				ImGui::Text("Vertices: %d", component.mesh.mesh->getVerticesCount());
				ImGui::Text("Indices: %d", component.mesh.mesh->getIndicesCount());
				ImGui::Text("Triangles: %d", component.mesh.mesh->getIndicesCount() / 3);
			}
		}
		else {
			ImGui::Text("Not a valid mesh drag one here");
		}
	}
	template<>
	void ComponentsRender::drawComponent<RigidbodyComponent>(GameObject gameObject, RigidbodyComponent& component) {
		
		bool change = false;
		change |= drawBoolControl("Use gravity", component.useGravity);
		//bool 3
		{
			ImGui::PushID("Linear Velocity");
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Linear Velocity");
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

			change |= ImGui::Checkbox("##X", &component.moveX);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Y", &component.moveY);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Z", &component.moveZ);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
		}
		//bool 3
		{
			ImGui::PushID("Angular Velocity");
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Angular Velocity");
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

			change |= ImGui::Checkbox("##X", &component.rotationX);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Y", &component.rotationY);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			change |= ImGui::Checkbox("##Z", &component.rotationZ);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
		}
		change |= drawFloatControl("Mass", component.mass);
		change |= drawVector3Control("Center of mass", component.massCenterPos);
		change |= drawBoolControl("Kinematic", component.kinematic);
		change |= drawBoolControl("Retain acceleration", component.retainAccelaration);
		if (change)
			component.updateFlags();
	}
	template<>
	void ComponentsRender::drawComponent<BoxColliderComponent>(GameObject gameObject, BoxColliderComponent& component) {
		
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawVector3Control("Size", component.size);
	}
	template<>
	void ComponentsRender::drawComponent<SphereColliderComponent>(GameObject gameObject, SphereColliderComponent& component) {
		
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawFloatControl("Size", component.radius);
	}
	template<>
	void ComponentsRender::drawComponent<CapsuleColliderComponent>(GameObject gameObject, CapsuleColliderComponent& component) {
		
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
		drawVector3Control("Offset", component.offset);
		drawFloatControl("Height", component.height);
		drawFloatControl("Radius", component.radius);
		drawBoolControl("Horizontal", component.horizontal);
	}
	template<>
	void ComponentsRender::drawComponent<MeshColliderComponent>(GameObject gameObject, MeshColliderComponent& component) {
		
		if (drawMeshEdit("Drag Mesh", component.mesh.uuid)) {
			if (AssetsManager::existsAndType(component.mesh.uuid, AssetType::Mesh)) {
				component.mesh = std::any_cast<MeshAsset>(AssetsManager::get(component.mesh.uuid).data);
				component.convexMesh = nullptr;
			}
		}
		if (component.mesh.hasMesh && component.convexMesh == nullptr) {
			if (component.mesh.mesh->getVerticesCount() > 255 * 3) {
				component.convexMesh = createRef<Mesh>(Mesh::copyAndLimit(component.mesh.mesh, 255 * 3));
			}
			else {
				component.convexMesh = component.mesh.mesh;
			}
		}
		if (component.mesh.hasMesh && component.convexMesh) {
			
			Ref<Mesh>& mesh = (component.convex ? component.convexMesh : component.mesh.mesh);

			if (mesh->getSubMeshCount()) {
				ImGui::Text("SubMesh Count: %d", (int)component.mesh.mesh->getSubMeshCount());
				for (uint32_t i = 0; i < mesh->getSubMeshCount(); i++) {
					SubMesh& m = mesh->getSubMesh(i);
					if (ImGui::TreeNodeEx((void*)((uint64_t)mesh->getSubMeshCount() + i + (uint32_t)gameObject), ImGuiTreeNodeFlags_OpenOnArrow, "Submesh %d", i)) {
						ImGui::Text("Vertices: %d", m.getVerticesCount());
						ImGui::Text("Indices: %d", m.getIndicesCount());
						ImGui::Text("Triangles: %d", m.getIndicesCount() / 3);
						ImGui::TreePop();
					}
				}
			}
			else {
				ImGui::Text("Vertices: %d", mesh->getVerticesCount());
				ImGui::Text("Indices: %d", mesh->getIndicesCount());
				ImGui::Text("Triangles: %d", mesh->getIndicesCount() / 3);
			}
		}
		else {
			component.convexMesh = nullptr;
			ImGui::Text("Not a valid mesh drag one here");
		}

		drawBoolControl("Convex", component.convex);
		drawFloatControl("Static friction", component.staticFriction);
		drawFloatControl("Dynamic friction", component.dynamicFriction);
		drawFloatControl("Restitution", component.restitution);
	}



	bool ComponentsRender::drawStringControl(const std::string& header, std::string& v) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::InputText("##String", &v);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawBoolControl(const std::string& header, bool& v) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::Checkbox("##CheckBoxBool", &v);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawBoolBitFlagControl(const std::string& header, uint32_t flag, uint32_t& flags) {
		bool value = (flags & flag);
		if (ComponentsRender::drawBoolControl(header, value))
			if (value)
				flags |= flag;
			else
				flags &= ~flag;
		return value;
	}
	bool ComponentsRender::drawIntControl(const std::string& header, int& v) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragInt("##INT", &v);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawInt3Control(const std::string& header, int& x, int& y, int& z) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragInt("##X", &x);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragInt("##Y", &y);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragInt("##Z", &z);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawUIntControl(const std::string& header, uint32_t& v, uint32_t min, uint32_t max) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		int i = v;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragInt("##UINT", &i,1,0);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		if (change)
			v = (uint32_t)glm::max(i, 0);

		if (min != max && change) {
			if (min > max) {
				v = (uint32_t)glm::max((int)min, i);
			}
			else {
				v = (uint32_t)glm::clamp(i, (int)min, (int)max);
			}
		}

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawIntSliderControl(const std::string& header, int& v, int min, int max) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::SliderInt("##INTSLIDER", &v, min, max);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawUIntSliderControl(const std::string& header, uint32_t& v, uint32_t min, uint32_t max) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();
		int i = v;
		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::SliderInt("##UINTSLIDER", &i, min, max);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();
		if (change)
			v = glm::max(i, 0);
		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawFloatControl(const std::string& header, float& v, float min, float max, float speed) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::DragFloat("##FLOAT", &v, speed, min, max, "%.3f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		if(min != max && change) {
			if (min > max) {
				v = glm::max(min, v);
			}
			else {
				v = glm::clamp(v, min, max);
			}
		}

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawFloatSliderControl(const std::string& header, float& v, float min, float max) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::SliderFloat("##FLOATSLIDER", &v, min, max, "%.3f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector2Control(const std::string& header, glm::vec2& vec) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });


		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector3Control(const std::string& header, glm::vec3& vec) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector4Control(const std::string& header, glm::vec4& vec) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##W", &vec.w, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawVector4Control(const std::string& header, glm::quat& quat) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });

		change |= ImGui::DragFloat("##X", &quat.x, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Y", &quat.y, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##Z", &quat.z, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		change |= ImGui::DragFloat("##W", &quat.w, 0.1f, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawComboControl(const std::string& header, int32_t& current_item, const char* items_separated_by_zeros, int height_in_items) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		change |= ImGui::Combo("##COMBO", &current_item, items_separated_by_zeros, height_in_items);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawComboControl(const std::string& header, int32_t& current_item, const std::vector<std::string>& items) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		if (ImGui::BeginCombo(header.c_str(), items[current_item].c_str())) {
			for (int i = 0; i < items.size(); i++) {
				bool selected = items[current_item] == items[i];
				if (ImGui::Selectable(items[i].c_str(), selected)) {
					auto itr = std::find(items.begin(), items.end(), items[i]);
					if (itr != items.cend())
						current_item = (uint32_t)std::distance(items.begin(), itr);
					else
						current_item = 0;
					change = true;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawMat4Control(const std::string& header, glm::mat4& v) {
		
		bool change = false;
		ImGui::Text(header.c_str());
		for (int i = 0; i < 4; i++) {
			glm::vec4 vec = v[i];
			ImGui::PushID(("Row " + std::to_string(i+1)).c_str());
			ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text(("Row " + std::to_string(i + 1)).c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });


			change |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			change |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::SameLine();


			change |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::SameLine();


			change |= ImGui::DragFloat("##W", &vec.w, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::EndColumns();

			ImGui::PopID();
		}

		return change;
	}
	bool ComponentsRender::drawHDRColorEdit(const std::string& header, glm::vec4& color) {
		bool change = false;
		change |= ImGui::ColorEdit3((header + " Color").c_str(), glm::value_ptr(color), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
		change |= ImGui::DragFloat((header + " Intensity").c_str(), &color[3]);
		return change;
	}
	bool ComponentsRender::drawAssetControl(const std::string& header, UUID& uuid, AssetType type) {
		Asset asset = NullAsset;
		AssetsManager::saveGet(uuid, asset);

		if (type == AssetType::Texture2D) {
			return drawTextureEdit(header, uuid, false);
		}


		ImGui::Text("Invalid Asset type");
		return false;
	}

	void ComponentsRender::drawHelpMarker(const char* desc) {
		ImGui::HelpMarker(desc);
	}

	bool ComponentsRender::drawTextureEdit(const std::string& header, UUID& uuid, bool cubeMap) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		//void* texture = reinterpret_cast<void*>((uint64_t)EditorResources::getEmptySlotTexture()->getRendererID());
		Ref<Texture> texture = EditorResources::getEmptySlotTexture();

		if (AssetsManager::exists(uuid))
			if (cubeMap) {
				ImGui::Text("SkyBox");
				texture = Previewing::get().get(uuid);
			}
			else
				texture = std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data);
				//texture = reinterpret_cast<void*>((uint64_t)std::any_cast<Ref<Texture>>(AssetsManager::get(uuid).data)->getRendererID());


		ImGui::Image(texture, ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));

		if (ImGui::BeginDragDropTarget()) {
			if (cubeMap) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::SkyBox)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					change = true;
				}
			}
			else {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Texture2D)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					change = true;
				}
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::RenderTexture)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					change = true;
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Remove")) {
			uuid = UUID::null;
			change = true;
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawGameObjectEdit(const std::string& header, UUID& uuid) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		std::string name = "Drag to assign!";

		if (uuid != UUID::null) {
			GameObject go = GameObject::getById(uuid, Scene::activeScene());
			if (go && go.hasComponent<GameObjectBaseComponent>())
				name = go.getComponent<GameObjectBaseComponent>().name;
		}

		ImGui::BulletText("Gameobject: %s", name);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE")) {
				ST_ASSERT(payload->DataSize == sizeof(GameObject), "GAMEOBJECT_HIERARCHY_MOVE Drag Drop payload size wrong");
				GameObject go = *(const GameObject*)payload->Data;
				uuid = go.getId();
				change = true;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Remove")) {
			uuid = UUID::null;
			change = true;
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();
		return change;
	}
	bool ComponentsRender::drawMaterialEdit(const std::string& header,  UUID& uuid, bool canChange) {
		
		ImGui::PushID(header.c_str());
		bool changed = false;
		ImGui::BeginColumns(0, 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(header.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 3 });
		bool exists = uuid != UUID::null && AssetsManager::existsAndType(uuid, AssetType::Material);
		if (!exists)
			ImGui::Image(EditorResources::getEmptySlotTexture(), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
			//ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getEmptySlotTexture()->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		else {
			ImGui::Image(Previewing::get().get(uuid), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
			//ImGui::Image(reinterpret_cast<void*>((uint64_t)Previewing::get().get(uuid)->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		}
		if (canChange) {
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Material)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					uuid = id;
					changed = true;
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::EndColumns();

		ImGui::PopID();

		if (exists && !changed) {
			Asset& asset = AssetsManager::get(uuid);
			Ref<Material>& mat = std::any_cast<Ref<Material>>(asset.data);
			if (mat->isReadonly) {
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			std::vector<MaterialDataType>& materialData = mat->getData();
			for (auto& dataType : materialData) {
				std::string name = dataType.name;
				Stulu::ShaderDataType type = dataType.type;
				auto shaderEntry = mat->getShaderEntry();
				if (type == Stulu::ShaderDataType::Float) {
					float v = std::any_cast<float>(dataType.data);
					if (shaderEntry->HasProperity(name)) {
						Ref<ShaderProperity> prop = shaderEntry->GetProperity(name);
						if (prop->getType() == ShaderProperity::Type::Range) {
							Ref<ShaderProperityRange> rangeProp = std::static_pointer_cast<ShaderProperityRange>(prop);
							if (drawFloatSliderControl(name, v, rangeProp->getMin(), rangeProp->getMax())) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if(drawFloatControl(name, v)) {
						dataType.data = v;
						changed |= true;
					}
					
				}
				else if (type == Stulu::ShaderDataType::Float2) {
					glm::vec2 v = std::any_cast<glm::vec2>(dataType.data);
					if (drawVector2Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Float3) {
					glm::vec3 v = std::any_cast<glm::vec3>(dataType.data);
					if (shaderEntry->HasProperity(name)) {
						Ref<ShaderProperity> prop = shaderEntry->GetProperity(name);
						if (prop->getType() == ShaderProperity::Type::Color) {
							Ref<ShaderProperityColor> rangeProp = std::static_pointer_cast<ShaderProperityColor>(prop);
							if (ImGui::ColorEdit3(name.c_str(), glm::value_ptr(v))) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if (drawVector3Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Float4) {
					glm::vec4 v = std::any_cast<glm::vec4>(dataType.data);
					if (shaderEntry->HasProperity(name)) {
						Ref<ShaderProperity> prop = shaderEntry->GetProperity(name);
						if (prop->getType() == ShaderProperity::Type::Color) {
							Ref<ShaderProperityColor> colorProp = std::static_pointer_cast<ShaderProperityColor>(prop);
							if (colorProp->isHDR()) {
								if (drawHDRColorEdit(name, v)) {
									dataType.data = v;
									changed |= true;
								}
							}
							else {
								if (ImGui::ColorEdit4(name.c_str(), glm::value_ptr(v))) {
									dataType.data = v;
									changed |= true;
								}
							}
							continue;
						}
					}
					if (drawVector4Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Mat4) {
					glm::mat4 v = std::any_cast<glm::mat4>(dataType.data);
					if (drawMat4Control(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Int) {
					int v = std::any_cast<int>(dataType.data);
					if (shaderEntry->HasProperity(name)) {
						Ref<ShaderProperity> prop = shaderEntry->GetProperity(name);
						if (prop->getType() == ShaderProperity::Type::Enum) {
							Ref<ShaderProperityEnum> enumProp = std::static_pointer_cast<ShaderProperityEnum>(prop);
							if (drawComboControl(name, v, enumProp->getNames())) {
								dataType.data = v;
								changed |= true;
							}
							continue;
						}
					}
					if (drawIntControl(name, v)) {
						dataType.data = v;
						changed |= true;
					}
				}
				else if (type == Stulu::ShaderDataType::Sampler) {
					auto& t = std::any_cast<MaterialTexture&>(dataType.data);
					UUID tUUID = t.uuid;
					if (drawTextureEdit(name, tUUID, t.type == (int)AssetType::SkyBox)) {
						std::any_cast<MaterialTexture&>(dataType.data).uuid = tUUID;
						changed |= true;
					}

				}
				else {
					ST_WARN("Uknown ShaderDataType or not supported: {0}", name);
				}
				if (shaderEntry->HasProperity(name)) {
					Ref<ShaderProperity> prop = shaderEntry->GetProperity(name);
					if (prop->getType() == ShaderProperity::Type::Marker) {
						ImGui::SameLine(); ComponentsRender::drawHelpMarker(std::static_pointer_cast<ShaderProperityMarker>(prop)->getText().c_str());
					}
				}
			}
			changed |= drawComboControl("Transparency Mode", (int&)mat->getTransparencyMode(), { "Opaque","Cutout","Transparent" });
			if(mat->getTransparencyMode() == TransparencyMode::Cutout)
				changed |= drawFloatControl("Alpha Cutout", mat->getAlphaCutOff());
			if (mat->isReadonly) {
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}else if (changed) {
				mat->update(materialData);
				mat->toDataStringFile(asset.path);
				Previewing::get().add(asset);
			}
			
		}
		return changed;
	}

	bool ComponentsRender::drawMeshEdit(const std::string& header, UUID& uuid) {
		
		ImGui::PushID(header.c_str());
		bool change = false;
		ImGui::Text(header.c_str());

		//ImGui::Image(reinterpret_cast<void*>((uint64_t)EditorResources::getObjectTexture()->getRendererID()), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
		ImGui::Image(EditorResources::getObjectTexture(), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Mesh)).c_str())) {
				UUID id = *(UUID*)payload->Data;
				uuid = id;
				change = true;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopID();
		return change;
	}

	Previewing::Previewing()  {
		
		scene = createRef<Scene>();

	}
	void Previewing::add(Asset& asset) {
		ST_PROFILING_FUNCTION();
		if (!AssetsManager::exists(asset.uuid))
			return;
		setUpScene(asset);
		scene->getData().enablePhsyics3D = false;
		scene->onRuntimeStart();
		scene->onUpdateRuntime(Timestep(.05f));
		scene->onRuntimeStop();


		scene->getRenderer()->ApplyPostProcessing(camera.getComponent<CameraComponent>().getNativeCamera()->getFrameBuffer());
		items[asset.uuid] = camera.getComponent<CameraComponent>().getNativeCamera()->getFrameBuffer()->getColorAttachment();
		scene.reset();
	}
	Ref<Texture>& Previewing::get(const UUID& id) {
		if (!exists(id)) {
			add(AssetsManager::get(id));
		}
		return items[id];
	}
	bool Previewing::exists(const UUID& id) {
		return items.find(id) != items.end();
	}
	void Previewing::setUpScene(Asset& asset) {
		scene = createRef<Scene>();
		scene->onViewportResize(200, 200);

		camera = scene->createGameObject("Camera");
		light = scene->createGameObject("Light");
		light.addComponent<LightComponent>(LightComponent::Directional).strength = 4;
		light.getComponent<TransformComponent>().rotation = glm::quat(glm::radians(glm::vec3(-45.0f, -45.0f, .0f)));

		camera.addComponent<CameraComponent>(CameraMode::Perspective).settings.clearColor = glm::vec4(.0f);
		camera.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		camera.getComponent<CameraComponent>().settings.clearType = CameraComponent::ClearType::Color;
		camera.getComponent<CameraComponent>().depth = 1;
		camera.getComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 1.2f);

		if (asset.type == AssetType::SkyBox) {
			camera.addComponent<SkyBoxComponent>().texture = std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>>(asset.data));
		}
		else {
			camera.addComponent<SkyBoxComponent>().texture = Resources::getDefaultSkyBox();
		}
		camera.getComponent<SkyBoxComponent>().texture->bind();
		camera.getComponent<CameraComponent>().settings.clearType = CameraComponent::ClearType::Skybox;

		renderObject = scene->createGameObject("Model");
		renderObject.saveAddComponent<MeshFilterComponent>().mesh = Resources::getCubeMeshAsset();
		if (asset.type == AssetType::SkyBox) {
			renderObject.saveAddComponent<MeshFilterComponent>().mesh = Resources::getSphereMeshAsset();
		}
		
		if (asset.type == AssetType::Material) {
			renderObject.saveAddComponent<MeshRendererComponent>().material = std::any_cast<Ref<Material>>(asset.data);
		}
		else {
			renderObject.saveAddComponent<MeshRendererComponent>().material = Resources::getReflectiveMaterial();
		}

		if (asset.type == AssetType::Model) {
			Model& model = std::any_cast<Model&>(asset.data);
			scene->destroyGameObject(renderObject);
			renderObject = scene->addModel(model);
			float furthestX = .0f;
			float furthestY = .0f;
			float furthestZ = .0f;
			for (MeshAsset& mesh : model.getMeshes()) {
				if (mesh.mesh) {
					glm::vec3 temp = mesh.mesh->getFurthesteachAxisFromPos(glm::vec3(.0f), 0);
					if (furthestX < temp.x)
						furthestX = temp.x;
					if (furthestY < temp.y)
						furthestY = temp.y;
					if (furthestZ < temp.z)
						furthestZ = temp.z;
				}
			}
			const float zoom = 1.5f;
			auto& transform = camera.getComponent<TransformComponent>();
			camera.getComponent<CameraComponent>().settings.zFar = std::max({ glm::abs(furthestX), glm::abs(furthestY), glm::abs(furthestZ) }) * 2.0f * zoom;
			camera.getComponent<CameraComponent>().updateProjection();
			transform.position = glm::abs(glm::vec3(furthestX, furthestY, furthestZ)) * zoom;
			transform.rotation = Math::lookAt({ 0,0,0 }, transform.position);
		}
	}
}