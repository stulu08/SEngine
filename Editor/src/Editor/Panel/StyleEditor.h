#pragma once
#include <Stulu.h>
struct ImGuiStyle;
#define ST_SUPPORTED_IMGUI_COLORS_COUNT 50
#define ST_STYLE_DATA_FILE_COUNT 14
struct ImVec4;
namespace Stulu {
	class StyleEditor {
	public:
		struct StyleFileData {
			int font = 0;
			int style = 0;
			float windowRounding = 7.5f;
			float FrameRounding = .0f;
			int WindowBorder = 0;
			int FrameBorder = 0;
			int PopupBorder = 0;
			float fontGlobalscale = 1.0f;
			bool AntiAliasedLines = true;
			bool AntiAliasedLinesUseTex = true;
			bool AntiAliasedFill = true;
			float CurveTessellationTol = 1.5f;
			float CircleSegmentMaxError = 1.6f;
			float GlobalAlpha = 1.0f;
		};
		static void init();
		static StyleFileData load();
		static void save();
		static void drawStyleEditor(ImGuiStyle* ref = NULL, bool* open = (bool*)0);

		static void saveColors();
		static void LoadStyleColors(ImGuiStyle* dst = NULL);

		static void setStyleByInt(int style);
		static void setFontByInt(int style);

		static void StyleColorsOceanDark(ImGuiStyle* dst = NULL);
		static void StyleColorsAmoledDark(ImGuiStyle* dst = NULL);
		static void StyleColorsDark(ImGuiStyle* dst = NULL);

		static inline void saveAll() { save(); saveColors(); }
	private:
		static void loadFonts();

		static bool drawStyleSelector();
		static void drawFontSelector();

		static inline int styleIndex = 0;
		static inline int fontIndex = 0;
	};
}