#include <stdio.h>
#include <cmath>
#include <algorithm>

#include "AudioDsp.h"
#include "driver/i2s.h"

#define PI 3.1415926535897931
#define MULT_S16 32767
#define DIV_S16 0.0000305185

AudioDsp::AudioDsp(int SR, int BS) :
fSampleRate(SR), //Sample rate
fBufferSize(BS), //Buffer size
fNumInputs(2),
fNumOutputs(2),
fRelativeRate(0.0),
fRelativeSize(0),
fHandle(nullptr),
fRunning(false),
TransVoixL(SR,BS),
TransVoixR(SR,BS)
{
  // config i2s pin numbers
  i2s_pin_config_t pin_config;
  pin_config = {
    //0:MCLK；5:SCLK
    .bck_io_num = 5,
    //I2S输出可以直接路由到数字/模拟转换器输出通道（25，26），直接产生模拟输出
    //ws：声道选择信号
    .ws_io_num = 25,
    .data_out_num = 26,
    .data_in_num = 35
  };

  // config i2
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = fSampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
    .dma_buf_count = 6,
    .dma_buf_len = 2*fBufferSize,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = false
  };

  // installing i2s driver
  i2s_driver_install((i2s_port_t)0, &i2s_config, 0, nullptr);
  i2s_set_pin((i2s_port_t)0, &pin_config);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
  REG_WRITE(PIN_CTRL, 0xFFFFFFF0);
}

// starts audio task
bool AudioDsp::start()
{
  if (!fRunning) {
    fRunning = true;
    return (xTaskCreatePinnedToCore(audioTaskHandler, "Audio DSP Task", 4096, (void*)this, 24, &fHandle, 0) == pdPASS);
  }
  else {
    return true;
  }
}

// stops audio task
void AudioDsp::stop()
{
  if (fRunning) {
    fRunning = false;
    vTaskDelay(1/portTICK_PERIOD_MS);
    fHandle = nullptr;
  }
}

void AudioDsp::setRelative(float RR, int RS){
  fRelativeRate = RR;
  fRelativeSize = RS;
  // setting up DSP objects
  TransVoixL.setting(RR,RS);
  TransVoixR.setting(RR,RS);
}

void AudioDsp::audioTask()
{
  // inifinite loop
  while (fRunning) {
    int gain = 2;
    int index = 0;
    float rest = 0.0;

    int16_t samples_data_in[fNumOutputs*fBufferSize];
    int16_t samples_data_out[fNumOutputs*fBufferSize];

    // retrieving input buffer
    size_t bytes_read = 0;
    i2s_read((i2s_port_t)0, &samples_data_in, fNumInputs*sizeof(int16_t)*fBufferSize, &bytes_read, portMAX_DELAY);

    for(int i = 0; i < fBufferSize; i++){
      float inSampleLa = 0.0;
      float inSampleLd = 0.0;
      float inSampleRa = 0.0;
      float inSampleRd = 0.0;

      index = int(i*fRelativeRate);
      rest = i*fRelativeRate - index;

      //Addition Direct
      if(index < fBufferSize){
        // input buffer to float
        inSampleLa = samples_data_in[index*fNumInputs]*DIV_S16;
        inSampleLd = samples_data_in[(index+1)*fNumInputs]*DIV_S16;
        inSampleRa = samples_data_in[index*fNumInputs+1]*DIV_S16;
        inSampleRd = samples_data_in[(index+1)*fNumInputs+1]*DIV_S16;
      
        // DSP
        inSampleLa = TransVoixL.sample(inSampleLa,inSampleLd,rest);
        inSampleRa = TransVoixR.sample(inSampleRa,inSampleRd,rest);
      }else{
        inSampleLa = TransVoixL.tsm(i);
        inSampleRa = TransVoixR.tsm(i);
      }

      // copying to output buffer
      samples_data_out[i*fNumOutputs] = inSampleLa*MULT_S16*gain;
      samples_data_out[i*fNumOutputs+1] = inSampleRa*MULT_S16*gain;
    }
    // transmitting output buffer
    size_t bytes_written = 0;
    i2s_write((i2s_port_t)0, &samples_data_out, fNumOutputs*sizeof(int16_t)*fBufferSize, &bytes_written, portMAX_DELAY);
  }
  // Task has to deleted itself beforee returning
  vTaskDelete(nullptr);
}

// static cast of the audio task
void AudioDsp::audioTaskHandler(void* arg)
{
  AudioDsp* audio = static_cast<AudioDsp*>(arg);
  audio->audioTask();
}
