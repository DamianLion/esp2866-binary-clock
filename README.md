# esp2866-binary-clock

Project for a 5 pin clock. Should only display the current day

## capabilties

- provide a ad hoc network to setup wifi
- status LED provide connection status
- connects to a NTP server
- gets current day
- displays day in binary format

## setup

monitor serial

```bash
pio device monitor -b 115200 -p /dev/cu.usbserial-14310
```

clear the esp

```bash
esptool.py --chip esp8266 --port /dev/cu.usbserial-14310 erase_flash
```

upload

```bash
pio run --target upload
```
