#ifndef PLAYBACKITEM_H
#define PLAYBACKITEM_H

#include "apa102.h"

class PlayBackItem {
public:
  virtual void updateLed(bool &finished) = 0;
   virtual ~PlayBackItem() {}
};
#endif