#pragma once
#include <string>

namespace Editor {
	class PremakeCompiler {
	public:
		PremakeCompiler(const std::string& target);

		bool GenerateBuildFiles();
	private:
		std::string m_target;
	};

	enum class BuildConfiguration {
		Debug = 1,
		Release = 2,
		Dist = 3,
	};
	
	struct BuildTarget {
	public:
		static constexpr uint8_t Runtime = 0x01;
		static constexpr uint8_t Native = 0x02;
		static constexpr uint8_t Managed = 0x04;

		constexpr BuildTarget(uint8_t value)
			: m_value(value){}

		operator uint8_t() const { return m_value; }
	private:
		uint8_t m_value;
	};

#ifdef ST_DEBUG
	constexpr BuildConfiguration StandartBuildConfig = BuildConfiguration::Debug;
#elif defined(ST_RELEASE)
	constexpr BuildConfiguration StandartBuildConfig = BuildConfiguration::Release;
#else
	constexpr BuildConfiguration StandartBuildConfig = BuildConfiguration::Dist;
#endif

	struct BuildSettings {
		constexpr BuildSettings(BuildConfiguration config, BuildTarget targets)
			: config(config), targets(targets){}

		BuildConfiguration config = StandartBuildConfig;
		BuildTarget targets = BuildTarget::Native | BuildTarget::Managed;
	};

	constexpr BuildSettings EditorBuildSettings = { StandartBuildConfig, BuildTarget::Native | BuildTarget::Managed };
	constexpr BuildSettings RuntimeBuildSettings = { StandartBuildConfig, BuildTarget::Native | BuildTarget::Managed | BuildTarget::Runtime };

	class EngineCompiler {
	public:
		EngineCompiler(const std::string& targetPath, const std::string& cachePath)
			: m_target(targetPath), m_cache(cachePath) {};

		bool CompileAssemblies(const BuildSettings& settings);

	private:
		std::string m_target;
		std::string m_cache;

		std::string GenerateBuildScript(const BuildSettings& settings);
	};
}