#pragma once
#include "Stulu/Events/Event.h"
#include "Stulu/Core/Timestep.h"

namespace Stulu {
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach(){}
		virtual void onDetach(){}
		virtual void onUpdate(Timestep timestep){}
		virtual void onImguiRender(Timestep timestep) {}
		virtual void onEvent(Event& e){}

		inline const std::string& getName() const { return m_dName; }
	protected:
		std::string m_dName;
	};
}

