#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "ES8388.h"

#include "AudioDsp.h"

extern "C"{
  void app_main(void);
}

void app_main(void){

  //ES8388是一种高性能、低功耗、低成本的音频编解码器。
  /*它由两路ADC，2通道DAC，话筒放大器、耳机放大器、数字音效、模拟混合和增益功能。
  ES8388采用先进的多位Δ∑调制技术实现数字与模拟之间的数据转换。
  多比特Δ∑调制器使器件对时钟抖动和低带外噪声的灵敏度低。
  它应用于：MID，MP3, MP4, PMP，无线音频，数码相机，摄像机，GPS领域，蓝牙，便携式音频设备。
  */
  //esp32-lyraTv4_3开发板所使用的编解码器是ES8388
  ES8388 es8388;
  es8388.init();

  // start audio DSP
  //sample rate: 48000, buffer size: 128
  AudioDsp audioDsp(48000,128);
  audioDsp.setRelative(1.2,106);//853
  audioDsp.start();

  // waiting forever
  vTaskSuspend(nullptr);
}
