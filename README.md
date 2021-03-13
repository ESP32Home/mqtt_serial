# mqtt serial
sends all mqtt "request" messages over the serial and received serial messages back on mqtt "response" topic

## example

    mosquitto_pub -t '/dimmer/request' -m "{\"all\":100}"


## request
```json
{"all":100}
```
