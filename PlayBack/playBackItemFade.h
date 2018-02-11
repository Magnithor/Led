#ifndef PLAYBACKITEMFADE_H
#define PLAYBACKITEMFADE_H

#include "playBackItem.h"
#include "color.h"
#include "../apa102.h"
#include <sys/time.h>
#include "../json.h"
#include <string>


class PlayBackItemFade : public PlayBackItem {
private:
  APA102* led;
  Rgb_bright_color *colors;
  bool first;
  double startTimer;
  Color* from;
  Color* to;
  double timeCircle;
  double timeRun;
  bool hasTimeRun;
  timeval now;
public:
  PlayBackItemFade(APA102 *apa102, Color* fromValue, Color* toValue, double timeCircle, double timeRun)  
  {
      this->colors = NULL;
      this->first = true;

      this->timeCircle = timeCircle;

      this->led = apa102;
      
      this->from = fromValue;
      this->to = toValue;
      this->hasTimeRun = timeRun > 0;
      this->timeRun = timeRun;
  }

  ~PlayBackItemFade() {
      if (this->colors != NULL) {
        delete[] this->colors;
      }

      if (this->from != NULL) {
          delete this->from;
      }

      if (this->to != NULL) {
          delete this->to;
      }
  }
  
  virtual int updateLed(bool &finished) override {
      int c = this->led->getCount();      
      if (this->first) {
          gettimeofday(&this->now, NULL);
          this->startTimer = this->now.tv_usec;
          this->startTimer /= 1000000;
          this->startTimer += this->now.tv_sec; 
          this->first = false;
          this->colors = new Rgb_bright_color[c];
      }

      gettimeofday(&this->now, NULL);
      double nowD = this->now.tv_usec;
      nowD /= 1000000;
      nowD += this->now.tv_sec;
     
      double deltaTime = nowD - this->startTimer;

      double radio = (double)deltaTime / (double) this->timeCircle;
      double frac = radio - (long)radio;
      if ( (long)radio% 2 == 1) {
          frac = 1 - frac;
      }

      finished = false;
      for (int i =0; i < c; i++) {
          uint8_t fromRed, fromGreen, fromBlue, fromBrightness,
            toRed, toGreen, toBlue, toBrightness;
          this->from->updateColor(i, fromRed, fromGreen, fromBlue, fromBrightness);
          this->to->updateColor(i, toRed, toGreen, toBlue, toBrightness);
          this->colors[i].red = fromRed + (frac * (toRed - fromRed)); 
          this->colors[i].green = fromGreen + (frac * (toGreen - fromGreen));
          this->colors[i].blue = fromBlue + (frac * (toBlue - fromBlue));
          this->colors[i].brightness = fromBrightness + (frac * (toBrightness - fromBrightness));
      }  
      this->led->write(this->colors);

      if (this->hasTimeRun && this->timeRun <= deltaTime) {    
        finished = true;
      }
      return 0;
 }
};
#endif