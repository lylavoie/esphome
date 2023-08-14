#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace ds1807s {

enum DS1807Swiper {
    POT0,
    POT1,
    BOTH,
};

class DS1807Scomponent : public number::Number, public PollingComponent, public i2c::I2CDevice {
    public:
        void setup() override;
        void dump_config() override;
        void update() override;
        void set_wiper(DS1807Swiper wiper){ this->wiper_ = wiper; };
        void set_zero_crossing(bool zero_crossing){ this->zero_crossing_enabled_ = zero_crossing; };
    protected:
        void control(float value) override;
        DS1807Swiper wiper_;
        bool zero_crossing_enabled_;
        bool readWipers(uint8_t * wipers);
};

}  // namespace ds1807
}  // namespace esphome