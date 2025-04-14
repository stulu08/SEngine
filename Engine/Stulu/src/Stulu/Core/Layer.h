#pragma once
#include "Stulu/Events/Event.h"
#include "Stulu/Types/Timestep.h"
#include "Stulu/Types/Stack.h"

namespace Stulu {
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

	template<>
	class Stack<Layer> {
	public:
		inline Stack<Layer>() {}
		inline ~Stack<Layer>() {
			deleteAll();
		}

		inline void pushLayer(Layer* layer) {
			m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
			layer->onAttach();
			m_layerInsertIndex++;
		}
		inline void pushOverlay(Layer* layer) {
			m_layers.emplace_back(layer);
			layer->onAttach();
		}
		inline void popLayer(Layer* layer) {
			auto it = std::find(m_layers.begin(), m_layers.begin() + m_layerInsertIndex, layer);
			if (it != m_layers.begin() + m_layerInsertIndex) {
				layer->onDetach();
				m_layers.erase(it);
				m_layerInsertIndex--;

			}
		}
		inline void popOverlay(Layer* layer) {
			auto it = std::find(m_layers.begin() + m_layerInsertIndex, m_layers.end(), layer);
			if (it != m_layers.end()) {
				layer->onDetach();
				m_layers.erase(it);
			}
		}

		inline void clear() {
			m_layers.clear();
		}
		inline void deleteAll() {
			for (Layer* layer : m_layers) {
				layer->onDetach();
				delete layer;
			}
			m_layers.clear();
		}

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }
	private:
		std::vector<Layer*> m_layers;
		uint32_t m_layerInsertIndex = 0;
	};

	using LayerStack = Stack<Layer>;
}

