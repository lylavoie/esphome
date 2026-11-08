#include "morse_code_sender.h"

#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome::morse_code_sender {

ESPHOME_LOG_TAG(TAG, "morse_code_sender");

void MorseCodeSender::setup() {
  this->output_->turn_off();
  this->disable_loop();
}

void MorseCodeSender::dump_config() {
  ESP_LOGCONFIG(TAG,
                "Morse Code Sender:\n"
                "  Unit length: %" PRIu32 " ms",
                this->unit_ms_);
}

void MorseCodeSender::send_keys(const std::string &keys) {
  if (!this->is_sending()) {
    this->queue_.clear();
    this->pos_ = 0;
  }
  this->queue_ += keys;
  ESP_LOGD(TAG, "Queued '%s'", keys.c_str());
  if (this->wait_ms_ == 0 && !this->key_down_) {
    this->next_element_(millis());
  }
  this->enable_loop();
}

void MorseCodeSender::loop() {
  const uint32_t now = App.get_loop_component_start_time();
  if (now - this->element_start_ < this->wait_ms_)
    return;

  if (this->key_down_) {
    // End of a dot/dash: key up for the one-unit inter-element gap.
    this->output_->turn_off();
    this->key_down_ = false;
    this->element_start_ = now;
    this->wait_ms_ = this->unit_ms_;
    return;
  }
  this->next_element_(now);
}

void MorseCodeSender::next_element_(uint32_t now) {
  this->element_start_ = now;
  this->wait_ms_ = 0;
  while (this->pos_ < this->queue_.size()) {
    const char c = this->queue_[this->pos_++];
    switch (c) {
      case '.':
        this->wait_ms_ = this->unit_ms_;
        break;
      case '-':
        this->wait_ms_ = this->unit_ms_ * 3;
        break;
      case ' ':
        // Adds to the one-unit gap after the previous element: 1 space = 3 units, 3 spaces = 7 units.
        this->wait_ms_ = this->unit_ms_ * 2;
        return;
      default:
        ESP_LOGW(TAG, "Ignoring invalid key '%c'", c);
        continue;
    }
    this->output_->turn_on();
    this->key_down_ = true;
    return;
  }
  // Nothing left to send.
  this->queue_.clear();
  this->pos_ = 0;
  this->disable_loop();
}

}  // namespace esphome::morse_code_sender
