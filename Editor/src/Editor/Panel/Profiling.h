#pragma once
#include <Stulu.h>
namespace Stulu {
	class ProfilingPanel {
	public:
		ProfilingPanel() {};
		void draw(Timestep& ts, bool* open);
	private:
		float m_updateTimer = 1.0f;

		std::vector<float> m_fps = { .0f };
		float m_fAvg = .0f;
		std::vector<float> m_delta = { .0f };
		float m_dAvg = .0f;
		std::vector<float> m_virtualM = { .0f };
		float m_vAvg = .0f;
		std::vector<float> m_physical = { .0f };
		float m_pAvg = .0f;
	};
}

