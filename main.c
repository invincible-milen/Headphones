#include "stm32f4xx.h"
#include "adc.h"
#include "spi.h"
#include "ST7735.h"

int main(){
    initSPI(spi1);
    initST7735();
    displayOn();
    initADC();
    startADC();

    fillScreen(BLACK);

    int type = 3;
    uint16_t data;
    int height;
    int mid;

    while(1){
        if(type == 1 || type == 2){
            data = (readADC() & 0x0FFF);

            height = (data * 128)/4095;
            if(height > 128) height = 127;
            if(height <= 0) height = 1;
        }

        if(type == 1){
            for(int x=0; x<128; x++){
                framebuffer[x+2][162] = BLACK;
            }
            framebuffer[height][162] = WHITE;
            framebuffer[height-1][162] = WHITE;
            framebuffer[height+1][162] = WHITE;

            scrollUp();

            flushFrameBuffer();
        }
        else if(type == 2){
            mid = 68;
            for(int x=0; x<128; x++){
                framebuffer[x+2][162] = BLACK;
            }

            if (height > mid){
                for(int x= ((mid*2))-height-1; x<=height+1; x++){
                    framebuffer[x][162] = WHITE;
                }
            }
            else if(height < mid){
                for(int x=height-1; x<= (mid*2)-height+1; x++){
                    framebuffer[x][162] = WHITE;
                }
            }
            else{
                framebuffer[height][162] = WHITE;
                framebuffer[height-1][162] = WHITE;
                framebuffer[height+1][162] = WHITE;
            }

            scrollUp();

            flushFrameBuffer();
        }

        else if(type == 3){
            //average
            uint16_t sum = 0;
            int N=5;
            for(int i=0; i<N; i++){
                data = readADC() & 0x0FFF;
                int16_t val = (int16_t)data - 2267;
                int16_t mask = (val) >> 15;
                uint16_t magnitude = ((val + mask) ^ mask);

                sum = sum + magnitude;
            }

            fillRectangle(0, 60, 127, 100, BLACK);

            uint16_t avg = sum/N;
            uint8_t barheight = (avg * 128) / 2048;
            if(barheight>127) barheight = 127;

            fillRectangle(0, 60, barheight, 100, RED);

        }
        else if(type == 4){
            uint16_t signal[32]; //here and below
            for(int i=0; i<32; i++){
                data = readADC() & 0x0FFF;
                int16_t val = ((int16_t)data - 2267);
                int16_t mask = (val) >> 15;
                signal[i] = ((val + mask) ^ mask);

            }

            uint16_t magnitudes[8];
            for(int b=0; b<8; b++){
                uint32_t sum=0;
                for(int i=0; i<4; i++){ // here and below
                    sum += signal[i+ (b*4)];
                }
                magnitudes[b] = sum>>2;
            }

            uint8_t barheights[8];
            uint8_t decay =7;

            for(int b=0; b<8; b++){
                uint8_t newheight;
                if(b==3 || b==4) newheight = (magnitudes[b] * 128) / 2048;
                else if(b==1 || b==2 || b==5 || b==6) newheight = (magnitudes[b] * 96) / 2048;
                else if(b==0 || b==7) newheight = (magnitudes[b] * 64) /2048;
                if(newheight>127) newheight = 127;

                if(newheight > barheights[b]){
                    barheights[b] = newheight;
                }
                else{
                    if(newheight>decay) barheights[b] -= decay;
                    else barheights[b] = 2;
                }

                uint16_t YS = (20*b)+5;

                fillRectangle(0, YS, 127, YS+10, BLACK);

                fillRectangle(0, YS, barheights[b], YS+10, RED);
            }
        }



    }
}
