#pragma once
#include "Stulu/Events/Event.h"

namespace Stulu {
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach(){}
		virtual void onDetach(){}
		virtual void onUpdate(){}
		virtual void onImguiRender() {}
		virtual void onEvent(Event& e){}

		inline const std::string& getName() const { return m_dName; }
	protected:
		std::string m_dName;
	};
}

