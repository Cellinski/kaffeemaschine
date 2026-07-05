#pragma once

#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include <string>
#include <vector>

namespace esphome {
namespace marax_reader {

class MaraXReaderComponent : public Component {
 public:
  MaraXReaderComponent();

  void set_uart(esphome::uart::UARTComponent *uart) { uart_ = uart; }

  void set_mode_sensor(text_sensor::TextSensor *s) { mode_sensor_ = s; }
  void set_sw_sensor(text_sensor::TextSensor *s) { sw_sensor_ = s; }

  void set_steam_sensor(sensor::Sensor *s) { steam_sensor_ = s; }
  void set_target_sensor(sensor::Sensor *s) { target_sensor_ = s; }
  void set_hx_sensor(sensor::Sensor *s) { hx_sensor_ = s; }
  void set_boost_sensor(sensor::Sensor *s) { boost_sensor_ = s; }

  void set_heat_sensor(binary_sensor::BinarySensor *s) { heat_sensor_ = s; }
  void set_pump_sensor(binary_sensor::BinarySensor *s) { pump_sensor_ = s; }

  void setup() override {}
  void loop() override;

 protected:
  void process_line(const std::string &line);

  esphome::uart::UARTComponent *uart_{nullptr};
  std::string buffer_;

  text_sensor::TextSensor *mode_sensor_{nullptr};
  text_sensor::TextSensor *sw_sensor_{nullptr};

  sensor::Sensor *steam_sensor_{nullptr};
  sensor::Sensor *target_sensor_{nullptr};
  sensor::Sensor *hx_sensor_{nullptr};
  sensor::Sensor *boost_sensor_{nullptr};

  binary_sensor::BinarySensor *heat_sensor_{nullptr};
  binary_sensor::BinarySensor *pump_sensor_{nullptr};
};

}  // namespace marax_reader
}  // namespace esphome
