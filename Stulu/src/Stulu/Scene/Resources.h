#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	class Resources {
	public:
		static void load();

		static Ref<Mesh>& getCubeMesh();
		static Ref<Mesh>& getPlaneMesh();
		static Ref<Mesh>& getSphereMesh();
		static Ref<Mesh>& getSkyBoxMesh();
		static Ref<Texture2D>& getBlackTexture();
	private:
		static void loadCubeMesh();
		static inline Ref<Mesh> s_cubeMesh = nullptr;
		static void loadPlaneMesh();
		static inline Ref<Mesh> s_planeMesh = nullptr;
		static void loadSphereMesh();
		static inline Ref<Mesh> s_sphereMesh = nullptr;
		static void loadSkyBoxMesh();
		static inline Ref<Mesh> s_skyBoxMesh = nullptr;
		static void loadBlackTexture();
		static inline Ref<Texture2D> s_blackTexture = nullptr;
	};
}
