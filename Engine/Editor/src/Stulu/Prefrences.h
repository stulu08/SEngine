#pragma once
#include <Stulu/Core/INI.h>

namespace Editor {
	class Prefrences {
	public:
		inline Prefrences(const std::string& file) {
			Load(file);
		};

		Prefrences(Prefrences&& other) = default;

		void Load(const std::string& file) {
			m_ini = Stulu::INI(file, true, Stulu::INI::PARSE_COMMENTS_HASH);
		}

		void Save(const std::string& file = "") {
			m_ini.save(file, Stulu::INI::SAVE_PADDING_SECTIONS | Stulu::INI::SAVE_PRUNE);
		}

		bool Section(const std::string& key) {
			return m_ini.select(key);
		}
		template<class T>
		T GetAs(const std::string& key, const T& nullReturn = T()) {
			return m_ini.getAs<T>(key, nullReturn);
		}
		std::string Get(const std::string& key, const std::string& nullReturn = "") {
			return m_ini.get(key, nullReturn);
		}
		template<class T>
		void Set(const std::string& key, const T& value) {
			m_ini.setAs<T>(key, value);
		}
		template<class T>
		void Set(const std::string& section, const std::string& key, const T& value) {
			m_ini.setAs<T>(section, key, value);
		}
		template<>
		void Set(const std::string& key, const std::string& value) {
			m_ini.set(key, value);
		}
		template<>
		void Set(const std::string& section, const std::string& key, const std::string& value) {
			m_ini.set(section, key, value);
		}
	private:
		Stulu::INI m_ini;
	};
}