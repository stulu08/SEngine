#include "st_pch.h"
#include "Gizmo.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "ImGuizmo.h"

namespace Stulu {
	struct CubeVertex {
		glm::vec3 position;
		glm::vec4 color;
	};
	static struct Data {
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::vec3 cameraPosition;

		float mX = 0.f;
		float mY = 0.f;
		float mWidth = 0.f;
		float mHeight = 0.f;
		float mXMax = 0.f;
		float mYMax = 0.f;
		float mDisplayRatio = 1.f;
		ImDrawList* mDrawList;

		static const uint32_t maxCubes = 1000;
		static const uint32_t maxIndices = maxCubes * 36;
		static const uint32_t maxVertices = maxCubes * 8;

		Ref<Shader> cubeShader;
		Ref<VertexArray> cubeVertexArray;
		Ref<VertexBuffer> cubeVertexBuffer;

		uint32_t cubeIndexCount = 0;
		CubeVertex* cubeVertexBufferBase = nullptr;
		CubeVertex* cubeVertexBufferPtr = nullptr;

		glm::vec4 cubeVertexPositions[8];


		bool used = false;
	}s_data;

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
		ST_PROFILING_FUNCTION();

		Ref<IndexBuffer> indexBuffer;
		{//indices
			uint32_t* cubeIndices = new uint32_t[s_data.maxIndices];
			uint32_t offset = 0;
			for (uint32_t i = 0; i < s_data.maxIndices; i += 36) {
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
			indexBuffer = IndexBuffer::create(s_data.maxIndices, cubeIndices);
			delete[] cubeIndices;
			s_data.cubeVertexPositions[0] = { -.5f, -.5f, .5f, 1.0f };
			s_data.cubeVertexPositions[1] = {  .5f, -.5f, .5f, 1.0f };
			s_data.cubeVertexPositions[2] = {  .5f,  .5f, .5f, 1.0f };
			s_data.cubeVertexPositions[3] = { -.5f,  .5f, .5f, 1.0f };

			s_data.cubeVertexPositions[4] = { -.5f, -.5f, -.5f, 1.0f };
			s_data.cubeVertexPositions[5] = {  .5f, -.5f, -.5f, 1.0f };
			s_data.cubeVertexPositions[6] = {  .5f,  .5f, -.5f, 1.0f };
			s_data.cubeVertexPositions[7] = { -.5f,  .5f, -.5f, 1.0f };
		}

		{//cubes
			s_data.cubeVertexArray = VertexArray::create();
			s_data.cubeVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(CubeVertex));
			s_data.cubeVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "a_pos" },
				{ ShaderDataType::Float4, "a_color" },
				});
			s_data.cubeVertexArray->addVertexBuffer(s_data.cubeVertexBuffer);
			s_data.cubeVertexArray->setIndexBuffer(indexBuffer);
			s_data.cubeVertexBufferBase = new CubeVertex[s_data.maxVertices];
		}
		s_data.cubeShader = Shader::create(Application::getEngineAssetDir() + "/Shaders/GizmoCube.glsl");
	}
	void Gizmo::Begin() {
		s_data.used = false;
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::Enable(true);
		s_data.mDrawList = ImGui::GetWindowDrawList();

		Renderer::uploadBufferData(s_data.projMatrix, s_data.viewMatrix, glm::vec3(), glm::vec3());

		resetCubes();
		resetSpheres();
		Renderer2D::begin();
	}
	void Gizmo::End() {
		RenderCommand::setDefault();
		RenderCommand::setDepthTesting(false);

		flushCubes();
		flushSpheres();
		Renderer2D::flush();
	}
	void Gizmo::setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView) {
		ST_PROFILING_FUNCTION();
		s_data.projMatrix = cameraProjection;
		s_data.viewMatrix = cameraView;
		s_data.cameraPosition = glm::inverse(cameraView)[3];
	}
	void Gizmo::setRect(const float x, const float y, const float width, const float height) {
		ST_PROFILING_FUNCTION();
		ImGuizmo::SetRect(x, y, width, height);
		s_data.mX = x;
		s_data.mY = y;
		s_data.mWidth = width;
		s_data.mHeight = height;
		s_data.mXMax = s_data.mX + s_data.mWidth;
		s_data.mYMax = s_data.mY + s_data.mHeight;
		s_data.mDisplayRatio = width / height;
	}
	bool Gizmo::IsUsing() {
		return s_data.used;
	}

	bool Gizmo::TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType) {
		ST_PROFILING_FUNCTION();
		if (gizmoEditType == GizmoTransformEditMode::None)
			return false;
		glm::mat4 transform = tc.transform;
		ImGuizmo::Manipulate(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix),
			(ImGuizmo::OPERATION)((int)gizmoEditType), ImGuizmo::MODE::LOCAL, glm::value_ptr(transform));
		if (ImGuizmo::IsUsing()) {
			s_data.used = true;
			glm::vec3 position, scale;
			glm::quat rotation;
			if (tc.parent) {
				TransformComponent parent = tc.parent.getComponent<TransformComponent>();
				glm::vec3 pos, sca;glm::quat rot; //new transformations
				Math::decomposeTransform(transform, pos, rot, sca);
				transform = 
					glm::translate(glm::mat4(1.0f), pos - parent.worldPosition) * 
					(glm::toMat4(rot) / glm::toMat4(parent.worldRotation)) *
					glm::scale(glm::mat4(1.0f),  sca / parent.worldScale) ;
			}
			Math::decomposeTransform(transform, position, rotation, scale);
			tc.position = position;
			tc.rotation = rotation;
			tc.scale = scale;
			return true;
		}
		return false;
	}
	void Gizmo::drawGrid(const glm::mat4& transform, float size) {
		ST_PROFILING_FUNCTION();
		ImGuizmo::DrawGrid(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix), glm::value_ptr(transform), size);
	}

	void Gizmo::drawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
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

	void Gizmo::drawTexture(const Ref<Texture>& texture, const glm::vec3& position, const glm::quat& rotation, const glm::vec2& scale, const glm::vec4& color) {
		Renderer2D::drawTexturedQuad(Math::createMat4(position, rotation, glm::vec3(scale, 1.0f)), texture, glm::vec2(1.0f), color);
	}
	void Gizmo::drawTextureBillBoard(const Ref<Texture>& texture, const glm::vec3& position, const glm::vec2& scale, const glm::vec3& up, const glm::vec4& color) {
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
		ST_PROFILING_FUNCTION();
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
	void Gizmo::drawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color) {
		drawCube(Math::createMat4(position, rotation, scale), color);
	}

	void Gizmo::drawSphere(const glm::mat4& transform, const glm::vec4& color) {
		//will do this using instancing
	}
	void Gizmo::drawSphere(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color) {
		drawSphere(Math::createMat4(position, rotation, scale), color);
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
			s_data.mDrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, ImDrawCornerFlags_All, border_size);
			s_data.mDrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, ImDrawCornerFlags_All, border_size);
		}
	}
	bool Gizmo::drawGUITextureButton(const Ref<Texture>& texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, const glm::vec2& uv1, const glm::vec2& uv2, const int borderSize, const glm::vec4& bgColor) {
		ImVec2 _pos(pos.x + s_data.mX, pos.y + s_data.mY);
		ImVec2 _max(ImVec2(size.x, size.y) + _pos);

		drawGUIRect(pos, size, bgColor, true);

		//if (bgColor.w > 0.0f)
			//s_data.mDrawList->AddRectFilled(_pos, _max, ImGui::GetColorU32(ImVec4(bgColor.x, bgColor.y, bgColor.z, bgColor.w)));

		s_data.mDrawList->AddImage(reinterpret_cast<void*>((uint64_t)texture->getRendererID()),
			_pos, _max, ImVec2(uv1.x, uv1.y), ImVec2(uv2.x, uv2.y),
			ImGui::GetColorU32(ImVec4(color.x, color.y, color.z, color.w))
		);

		bool pressed = ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(_pos, _max);
		s_data.used |= pressed;
		return pressed;
	}

	void Gizmo::resetCubes() {
		s_data.cubeIndexCount = 0;
		s_data.cubeVertexBufferPtr = s_data.cubeVertexBufferBase;
	}
	void Gizmo::resetSpheres() {
	}
	void Gizmo::flushCubes() {
		if (s_data.cubeIndexCount > 0) {
			uint32_t dataSize = uint32_t((uint8_t*)s_data.cubeVertexBufferPtr - (uint8_t*)s_data.cubeVertexBufferBase);
			s_data.cubeVertexBuffer->setData(s_data.cubeVertexBufferBase, dataSize);

			s_data.cubeShader->bind();
			RenderCommand::setDepthTesting(false);
			RenderCommand::setCullMode(CullMode::Back);
			RenderCommand::drawIndexed(s_data.cubeVertexArray, s_data.cubeIndexCount);
		}
	}
	void Gizmo::flushSpheres() {
	}
}