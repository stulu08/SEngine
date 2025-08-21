#pragma once

#include <Stulu.h>

#include "Project.h"
#include "Prefrences.h"

namespace Editor {
	class MainLayer;

	class App : public Stulu::Application {
	public:
		App(const Stulu::ApplicationInfo& info, Project&& project, Prefrences&& prefs);
		virtual ~App();

		inline Prefrences& GetPrefs() { return m_prefrences; }
		inline MainLayer& GetLayer() { return *m_layer; }
		inline Project& GetProject() { return m_project; }
		inline const Project& GetProject() const { return m_project; }

		inline static const std::string GetEditorDataPath() { return "Data/Stulu/Editor"; };
		inline static App& get() { return *s_instance; }
	private:
		Prefrences m_prefrences;
		MainLayer* m_layer;
		Project m_project;
		static App* s_instance;
	};
}