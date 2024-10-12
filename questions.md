## Plan of attack
* [x] Add a slammer button
* [x] Get sound output working
* [x] Hook up buzzer via mosfet?
* [x] Proper wiring
* [ ] Proper volume from speaker
* [ ] Missing screw
* [ ] Add rotary detector
* [ ] Rotary connector
* [ ] Get microphone input working
* [ ] prevent default echoing (if needed?)
* [x] Create timer framework
* [x] Add battery
* [ ] Add battery indicator
* [x] Hook up all buttons
* [ ] Figure out interface (buttons, slammer, )
* [ ] Add "Operator" sound
* [ ] Add chatgpt integration? Talk to GPT-4o



## Questions
### Can we use the onboard ADC / DAC instead of I2S

### How does I2S work?


### How can we schedule real time tasks like with xQueueReceive
https://www.freertos.org/Documentation/02-Kernel/04-API-references/11-Software-timers/01-xTimerCreate

xTimerCreate can be used.

### Where do we actually get the bluetooth bits?

### Is DMA used?

### How can we quit the idf.py monitor stuff?

Ctrl ]
or Ctrl x seems like it should work?

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/tools/idf-monitor.html



### Which commands should be used ideally?

We have 6 buttons
- Red
- Black
- 4x white

And two more inputs:
- Rotary dial
- Horn slammer
    if current call: hang up
    if no call: 

And then three indicators:
- Flashing indicators
- Buzzer


### Brainstorm about cool demo

Pick up and get Google assistant right away?

It would be really cool if we could make it say "Operator" in a cool voice.

### Can we have two calls simultaneously?
And if so, can we use the black and red buttons to switch between them?
Also red / black could be assistant vs actual call?




### The readme says the sound is currently echoed back. Is that true?




### What is "operator name"
ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT


