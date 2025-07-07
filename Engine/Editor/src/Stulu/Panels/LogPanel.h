#pragma once
#include "Stulu/Panel.h"
#include "Stulu/Notification.h"

namespace Editor {
	struct LogEntry {
		LogEntry() = default;
		LogEntry(const LogEntry&) = default;
		LogEntry(LogEntry&&) = default;

		Stulu::Log::Level level;
		std::string msg;
		std::chrono::system_clock::time_point timestamp;
	};

	class LogPanel : public Panel {
	public:
		LogPanel();
		virtual ~LogPanel();

		virtual void Update();

		virtual void DrawImGui() override;

		static inline void Clear() {
			s_entries.clear();
		}
		static inline void AddEntry(LogEntry&& entry) {
			if ((uint32_t)entry.level >= (uint32_t)Stulu::Log::Level::error)
				s_alert = true;


			switch (entry.level)
			{
			case Stulu::Log::Level::error:
				ImGui::InsertNotification({ ImGuiToastType::Error, 3000, entry.msg.c_str() });
				break;
			case Stulu::Log::Level::critical:
				ImGui::InsertNotification({ ImGuiToastType::Error, 3000, entry.msg.c_str() });
				break;
			default:
				break;
			}

			s_entries.push_back(std::move(entry));
		}

		bool IsClearOnPlay() const {
			return m_clearOnPlay;
		}
	private:
		static std::vector<LogEntry> s_entries;
		static bool s_alert;

		bool m_showInfo : 1;
		bool m_showWarn : 1;
		bool m_showError : 1;
		bool m_clearOnPlay : 1;

		void DrawEntry(const LogEntry& entry) const;
	};

	class MemoryLogSink : public spdlog::sinks::base_sink<std::mutex> {
	public:
		std::vector<LogEntry> log_entries;

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override {
			// Clone message into string
			spdlog::memory_buf_t formatted;
			base_sink<std::mutex>::formatter_->format(msg, formatted);

			LogEntry entry = {
				(Stulu::Log::Level)msg.level,
				std::string(formatted.data(), formatted.size()),
				std::chrono::system_clock::now()
			};

			LogPanel::AddEntry(std::move(entry));
		}

		void flush_() override {}
	};
}


