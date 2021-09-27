#pragma once
#include <Stulu/Renderer/Texture.h>
namespace Stulu {
	class EditorViewportPanel {
	public:
		EditorViewportPanel(const char* name);

		void draw(Ref<Texture> texture, bool end = true);
		void endDraw();
	private:
		const char* m_name;
		uint32_t m_viewPortPanelWidth;
		uint32_t m_viewPortPanelHeight;
		bool m_viewPortFocused = false;
		bool m_viewPortHovered = false;

		friend class EditorLayer;
	};
}