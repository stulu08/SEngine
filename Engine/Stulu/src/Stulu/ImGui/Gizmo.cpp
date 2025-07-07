#include "st_pch.h"
#include "Gizmo.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "ImGuizmo.h"
#include "ImCurveEdit.h"
#include "ImGradient.h"
#include "GraphEditor.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"

namespace Stulu {
	struct CubeVertex {
		glm::vec3 position;
		glm::vec4 color;
	};
	struct SphereInstanceData {
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec4 color = glm::vec4(1.0f);
	};
	static struct Data {
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		glm::mat4 projMatrix = glm::mat4(1.0f);
		glm::vec3 cameraPosition = glm::vec3(0.0f);

		std::string window;
		bool* windowOpen = nullptr;
		bool windowBeginCalled = false;

		float mX = 0.f;
		float mY = 0.f;
		float mWidth = 0.f;
		float mHeight = 0.f;
		float mXMax = 0.f;
		float mYMax = 0.f;
		float mDisplayRatio = 1.f;
		ImDrawList* mDrawList = nullptr;

		Ref<FrameBuffer> drawBuffer;

		//using batching
		static const uint32_t maxCubes = 1000;
		static const uint32_t maxIndices = maxCubes * 36;
		static const uint32_t maxVertices = maxCubes * 8;
		
		Ref<Shader> cubeShader;
		Ref<VertexArray> cubeVertexArray;
		Ref<VertexBuffer> cubeVertexBuffer;
		uint32_t cubeIndexCount = 0;
		Scope<CubeVertex[]> cubeVertexBufferBase = nullptr;
		CubeVertex* cubeVertexBufferPtr = nullptr;
		glm::vec4 cubeVertexPositions[8];

		//using instancing
		static const uint32_t maxSpheres = ST_MAX_INSTANCES; //the buffer has 16kb or smth like this and 180 * sizeof(SphereInstanceData) is smth near 15kb thats why not more than 180, 187 is also find and it breaks at 200 cause yes
		Ref<Shader> sphereShader;
		Ref<VertexArray> sphereVertexArray;
		Scope<SphereInstanceData[]> sphereDataBufferBase = nullptr;
		SphereInstanceData* sphereDataBufferPtr = nullptr;
		uint32_t sphereInstanceCount = 0;

		bool used = false, hovered = false;
	}s_data;

	void LoadGizmoStyle();

	ImVec4 ST_glmVec4ToImGui(const glm::vec4& vec) {
		return ImVec4(vec.x, vec.y, vec.z, vec.a);
	}
	ImU32 ST_glmVec4ToImGuiColor(const glm::vec4& vec) {
		return ImGui::ColorConvertFloat4ToU32(ImVec4(vec.x, vec.y, vec.z, vec.a));
	}
	ImVec2 ST_glmVec2ToImGui(const glm::vec2& vec) {
		return ImVec2(vec.x, vec.y);
	}

	void Gizmo::init() {
		FrameBufferSpecs specs;
		specs.width = Application::get().getWidth();
		specs.height = Application::get().getHeight();

		s_data.drawBuffer = FrameBuffer::create(specs, TextureFormat::RGBA);

		{//cubes
			Ref<IndexBuffer> indexBuffer;
			std::vector<uint32_t> cubeIndices = std::vector<uint32_t>(s_data.maxIndices);
			uint32_t offset = 0;
			for (size_t i = 0; i < s_data.maxIndices; i += 36) {
				//front
				cubeIndices[i +  0] = offset + 0;
				cubeIndices[i +  1] = offset + 1;
				cubeIndices[i +  2] = offset + 2;
				cubeIndices[i +  3] = offset + 2;
				cubeIndices[i +  4] = offset + 3;
				cubeIndices[i +  5] = offset + 0;
				//back
				cubeIndices[i +  6] = offset + 6;
				cubeIndices[i +  7] = offset + 5;
				cubeIndices[i +  8] = offset + 4;
				cubeIndices[i +  9] = offset + 4;
				cubeIndices[i + 10] = offset + 7;
				cubeIndices[i + 11] = offset + 6;
				//top
				cubeIndices[i + 12] = offset + 2;
				cubeIndices[i + 13] = offset + 6;
				cubeIndices[i + 14] = offset + 7;
				cubeIndices[i + 15] = offset + 7;
				cubeIndices[i + 16] = offset + 3;
				cubeIndices[i + 17] = offset + 2;
				//bottom
				cubeIndices[i + 18] = offset + 4;
				cubeIndices[i + 19] = offset + 5;
				cubeIndices[i + 20] = offset + 1;
				cubeIndices[i + 21] = offset + 1;
				cubeIndices[i + 22] = offset + 0;
				cubeIndices[i + 23] = offset + 4;
				//right
				cubeIndices[i + 24] = offset + 4;
				cubeIndices[i + 25] = offset + 0;
				cubeIndices[i + 26] = offset + 3;
				cubeIndices[i + 27] = offset + 3;
				cubeIndices[i + 28] = offset + 7;
				cubeIndices[i + 29] = offset + 4;
				//left
				cubeIndices[i + 30] = offset + 2;
				cubeIndices[i + 31] = offset + 1;
				cubeIndices[i + 32] = offset + 5;
				cubeIndices[i + 33] = offset + 5;
				cubeIndices[i + 34] = offset + 6;
				cubeIndices[i + 35] = offset + 2;


				offset += 8;
			}
			indexBuffer = IndexBuffer::create(s_data.maxIndices, cubeIndices.data());
			
			s_data.cubeVertexPositions[0] = { -.5f, -.5f, .5f, 1.0f };
			s_data.cubeVertexPositions[1] = {  .5f, -.5f, .5f, 1.0f };
			s_data.cubeVertexPositions[2] = {  .5f,  .5f, .5f, 1.0f };
			s_data.cubeVertexPositions[3] = { -.5f,  .5f, .5f, 1.0f };

			s_data.cubeVertexPositions[4] = { -.5f, -.5f, -.5f, 1.0f };
			s_data.cubeVertexPositions[5] = {  .5f, -.5f, -.5f, 1.0f };
			s_data.cubeVertexPositions[6] = {  .5f,  .5f, -.5f, 1.0f };
			s_data.cubeVertexPositions[7] = { -.5f,  .5f, -.5f, 1.0f };


			s_data.cubeVertexArray = VertexArray::create();
			s_data.cubeVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(CubeVertex));
			s_data.cubeVertexBuffer->setLayout({
				PositionBufferElement,
				ColorBufferElement,
				});
			s_data.cubeVertexArray->addVertexBuffer(s_data.cubeVertexBuffer);
			s_data.cubeVertexArray->setIndexBuffer(indexBuffer);
			s_data.cubeVertexBufferBase = Scope<CubeVertex[]>(new CubeVertex[s_data.maxVertices]);
			s_data.cubeShader = Renderer::getShaderSystem()->GetShader("Gizmo/Cube");
		}
		{//spheres
			s_data.sphereVertexArray = VertexArray::create();
			//copy sphere mesh
			{
				MeshAsset sphereMesh = Resources::SphereMesh();

				Ref<VertexBuffer> vb = VertexBuffer::create((uint32_t)(sphereMesh->GetVerticesCount() * sphereMesh->GetStride()), &sphereMesh->GetVertices()[0]);
				vb->setLayout(Mesh::DefaultVertexLayout());
				s_data.sphereVertexArray->addVertexBuffer(vb);

				Ref<IndexBuffer> ib = IndexBuffer::create((uint32_t)sphereMesh->GetIndices().size(), (uint32_t*)sphereMesh->GetIndices().data());
				s_data.sphereVertexArray->setIndexBuffer(ib);
			}
			
			s_data.sphereDataBufferBase = Scope<SphereInstanceData[]>(new SphereInstanceData[s_data.maxSpheres]);
			s_data.cubeShader = Renderer::getShaderSystem()->GetShader("Gizmo/Sphere");
		}

		LoadGizmoStyle();
	}
	void Gizmo::ShutDown() {
		s_data = Data();
	}
	bool Gizmo::Begin() {
		const bool windowOpen = s_data.windowOpen ? *s_data.windowOpen : true;

		// we need to call ImGui::End, when close button is pressed Begin will not be called and one close call is to much
		s_data.windowBeginCalled = windowOpen;
		s_data.used = false;
		s_data.hovered = false;
		s_data.mDrawList = 0;
		
		if (windowOpen && ImGui::Begin(s_data.window.c_str(), s_data.windowOpen)) {
			//check for resize
			if (s_data.mWidth != s_data.drawBuffer->getColorAttachment()->getWidth() || s_data.mHeight != s_data.drawBuffer->getColorAttachment()->getHeight()) {
				s_data.drawBuffer->resize((uint32_t)s_data.mWidth, (uint32_t)s_data.mHeight);
			}

			// set guizmo to initial state
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
			ImGuizmo::Enable(true);
			s_data.mDrawList = ImGui::GetWindowDrawList();

			// clear framebuffer
			RenderCommand::setDefault();
			s_data.drawBuffer->bind();
			RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, .0f));
			RenderCommand::clear();
			Renderer::uploadCameraBufferData(s_data.projMatrix, s_data.viewMatrix, glm::vec3(), glm::vec3());

			// set renderer to initial state
			resetCubes();
			resetSpheres();
			Renderer2D::begin();

			return true;
		}
		else {
			// disabling at anther point will reset the mouse input and gizmo will not work
			ImGuizmo::Enable(false);
		}

		return false;
	}
	void Gizmo::End() {
		ST_PROFILING_SCOPE("Gizmo - Render Pass");

		if (s_data.windowBeginCalled) {
			// mDrawList is null when outside of window
			if (s_data.mDrawList) {
				// render
				flushCubes();
				flushSpheres();
				Renderer2D::flush();

				// reset
				s_data.drawBuffer->unbind();
			}
			ImGui::End();
		}
	}
	void Gizmo::ApplyToFrameBuffer(const Ref<FrameBuffer>& camera) {
		RenderCommand::setDepthTesting(false);
		s_data.drawBuffer->getColorAttachment()->bind(0);
		Renderer::ScreenQuad(camera, Resources::FullscreenShader());
	}
	void Gizmo::setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView) {
		s_data.projMatrix = cameraProjection;
		s_data.viewMatrix = cameraView;
		s_data.cameraPosition = glm::inverse(cameraView)[3];
	}
	void Gizmo::setRect(const float x, const float y, const float width, const float height) {
		ImGuizmo::SetRect(x, y, width, height);
		s_data.mX = x;
		s_data.mY = y;
		s_data.mWidth = width;
		s_data.mHeight = height;
		s_data.mXMax = s_data.mX + s_data.mWidth;
		s_data.mYMax = s_data.mY + s_data.mHeight;
		s_data.mDisplayRatio = width / height;
	}
	void Gizmo::setWindow(const std::string& windowName, bool* windowOpenPtr) {
		s_data.window = windowName;
		s_data.windowOpen = windowOpenPtr;
	}
	bool Gizmo::WasUsed() {
		return s_data.used;
	}

	bool Gizmo::WasHovered() {
		return s_data.hovered;
	}
	bool Gizmo::TransformEdit(const GameObject& object, GizmoTransformEditMode gizmoEditType, const glm::vec3& snap) {
		TransformComponent& transform = object.getComponent<TransformComponent>();
		glm::mat4 matrix = transform.GetWorldTransform();

		const float* snapPtr = snap == glm::vec3(0.0f) ? nullptr : glm::value_ptr(snap);

		ImGuizmo::MANIPULATED operated = ImGuizmo::Manipulate(
			glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix),
			(ImGuizmo::OPERATION)((uint32_t)gizmoEditType), ImGuizmo::MODE::LOCAL,
			glm::value_ptr(matrix), nullptr, snapPtr
		);

		s_data.hovered = ImGuizmo::IsOver();
		if (ImGuizmo::IsUsing() && operated) {
			s_data.used = true;

			glm::vec3 position, scale;
			glm::quat rotation;
			Math::decomposeTransform(matrix, position, rotation, scale);

			if (operated & ImGuizmo::TRANSLATED)
				transform.SetWorldPosition(position);
			if (operated & ImGuizmo::ROTATED)
				transform.SetWorldRotation(rotation);
			if (operated & ImGuizmo::SCALED)
				transform.SetWorldScale(scale);
			return true;
		}
		return false;
	}
	bool Gizmo::TransformEdit(const std::vector<entt::entity>& objects, Registry* registry, GizmoTransformEditMode gizmoEditType, const glm::vec3& snap) {
		if (objects.empty()) return false;
		if (objects.size() == 1) {
			return TransformEdit({ objects[0], registry }, gizmoEditType, snap);
		}

		// Calculate average position and scale
		glm::vec3 averageScale(0.0f);
		glm::vec3 averagePosition(0.0f);

		for (entt::entity obj : objects) {
			const auto& transform = GameObject(obj, registry).getComponent<TransformComponent>();
			averagePosition += transform.GetWorldPosition();
			averageScale += transform.GetWorldScale();
		}

		averagePosition /= static_cast<float>(objects.size());
		averageScale /= static_cast<float>(objects.size());

		// Build initial gizmo transform matrix
		glm::mat4 gizmoTransform = glm::translate(glm::mat4(1.0f), averagePosition) *
			glm::scale(glm::mat4(1.0f), averageScale);

		const glm::mat4 originalTransform = gizmoTransform;

		const float* snapPtr = (snap == glm::vec3(0.0f)) ? nullptr : glm::value_ptr(snap);
		
		// Manipulate using ImGuizmo
		ImGuizmo::MANIPULATED operated = ImGuizmo::Manipulate(
			glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix),
			static_cast<ImGuizmo::OPERATION>((uint32_t)gizmoEditType), ImGuizmo::MODE::LOCAL,
			glm::value_ptr(gizmoTransform), nullptr, snapPtr
		);

		s_data.hovered |= ImGuizmo::IsOver();

		if (ImGuizmo::IsUsing() && operated) {
			s_data.used = true;

			glm::mat4 delta = glm::inverse(originalTransform) * gizmoTransform;

			for (entt::entity obj : objects) {
				auto& transform = GameObject(obj, registry).getComponent<TransformComponent>();

				glm::mat4 world = transform.GetWorldTransform();
				world = delta * world;

				glm::vec3 position, scale;
				glm::quat rotation;
				Math::decomposeTransform(world, position, rotation, scale);

				if (operated & ImGuizmo::TRANSLATED)
					transform.SetWorldPosition(position);
				if (operated & ImGuizmo::ROTATED)
					transform.SetWorldRotation(rotation);
				if (operated & ImGuizmo::SCALED)
					transform.SetWorldScale(scale);
			}

			return true;
		}

		return false;
	}
	void Gizmo::drawGrid(const glm::mat4& transform, float size) {
		ImGuizmo::DrawGrid(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix), glm::value_ptr(transform), size);
	}

	bool Gizmo::ViewManipulate(TransformComponent& cameraTransform, const glm::vec3& selectedPosition, ImVec2 pos, ImVec2 size) {
		ImVec4 bgColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
		bgColor.w *= 0.5f;

		glm::mat4 pivotTranslate = glm::translate(glm::mat4(1.0f), -selectedPosition);
		glm::mat4 invPivotTranslate = glm::translate(glm::mat4(1.0f), selectedPosition);
		glm::mat4 offsetView = pivotTranslate * s_data.viewMatrix;

		ImGuizmo::ViewManipulate(
			glm::value_ptr(offsetView), glm::value_ptr(s_data.projMatrix),
			ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
			glm::value_ptr(invPivotTranslate), glm::distance(cameraTransform.position, selectedPosition), pos, size,
			ImGui::ColorConvertFloat4ToU32(bgColor));


		s_data.hovered |= ImGuizmo::IsViewManipulateHovered();
		if (ImGuizmo::IsUsingViewManipulate()) {
			s_data.used = true;
			s_data.viewMatrix = invPivotTranslate * offsetView;

			glm::vec3 position, scale;
			glm::quat rotation;
			Math::decomposeTransform(glm::inverse(s_data.viewMatrix), position, rotation, scale);
			//cameraTransform.SetWorldPosition(position);
			cameraTransform.SetWorldRotation(rotation);
			return true;
		}
		return false;
	}

	void Gizmo::drawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color) {
		Renderer2D::drawLine(begin, end, color);
	}

	void Gizmo::drawRect(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color) {
		glm::vec3 p0 = glm::vec3(position.x - scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);
		drawLine(p0, p1, color);
		drawLine(p1, p2, color);
		drawLine(p2, p3, color);
		drawLine(p3, p0, color);
	}
	void Gizmo::drawRect(const glm::mat4& transform, const glm::vec4& color) {
		static glm::vec4 vertexPositions[4]{
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * vertexPositions[i];

		drawLine(lineVertices[0], lineVertices[1], color);
		drawLine(lineVertices[1], lineVertices[2], color);
		drawLine(lineVertices[2], lineVertices[3], color);
		drawLine(lineVertices[3], lineVertices[0], color);
	}

	void Gizmo::drawOutlineCube(const glm::mat4& transform, const glm::vec4& color) {
		static glm::vec4 vertexPositions[8]{
			{ -0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f, -0.5f, 0.5f, 1.0f },
			{ 0.5f, -0.5f, 0.5f, 1.0f },
			{ 0.5f,  0.5f, 0.5f, 1.0f },
			{ -0.5f,  0.5f, 0.5f, 1.0f },
		};
		glm::vec3 lineVertices[8];
		for (size_t i = 0; i < 8; i++)
			lineVertices[i] = transform * vertexPositions[i];

		//back
		drawLine(lineVertices[0], lineVertices[1], color);
		drawLine(lineVertices[1], lineVertices[2], color);
		drawLine(lineVertices[2], lineVertices[3], color);
		drawLine(lineVertices[3], lineVertices[0], color);
		//front
		drawLine(lineVertices[4], lineVertices[5], color);
		drawLine(lineVertices[5], lineVertices[6], color);
		drawLine(lineVertices[6], lineVertices[7], color);
		drawLine(lineVertices[7], lineVertices[4], color);
		//connections between front and back
		drawLine(lineVertices[0], lineVertices[4], color);
		drawLine(lineVertices[1], lineVertices[5], color);
		drawLine(lineVertices[2], lineVertices[6], color);
		drawLine(lineVertices[3], lineVertices[7], color);
	}
	void Gizmo::drawOutlineCube(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transform, const glm::vec4& color) {
		//back
		drawLine(transform * glm::vec4{ min.x, min.y, min.z, 1.0f }, transform * glm::vec4{ min.x, max.y, min.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ max.x, min.y, min.z, 1.0f }, transform * glm::vec4{ max.x, max.y, min.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ min.x, min.y, min.z, 1.0f }, transform * glm::vec4{ max.x, min.y, min.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ min.x, max.y, min.z, 1.0f }, transform * glm::vec4{ max.x, max.y, min.z, 1.0f }, color);
		//front
		drawLine(transform * glm::vec4{ min.x, min.y, max.z, 1.0f }, transform * glm::vec4{ min.x, max.y, max.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ max.x, min.y, max.z, 1.0f }, transform * glm::vec4{ max.x, max.y, max.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ min.x, min.y, max.z, 1.0f }, transform * glm::vec4{ max.x, min.y, max.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ min.x, max.y, max.z, 1.0f }, transform * glm::vec4{ max.x, max.y, max.z, 1.0f }, color);
		//connections between front and back
		drawLine(transform * glm::vec4{ min.x, max.y, min.z, 1.0f }, transform * glm::vec4{ min.x, max.y, max.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ max.x, max.y, min.z, 1.0f }, transform * glm::vec4{ max.x, max.y, max.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ min.x, min.y, min.z, 1.0f }, transform * glm::vec4{ min.x, min.y, max.z, 1.0f }, color);
		drawLine(transform * glm::vec4{ max.x, min.y, min.z, 1.0f }, transform * glm::vec4{ max.x, min.y, max.z, 1.0f }, color);

	}

	void Gizmo::drawTexture(Texture2D* texture, const glm::vec3& position, const glm::quat& rotation, const glm::vec2& scale, const glm::vec4& color) {
		Renderer2D::drawTexturedQuad(Math::createMat4(position, rotation, glm::vec3(scale, 1.0f)), texture, glm::vec2(1.0f), color);
	}
	void Gizmo::drawTextureBillBoard(Texture2D* texture, const glm::vec3& position, const glm::vec2& scale, const glm::vec3& up, const glm::vec4& color) {
		Renderer2D::drawTexturedQuad(
			Math::createMat4(position, Math::lookAt(position, s_data.cameraPosition, up), glm::vec3(scale, 1.0f))
			, texture, { 1.0f,1.0f }, color);
	}

	void Gizmo::drawCircle(const glm::vec3& position, const glm::vec2& scale, const glm::quat& rotation, const glm::vec4& color, float thickness, float fade) {
		Renderer2D::drawCircle(Math::createMat4(position, rotation, glm::vec3(scale, 1.0f)), color, thickness, fade);

	}
	void Gizmo::drawCircleBillBoard(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, float thickness, float fade, const glm::vec3& up) {
		glm::mat4 viewMat = glm::lookAt(position, s_data.cameraPosition, glm::vec3(up));
		Renderer2D::drawCircle(
			Math::createMat4(position, Math::lookAt(position, s_data.cameraPosition, up), glm::vec3(scale, 1.0f))
			, color, thickness, fade);

	}

	void Gizmo::drawCube(const glm::mat4& transform, const glm::vec4& color) {
		if (s_data.cubeIndexCount >= s_data.maxIndices) {
			flushCubes();
			resetCubes();
		}

		for (int i = 0; i < 8; i++) {
			s_data.cubeVertexBufferPtr->position = transform * s_data.cubeVertexPositions[i];
			s_data.cubeVertexBufferPtr->color = color;
			s_data.cubeVertexBufferPtr++;
		}
		s_data.cubeIndexCount += 36;
	}
	void Gizmo::drawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const glm::vec4& color) {
		drawCube(Math::createMat4(position, rotation, scale), color);
	}

	void Gizmo::drawSphere(const glm::mat4& transform, const glm::vec4& color) {
		if (s_data.sphereInstanceCount >= s_data.maxSpheres) {
			flushSpheres();
			resetSpheres();
		}
		s_data.sphereDataBufferPtr->transform = transform;
		s_data.sphereDataBufferPtr->color = color;
		s_data.sphereDataBufferPtr++;

		s_data.sphereInstanceCount++;
	}
	void Gizmo::drawSphere(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const glm::vec4& color) {
		drawSphere(Math::createMat4(position, rotation, scale), color);
	}

	void Gizmo::DrawArc(const glm::vec3& center, const glm::vec3& right, const glm::vec3& up, float radius, float startAngle, float endAngle, int segments, const glm::vec4& color) {
		for (int i = 0; i < segments; ++i) {
			float theta1 = startAngle + (endAngle - startAngle) * (i / (float)segments);
			float theta2 = startAngle + (endAngle - startAngle) * ((i + 1) / (float)segments);

			glm::vec3 p1 = center + right * glm::cos(theta1) * radius + up * glm::sin(theta1) * radius;
			glm::vec3 p2 = center + right * glm::cos(theta2) * radius + up * glm::sin(theta2) * radius;

			drawLine(p1, p2, color);
		}
	}

	void Gizmo::drawCapsule(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, float radius, float height, bool vertical, const glm::vec4& color) {
		const int segments = 16; // Number of segments for arc rendering
		float halfHeight = (height * 0.5f) - radius; // Cylinder part height

		// Capsule orientation axes
		glm::vec3 up = glm::vec3(0, 1, 0); // Default: vertical along Y
		glm::vec3 right = glm::vec3(1, 0, 0); // Local X-axis
		glm::vec3 forward = glm::vec3(0, 0, 1); // Local Z-axis

		if (vertical) {
			up = glm::vec3(1, 0, 0); // Align along X-axis
			right = glm::vec3(0, 1, 0);
		}

		// Apply object rotation
		up = rotation * up;
		right = rotation * right;
		forward = rotation * forward;

		// Compute capsule top/bottom
		glm::vec3 bottom = position - up * halfHeight;
		glm::vec3 top = position + up * halfHeight;

		// Draw the cylinder edges
		drawLine(bottom + right * radius, top + right * radius, color);
		drawLine(bottom - right * radius, top - right * radius, color);
		drawLine(bottom + forward * radius, top + forward * radius, color);
		drawLine(bottom - forward * radius, top - forward * radius, color);

		// Draw top and bottom circles (caps)
		drawCircle(top, glm::vec2(radius * scale.x, radius * scale.z), rotation, color);
		drawCircle(bottom, glm::vec2(radius * scale.x, radius * scale.z), rotation, color);

		// Draw arcs on the sides
		DrawArc(bottom, forward, up, radius, 0, glm::pi<float>(), segments, color);
		DrawArc(top, forward, up, radius, glm::pi<float>(), glm::two_pi<float>(), segments, color);
		DrawArc(bottom, right, up, radius, 0, glm::pi<float>(), segments, color);
		DrawArc(top, right, up, radius, glm::pi<float>(), glm::two_pi<float>(), segments, color);
	}


	void Gizmo::drawGUIRect(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, bool border, float border_size) {
		ImVec2 p_min(pos.x + s_data.mX, pos.y + s_data.mY);
		ImVec2 p_max(ImVec2(size.x, size.y) + p_min);

		float rounding = .0f;
		
		s_data.mDrawList->AddRectFilled(p_min, p_max, ImGui::GetColorU32(ImVec4(color.x, color.y, color.z, color.w)), rounding);
		if(border_size == 0)
			border_size = ImGui::GetStyle().FrameBorderSize;
		if (border && border_size > 0.0f)
		{
			s_data.mDrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, ImDrawFlags_RoundCornersAll, border_size);
			s_data.mDrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, ImDrawFlags_RoundCornersAll, border_size);
		}
	}
	bool Gizmo::drawGUITextureButton(Texture2D* texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, const glm::vec2& uv1, const glm::vec2& uv2, const int borderSize, const glm::vec4& bgColor) {
		ImVec2 _pos(pos.x + s_data.mX, pos.y + s_data.mY);
		ImVec2 _max(ImVec2(size.x, size.y) + _pos);

		drawGUIRect(pos, size, bgColor, true);

		//if (bgColor.w > 0.0f)
			//s_data.mDrawList->AddRectFilled(_pos, _max, ImGui::GetColorU32(ImVec4(bgColor.x, bgColor.y, bgColor.z, bgColor.w)));

		s_data.mDrawList->AddImage(ImGui::StuluTextureToImGui(texture),
		//s_data.mDrawList->AddImage(reinterpret_cast<void*>((uint64_t)texture->getRendererID()),
			_pos, _max, ImVec2(uv1.x, uv1.y), ImVec2(uv2.x, uv2.y),
			ImGui::GetColorU32(ImVec4(color.x, color.y, color.z, color.w))
		);

		bool hovered = ImGui::IsMouseHoveringRect(_pos, _max);
		bool pressed = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
		s_data.used |= pressed;
		s_data.hovered |= hovered;
		return pressed;
	}

	void Gizmo::drawText(const std::string& text, const glm::vec3& worldPos, const glm::vec4& color) {
		const glm::vec2 screen = Math::WorldToScreen(worldPos, s_data.projMatrix * s_data.viewMatrix, { s_data.mWidth, s_data.mHeight }, { s_data.mX, s_data.mY });
		s_data.mDrawList->AddText({ screen.x ,screen.y }, ImGui::ColorConvertFloat4ToU32({ color.x,color.y,color.z,color.w }), text.c_str());
	}

	void Gizmo::resetCubes() {
		s_data.cubeIndexCount = 0;
		s_data.cubeVertexBufferPtr = s_data.cubeVertexBufferBase.get();
	}
	void Gizmo::resetSpheres() {
		s_data.sphereInstanceCount = 0;
		s_data.sphereDataBufferPtr = s_data.sphereDataBufferBase.get();
	}
	void Gizmo::flushCubes() {
		if (s_data.cubeIndexCount > 0) {
			uint32_t dataSize = uint32_t((uint8_t*)s_data.cubeVertexBufferPtr - (uint8_t*)s_data.cubeVertexBufferBase.get());
			s_data.cubeVertexBuffer->setData(s_data.cubeVertexBufferBase.get(), dataSize);

			s_data.cubeShader->bind();
			RenderCommand::drawIndexed(s_data.cubeVertexArray, s_data.cubeIndexCount);
		}
	}
	void Gizmo::flushSpheres() {
		if (s_data.sphereInstanceCount > 0) {
			
			uint32_t dataSize = uint32_t((uint8_t*)s_data.sphereDataBufferPtr - (uint8_t*)s_data.sphereDataBufferBase.get());
			//uint32_t dataSize = s_data.sphereInstanceCount * (uint32_t)sizeof(SphereInstanceData);
			Renderer::uploadBufferData(BufferBinding::Model, s_data.sphereDataBufferBase.get(), dataSize);

			s_data.sphereShader->bind();
			RenderCommand::drawIndexed(s_data.sphereVertexArray, 0, s_data.sphereInstanceCount);
			
		}
	}

	inline void LoadGizmoStyle() {
		auto& style = ImGuizmo::GetStyle();

		style.TranslationLineArrowSize = 9.0f;
		style.TranslationLineThickness = 4.0f;
		style.ScaleLineCircleSize = 9.0f;
		style.ScaleLineThickness = 4.0f;
		style.RotationLineThickness = 3.0f;
		style.RotationOuterLineThickness = 4.0f;

		style.Colors[ImGuizmo::DIRECTION_X] = ImVec4(0.847f, 0.369f, 0.443f, 1.000f);
		style.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.145f, 0.667f, 0.145f, 1.000f);
		style.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.173f, 0.325f, 0.800f, 1.000f);
		style.Colors[ImGuizmo::PLANE_X] = ImVec4(0.847f, 0.408f, 0.478f, 1.000f);
		style.Colors[ImGuizmo::PLANE_Y] = ImVec4(0.333f, 0.671f, 0.333f, 1.000f);
		style.Colors[ImGuizmo::PLANE_Z] = ImVec4(0.259f, 0.404f, 0.851f, 1.000f);
		style.Colors[ImGuizmo::SELECTION] = ImVec4(0.800f, 0.667f, 0.125f, 1.000f);
	}
}