# HotspotCubecell

Monitor and control your offgrid Helium hotspot over LoRaWAN.

Depends only on battery or solar power and LoRaWAN coverage for its operation, so keeps running even if hotspot is having problems.

* Remotely monitor parameters like temperature and battery state.
* Remotely control your charge controller to power cycle the hotspot and cell modem.

Hardware used:
* Cubecell HTCC-AB01 dev board and LoRaWAN antenna
* Cubecell HDC-1080 temperature and humidity sensor
* MAX3232 Logic-to-RS232 converter board (for connection to Renogy charge controller)
* 12V to 3.3V step down converter (to power the Cubecell directly off the battery)
 

Follow these instructions to patch ADR functionality to ensure the device does not reduce data rate, so that large packets can be sent on a prolonged basis:
`https://docs.helium.com/use-the-network/devices/development/heltec/cubecell-dev-board/arduino/#patching-adr-functionality`
