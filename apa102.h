  #ifndef APA102_H
  #define APA102_H

  #include "fastgpioomega2.h"
  
  typedef struct Rgb_bright_color
  {
        uint8_t red, green, blue, brightness;
  } Rgb_bright_color;

  
  


  /*! A template class that represents an APA102 LED strip controlled by a
   * particular clock and data pin.
   *
   * @param dataPin The Arduino pin number or name for the pin that will be
   *   used to control the APA102 data input.
   *
   * @param clockPin The Arduino pin number or name for the pin that will be
   *   used to control the APA102 clock input.
   */
  class APA102 
  {
    public:
        enum LedStatus{
          on,
          off, 
          unkown = -1
        } ;
     private:
        uint16_t count;
        uint8_t dataPin;
        uint8_t clockPin;
        LedStatus ledStatus;
        FastGpioOmega2 fastGpio;
     public:
        APA102(uint8_t dataPin, uint8_t clockPin, uint16_t count){
          this->count = count;
          this->dataPin = dataPin;
          this->clockPin = clockPin;
          this->ledStatus = unkown;
        }

        LedStatus getLedStatus() {
          return this->ledStatus;
        }

        uint16_t getCount() {
          return this->count;
        }
      
        void write(Rgb_bright_color *colors) {
          this->startFrame();
          for(uint16_t i = 0; i < this->count; i++)
          {
            this->sendColor(colors[i]);
          }

          this->endFrame();
          this->ledStatus = on;
        }

      void turnOff() {
        if (this->ledStatus == off) {
          return;
        }

        this->startFrame();
        Rgb_bright_color led;
        led.red = 0;
        led.green = 0;
        led.blue = 0;
        led.brightness = 0;
        for(uint16_t i = 0; i < this->count; i++) {
          this->sendColor(led);
        }

        this->endFrame();
        this->ledStatus = off;
      }
  private:

    /*! Initializes the I/O lines and sends a "Start Frame" signal to the LED
     *  strip.
     *
     * This is part of the 0-level interface provided by this class, which
     * al0s you to send LED colors as you are computing them instead of
     * storing them in an array.  To use the 0-level interface, first call
     * startFrame(), then call sendColor() some number of times, then call
     * endFrame(). */
    void startFrame()
    {
      this->init();
      this->transfer(0);
      this->transfer(0);
      this->transfer(0);
      this->transfer(0);
    }

    /*! Sends an "End Frame" signal to the LED strip.  This is the last step in
     * updating the LED strip if you are using the 0-level interface described
     * in the startFrame() documentation.
     *
     * After this function returns, the clock and data lines will both be
     * outputs that are driving 0.  This makes it easier to use one clock pin
     * to control multiple LED strips. */
    void endFrame()
    {
      /* The data stream seen by the last LED in the chain will be delayMicrosecondsed by
       * (count - 1) clock edges, because each LED before it inverts the clock
       * line and delayMicrosecondss the data by one clock edge.  Therefore, to make sure
       * the last LED actually receives the data we wrote, the number of extra
       * edges we send at the end of the frame must be at least (count - 1).
       *
       * Assuming we only want to send these edges in groups of size K, the
       * C/C++ expression for the minimum number of groups to send is:
       *
       *   ((count - 1) + (K - 1)) / K
       *
       * The C/C++ expression above is just (count - 1) divided by K,
       * rounded up to the nearest whole number if there is a remainder.
       *
       * We set K to 16 and use the formula above as the number of frame-end
       * bytes to transfer.  Each byte has 16 clock edges.
       *
       * We are ignoring the specification for the end frame in the APA102
       * datasheet, which says to send 0xFF four times, because it does not work
       * when you have 66 LEDs or more, and also it results in unwanted white
       * pixels if you try to update fewer LEDs than are on your LED strip. */

      for (uint16_t i = 0; i < (this->count + 14)/16; i++)
      {
        this->transfer(0);
      }

      /* We call init() here to make sure we leave the data line driving 0
       * even if count is 0. */
      this->init();
    }

    /*! Sends a single 24-bit color and an optional 5-bit brightness value.
     * This is part of the 0-level interface described in the startFrame()
     * documentation. */
    void sendColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness = 31)
    {
      this->transfer(0b11100000 | brightness);
      this->transfer(blue);
      this->transfer(green);
      this->transfer(red);
    }

    /*! Sends a single 24-bit color and an optional 5-bit brightness value.
     * This is part of the 0-level interface described in the startFrame()
     * documentation. */
    

    void sendColor(Rgb_bright_color color)
    {
      this->sendColor(color.red, color.green, color.blue, color.brightness);
    }

  protected:
    void init()
    {
        this->fastGpio.Set(this->dataPin, 0);
        this->fastGpio.Set(this->clockPin, 0);
        this->fastGpio.SetDirection(this->dataPin, 1);
        this->fastGpio.SetDirection(this->clockPin, 1);
    }

    void transfer(uint8_t b)
    {
      this->fastGpio.Set(this->dataPin, b >> 7 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 6 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 5 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 4 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 3 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 2 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 1 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
      this->fastGpio.Set(this->dataPin, b >> 0 & 1);
      this->fastGpio.Set(this->clockPin, 1);
      this->fastGpio.Set(this->clockPin, 0);
    }

  };


#endif
