#pragma once

#include <Stulu.h>

#include "MainLayer.h"
#include "Project.h"

namespace Editor {

	class App : public Stulu::Application {
	public:
		App(const Stulu::ApplicationInfo& info, Project&& project);
		virtual ~App();

		inline MainLayer& GetLayer() { return *m_layer; }
		inline Project& GetProject() { return m_project; }
		inline const Project& GetProject() const { return m_project; }

		inline static const std::string GetEditorDataPath() { return "Data/Stulu/Editor"; };
		inline static App& get() { return *s_instance; }
	private:
		MainLayer* m_layer;
		Project m_project;
		static App* s_instance;
	};
}