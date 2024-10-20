## Plan of attack
* [x] Add a slammer button
* [x] Get sound output working
* [x] Hook up buzzer via mosfet?
* [x] Proper wiring
* [x] Proper volume from speaker
* [x] Missing screw
* [x] Add rotary detector
* [x] Rotary connector
* [x] Get microphone input working
* [x] Why is there so much distortion? oscilloscope time -> mic is broken
* [x] Hook up all buttons
* [x] who makes the bluetooth connection? -> chatgpt says that the HF should initiate the connection when powering up
* [x] Create timer framework
* [x] Add battery
* [ ] prevent default echoing (if needed?)
* [ ] Add battery indicator
* [ ] Figure out interface (buttons, slammer, )
* [ ] Add "Operator" sound
* [ ] Add chatgpt integration? Talk to GPT-4o
* [ ] debounce horn lift
* [ ] fix the 


## Questions
### Can we use the onboard ADC / DAC instead of I2S

### How does I2S work?
The device emits a clock and a word-select channel (for left/right). Then an input and output pin are used to send/receive the data.

### How can we schedule real time tasks like with xQueueReceive
https://www.freertos.org/Documentation/02-Kernel/04-API-references/11-Software-timers/01-xTimerCreate

xTimerCreate can be used.

### Where do we actually get the bluetooth bits?
Automatically set by the proprietary bluetooth stack, when configured via PCM.

Alternatively we can use HCI and get/set it ourselves.

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
Yes, but only when the same GPIO for PCM input and output is used




### What is "operator name"
ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT probably the mobile network operator name



## Notes

### GPIO25
Pin 25 is hooked up to a low-power configuration or something, so I had quite some trouble figuring out whether there was a short in the circuit or not.

### Broken microphones?
Debugging th


### PCM I2S parameters tweaking
This is currently not possible

### Workaround for CVSD
The esp stack needed to be modified to disable high quality audio in the bluetooth negotiation. TODO add link

