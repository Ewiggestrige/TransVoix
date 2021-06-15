#ifndef TRANSVOIX_H_INCLUDE
#define TRANSVOIX_H_INCLUDE

class TransVoix{
  public:
    TransVoix(int SR, int BS);
    ~TransVoix(); //析构函数

    void setting(float r, int size); //设置相对采样频率，采样后buffer大小，OLA大小，初始化buffer
    float sample(float in1, float in2, float rest);
    float tsm(int i);
  private:
    float samplingRate, relativeRate;//原始采样频率，相对采样频率
    int bufferSize, treatSize; //原始buffer大小，采样后buffer大小
    int index;
    float* buffer;
};

#endif
