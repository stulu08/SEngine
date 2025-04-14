#pragma once
#include "Stulu/Core/Core.h"

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
}

