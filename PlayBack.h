#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "apa102.h"

class PlayBackItem {
public:
  virtual void UpdateLed(bool &finished, APA102Base* ledStrip) = 0;
  virtual void Restart() = 0;
};

class PlayBack {
public:

    void Update();
};

#endif
