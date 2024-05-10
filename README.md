# W25Q128FV ESP32

Project to write to and read from Winbond W25Q128FV flash memory with an ESP32 Arduino using its VSPI.

* **In order for this program to work the frequency of the SPI needs to be lowered from 50MHz to 5MHz or even lower (lib/SerialFlash/SerialFlashChip.cpp line 33 SPICONFIG)**
## Pin layout
This is how the ESP32 should be connected to the W25Q128FV chip


| ESP32 | W25Q128FV |
| ------------- | ------------- |
| 3.3V  | VCC |
|  GPIO4 | CS  |
|  GPIO18 | CLK |
|  GPIO19 | MISO  |
| GPIO23  | MOSI  |

