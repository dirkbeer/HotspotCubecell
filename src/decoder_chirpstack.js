// Decode uplink function.
//
// Input is an object with the following fields:
// - bytes = Byte array containing the uplink payload, e.g. [255, 230, 255, 0]
// - fPort = Uplink fPort.
// - variables = Object containing the configured device variables.
//
// Output must be an object with the following fields:
// - data = Object representing the decoded payload.
function decodeUplink(input) {
    var bytes = input.bytes;
    var port = input.fPort;

    var temperature = bytes[0]<<24>>16 | bytes[1]; // combination left and right shift allows for decoding negative numbers
    var humidity = bytes[2]<<24>>16 | bytes[3];
    var battery_soc = bytes[4]<<24>>16 | bytes[5];
    var battery_voltage = bytes[6]<<24>>16 | bytes[7];
    var battery_current = bytes[8]<<24>>16 | bytes[9];
    var controller_temperature = bytes[10]<<24>>16 | bytes[11]; 
    var load_voltage = bytes[12]<<24>>16 | bytes[13];
    var load_current = bytes[14]<<24>>16 | bytes[15];
    var load_power = bytes[16]<<24>>16 | bytes[17];
    var solar_voltage = bytes[18]<<24>>16 | bytes[19];
    var solar_current = bytes[20]<<24>>16 | bytes[21];
    var solar_power = bytes[22]<<24>>16 | bytes[23];
    var day_battery_voltage_min = bytes[24]<<24>>16 | bytes[25];
    var day_battery_voltage_max = bytes[26]<<24>>16 | bytes[27];
    var day_battery_current_maxcharge = bytes[28]<<24>>16 | bytes[29];
    var day_battery_current_maxdischarge = bytes[30]<<24>>16 | bytes[31];
    var day_battery_power_maxcharge = bytes[32]<<24>>16 | bytes[33];
    var day_battery_power_maxdischarge = bytes[34]<<24>>16 | bytes[35];
    var day_battery_ah_charge = bytes[36]<<24>>16 | bytes[37];
    var day_battery_ah_discharge = bytes[38]<<24>>16 | bytes[39];
    var day_wh_generation = bytes[40]<<24>>16 | bytes[41];
    var day_wh_consumption = bytes[42]<<24>>16 | bytes[43];
    var total_days = bytes[44]<<24>>16 | bytes[45];
    var total_overdischarges = bytes[46]<<24>>16 | bytes[47];
    var total_fullcharges = bytes[48]<<24>>16 | bytes[49];
    var load_status = bytes[50]>>7;
    var charging_state = bytes[51]; 
  
    var decodedPayload = {
        "temperature": Math.round(temperature / 100),
        "humidity": Math.round(humidity / 100),
        "battery_soc": battery_soc,
        "battery_voltage": battery_voltage / 10,
        "battery_current": battery_current / 100,
        "controller_temperature": Math.round((controller_temperature / 100 - 32 ) * 5 / 9),
        "load_voltage": load_voltage / 10,
        "load_current": load_current / 100,
        "load_power": load_power,
        "solar_voltage": solar_voltage / 10,
        "solar_current": solar_current / 100,
        "solar_power": solar_power,
        "day_battery_voltage_max": day_battery_voltage_max / 10,
        "day_battery_voltage_min": day_battery_voltage_min / 10,
        "day_battery_current_maxcharge": day_battery_current_maxcharge / 100,
        "day_battery_current_maxdischarge": day_battery_current_maxdischarge / 100,
        "day_battery_power_maxcharge": day_battery_power_maxcharge,
        "day_battery_power_maxdischarge": day_battery_power_maxdischarge,
        "day_battery_ah_charge": day_battery_ah_charge,
        "day_battery_ah_discharge": day_battery_ah_discharge,
        "day_wh_generation": day_wh_generation / 10,
        "day_wh_consumption": day_wh_consumption / 10,
        "total_days": total_days,
        "total_overdischarges": total_overdischarges,
        "total_fullcharges": total_fullcharges,
        "load_status": load_status,
        "charging_state": ChargingState(charging_state)
    };
  
    function ChargingState(state) {
        switch(state) {
          case 0:
            return "deactivated";
          case 1:
            return "activated";
          case 2:
            return "mppt";
          case 3:
            return "equalizing";
          case 4:
            return "boost";
          case 5:
            return "floating";
          case 6:
            return "limiting";
          default:
            return "unknown";
        }
    }
  
    return {
        data: decodedPayload
    };
}

// Encode downlink function (if needed).
//
// Input is an object with the following fields:
// - data = Object representing the payload that must be encoded.
// - variables = Object containing the configured device variables.
//
// Output must be an object with the following fields:
// - bytes = Byte array containing the downlink payload.
function encodeDownlink(input) {
    // Implement downlink encoding if required.
    return {
        bytes: []
    };
}
