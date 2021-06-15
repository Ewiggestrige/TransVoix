#ifndef TRANSVOIX_H_INCLUDE
#define TRANSVOIX_H_INCLUDE

class TransVoix{
  public:
    TransVoix(int SR, int BS);
    ~TransVoix(); //析构函数

    float hamming(int i, int N);
    void slice(int ws, int ola, int number);
    void setting(float r, int size); //设置相对采样频率，采样后buffer大小，OLA大小，初始化buffer
    //void treat(float* in, float* out);
    //float origin(float in);
    float sample(float in1, float in2, float rest);
    float tsm(int i);
    float ola(float in1, float in2, float rest);
  private:
    float samplingRate, relativeRate;//原始采样频率，相对采样频率
    int bufferSize, treatSize; //原始buffer大小，采样后buffer大小
    //float* treatBuffer; //存储重新采样后的数据
    int windowSize, olaSize, nSlice;
    int index;
    //float* hanning;
    float* buffer;
};

#endif
