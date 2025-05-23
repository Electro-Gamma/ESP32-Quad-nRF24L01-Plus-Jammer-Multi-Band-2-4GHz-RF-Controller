#include "RF24.h"
#include <SPI.h>
#include "esp_bt.h"
#include "esp_wifi.h"

// === Channel Definitions ===
byte bluetooth_even_channels[] = {
    2,  4,  6,  8,  10, 12, 14, 16, 18, 20,
    22, 24, 26, 28, 30, 32, 34, 36, 38, 40,
    42, 44, 46, 48, 50, 52, 54, 56, 58, 60,
    62, 64, 66, 68, 70, 72, 74, 76, 78, 80
};
byte bluetooth_odd_channels[] = {
    1,  3,  5,  7,  9,  11, 13, 15, 17, 19,
    21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
    41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
    61, 63, 65, 67, 69, 71, 73, 75, 77, 79
};
byte wifi_channels[] = {
    6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
    22, 24, 26, 28,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
    46, 48, 50, 52,
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68
};
byte ble_channels[] = {1, 2, 3, 25, 26, 27, 79, 80, 81};
byte usb_channels[] = {40, 50, 60};
byte video_channels[] = {70, 75, 80};
byte rc_channels[] = {1, 3, 5, 7};
byte full_channels[] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
    85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
};

const int num_bluetooth_even = sizeof(bluetooth_even_channels) / sizeof(bluetooth_even_channels[0]);
const int num_bluetooth_odd = sizeof(bluetooth_odd_channels) / sizeof(bluetooth_odd_channels[0]);
const int num_wifi = sizeof(wifi_channels) / sizeof(wifi_channels[0]);
const int num_ble = sizeof(ble_channels) / sizeof(ble_channels[0]);
const int num_usb = sizeof(usb_channels) / sizeof(usb_channels[0]);
const int num_video = sizeof(video_channels) / sizeof(video_channels[0]);
const int num_rc = sizeof(rc_channels) / sizeof(rc_channels[0]);
const int num_full = sizeof(full_channels) / sizeof(full_channels[0]);

// === Radio Instances ===
// HSPI Bus (Modules 1 & 2)
RF24 radio_hspi1(26, 15); // CE=26, CSN=15 (Module 1)
RF24 radio_hspi2(27, 25); // CE=27, CSN=25 (Module 2)
// VSPI Bus (Modules 3 & 4)
RF24 radio_vspi1(4, 2);   // CE=4,  CSN=2  (Module 3)
RF24 radio_vspi2(16, 17); // CE=16, CSN=17 (Module 4)

// === State Variables ===
byte ch[4] = {45, 45, 45, 45}; // Current channel for each module
char mode = 'r';               // Default mode: random Bluetooth hopping
bool jammersActive = false;    // Flag for running state

void setup() {
  Serial.begin(115200);
  printWelcomeMessage();
}

void loop() {
  handleSerialInput();
  if (jammersActive) runCurrentMode();
}

void printWelcomeMessage() {
  Serial.println("\nRF Jammer Controller Ready for 4 nRF24 Modules");
  Serial.println("System is in standby mode");
  printModeHelp();
  Serial.println("Send 'start' to initialize jammers");
  Serial.println("Send 'stop' to deactivate jammers");
}

void handleSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.equalsIgnoreCase("start")) {
      if (!jammersActive) {
        initializeJammers();
        jammersActive = true;
        Serial.println("Jammers activated and running");
      } else {
        Serial.println("Jammers are already active");
      }
    }
    else if (input.equalsIgnoreCase("stop")) {
      if (jammersActive) {
        deactivateJammers();
        jammersActive = false;
        Serial.println("Jammers deactivated");
      } else {
        Serial.println("Jammers are already inactive");
      }
    }
    else if (input.length() == 1 && isValidMode(input.charAt(0))) {
      mode = input.charAt(0);
      Serial.print("Mode changed to: ");
      printModeName(mode);
      if (jammersActive) {
        Serial.println("(Change will take effect immediately)");
      }
    }
    else if (input.equalsIgnoreCase("help") || input.equals("?")) {
      printModeHelp();
    }
    else {
      Serial.println("Unknown command. Send 'help' for options.");
    }
  }
}

void initializeJammers() {
  // Disable ESP32 Bluetooth and Wi-Fi
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();

  // Initialize HSPI (Modules 1 & 2)
  SPIClass *hspi = new SPIClass(HSPI);
  hspi->begin(14, 12, 13, -1); // SCK=14, MISO=12, MOSI=13
  if (radio_hspi1.begin(hspi)) {
    Serial.println("HSPI Module 1 Jammer Initialized!");
    configureRadio(radio_hspi1, ch[0]);
  } else {
    Serial.println("HSPI Module 1 initialization failed!");
  }
  if (radio_hspi2.begin(hspi)) {
    Serial.println("HSPI Module 2 Jammer Initialized!");
    configureRadio(radio_hspi2, ch[1]);
  } else {
    Serial.println("HSPI Module 2 initialization failed!");
  }

  // Initialize VSPI (Modules 3 & 4)
  SPIClass *vspi = new SPIClass(VSPI);
  vspi->begin(18, 19, 23, -1); // SCK=18, MISO=19, MOSI=23
  if (radio_vspi1.begin(vspi)) {
    Serial.println("VSPI Module 3 Jammer Initialized!");
    configureRadio(radio_vspi1, ch[2]);
  } else {
    Serial.println("VSPI Module 3 initialization failed!");
  }
  if (radio_vspi2.begin(vspi)) {
    Serial.println("VSPI Module 4 Jammer Initialized!");
    configureRadio(radio_vspi2, ch[3]);
  } else {
    Serial.println("VSPI Module 4 initialization failed!");
  }
}

void deactivateJammers() {
  radio_hspi1.stopConstCarrier();
  radio_hspi2.stopConstCarrier();
  radio_vspi1.stopConstCarrier();
  radio_vspi2.stopConstCarrier();
  Serial.println("All jammers stopped transmission");
}

void runCurrentMode() {
  byte newCh[4];
  switch (mode) {
    case 'r': // Random Bluetooth hopping (odd/even)
      newCh[0] = bluetooth_odd_channels[random(num_bluetooth_odd)];
      newCh[1] = bluetooth_even_channels[random(num_bluetooth_even)];
      newCh[2] = bluetooth_odd_channels[random(num_bluetooth_odd)];
      newCh[3] = bluetooth_even_channels[random(num_bluetooth_even)];
      break;
    case 'w': // WiFi channels
      for (int i = 0; i < 4; i++) newCh[i] = wifi_channels[random(num_wifi)];
      break;
    case 'b': // BLE advertising channels
      for (int i = 0; i < 4; i++) newCh[i] = ble_channels[random(num_ble)];
      break;
    case 'u': // USB wireless channels
      for (int i = 0; i < 4; i++) newCh[i] = usb_channels[random(num_usb)];
      break;
    case 'v': // Video streaming channels
      for (int i = 0; i < 4; i++) newCh[i] = video_channels[random(num_video)];
      break;
    case 'c': // RC toys/drones channels
      for (int i = 0; i < 4; i++) newCh[i] = rc_channels[random(num_rc)];
      break;
    case 'f': // Full spectrum
      for (int i = 0; i < 4; i++) newCh[i] = full_channels[random(num_full)];
      break;
    default:
      for (int i = 0; i < 4; i++) newCh[i] = ch[i]; // Maintain current channels
      break;
  }
  updateChannels(newCh);
  delayMicroseconds(random(60));
}

void updateChannels(byte newCh[4]) {
  if (newCh[0] != ch[0]) { radio_hspi1.setChannel(newCh[0]); ch[0] = newCh[0]; }
  if (newCh[1] != ch[1]) { radio_hspi2.setChannel(newCh[1]); ch[1] = newCh[1]; }
  if (newCh[2] != ch[2]) { radio_vspi1.setChannel(newCh[2]); ch[2] = newCh[2]; }
  if (newCh[3] != ch[3]) { radio_vspi2.setChannel(newCh[3]); ch[3] = newCh[3]; }
}

bool isValidMode(char m) {
  return (m == 'r' || m == 'w' || m == 'b' || m == 'u' || m == 'v' || m == 'c' || m == 'f');
}

void printModeHelp() {
  Serial.println("\nAvailable commands:");
  Serial.println("start  - Initialize and activate jammers");
  Serial.println("stop   - Deactivate jammers");
  Serial.println("help/? - Show this help");
  Serial.println("\nAvailable modes (send single character):");
  Serial.println("r - Random Bluetooth hopping (odd/even)");
  Serial.println("w - WiFi channels");
  Serial.println("b - BLE advertising channels");
  Serial.println("u - USB wireless channels");
  Serial.println("v - Video streaming channels");
  Serial.println("c - RC toys/drones channels");
  Serial.println("f - Full spectrum (all channels)");
}

void printModeName(char m) {
  switch(m) {
    case 'r': Serial.println("Random Bluetooth hopping (odd/even)"); break;
    case 'w': Serial.println("WiFi channels"); break;
    case 'b': Serial.println("BLE advertising channels"); break;
    case 'u': Serial.println("USB wireless channels"); break;
    case 'v': Serial.println("Video streaming channels"); break;
    case 'c': Serial.println("RC toys/drones channels"); break;
    case 'f': Serial.println("Full spectrum (all channels)"); break;
    default: Serial.println("Unknown mode"); break;
  }
}

void configureRadio(RF24& radio, byte channel) {
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.startConstCarrier(RF24_PA_MAX, channel);
}
