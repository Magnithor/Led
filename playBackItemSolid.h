#ifndef PLAYBACKITEMSOLID_H
#define PLAYBACKITEMSOLID_H

#include "playBackItem.h"
#include "apa102.h"

class PlayBackItemSolid : public PlayBackItem {
private:
  APA102* led;
  Rgb_bright_color *colors;
public:
  PlayBackItemSolid(APA102 *apa102, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)  {
      this->led = apa102;
      this->colors = new Rgb_bright_color[apa102->GetCount()];  
  }
  ~PlayBackItemSolid(){
      delete[] this->colors;
  }
  virtual void UpdateLed(bool &finished) override {
      finished = false;
      this->led->write(this->colors);
 }
};
#endif