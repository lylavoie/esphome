#include "ds1807s.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ds1807s {

static const char *const TAG = "ds1807s.number";

static const uint8_t DS1807S_COMMAND_WRITE_POT_0 = 0xA9;
static const uint8_t DS1807S_COMMAND_WRITE_POT_1 = 0xAA;
static const uint8_t DS1807S_COMMAND_WRITE_BOTH_POTS = 0xAF;
static const uint8_t DS1807S_ENABLE_ZERO_CROSSING = 0xBD;
static const uint8_t DS1807S_DISABLE_ZERO_CROSSING = 0xBE;

void DS1807Scomponent::setup(){
    uint8_t zero[1];
    ESP_LOGI(TAG, "Setting up zero crossing...");
    if (this->zero_crossing_enabled_)
        zero[0] = DS1807S_ENABLE_ZERO_CROSSING;
    if (this->zero_crossing_enabled_ == false)
        zero[0] = DS1807S_DISABLE_ZERO_CROSSING;
    if (this->write(zero, 1)){
        ESP_LOGE(TAG, "Failed setting zero crossing mode!");
        this->mark_failed();
    }
}

void DS1807Scomponent::dump_config() {
    ESP_LOGCONFIG(TAG, "DS1807S:");
    LOG_I2C_DEVICE(this);
    if (this->is_failed()) 
        ESP_LOGE(TAG, "Connection with DS1807S failed!");
    LOG_NUMBER(TAG, "DS1807S Number", this);
}

void DS1807Scomponent::update(){
    uint8_t wipers[2];

    if (!this->read_bytes(0x0, wipers, 2)){
        ESP_LOGE(TAG, "Failed reading wiper values!");
        this->mark_failed();
        this->publish_state(NAN);
        return;
    }
    ESP_LOGD(TAG, "Read wipers as [%d, %d]", wipers[0], wipers[1]);

    switch (this->wiper_)
    {
    case DS1807Swiper::POT0:
        this->publish_state(wipers[0]);
        break;
    case DS1807Swiper::POT1:
        this->publish_state(wipers[1]);
        break;
    case DS1807Swiper::BOTH:
        this->publish_state((wipers[0]+wipers[1])/2);
        break;
    }
}

void DS1807Scomponent::control(float value){
    uint8_t wiper_value = static_cast<unsigned int>(value) & 0xFF;
    ESP_LOGI(TAG, "Writing new wiper value: %d", wiper_value);
    bool status;
    switch (this->wiper_)
    {
    case DS1807Swiper::POT0:
        status = this->write_byte(DS1807S_COMMAND_WRITE_POT_0, wiper_value);
        break;
    case DS1807Swiper::POT1:
        status = this->write_byte(DS1807S_COMMAND_WRITE_POT_1, wiper_value);
        break;
    case DS1807Swiper::BOTH:
        status = this->write_byte(DS1807S_COMMAND_WRITE_BOTH_POTS, wiper_value);
        break;
    }
    if (!status){
        ESP_LOGE(TAG, "Failed writing wiper value!");
        this->mark_failed();
        this->publish_state(NAN);
        return;
    }
    this->update();
}

bool DS1807Scomponent::readWipers(uint8_t *wipers){
    //this->set_i2c_address(0x28 || 0x01);
    
    return true;
}

}  // namespace ds1807s
}  // namespace esphome