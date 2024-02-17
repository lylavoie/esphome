#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/number/number.h"
#include "esphome/core/automation.h"
#include <vector>

namespace esphome {
namespace nixie_number {

class NixieNumberComponent : public number::Number, public PollingComponent {
 protected:
  void control(float value) override;

 public:
  NixieNumberComponent() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const { return setup_priority::LATE; }
  void update() override;
  void loop() override;

  void set_count_back(bool count_back) { this->count_back_ = count_back; };
  void set_count_back_speed(uint8_t speed) { this->count_back_speed_ = speed; };
  void set_pins(std::vector<GPIOPin *> pins) { pins_ = std::move(pins); };

 private:
  uint8_t previous_value_{0};
  unsigned long count_back_last_called_{0};
  bool count_back_{false};
  uint8_t count_back_speed_{5};
  uint8_t my_current_number_{0};
  uint8_t my_target_number_{0};
  std::vector<GPIOPin *> pins_;
  void set_outputs_(uint8_t value);
};

}  // namespace nixie_number
}  // namespace esphome
