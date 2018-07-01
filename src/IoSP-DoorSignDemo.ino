/*
 * Project IoSP-DoorSignDemo
 * Description:
 * Author: 3magku
 * Date:2018-06-30
 */

// -----------------------------------------------
// Controlling an epaper display over the Internet
// -----------------------------------------------

/*
   IMPORTS & DEFINITIONS
 */

// Waveshare e-paper module
#include <Particle.h>
#include <Arduino.h>
#include <epd1in54.h>
#include <epdpaint.h>

// Image data for "We are open", "Sorry, we are closed" and "Be right back" ...
#include "imagedata_open.h"
#include "imagedata_closed.h"

// E-ink colours
#define COLORED 0
#define UNCOLORED 1

/*
   VARIABLES
 */

// controlling debugging ...
boolean debugging = true;

// E-paper image buffer and handle
unsigned char image[2048];
Paint paint(image, 0, 0);
Epd epd;

/*
   SETUP
 */
void setup()
{
  // Initialize e-paper
  debugMessage("initializing epaper ...");
  int epd_rc = epd.Init(lut_full_update);
  if (epd_rc != 0)
  {
    debugMessage("epaper intialization failed!");
    return;
  }

  debugMessage("registering functions ...");
  // We are going to declare a Particle.function so that we can turn the debugging on and off from the cloud.
  Particle.function("debug", debugToggle);
  // This is saying that when we ask the cloud for the function "debug", it will employ the function debugToggle() from this app.

  // We are also going to declare a Particle.function so that we can switch the sign from the cloud.
  Particle.function("sign", signToggle);
  // This is saying that when we ask the cloud for the function "sign", it will employ the function signToggle() from this app.

  debugMessage("ready.");
}

/*
   LOOP
 */
// Since we're waiting for input through the cloud this time,
// we don't actually need to put anything in the loop
void loop()
{
  // Nothing to do here
}

/*
  FUNCTIONS
*/

// Logging debug messages to cloud as DEBUG event ...

void debugMessage(String message)
{
  if (!debugging)
    return;
  Spark.publish("DEBUG", message);
}

void debugInt(String name, int value)
{
  if (!debugging)
    return;
  char msg[50];
  sprintf(msg, "%s = %d", name.c_str(), value);
  debugMessage(msg);
}

int debugToggle(String command)
{
  if (command == "on" or command == "true ")
  {
    debugging = true;
    return 1;
  }
  else if (command == "off" or command == "false ")
  {
    debugging = false;
    return 1;
  }
  else
  {
    return -1;
  }
}

// We're going to have a super cool function now that gets called when a matching API request is sent
// This is the signToggle function we registered to the "sign" Particle.function earlier.
int signToggle(String command)
{
  /* Particle.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands
    on how the function should be used. In this case, telling the function "closed"
    will show the "sorry, we ar closed!" image and telling it "open" will show
    the "we are open!" image.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 0 for "open", 1 for closed, and -1 if we received a totally
    bogus command that didn't do anything to change the display.
    */

  debugMessage("command received ...");

  if (command == "open")
  {
    debugMessage("command: 'open'");
    displayImage(true, 0);
    return 0;
  }
  else if (command == "closed")
  {
    debugMessage("command: 'close'");
    displayImage(true, 1);
    return 1;
  }
  else
  {
    debugMessage("unknown command.");
    return -1;
  }
}

// Handler called function for displaying images:
void displayImage(boolean clean, int number)
{

  debugMessage("displaying image ...");
  debugInt("clean", clean);
  debugInt("number", number);

  if (clean)
  {
    ePaperClear();
  }

  const unsigned char *image_data;
  switch (number)
  {
  case 0:
    image_data = IMAGE_DATA_OPEN;
    break;
  case 1:
    image_data = IMAGE_DATA_CLOSED;
    break;
  }

  ePaperShowImage(image_data);
}

// E-paper functions ...

// Clear e-ink display:
void ePaperClear()
{
  /**
     From epd1in54-demo.ino - Waveshare 1.54inch e-paper display demo:
     There are 2 memory areas embedded in the e-paper display
     and once the display is refreshed, the memory area will be auto-toggled,
     i.e. the next action of SetFrameMemory will set the other memory area
     therefore you have to clear the frame memory twice.
  */

  debugMessage("clearing epaper ...");

  epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
  epd.DisplayFrame();

  debugMessage("epaper cleared.");
}

// Display image on e-ink display:
void ePaperShowImage(const unsigned char image_data[])
{
  /**
      From epd1in54-demo.ino - Waveshare 1.54inch e-paper display demo:
      There are 2 memory areas embedded in the e-paper display
      and once the display is refreshed, the memory area will be auto-toggled,
      i.e. the next action of SetFrameMemory will set the other memory area
      therefore you have to clear the frame memory twice.
  */

  debugMessage("showing image ...");

  epd.SetFrameMemory(image_data);
  epd.DisplayFrame();
  epd.SetFrameMemory(image_data);
  epd.DisplayFrame();

  debugMessage("image showing.");
}
