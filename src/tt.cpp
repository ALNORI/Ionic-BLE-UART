/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "BLE.h"
#include "ble/BLE.h"
#include "MCP23S17.h"
#include <cstdlib>
//#include "SoftPdmOut.h"
#include "UARTService.h"
#include "LEDService.h"

#define NEED_CONSOLE_OUTPUT 1 /* Set this if you need debug messages on the console; \
                               * it will have an impact on code-size and power consumption. */

#if NEED_CONSOLE_OUTPUT
#define DEBUG(...)       \
  {                      \
    printf(__VA_ARGS__); \
  }
#else
#define DEBUG(...) /* nothing */
#endif             /* #if NEED_CONSOLE_OUTPUT */

#define led0CMD "led0"
#define CMD_LENGTH 4

#define BINARY(a, b, c, d, e, f, g, h) (a << 7 | b << 6 | c << 5 | d << 4 | e << 3 | f << 2 | g << 1 | h)
//    // chips[i]->write(PORT_A , BINARY(0,1,0,1,0,1,0,1));
//*********************MCP23S17*************************
// Create SPI bus
SPI spi(p12, p13, p15);
void init();
char opcodes[2];
// Chip objects
MCP23S17 *chips[2];
//  Set all 8 Port A bits to output direction

//*******************************************************
void onbuttonPressed3();
void onbuttonPressed4();
void init();
BLEDevice ble; // Create Bluetooth object
/*DigitalOut led1(LED1 );                        // Set the pin attached to LED1 as an output DigitalOut led1(P0_16);
PwmOut led0(led0);                        // Set the pin attached to led0 as an outputPwmOut led0(P0_15);
InterruptIn button17(BUTTON1);
InterruptIn button19(button19);
InterruptIn button20(button20);
InterruptIn button4(BUTTON4);
 */
//Miniibeacon                             // Create Bluetooth object
DigitalOut led1(LED1); // Set the pin attached to LED1 as an output DigitalOut led1(P0_16);
PwmOut led2(LED2);
PwmOut led3(LED3);
DigitalOut led4(LED4);        // Set the pin attached to led0 as an outputPwmOut led0(P0_15);
InterruptIn button1(BUTTON1); //A OBS! vita undreknappen är P0_18
InterruptIn button2(BUTTON2); //B
InterruptIn button3(BUTTON3); //C
InterruptIn button4(BUTTON4); //D

//PwmOut myled1(P0_1);
//PwmOut myled0(P0_2);

UARTService *uart;
uint8_t a = 'A';
uint8_t b = 'B';
uint8_t c = 'C';
uint8_t d = 'D';
uint8_t e = 'E';
uint8_t f = 'F';
uint8_t g = 'G';
uint8_t h = 'H';
uint8_t j = 'J';
uint8_t k = 'K';
uint8_t l = 'L';
uint8_t m = 'M';
uint8_t n = 'N';
uint8_t o = 'O';
uint8_t stop = 'S';

float brightness = 0.0;  // How bright the LED is
float fadeAmount = 0.02; // How many points to fade the LED by

const static char DEVICE_NAME[] = "LED";
static const uint16_t uuid16_list[] = {LEDService::LED_SERVICE_UUID};

LEDService *ledServicePtr;

/* BLE disconnected callback */

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
  DEBUG("Disconnected!\n\r");
  DEBUG("Restarting the advertising process\n\r");
  ble.startAdvertising();
}

/* BLE UART data received callback */
void onDataWritten(const GattWriteCallbackParams *params)
{
  if ((params->handle == ledServicePtr->getValueHandle()) && (params->len == 1))
  {

    //uint16_t bytesRead = params->len;
    uint8_t bytesRead = params->data[0];
    DEBUG("received %u bytes\n\r", bytesRead);
    DEBUG("'\n\r")

    brightness = brightness + bytesRead / 100;
    led3.write(brightness);

    if (brightness == 1)
    {
      led3.write(0);
      wait_ms(5);
      brightness = 0;
    }
  }
  if ((uart != NULL) && (params->handle == uart->getTXCharacteristicHandle()))
  { //If characters received over BLE
    uint16_t bytesRead = params->len;
    DEBUG("received %u bytes\n\r", bytesRead);
    DEBUG("Received string: '");
    DEBUG((const char *)params->data[0]); //Note the size of data expands to the largest string received. Need to use bytesRead to resize.
    DEBUG("'\n\r")
    if (!strncmp(led0CMD, (const char *)params->data, CMD_LENGTH - 1)) // Gämför de tre(CMD_LENGTH-1) första chars i "inkommen text2 och i "led0"
                                                                       // http://www.tutorialspoint.com/c_standard_library/c_function_strncmp.htm
    {
      float value;
      char cmd[CMD_LENGTH];
      sscanf((const char *)params->data, "%s %f", cmd, &value); //"led0 1"delar på texen och sparar i cmd[CMD_LENGTH] och &värdet i value ;// http://www.tutorialspoint.com/c_standard_library/c_function_sscanf.htm
      //led0 = value;
      // led3 =
      // led2 =
      DEBUG("Cmd: %s LED Level = %f\n\r", cmd, value);
    }

    if (params->data[0] == 'R')
    {
      ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), params->data, bytesRead); // Echo received characters back over BLE
      onbuttonPressed4();
    }

    else if (params->data[0] == 'O')
    {
      ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), params->data, bytesRead); // Echo received characters back over BLE
      onbuttonPressed3();
    }
    //(const char *)Bism";

    // const uint8_t* p = reinterpret_cast<const uint8_t*>(name.c_str());
  }
}

/* Periodic Ticker callback */
void periodicCallback(void)
{
  led1 = !led1; // Toggle LED 1
}

void onStoppSong()
{
}
void onbuttonPressed()
{
  wait(0.02);
  ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), &a, sizeof(bool));
}
void onbuttonPressed2()
{
  wait(0.02);
  ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), &b, sizeof(bool));
}

void onbuttonPressed3()
{
  wait(0.02);
  ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), &d, sizeof(bool)); // Echo received characters back over BLE

  led2.period(0.05f); // 4 second period
  led2.pulsewidth(2); // 2 second pulse (on)
  wait(5);
  led2.pulsewidth(0);
}

void onbuttonPressed4()
{
  float brightness = 0.0;  // How bright the LED is
  float fadeAmount = 0.02; // How many points to fade the LED by

  for (int i = 0; i < 1; i + fadeAmount)
  {
    led3.write(brightness);               // Write a PWM analog brightness value on LED
    wait_ms(30);                          // wait for 30ms to see the dimming effect
    brightness = brightness + fadeAmount; // Increase the brightness
    if ((int)brightness == 1.0f)          // Turn brightness to 0 at the ends of the fade (f is for literal float - Delete the warning)
      // brightness = 0;
      return;
  }

  wait(0.02);
  ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), &c, sizeof(bool)); // Echo received characters back over BLE
}

void onBleInitError(BLE &ble, ble_error_t error)
{
  /* Initialization error handling should go here */
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
  BLE &ble = params->ble;
  ble_error_t error = params->error;

  if (error != BLE_ERROR_NONE)
  {
    /* In case of error, forward the error handling to onBleInitError */
    onBleInitError(ble, error);
    return;
  }

  /* Ensure that it is the default instance of BLE */
  if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE)
  {
    return;
  }

  ble.gap().onDisconnection(disconnectionCallback);
  ble.gattServer().onDataWritten(onDataWritten);

  bool initialValueForLEDCharacteristic = false;
  ledServicePtr = new LEDService(ble, initialValueForLEDCharacteristic);

  /* setup advertising */
  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)"Led service", sizeof("Led service"));
  ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
  ble.gap().startAdvertising();
}

int main(void)
{

  init();

  Ticker ticker;                      // Create period timer
  ticker.attach(periodicCallback, 1); // Attach ticker callback function with a period of 1 second

  BLE &ble = BLE::Instance();
  ble.init(bleInitComplete);

  // set the function onButtonPush to be called every time that pin goes from 0 to 1.
  button1.rise(&onbuttonPressed);
  button2.rise(&onbuttonPressed2);
  button3.rise(&onbuttonPressed3);
  button4.rise(&onbuttonPressed4);

  DEBUG("Initialising the nRF51822\n\r");
  ble.init();
  ble.onDisconnection(disconnectionCallback); // Define callback function for BLE disconnection event
  ble.onDataWritten(onDataWritten);           // Define callback function for BLE Data received event

  /* setup advertising */
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED); // Indicate that Legacy Bluetooth in not supported
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                   (const uint8_t *)"BBLE UART-NRF51-DK", sizeof("BLE UART-NRF51-DK") - 1);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                   (const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));

  ble.setAdvertisingInterval(1000); // Set advertising interval to 1 second
  ble.startAdvertising();           // Start advertising

  UARTService uartService(ble); // Create BLE UART service
  uart = &uartService;          // Initalise pointer to point to UART Service

  while (true)
  {
    ble.waitForEvent();
  }
}

void init()
{
  led2 = 0;
}
