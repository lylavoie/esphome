#include "hv5522.h"
#include "esphome/core/log.h"
// #include <sstream>
// #include <string>
// #include <iomanip>

namespace esphome {
namespace hv5522 {

static const char *const TAG = "HV5522";

void HV5522component::setup() {
  ESP_LOGD(TAG, "Setting up HV5522 via SPI bus...");
  this->spi_setup();
  this->latch_pin_->setup();
  this->latch_pin_->digital_write(false);
  this->write_bytes();
}

void HV5522component::dump_config() { ESP_LOGCONFIG(TAG, "HV5522:"); }

void HV5522component::digital_write_(uint16_t pin, bool value) {
  if (pin > this->max_pins_ * 8) {
    ESP_LOGE(TAG, "Pin %u is out of range! Maximum pin number with %u chips is %u.", pin, this->chip_count_,
             (this->max_pins_ * 8));
    return;
  }
  if (value) {
    this->output_bytes_[(this->max_pins_) - (pin / 8) - 1] |= (1 << (pin % 8));
  } else {
    this->output_bytes_[(this->max_pins_) - (pin / 8) - 1] &= ~(1 << (pin % 8));
  }
  this->write_bytes();
}

void HV5522component::write_bytes() {
#ifdef ESP_LOGV
  ESP_LOGV(TAG, "Writing out %u bytes via SPI bus in this order:", this->output_bytes_.size());
  for (auto byte = this->output_bytes_.rbegin(); byte != this->output_bytes_.rend(); ++byte)
    ESP_LOGV(TAG, "  0x%02X", *byte);
#endif
  this->enable();
  this->write_array(this->output_bytes_);
  this->disable();
  ESP_LOGV(TAG, "Pulsing the latch pin to set registers.");
  this->latch_pin_->digital_write(true);
  this->latch_pin_->digital_write(false);
}

void HV5522Pin::digital_write(bool value) { this->parent_->digital_write_(this->pin_, value != this->inverted_); }

std::string HV5522Pin::dump_summary() const { return str_snprintf("%u GPIO output via HV5522", 27, this->pin_); }

}  // namespace hv5522
}  // namespace esphome
