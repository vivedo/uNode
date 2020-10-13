# ESP8266 based Art-Net/sACN to DMX+RDM converter

This projects aims to create a simple, compact and cheap 2 port DMX+RDM I/O converter for Art-net and sACN.

It should be able run on ESP01 modules, limited to 1 DMX/RDM port (maybe) + 1 DMX only, or fully on ESP12 or similar (ESP07 is recommended for the external antenna)

Built on top of [ESP8266 RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.3)  _(release v3.3)_

## Roadmap

- [ ] WiFi management (AP/Station mode auto switch)
- [x] Web config panel
  - [ ] Storing configurations
  - [x] Captive portal
- [ ] Handling Art-Net
  - [x] Processing ArtDmx packets
  - [x] Replying to ArtPoll packets
  - [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Fabricating and sending back ArtDmx packets (DMX Input)
  - [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Processing ArtNzs packets (RDM)
  - [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Fabricating and sending back ArtNzs packets (RDM)
  - [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Handling ArtCmd packets _(?)_
- [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Handling sACN
  - [ ] Figuring out how sACN works
  - [ ] Implementing E1.33 for RDM _(?)_
- [ ] DMX/RDM
  - [x] Sending DMX
  - [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Receiving DMX
  - [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Sending/receiving RDM
- [ ] ![Low Priority](https://img.shields.io/badge/priority-low-red) Reverse signal/backup mode

## Credits

- Ivan Belokobylskiy [debvis](https://github.com/devbis) for [esp8266-rtos-softuart](https://github.com/devbis/esp8266-rtos-softuart) _(and all its previous contributors)_
