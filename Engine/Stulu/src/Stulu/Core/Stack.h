#pragma once
#include "Stulu/Core/Layer.h"

#include <vector>

namespace Stulu {
	template<class T>
	class Stack {
	public:
		inline Stack() {}
		inline ~Stack() {
			deleteAll();
		}

		inline void push(T* element) {
			m_elements.emplace(m_elements.begin() + m_inserIndex, element);
			m_inserIndex++;
		}
		// Will always stay at the back
		inline void pushOverlay(T* element) {
			m_elements.emplace_back(element);
		}
		inline void pop(T* element) {
			auto it = std::find(m_elements.begin(), m_elements.begin() + m_inserIndex, element);
			if (it != m_elements.begin() + m_inserIndex) {
				m_elements.erase(it);
				m_inserIndex--;
			}
		}
		inline void popOverlay(T* element) {
			auto it = std::find(m_elements.begin() + m_inserIndex, m_elements.end(), element);
			if (it != m_elements.end()) {
				m_elements.erase(it);
			}
		}

		inline void clear() {
			m_elements.clear();
		}
		inline void deleteAll() {
			for (T* element : m_elements) {
				delete element;
			}
			m_elements.clear();
		}

		auto begin() { return m_elements.begin(); }
		auto end() { return m_elements.end(); }
	private:
		std::vector<T*> m_elements;
		uint32_t m_inserIndex = 0;
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

