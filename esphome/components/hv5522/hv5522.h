#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/defines.h"
#include "esphome/components/spi/spi.h"
#include <vector>

namespace esphome {
namespace hv5522 {

class HV5522component : public Component,
                        public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_TRAILING, spi::DATA_RATE_8MHZ> {
 public:
  HV5522component() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }
  void set_latch_pin(GPIOPin *pin) { this->latch_pin_ = pin; }
  void set_chip_count(uint8_t count) {
    this->chip_count_ = count;
    this->max_pins_ = count * 4;
    this->output_bytes_.resize(count * 4);
  }

 protected:
  friend class HV5522Pin;
  friend class HV5522NumberComponent;
  void digital_write_(uint16_t pint, bool value);
  void write_bytes();

  GPIOPin *latch_pin_;
  uint8_t chip_count_;
  uint8_t max_pins_;
  std::vector<uint8_t> output_bytes_;
};

class HV5522Pin : public GPIOPin, public Parented<HV5522component> {
 public:
  void setup() override{};
  void pin_mode(gpio::Flags flags) override{};
  bool digital_read() override { return false; }
  void digital_write(bool value) override;
  std::string dump_summary() const override;
  void set_pin(uint16_t pin) { pin_ = pin; }
  void set_inverted(bool inverted) { inverted_ = inverted; }

 protected:
  uint16_t pin_;
  bool inverted_;
};

}  // namespace hv5522
}  // namespace esphome
