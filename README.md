# mqtt serial
sends all mqtt "request" messages over the serial and received serial messages back on mqtt "response" topic

## example

    mosquitto_pub -t '/dimmer/request' -m "{\"all\":100}"


## request
```json
{"all":100}
```

# Documentation

* [Home Smart Mesh - M5Stack Atom](https://www.homesmartmesh.com//docs/microcontrollers/esp32/m5stack-atom/)
