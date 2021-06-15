#include <cmath>

#include "TransVoix.h"



TransVoix::TransVoix(int SR, int BS):
samplingRate(SR),
relativeRate(0.0),
bufferSize(BS),
treatSize(0),
index(0)
{
  buffer = new float[BS];
  for(int i = 0; i < BS; i++){
    buffer[i] = 0.0;
  }
}

TransVoix::~TransVoix()
{
  delete[] buffer;
}

void TransVoix::setting(float r, int size){
  relativeRate = r;
  treatSize = size;
}

// This sample is not the best choice for upsampling
// float TransVoix::sample(float in1, float in2, float rest){
//   float out;
//   out = in1 + (in2 - in1)*rest;
//   buffer[index] = out;
//   index = (index + 1)%bufferSize;
//   return out;
// }

float TransVoix::sample(float in1, float in2, float rest){
  float out;
  out = in1;
  buffer[index] = out;
  index = (index + 1)%bufferSize;
  return out;
}

float TransVoix::tsm(int i){
  float out;
  out = buffer[index - i + treatSize];
  return out;
}
