#include "nixie_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nixie_number {

static const char *const TAG = "nixie.number";

void NixieNumberComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up nixie number...");
  // initialize pins
  for (auto *pin : this->_pins) {
    pin->setup();
    pin->digital_write(false);
  }
}  // NixieNumberComponent::setup()

void NixieNumberComponent::dump_config() {
  LOG_NUMBER("", "Nixie Number", this);
  ESP_LOGCONFIG(TAG, "  Count Back: %s", YESNO(this->_count_back));
  ESP_LOGCONFIG(TAG, "  Count Back Speed: %d", this->_count_back_speed);
  LOG_UPDATE_INTERVAL(this);
}  // NixieNumberComponent::dump_config()

void NixieNumberComponent::loop() {
  static unsigned long count_back_last_called = 0;
  if (this->_my_current_number > this->_my_target_number) {
    if (millis() > count_back_last_called + this->_count_back_speed) {
      count_back_last_called = millis();
      this->_set_outputs(--this->_my_current_number);
    }
  }
}  // NixieNumberComponent::loop()

void NixieNumberComponent::control(float value) {
  this->_my_target_number = static_cast<uint8_t>(value);
  if (!this->_count_back || this->_my_current_number < value) {
    this->_my_current_number = this->_my_target_number;
  }
  this->_set_outputs(this->_my_current_number);
  this->publish_state(this->_my_target_number);
}  // NixieNumberComponent::control(float value)

void NixieNumberComponent::update() { this->publish_state(this->_my_target_number); }

void NixieNumberComponent::_set_outputs(uint8_t value) {
  static auto previous_value = 0;
  this->_pins[previous_value]->digital_write(false);
  previous_value = value;
  this->_pins[value]->digital_write(true);
}  // NixieNumberComponent::_set_outputs(uint8_t value)

}  // namespace nixie_number
}  // namespace esphome
