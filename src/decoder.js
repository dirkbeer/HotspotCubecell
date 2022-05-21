function Decoder(bytes, port) {
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
  var day_battery_voltage_max = bytes[24]<<24>>16 | bytes[25];
  var day_battery_voltage_min = bytes[26]<<24>>16 | bytes[27];
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
      "battery-soc": battery_soc,
      "battery-voltage": battery_voltage / 10,
      "battery-current": battery_current / 100,
      "controller-temperature": Math.round((controller_temperature / 100 - 32 ) * 5 / 9),
      "load-voltage": load_voltage / 10,
      "load-current": load_current / 100,
      "load-power": load_power,
      "solar-voltage": solar_voltage / 10,
      "solar-current": solar_current / 100,
      "solar-power": solar_power,
      "day-battery-voltage-max": day_battery_voltage_max / 10,
      "day-battery-voltage-min": day_battery_voltage_min / 10,
      "day-battery-current-maxcharge": day_battery_current_maxcharge / 100,
      "day-battery-current-maxdischarge": day_battery_current_maxdischarge / 100,
      "day-battery-power-maxcharge": day_battery_power_maxcharge,
      "day-battery-power-maxdischarge": day_battery_power_maxdischarge,
      "day-battery-ah-charge": day_battery_ah_charge,
      "day-battery-ah-discharge": day_battery_ah_discharge,
      "day-wh-generation": day_wh_generation / 10,
      "day-wh-consumption": day_wh_consumption / 10,
      "total-days": total_days,
      "total-overdischarges": total_overdischarges,
      "total-fullcharges": total_fullcharges,
      "load-status": load_status,
      "charging-state": ChargingState(charging_state)
  };

  return Serialize(decodedPayload)
}

// Generated: do not touch unless your Google Form fields have changed
var field_mapping = {
  "temperature": "entry.1906729410",
  "humidity": "entry.785473889",
  "battery-soc": "entry.1287819448",
  "battery-voltage": "entry.695869569",
  "battery-current": "entry.1156521277",
  "controller-temperature": "entry.2089678221",
  "load-voltage": "entry.678763833",
  "load-current": "entry.185045316",
  "load-power": "entry.1437865895",
  "solar-voltage": "entry.49016021",
  "solar-current": "entry.342287739",
  "solar-power": "entry.1074328515",
  "day-battery-voltage-min": "entry.877779742",
  "day-battery-voltage-max": "entry.944285624",
  "day-battery-current-maxcharge": "entry.517490028",
  "day-battery-current-maxdischarge": "entry.10669463",
  "day-battery-power-maxcharge": "entry.684692787",
  "day-battery-power-maxdischarge": "entry.387125012",
  "day-battery-ah-charge": "entry.229051270",
  "day-battery-ah-discharge": "entry.1325733570",
  "day-wh-generation": "entry.1277755592",
  "day-wh-consumption": "entry.1600130627",
  "total-days": "entry.1702998530",
  "total-overdischarges": "entry.1796031003",
  "total-fullcharges": "entry.892290670",
  "load-status": "entry.1752499855",
  "charging-state": "entry.373582938"
};
// End Generated

function Serialize(payload) {
  var str = [];
  for (var key in payload) {
    if (payload.hasOwnProperty(key)) {
      var name = encodeURIComponent(field_mapping[key]);
      var value = encodeURIComponent(payload[key]);
      str.push(name + "=" + value);
    }
  }
  return str.join("&");
}

function ChargingState(state) {
  switch(state) {
    case 0:
      return "deactivated";
      break;
    case 1:
      return "activated";
      break;
    case 2:
      return "mppt";
      break;
    case 3:
      return "equalizing";
      break;
    case 4:
      return "boost";
      break;
    case 5:
      return "floating";
      break;
    case 6:
      return "limiting";
      break;
  }
}
// DO NOT REMOVE: Google Form Function
  