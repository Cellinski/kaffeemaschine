#include "marax_reader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace marax_reader {

static const char *TAG = "marax_reader";

void MaraXReaderComponent::loop() {
  if (uart_ == nullptr) {
    ESP_LOGE(TAG, "UART not set!");
    return;
  }

  uint8_t c;
  while (uart_->available()) {
    if (uart_->read_byte(&c)) {
      if (c == '\n') {
        ESP_LOGD(TAG, "Received line: %s", buffer_.c_str());
        process_line(buffer_);
        buffer_.clear();
      } else if (c != '\r') {
        buffer_ += static_cast<char>(c);
      }
    }
  }
}

void MaraXReaderComponent::process_line(const std::string &line) {
  ESP_LOGD(TAG, "Processing: %s", line.c_str());

  std::vector<std::string> parts;
  size_t start = 0;
  size_t end = line.find(',');

  while (end != std::string::npos) {
    parts.push_back(line.substr(start, end - start));
    start = end + 1;
    end = line.find(',', start);
  }
  parts.push_back(line.substr(start));

  if (parts.size() < 6) {
    ESP_LOGW(TAG, "Invalid data length: %d", parts.size());
    return;
  }

  // Mode + SW
  std::string mode_sw = parts[0];
  char mode_char = mode_sw[0];
  std::string sw_version = mode_sw.substr(1);

  std::string mode = (mode_char == 'C') ? "Coffee" : "Steam";

  mode_sensor_->publish_state(mode);
  sw_sensor_->publish_state(sw_version);

  steam_sensor_->publish_state(std::stoi(parts[1]));
  target_sensor_->publish_state(std::stoi(parts[2]));
  hx_sensor_->publish_state(std::stoi(parts[3]));
  boost_sensor_->publish_state(std::stoi(parts[4]));

  heat_sensor_->publish_state(parts[5] == "1");
  pump_sensor_->publish_state(parts[5] == "1");
}

}  // namespace marax_reader
}  // namespace esphome
