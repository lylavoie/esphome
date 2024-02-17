#include "nixie_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nixie_number {

static const char *const TAG = "nixie.number";

void NixieNumberComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up nixie number...");
  // initialize pins
  for (auto pin : this->pins_) {
    pin->setup();
    pin->digital_write(false);
  }
}  // NixieNumberComponent::setup()

void NixieNumberComponent::dump_config() {
  LOG_NUMBER("", "Nixie Number", this);
  ESP_LOGCONFIG(TAG, "  Count Back: %s", YESNO(this->count_back_));
  ESP_LOGCONFIG(TAG, "  Count Back Speed: %d", this->count_back_speed_);
  ESP_LOGCONFIG(TAG, "  Output Pins:");
  for (auto pin : this->pins_)
    LOG_PIN("    Pin: ", pin);
  LOG_UPDATE_INTERVAL(this);
}  // NixieNumberComponent::dump_config()

void NixieNumberComponent::loop() {
  if (this->count_back_ && this->my_current_number_ > this->my_target_number_) {
    if (millis() > this->count_back_last_called_ + this->count_back_speed_) {
      this->count_back_last_called_ = millis();
      this->set_outputs_(--this->my_current_number_);
    }
  }
}  // NixieNumberComponent::loop()

void NixieNumberComponent::control(float value) {
  this->my_target_number_ = static_cast<uint8_t>(value);
  if (!this->count_back_ || this->my_current_number_ < this->my_target_number_) {
    this->my_current_number_ = this->my_target_number_;
  }
  this->set_outputs_(this->my_current_number_);
  this->publish_state(this->my_target_number_);
}  // NixieNumberComponent::control(float value)

void NixieNumberComponent::update() { this->publish_state(this->my_target_number_); }

void NixieNumberComponent::set_outputs_(uint8_t value) {
  for (auto pin : this->pins_)
    pin->digital_write(false);
  // this->pins_[this->previous_value_]->digital_write(false);
  // this->previous_value_ = value;
  this->pins_[value]->digital_write(true);
}  // NixieNumberComponent::_set_outputs(uint8_t value)

}  // namespace nixie_number
}  // namespace esphome
