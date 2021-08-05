#pragma once

namespace Stulu {
	class Platform {
	public:
		inline static float getTime() { return s_instance->impl_getTime(); }
	protected:
		virtual float impl_getTime() = 0;
	private:
		static Scope<Platform> s_instance;
	};
}