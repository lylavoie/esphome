#pragma once

#include <string>

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome::morse_code_sender {

class MorseCodeSender : public Component {
 public:
  explicit MorseCodeSender(switch_::Switch *output) : output_(output) {}

  void setup() override;
  void loop() override;
  void dump_config() override;

  /// Keying rate in words per minute (PARIS standard: one unit = 1200 / wpm ms).
  void set_wpm(uint32_t wpm) { this->unit_ms_ = 1200 / wpm; }

  /// Queue keys to send: '.' = dot, '-' = dash, ' ' = extra silence (letter/word gap).
  void send_keys(const std::string &keys);

  bool is_sending() const { return this->pos_ < this->queue_.size() || this->key_down_ || this->wait_ms_ != 0; }

 protected:
  void next_element_(uint32_t now);

  switch_::Switch *output_;
  std::string queue_;
  size_t pos_{0};
  uint32_t unit_ms_{60};
  uint32_t element_start_{0};
  uint32_t wait_ms_{0};
  bool key_down_{false};
};

}  // namespace esphome::morse_code_sender
