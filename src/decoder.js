function Decoder(bytes, port) {
    var temperature = bytes[0] << 8 | bytes[1];
    var humidity = (bytes[2] << 8) | bytes[3];
    var battery_voltage = bytes[4] << 8 | bytes[5];
    var load_current = (bytes[6] << 8) | bytes[7];
  
    var decodedPayload = {
      "temperature": temperature / 100,
      "humidity": humidity / 100,
      "battery-voltage": battery_voltage / 10,
      "load-current": load_current / 100
    };
    // END TODO
  
    return Serialize(decodedPayload)
  }
  
  // Generated: do not touch unless your Google Form fields have changed
  var field_mapping = {
    "temperature": "entry.1315637750",
    "humidity": "entry.1558970503",
    "battery-voltage": "entry.1192891319",
    "load-current": "entry.2007004356"
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
    