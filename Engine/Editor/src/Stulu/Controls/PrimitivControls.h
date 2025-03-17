#pragma once
#include "BaseControls.h"
#include <magic_enum/magic_enum.hpp>

namespace Editor {
    namespace Controls {
        inline bool Float(const std::string& label, float& value, float min = -FLT_MAX, float max = FLT_MAX, float speed = 1.0f, const char* format = "%.3f") {
            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                bool re = ImGui::DragFloat("##value_float", &value, speed, min, max, format);
                ImGui::PopItemWidth();
                return re;
            });
        }
        inline bool Int(const std::string& label, int32_t& value, int32_t min = -INT32_MAX, int32_t max = INT32_MAX, float speed = 1.0f, const char* format = "%d") {
            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                bool re = ImGui::DragInt("##value_int", &value, speed, min, max, format);
                ImGui::PopItemWidth();
                return re;
            });
        }
        inline bool UInt(const std::string& label, uint32_t& value, uint32_t min = 0, uint32_t max = INT32_MAX, float speed = 1.0f, const char* format = "%d") {
            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                int32_t tempValue = static_cast<int32_t>(value);
                bool re = ImGui::DragInt("##value_uint", &tempValue, speed, min, max, format);
                value = static_cast<uint32_t>(tempValue);
                ImGui::PopItemWidth();
                return re;
            });
        } 
        inline bool Bool(const std::string& label, bool& value) {
            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                bool re = ImGui::Checkbox("##value_bool", &value);
                ImGui::PopItemWidth();
                return re;
            });
        }
        inline bool BitFlag(const std::string& header, uint32_t flag, uint32_t& flagsVar) {
            bool value = (flagsVar & flag);
            if (Bool(header, value))
                if (value)
                    flagsVar |= flag;
                else
                    flagsVar &= ~flag;
            return value;
        }
        namespace Slider {
            inline bool Float(const std::string& label, float& value, float min = 0, float max = 1.0f, const char* format = "%.3f") {
                return LabeledBaseControl(label, [&]() {
                    ImGui::PushItemWidth(-1);
                    bool re = ImGui::SliderFloat("##value_slider_float", &value, min, max, format);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
            inline bool Int(const std::string& label, int32_t& value, int32_t min = -100, int32_t max = 100, const char* format = "%d") {
                return LabeledBaseControl(label, [&]() {
                    ImGui::PushItemWidth(-1);
                    bool re = ImGui::SliderInt("##value_slider_int", &value, min, max, format);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
            inline bool UInt(const std::string& label, uint32_t& value, uint32_t min = 0, uint32_t max = 100, const char* format = "%d") {
                return LabeledBaseControl(label, [&]() {
                    ImGui::PushItemWidth(-1);
                    int32_t tempValue = static_cast<int32_t>(value);
                    bool re = ImGui::SliderInt("##value_slider_uint", &tempValue, min, max, format);
                    value = static_cast<uint32_t>(tempValue);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
        }

        inline bool Combo(const std::string& label, int& currentValue, const std::string& optionsString) {
            static std::vector<std::string> items = Stulu::SplitString(optionsString);
            std::vector<const char*> itemPointers;
            for (const auto& item : items) {
                itemPointers.push_back(item.c_str());
            }

            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                bool re = ImGui::Combo("##value_combo", &currentValue, itemPointers.data(), (int)items.size());
                ImGui::PopItemWidth();
                return re;
            });
        }

        template<class E>
        inline bool Combo(const std::string& label, E& currentValue) {
            static constexpr auto names = magic_enum::enum_names<E>();
            static std::array<const char*, names.size()> items = {};

            // Convert to const char* array
            for (size_t i = 0; i < names.size(); ++i) {
                items[i] = names[i].data();
            }

            return LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);

                int currentIndex = static_cast<int>(currentValue);
                bool re = ImGui::Combo("##value_combo", &currentIndex, items.data(), static_cast<int>(items.size()));
                if (re) {
                    currentValue = static_cast<E>(currentIndex);
                }

                ImGui::PopItemWidth();
                return re;
                });
        }
    }
}