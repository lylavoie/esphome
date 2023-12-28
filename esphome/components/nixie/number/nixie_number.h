#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/number/number.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace nixie_number {

class NixieNumberComponent : public number::Number, public PollingComponent {
 protected:
  GPIOPin *_zero_pin;
  GPIOPin *_one_pin;
  GPIOPin *_two_pin;
  GPIOPin *_three_pin;
  bool _count_back{false};
  uint8_t _count_back_speed{5};
  uint8_t _my_current_number{0};
  uint8_t _my_target_number{0};

  void control(float value) override;
  void _set_outputs(uint8_t value);

 public:
  NixieNumberComponent() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const { return setup_priority::IO; }
  void update() override;
  void loop() override;

  void set_zero_pin(GPIOPin *pin) { this->_zero_pin = pin; };
  void set_one_pin(GPIOPin *pin) { this->_one_pin = pin; };
  void set_two_pin(GPIOPin *pin) { this->_two_pin = pin; };
  void set_three_pin(GPIOPin *pin) { this->_three_pin = pin; };
  void set_count_back(bool count_back) { this->_count_back = count_back; };
  void set_count_back_speed(uint8_t speed) { this->_count_back_speed = speed; };

 private:
};

}  // namespace nixie_number
}  // namespace esphome
