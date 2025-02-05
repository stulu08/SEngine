#pragma once
#include "BaseControls.h"

namespace Editor {
    namespace Controls {
        inline void Float(const std::string& label, float& value, float min = -FLT_MAX, float max = FLT_MAX, float speed = 1.0f, const char* format = "%.3f") {
            LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                bool re = ImGui::DragFloat("##value", &value, speed, min, max, format);
                ImGui::PopItemWidth();
                return re;
            });
        }
        inline void Int(const std::string& label, int32_t& value, int32_t min = -INT32_MAX, int32_t max = INT32_MAX, float speed = 1.0f, const char* format = "%d") {
            LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                bool re = ImGui::DragInt("##value", &value, speed, min, max, format);
                ImGui::PopItemWidth();
                return re;
            });
        }
        inline void UInt(const std::string& label, uint32_t& value, uint32_t min = 0, uint32_t max = INT32_MAX, float speed = 1.0f, const char* format = "%d") {
            LabeledBaseControl(label, [&]() {
                ImGui::PushItemWidth(-1);
                int32_t tempValue = static_cast<int32_t>(value);
                bool re = ImGui::DragInt("##value", &tempValue, speed, min, max, format);
                value = static_cast<uint32_t>(value);
                ImGui::PopItemWidth();
                return re;
            });
        }
        namespace Slider {
            inline void Float(const std::string& label, float& value, float min = 0, float max = 1.0f, const char* format = "%.3f") {
                LabeledBaseControl(label, [&]() {
                    ImGui::PushItemWidth(-1);
                    bool re = ImGui::SliderFloat("##value", &value, min, max, format);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
            inline void Int(const std::string& label, int32_t& value, int32_t min = -100, int32_t max = 100, const char* format = "%d") {
                LabeledBaseControl(label, [&]() {
                    ImGui::PushItemWidth(-1);
                    bool re = ImGui::SliderInt("##value", &value, min, max, format);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
            inline void UInt(const std::string& label, uint32_t& value, uint32_t min = 0, uint32_t max = 100, const char* format = "%d") {
                LabeledBaseControl(label, [&]() {
                    ImGui::PushItemWidth(-1);
                    int32_t tempValue = static_cast<int32_t>(value);
                    bool re = ImGui::SliderInt("##value", &tempValue, min, max, format);
                    value = static_cast<uint32_t>(value);
                    ImGui::PopItemWidth();
                    return re;
                    });
            }
        }
    }
}