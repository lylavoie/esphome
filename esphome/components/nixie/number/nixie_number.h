#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/number/number.h"
#include "esphome/core/automation.h"
#include <vector>

namespace esphome {
namespace nixie_number {

class NixiePins : public GPIOPin {};

class NixieNumberComponent : public number::Number, public PollingComponent {
 protected:
  std::vector<GPIOPin *> _pins;
  bool _count_back{false};
  uint8_t _count_back_speed{5};
  uint8_t _my_current_number{0};
  uint8_t _my_target_number{0};

  void control(float value) override;

 public:
  NixieNumberComponent() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const { return setup_priority::IO; }
  void update() override;
  void loop() override;

  void set_count_back(bool count_back) { this->_count_back = count_back; };
  void set_count_back_speed(uint8_t speed) { this->_count_back_speed = speed; };
  void set_pins(std::vector<GPIOPin *> pins) { _pins = std::move(pins); };

 private:
  void _set_outputs(uint8_t value);
};

}  // namespace nixie_number
}  // namespace esphome
