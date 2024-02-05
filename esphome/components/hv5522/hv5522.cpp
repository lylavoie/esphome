#include "hv5522.h"
#include "esphome/core/log.h"
#include <sstream>
#include <string>
#include <iomanip>

namespace esphome {
namespace hv5522 {

static const char *const TAG = "HV5522";

void HV5522GPIOcomponent::setup() {
  ESP_LOGD(TAG, "Setting up HV5522 using GPIO pins...");
  this->clock_pin_->setup();
  this->data_pin_->setup();
  this->clock_pin_->digital_write(true);
  this->data_pin_->digital_write(false);
  this->latch_pin_->setup();
  this->latch_pin_->digital_write(false);
  this->write_gpio();
}

#ifdef USE_SPI
void HV5522SPIcomponent::setup() {
  ESP_LOGD(TAG, "Setting up HV5522 via SPI bus...");
  this->spi_setup();
  this->latch_pin_->setup();
  this->latch_pin_->digital_write(false);
  this->write_gpio();
}
#endif

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

void HV5522GPIOcomponent::write_gpio() {
  ESP_LOGV(TAG, "Shifting out %u bytes using GPIO...", this->output_bytes_.size());
  for (auto byte = this->output_bytes_.rbegin(); byte != this->output_bytes_.rend(); byte++) {
    ESP_LOGV(TAG, "  Writing byte: 0x%02X", *byte);
    for (int8_t i = 7; i >= 0; i--) {
      bool bit = (*byte >> i) & 0x01;
      ESP_LOGVV(TAG, "    Writing bit: 0x%X", bit);
      this->data_pin_->digital_write(bit);
      this->clock_pin_->digital_write(false);
      this->clock_pin_->digital_write(true);
    }
  }
  HV5522component::write_gpio();
}

#ifdef USE_SPI
void HV5522SPIcomponent::write_gpio() {
  ESP_LOGV(TAG, "Writing out %u bytes via SPI bus", this->output_bytes_.size());
  for (auto byte = this->output_bytes_.rbegin(); byte != this->output_bytes_.rend(); byte++) {
    ESP_LOGV(TAG, "  Writing byte: 0x%02X", *byte);
    this->enable();
    this->transfer_byte(*byte);
    this->disable();
  }
  HV5522component::write_gpio();
}
#endif

void HV5522component::write_gpio() {
  ESP_LOGV(TAG, "Pulsing the latch pin to set registers.");
  this->latch_pin_->digital_write(true);
  this->latch_pin_->digital_write(false);
}

void HV5522Pin::digital_write(bool value) { this->parent_->digital_write_(this->pin_, value != this->inverted_); }

std::string HV5522Pin::dump_summary() const { return str_snprintf("%u GPIO output via HV5522", 27, this->pin_); }

}  // namespace hv5522
}  // namespace esphome
