#include "hv5522.h"
#include "esphome/core/log.h"
#include <sstream>
#include <string>
#include <iomanip>

namespace esphome {
namespace hv5522 {

static const char *const TAG = "HV5522";

void HV5522component::setup() {
  ESP_LOGD(TAG, "Setting up HV5522 via SPI bus...");
  this->spi_setup();
  this->latch_pin_->setup();
  this->latch_pin_->digital_write(false);
  this->write_gpio();
}

void HV5522component::dump_config() { ESP_LOGCONFIG(TAG, "HV5522:"); }

void HV5522component::digital_write_(uint16_t pin, bool value) {
  if (pin >= this->chip_count_ * 4 * 8) {
    ESP_LOGE(TAG, "Pin %u is out of range! Maximum pin number with %u chips is %u.", pin, this->chip_count_,
             (this->chip_count_ * 4 * 8) - 1);
    return;
  }
  if (value) {
    this->output_bytes_[pin / 8] |= (1 << (pin % 8));
  } else {
    this->output_bytes_[pin / 8] &= ~(1 << (pin % 8));
  }
  this->write_gpio();
}

void HV5522component::write_gpio() {
  ESP_LOGV(TAG, "Writing out %u bytes via SPI bus", this->output_bytes_.size());
  static std::vector<uint8_t> bytes;
  bytes = this->output_bytes_;

  // for (auto byte = this->output_bytes_.rbegin(); byte != this->output_bytes_.rend(); byte++) {
  //   ESP_LOGV(TAG, "  Writing byte: 0x%02X", *byte);
  //   this->enable();
  //   this->transfer_byte(*byte);
  //   this->disable();
  // }

  std::reverse(bytes.begin(), bytes.end());
  this->enable();
  this->write_array(bytes);
  this->disable();
  ESP_LOGV(TAG, "Pulsing the latch pin to set registers.");
  this->latch_pin_->digital_write(true);
  this->latch_pin_->digital_write(false);
}

void HV5522Pin::digital_write(bool value) { this->parent_->digital_write_(this->pin_, value != this->inverted_); }

std::string HV5522Pin::dump_summary() const { return str_snprintf("%u GPIO output via HV5522", 27, this->pin_); }

}  // namespace hv5522
}  // namespace esphome
