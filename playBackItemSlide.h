#ifndef PLAYBACKITEMSLIDE_H
#define PLAYBACKITEMSLIDE_H

#include "playBackItem.h"
#include "apa102.h"
#include <sys/time.h>
#include "json.h"
#include <string>

class PlayBackItemSlide : public PlayBackItem {
private:
  APA102* led;
  Rgb_bright_color *colors;
  bool first;
  double startTimer;
  uint8_t redFrom, greenFrom, blueFrom, brightnessFrom;
  int redDelta, greenDelta, blueDelta, brightnessDelta;
  double timeCircle;
  double timeRun;
  bool hasTimeRun;
  timeval now;
public:
  PlayBackItemSlide(APA102 *apa102, json::Object input) {
      this->colors = NULL;
      this->led = apa102;      
  }

  ~PlayBackItemSlide() {
      if (this->colors == NULL) {
        delete[] this->colors;
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
          this->colors[i].red = this->redFrom + (frac * this->redDelta); 
          this->colors[i].green = this->greenFrom + (frac * this->greenDelta);
          this->colors[i].blue = this->blueFrom + (frac * this->blueDelta);
          this->colors[i].brightness = this->brightnessFrom + (frac * this->brightnessDelta);
      }  
      this->led->write(this->colors);

      if (this->hasTimeRun && this->timeRun <= deltaTime) {    
        finished = true;
      }
      return 0;
 }
};
#endif