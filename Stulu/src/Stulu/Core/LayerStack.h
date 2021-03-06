#pragma once
#include "Stulu/Core/Layer.h"

#include <vector>

namespace Stulu {
	class STULU_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		void popLayer(Layer* layer);
		void popOverlay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }
	private:
		std::vector<Layer*> m_layers;
		uint32_t m_layerInsertIndex = 0;
	};
}

