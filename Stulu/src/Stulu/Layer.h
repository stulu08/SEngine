#pragma once
#include "Stulu/Core.h"
#include "Stulu/Events/Event.h"

namespace Stulu {
	class STULU_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach(){}
		virtual void onDetach(){}
		virtual void onUpdate(){}
		virtual void onEvent(Event& e){}

		inline const std::string& getName() const { return m_dName; }
	protected:
		std::string m_dName;
	};
}

