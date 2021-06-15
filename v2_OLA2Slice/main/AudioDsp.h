#ifndef __AUDIODSP_H
#define __AUDIODSP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "TransVoix.h"

class AudioDsp
{
public:
  AudioDsp(int SR, int BS);
  bool start();
  void stop();
  void setRelative(float RR, int RS);
private:
  void audioTask();
  static void audioTaskHandler(void* arg);

  int fSampleRate, fBufferSize, fNumInputs, fNumOutputs;
  float fRelativeRate;
  int fRelativeSize, fWindowSize, fOlaSize;
  TaskHandle_t fHandle;
  bool fRunning;

  TransVoix TransVoixL, TransVoixR;
};

#endif
