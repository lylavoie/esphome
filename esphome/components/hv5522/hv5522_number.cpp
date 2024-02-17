#include "hv5522_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hv5522 {

static const char *const TAG = "hv5522.number";

void HV5522NumberComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up nixie number...");
  // FIXME: set all pins to off
}  // HV5522NumberComponent::setup()

void HV5522NumberComponent::dump_config() {
  LOG_NUMBER("", "Nixie Number", this);
  ESP_LOGCONFIG(TAG, "  Count Back: %s", YESNO(this->count_back_));
  if (this->count_back_)
    ESP_LOGCONFIG(TAG, "  Count Back Speed: %d", this->count_back_speed_);
  ESP_LOGCONFIG(TAG, "  Output Pins:");
  for (auto pin = this->pins_.begin(); pin != this->pins_.end(); ++pin)
    ESP_LOGCONFIG(TAG, "    Digit %d: pin number %d", std::distance(this->pins_.begin(), pin), *pin);
  LOG_UPDATE_INTERVAL(this);
}  // HV5522NumberComponent::dump_config()

void HV5522NumberComponent::loop() {
  if (this->count_back_ && this->my_current_number_ > this->my_target_number_) {
    if (millis() > this->count_back_last_called_ + this->count_back_speed_) {
      this->count_back_last_called_ = millis();
      this->set_outputs_(--this->my_current_number_);
    }
  }
}  // HV5522NumberComponent::loop()

void HV5522NumberComponent::control(float value) {
  this->my_target_number_ = static_cast<uint8_t>(value);
  if (!this->count_back_ || this->my_current_number_ < this->my_target_number_) {
    this->my_current_number_ = this->my_target_number_;
  }
  this->set_outputs_(this->my_current_number_);
  this->publish_state(this->my_target_number_);
}  // HV5522NumberComponent::control(float value)

void HV5522NumberComponent::update() { this->publish_state(this->my_target_number_); }

void HV5522NumberComponent::set_outputs_(uint8_t value) {
  for (uint8_t pin : this->pins_) {
    this->parent_->output_bytes_[this->parent_->max_pins_ - (pin / 8) - 1] &= ~(1 << (pin % 8));
  }
  this->parent_->output_bytes_[this->parent_->max_pins_ - (this->pins_[value] / 8) - 1] |=
      (1 << (this->pins_[value] % 8));
  this->parent_->write_bytes();
  this->parent_->write_bytes();
}  // HV5522NumberComponent::_set_outputs(uint8_t value)

}  // namespace hv5522
}  // namespace esphome
