#pragma once
#include "Stulu/Scene/Scene.h"

namespace Stulu {
	class SceneSerializer {
	public:
		SceneSerializer(Ref<Scene>& scene)
			: m_scene(scene){}

		void serialze(const std::string& path);
		void deSerialze(const std::string& path);
	private:
		Ref<Scene> m_scene;
	};
}