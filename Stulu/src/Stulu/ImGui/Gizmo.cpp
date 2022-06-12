#include "st_pch.h"
#include "Gizmo.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "ImGuizmo.h"

namespace Stulu {
	glm::vec4 ST_TransformPoint(const glm::vec4& vec, const glm::mat4& matrix) {
		glm::vec4 out;
		float x = vec.x, y = vec.y, z = vec.z;
		out.x = x * matrix[0][0] + y * matrix[1][0] + z * matrix[2][0] + matrix[3][0];
		out.y = x * matrix[0][1] + y * matrix[1][1] + z * matrix[2][1] + matrix[3][1];
		out.z = x * matrix[0][2] + y * matrix[1][2] + z * matrix[2][2] + matrix[3][2];
		out.w = x * matrix[0][3] + y * matrix[1][3] + z * matrix[2][3] + matrix[3][3];

		return out;
	}
	Gizmo::Data Gizmo::s_data;
	ImVec4 ST_glmVec4ToImGui(const glm::vec4& vec) {
		return ImVec4(vec.x, vec.y, vec.z, vec.a);
	}
	ImU32 ST_glmVec4ToImGuiColor(const glm::vec4& vec) {
		return ImGui::ColorConvertFloat4ToU32(ImVec4(vec.x, vec.y, vec.z, vec.a));
	}
	ImVec2 ST_glmVec2ToImGui(const glm::vec2& vec) {
		return ImVec2(vec.x, vec.y);
	}
	glm::vec2 Gizmo::worldToPos(const glm::vec3& worldSpace) {
		glm::vec2 position = glm::vec2(s_data.mX, s_data.mY), size = glm::vec2(s_data.mWidth, s_data.mHeight);

		glm::vec4 trans;
		trans = ST_TransformPoint(s_data.viewMatrix * glm::vec4(worldSpace, 1.0f), s_data.projMatrix);
		//trans = s_data.projMatrix * (s_data.viewMatrix * glm::vec4(worldSpace, 1.0f));
		trans *= 0.5f / trans.w;
		trans += glm::vec4(0.5f, 0.5f, .0f, .0f);
		trans.y = 1.f - trans.y;
		trans.x *= size.x;
		trans.y *= size.y;
		trans.x += position.x;
		trans.y += position.y;
		return glm::vec2(trans.x, trans.y);
	}
	void Gizmo::Begin() {
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::Enable(true);
		s_data.mDrawList = ImGui::GetWindowDrawList();
	}
	void Gizmo::End() {
	}
	void Gizmo::setCamData(const glm::mat4& cameraProjection, const glm::mat4& cameraView) {
		ST_PROFILING_FUNCTION();
		s_data.projMatrix = cameraProjection;
		s_data.viewMatrix = cameraView;
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

	bool Gizmo::TransformEdit(TransformComponent& tc, GizmoTransformEditMode gizmoEditType) {
		ST_PROFILING_FUNCTION();
		if (gizmoEditType == GizmoTransformEditMode::None)
			return false;
		glm::mat4 transform = tc.transform;
		ImGuizmo::Manipulate(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix),
			(ImGuizmo::OPERATION)((int)gizmoEditType), ImGuizmo::MODE::LOCAL, glm::value_ptr(transform));
		if (ImGuizmo::IsUsing()) {
			
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
	void Gizmo::drawGrid(const glm::mat4& matrix, const float& size) {
		ST_PROFILING_FUNCTION();
		ImGuizmo::DrawGrid(glm::value_ptr(s_data.viewMatrix), glm::value_ptr(s_data.projMatrix), glm::value_ptr(matrix), size);
	}
	void Gizmo::drawLine(const glm::vec3& begin, const glm::vec3& end, const glm::vec4& color) {
		ST_PROFILING_FUNCTION();
		s_data.mDrawList->AddLine(ST_glmVec2ToImGui(worldToPos(begin)), ST_glmVec2ToImGui(worldToPos(end)), ST_glmVec4ToImGuiColor(color), 2.0f);
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
		static glm::vec4 vertexPositions[4];
		vertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		vertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		vertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		vertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * vertexPositions[i];

		drawLine(lineVertices[0], lineVertices[1], color);
		drawLine(lineVertices[1], lineVertices[2], color);
		drawLine(lineVertices[2], lineVertices[3], color);
		drawLine(lineVertices[3], lineVertices[0], color);
	}
	void Gizmo::drawOutlineCube(const glm::mat4& transform, const glm::vec4& color) {
		static glm::vec4 vertexPositions[8];
		vertexPositions[0] = { -0.5f, -0.5f, -0.5f, 1.0f };
		vertexPositions[1] = { 0.5f, -0.5f, -0.5f, 1.0f };
		vertexPositions[2] = { 0.5f,  0.5f, -0.5f, 1.0f };
		vertexPositions[3] = { -0.5f,  0.5f, -0.5f, 1.0f };
		vertexPositions[4] = { -0.5f, -0.5f, 0.5f, 1.0f };
		vertexPositions[5] = { 0.5f, -0.5f, 0.5f, 1.0f };
		vertexPositions[6] = { 0.5f,  0.5f, 0.5f, 1.0f };
		vertexPositions[7] = { -0.5f,  0.5f, 0.5f, 1.0f };
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
}