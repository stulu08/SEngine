#include "st_pch.h"
#include "LayerStack.h"

namespace Stulu {
	LayerStack::LayerStack() { }
	LayerStack::~LayerStack(){
		for (Layer* layer : m_layers)
			delete layer;
	}

	void LayerStack::pushLayer(Layer* layer){
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
		layer->onAttach();
		m_layerInsertIndex++;
	}
	void LayerStack::pushOverlay(Layer* layer){
		m_layers.emplace_back(layer);
		layer->onAttach();
	}
	void LayerStack::popLayer(Layer* layer){
		auto it = std::find(m_layers.begin(), m_layers.begin() + m_layerInsertIndex, layer);

		if (it != m_layers.end()) {
			layer->onDetach();
			m_layers.erase(it);
			m_layerInsertIndex--;

		}
	}
	void LayerStack::popOverlay(Layer* layer){
		auto it = std::find(m_layers.begin() + m_layerInsertIndex, m_layers.end(), layer);

		if (it != m_layers.end()) {
			layer->onDetach();
			m_layers.erase(it);
		}
	}
}
