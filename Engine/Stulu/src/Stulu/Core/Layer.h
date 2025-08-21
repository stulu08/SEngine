#pragma once
#include "Stulu/Events/Event.h"
#include "Stulu/Types/Timestep.h"
#include "Stulu/Types/Stack.h"

namespace Stulu {
	// Here stuff like registering components, script bindings etc should happen
	// It is loaded once on application startup
	// Use SceneLayers to access other modules and use the EventCaller to get specific SceneLayers created by Modules
	// Scene layers are where game logic should happen, see PhysicsLayer for example, a check is done before accessing the PhysicsModule
	class STULU_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach(){}
		virtual void onDetach(){}
		virtual void onUpdate(Timestep timestep){}
		virtual void onImguiRender(Timestep timestep) {}
		virtual void onRenderGizmo() {}
		virtual void onEvent(Event& e){}

		inline const std::string& getName() const { return m_dName; }
	protected:
		std::string m_dName;
	};
}

