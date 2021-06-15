#include <cmath>

#include "TransVoix.h"



TransVoix::TransVoix(int SR, int BS):
samplingRate(SR),
relativeRate(0.0),
bufferSize(BS),
treatSize(0),
//treatBuffer(nullptr),
windowSize(0),
olaSize(0),
nSlice(0),
// decalade(0),
// totalDecalade(0),
// lastDecalade(0),
index(0)
{
  buffer = new float[BS];
  for(int i = 0; i < BS; i++){
    buffer[i] = 0.0;
  }
}

TransVoix::~TransVoix()
{
  // delete[] treatBuffer;
  delete[] buffer;
}

float TransVoix::hamming(int i, int N){
  float out;
  out = 0.54 - 0.46 * cos(2*3.14159*i/N);
  return out;
}

void TransVoix::slice(int ws, int ola, int number){
  windowSize = ws;
  olaSize = ola;
  nSlice = number;
}

void TransVoix::setting(float r, int size){
  relativeRate = r;
  treatSize = size;
  // treatBuffer = new float[size];
  // for(int i = 0; i < size; i++){
  //   treatBuffer[i] = 0.0;
  // }
}

// void TransVoix::treat(float* in, float* out){
//   int indice;
//
//   //第一步：将原来的数据通过穿插采样的方式获取其他频率的数据
//   for(int i = 0; i < treatSize; i++){
//     indice = int(i*relativeRate);
//     treatBuffer[i] = (in[indice + 1] - in[indice])*(i*relativeRate - indice) + in[indice];
//   }
//
//   indice = 0;
//   //第二步：将拉伸过的数据通过OLA变成原来的长度
//   //增高音调->扩展音频长度
//   if(relativeRate > 1){
//     for(int i = 0; i < windowSize - olaSize + lastDecalade; i++){
//       out[indice] = treatBuffer[i];
//       indice ++ ;
//     }
//     for(int i = 1; i < nSlice-1; i++){
//       for(int j = 0; j < olaSize - decalade; j++){
//         out[indice] = (treatBuffer[i*windowSize - olaSize + j] + treatBuffer[i*windowSize - olaSize + decalade + j])/2;
//         indice ++;
//       }
//       for(int j = 0; j < windowSize - 2 * (olaSize - decalade); j++){
//         out[indice] = treatBuffer[i*windowSize - decalade + j];
//         indice ++;
//       }
//     }
//   }else{
//     for(int i = 0; i < windowSize - olaSize - lastDecalade; i++){
//       out[indice] = treatBuffer[i];
//       indice ++ ;
//     }
//     for(int i = 1; i < nSlice-1; i++){
//       for(int j = 0; j < olaSize + (2 * decalade); j++){
//         out[indice] = (treatBuffer[i*windowSize - olaSize + j] + treatBuffer[i*windowSize - olaSize - decalade + j])/2;
//         indice ++;
//       }
//       for(int j = 0; j < windowSize - 2 * (olaSize + decalade); j++){
//         out[indice] = treatBuffer[i*windowSize + decalade + j];
//         indice ++;
//       }
//     }
//   }
// }
//
// float TransVoix::origin(float in){
//   float out = in;
//   return out;
// }

float TransVoix::sample(float in1, float in2, float rest){
  float out;
  out = in1 + (in2 - in1)*rest;
  buffer[index] = out;
  index = (index + 1)%bufferSize;
  return out;
}

float TransVoix::tsm(int i){
  float out;
  out = buffer[index - i + treatSize];
  return out;
}

float TransVoix::ola(float in1, float in2, float rest){
  float out, ham1, ham2;
  ham1 = hamming(index - windowSize + olaSize, olaSize);
  ham2 = hamming(int(index - windowSize + 1.5*olaSize), olaSize);
  out = buffer[index - olaSize/2]*ham2 + (in1 + (in2 - in1)*rest)*ham1;
  buffer[index] = out;
  index = (index + 1)%bufferSize;
  return out;
}
