function Decoder(bytes, port) {
    var temperature = bytes[0]<<24>>16 | bytes[1]; // combination left and right shift allows for decoding negative numbers
    var humidity = bytes[2]<<24>>16 | bytes[3];
    var battery_voltage = bytes[4]<<24>>16 | bytes[5];
    var battery_soc = bytes[6]<<24>>16 | bytes[7];
    var load_voltage = bytes[8]<<24>>16 | bytes[9];
    var load_current = bytes[10]<<24>>16 | bytes[11];
  
    var decodedPayload = {
      "temperature": temperature / 100,
      "humidity": humidity / 100,
      "battery-voltage": battery_voltage / 10,
      "battery-soc": battery_soc,
      "load-voltage": load_voltage / 10,
      "load-current": load_current / 100
    };
  
    return Serialize(decodedPayload)
  }
  
  // Generated: do not touch unless your Google Form fields have changed
  var field_mapping = {
    "temperature": "entry.1315637750",
    "humidity": "entry.1558970503",
    "battery-voltage": "entry.1192891319",
    "battery-soc": "entry.659356187",
    "load-voltage": "entry.2007004356",
    "load-current": "entry.2055582782"
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
  // DO NOT REMOVE: Google Form Function
    