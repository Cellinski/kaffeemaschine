#include "marax_reader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace marax_reader {

static const char *TAG = "marax_reader";
MaraXReaderComponent::MaraXReaderComponent() {
  buffer_.reserve(128);
}
// -------------------------
// safe helpers
// -------------------------
// =========================
// helper functions (HIER)
// =========================

static inline int safe_stoi(const std::string &s, int fallback = 0) {
  if (s.empty()) return fallback;

  int result = 0;
  bool negative = false;
  size_t i = 0;

  if (s[0] == '-') {
    negative = true;
    i = 1;
  }

  for (; i < s.size(); i++) {
    if (s[i] < '0' || s[i] > '9') {
      return fallback;
    }
    result = result * 10 + (s[i] - '0');
  }

  return negative ? -result : result;
}

static inline bool safe_bool(const std::string &s) {
  return s == "1" || s == "true" || s == "TRUE";
}

// -------------------------
// main loop
// -------------------------
void MaraXReaderComponent::loop() {
  if (!uart_) {
    ESP_LOGE(TAG, "UART not set!");
    return;
  }

  uint8_t c;

  while (uart_->available()) {
    if (uart_->read_byte(&c)) {

      // line complete
      if (c == '\n') {
        if (!buffer_.empty()) {
          process_line(buffer_);
        }
        buffer_.clear();
      }

      // ignore CR
      else if (c != '\r') {
        // buffer protection (IMPORTANT)
        if (buffer_.size() < 128) {
          buffer_ += static_cast<char>(c);
        } else {
          ESP_LOGW(TAG, "Line too long, resetting buffer");
          buffer_.clear();
        }
      }
    }
  }
}

// -------------------------
// parser
// -------------------------
void MaraXReaderComponent::process_line(const std::string &line) {
  ESP_LOGD(TAG, "Line: %s", line.c_str());

  std::vector<std::string> parts;
  parts.reserve(8);

  size_t start = 0;
  size_t end = line.find(',');

  while (end != std::string::npos) {
    parts.push_back(line.substr(start, end - start));
    start = end + 1;
    end = line.find(',', start);
  }
  parts.push_back(line.substr(start));

  // -------------------------
  // validation
  // -------------------------
  if (parts.size() < 6) {
    ESP_LOGW(TAG, "Invalid frame (too short): %d fields", parts.size());
    return;
  }

  // -------------------------
  // mode + firmware
  // -------------------------
  const std::string &mode_sw = parts[0];

  if (mode_sw.empty()) {
    ESP_LOGW(TAG, "Empty mode/sw field");
    return;
  }

  char mode_char = mode_sw[0];
  std::string sw_version = (mode_sw.size() > 1) ? mode_sw.substr(1) : "";

  std::string mode = (mode_char == 'C') ? "Coffee" : "Steam";

  if (mode_sensor_) mode_sensor_->publish_state(mode);
  if (sw_sensor_) sw_sensor_->publish_state(sw_version);

  // -------------------------
  // numeric values
  // -------------------------
  if (steam_sensor_)  steam_sensor_->publish_state(safe_stoi(parts[1]));
  if (target_sensor_) target_sensor_->publish_state(safe_stoi(parts[2]));
  if (hx_sensor_)     hx_sensor_->publish_state(safe_stoi(parts[3]));
  if (boost_sensor_)  boost_sensor_->publish_state(safe_stoi(parts[4]));

  // -------------------------
  // binary sensors
  // -------------------------
  if (parts.size() > 5 && heat_sensor_)
    heat_sensor_->publish_state(safe_bool(parts[5]));

  if (parts.size() > 6 && pump_sensor_)
    pump_sensor_->publish_state(safe_bool(parts[6]));
}

}  // namespace marax_reader
}  // namespace esphome
