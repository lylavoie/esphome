#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/defines.h"

#ifdef USE_SPI
#include "esphome/components/spi/spi.h"
#endif

#include <vector>

namespace esphome {
namespace hv5522 {

class HV5522component : public Component {
 public:
  HV5522component() = default;
  void setup() override = 0;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }
  void set_latch_pin(GPIOPin *pin) { this->latch_pin_ = pin; }
  void set_chip_count(uint8_t count) {
    this->chip_count_ = count;
    this->output_bytes_.resize(count * 4);
  }

 protected:
  friend class HV5522Pin;
  void digital_write_(uint16_t pint, bool value);
  virtual void write_gpio();

  GPIOPin *latch_pin_;
  uint8_t chip_count_;
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

class HV5522GPIOcomponent : public HV5522component {
 public:
  void setup() override;
  void set_clock_pin(GPIOPin *pin) { this->clock_pin_ = pin; }
  void set_data_pin(GPIOPin *pin) { this->data_pin_ = pin; }

 protected:
  void write_gpio() override;
  GPIOPin *clock_pin_;
  GPIOPin *data_pin_;
};

#ifdef USE_SPI
class HV5522SPIcomponent : public HV5522component,
                           public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_HIGH,
                                                 spi::CLOCK_PHASE_TRAILING, spi::DATA_RATE_8MHZ> {
 public:
  void setup() override;

 protected:
  void write_gpio() override;
};
#endif

}  // namespace hv5522
}  // namespace esphome
