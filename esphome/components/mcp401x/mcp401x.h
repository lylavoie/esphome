#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace mcp401x {

class MCP401x : public output::FloatOutput, public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void set_initial_value(float initial_value) { initial_value_ = initial_value; }

 protected:
  void write_state(float state) override;
  float initial_value_;
};

}  // namespace mcp401x
}  // namespace esphome
