#include "mcp401x.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mcp401x {

static const char *const TAG = "mcp401x";

void MCP401x::setup() {
  ESP_LOGI(TAG, "Setting up MCP401x digital pot...");
  if (this->write(nullptr, 0) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "No MCP401x detected on the bus!");
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "Setting initial value of digital pot to %f", this->initial_value_);
  this->write_state(this->initial_value_);
}

void MCP401x::dump_config() {
  ESP_LOGCONFIG(TAG, "MCP401x:");
  LOG_I2C_DEVICE(this);
}

void MCP401x::write_state(float state) {
  auto new_wiper = static_cast<uint8_t>(roundf(state * 127));
  ESP_LOGV(TAG, "Attempting to writing new wiper state: %d", new_wiper);
  if (this->write(&new_wiper, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed writing new value to wiper!");
    this->mark_failed();
  }
}

}  // namespace mcp401x
}  // namespace esphome
