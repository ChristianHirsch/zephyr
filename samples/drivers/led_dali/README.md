# Fan control

Important: There seems to be a problem with LEDs when fan is not connected or operating (not connected nor running). Connect FAN and LEDs for proper operation. Check if minimum PWM for fan needs to be set for LEDs to work when fan is not running.

## Build

```
west build -b adafruit_feather_stm32f405
```

## Flash

```
west flash --runner jlink
```
