#pragma once

namespace StuluBindings {
	class Folders {
	public:
		static inline MonoString* AssetsPath() {
			return Stulu::Mono::String::New(getCoreDomain(), Stulu::Resources::AppAssetDir);
		}
	};
}