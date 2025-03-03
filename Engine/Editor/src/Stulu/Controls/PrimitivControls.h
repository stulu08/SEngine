#pragma once
#include "BaseControls.h"

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
                value = static_cast<uint32_t>(value);
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
                    value = static_cast<uint32_t>(value);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
        }
    }
}