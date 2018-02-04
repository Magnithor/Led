#ifndef PLAYBACKITEMSOLID_H
#define PLAYBACKITEMSOLID_H

#include "playBackItem.h"
#include "apa102.h"
#include <sys/time.h>

class PlayBackItemSolid : public PlayBackItem {
private:
  APA102* led;
  Rgb_bright_color *colors;
  bool first;
  uint8_t  red, green, blue, brightness;
  double timeRun;
  bool hasTimeRun;
  timeval now;
  double startTimer;

public:
  PlayBackItemSolid(APA102 *apa102, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) {
      this->led = apa102;
      this->red = red;
      this->green = green;
      this->blue = blue;
      this->brightness = brightness;
      this->first = true;
      this->hasTimeRun = true;
  }
  
  PlayBackItemSolid(APA102 *apa102, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness,
        double timeRun) : PlayBackItemSolid(apa102, 
            red, green, blue, brightness
        )  
  {
      this->hasTimeRun = true;
      this->timeRun = timeRun;
  }

  ~PlayBackItemSolid(){
      delete[] this->colors;
  }
  
  virtual int updateLed(bool &finished) override {
      if (first) {
        this->first = false;
        gettimeofday(&this->now, NULL);
        this->startTimer = this->now.tv_usec;
        this->startTimer /= 1000000;
        this->startTimer += this->now.tv_sec; 
        int c = this->led->getCount();
        this->colors = new Rgb_bright_color[c];
        for (int i =0; i <c; i++){
            this->colors[i].red = this->red;
            this->colors[i].green = this->green;
            this->colors[i].blue = this->blue;
            this->colors[i].brightness = this->brightness;
        }  
      }

      finished = false;
      this->led->write(this->colors);
      if (this->hasTimeRun) {
        gettimeofday(&this->now, NULL);
        double nowD = this->now.tv_usec;
        nowD /= 1000000;
        nowD += this->now.tv_sec;     
        double deltaTime = nowD - this->startTimer;
   
        if (this->timeRun <= deltaTime) {    
          finished = true;
        }
      }

      return -1;
 }
};
#endif