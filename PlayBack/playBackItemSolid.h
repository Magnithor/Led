#ifndef PLAYBACKITEMSOLID_H
#define PLAYBACKITEMSOLID_H

#include "playBackItem.h"
#include "color.h"
#include "../apa102.h"
#include <sys/time.h>

class PlayBackItemSolid : public PlayBackItem {
private:
  APA102* led;
  Rgb_bright_color *colors;
  Color* color;
  bool first;
  double timeRun;
  bool hasTimeRun;
  timeval now;
  double startTimer;

public:
  PlayBackItemSolid(APA102 *apa102, Color* color) {
      this->color = color;
      this->led = apa102;
      this->first = true;
      this->hasTimeRun = true;
  }
  
  PlayBackItemSolid(APA102 *apa102, Color* color, double timeRun) : PlayBackItemSolid(apa102, color)  
  {
      this->hasTimeRun = true;
      this->timeRun = timeRun;
  }

  ~PlayBackItemSolid() {
      if (this->colors != NULL) {
        delete[] this->colors;
      }
      if (this->color != NULL) {
        delete this->color;
      }
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
        for (int i = 0; i <c; i++) {
          this->color->updateColor(i, this->colors[i].red, this->colors[i].green, this->colors[i].blue, this->colors[i].brightness);
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