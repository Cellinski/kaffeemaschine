import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, text_sensor, binary_sensor
from esphome.const import CONF_ID, CONF_UART_ID

marax_ns = cg.esphome_ns.namespace("marax_reader")
MaraXReaderComponent = marax_ns.class_("MaraXReaderComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MaraXReaderComponent),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),

    cv.Required("mode"): cv.use_id(text_sensor.TextSensor),
    cv.Required("sw_version"): cv.use_id(text_sensor.TextSensor),

    cv.Required("steam_temp"): cv.use_id(sensor.Sensor),
    cv.Required("target_steam_temp"): cv.use_id(sensor.Sensor),
    cv.Required("hx_temp"): cv.use_id(sensor.Sensor),
    cv.Required("boost_counter"): cv.use_id(sensor.Sensor),

    cv.Required("heating_element"): cv.use_id(binary_sensor.BinarySensor),
    cv.Required("pump"): cv.use_id(binary_sensor.BinarySensor),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    uart_component = await cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart(uart_component))

    cg.add(var.set_mode_sensor(await cg.get_variable(config["mode"])))
    cg.add(var.set_sw_sensor(await cg.get_variable(config["sw_version"])))

    cg.add(var.set_steam_sensor(await cg.get_variable(config["steam_temp"])))
    cg.add(var.set_target_sensor(await cg.get_variable(config["target_steam_temp"])))
    cg.add(var.set_hx_sensor(await cg.get_variable(config["hx_temp"])))
    cg.add(var.set_boost_sensor(await cg.get_variable(config["boost_counter"])))

    cg.add(var.set_heat_sensor(await cg.get_variable(config["heating_element"])))
    cg.add(var.set_pump_sensor(await cg.get_variable(config["pump"])))
