#pragma once
#include <Stulu.h>
namespace Stulu {
	class ProfilingPanel {
	public:
		ProfilingPanel() {};
		void draw(Timestep& ts, bool* open);
	private:
		float m_updateTimer = 1.0f;

		std::vector<float> m_fps;
		float m_fAvg = .0f;
		std::vector<float> m_delta;
		float m_dAvg = .0f;
		std::vector<float> m_virtualM;
		float m_vAvg = .0f;
		std::vector<float> m_physical;
		float m_pAvg = .0f;
	};
}

