#include <cstdlib>
#include <cstdint>
#include <emscripten.h>
#include <cmath>

int random(int min, int max) {
   return min + rand() % (max - min + 1);
}



struct RGB {
   double r;
   double g;
   double b;
};

RGB hsvToRgb(double h, double s, double v) {
   double r, g, b;
   int i;
   double f, p, q, t;

   // Convert HSV to RGB
   if(s == 0) {
       // Achromatic (grey)
       r = g = b = v;
   } else {
       h /= 60;        // sector 0 to 5
       i = floor(h);
       f = h - i;      // factorial part of h
       p = v * (1 - s);
       q = v * (1 - s * f);
       t = v * (1 - s * (1 - f));
       switch(i) {
           case 0:
               r = v;
               g = t;
               b = p;
               break;
           case 1:
               r = q;
               g = v;
               b = p;
               break;
           case 2:
               r = p;
               g = v;
               b = t;
               break;
           case 3:
               r = p;
               g = q;
               b = v;
               break;
           case 4:
               r = t;
               g = p;
               b = v;
               break;
           default:    // case 5:
               r = v;
               g = p;
               b = q;
               break;
       }
   }

   return {r, g, b};
}
struct Train {
    int length;
    float position;
    float speed;
    RGB color;
};

#define MAX_TRAINS 100
#define MAX_PIXELS 1024

Train trains[MAX_TRAINS];
RGB pixels[MAX_PIXELS];

uint32_t strip_length = 0;
bool initialized = false;


#define TRAINS_USE_RAND_RGB false
#define NUM_TRAINS 21
#define MIN_SATURATION 0.8
#define MIN_VALUE 0.9


void initialize();

extern "C" {
   // API functions provided by the host environment
   extern uint32_t size();
   extern uint32_t get_pixel(uint32_t idx);
   extern void set_pixel(uint32_t idx, uint32_t col);
   extern uint64_t time();

   EMSCRIPTEN_KEEPALIVE void tick(float elapsed_time) {
       if (!initialized) {
           initialize();
       }
       // Update the position of each train
       for (int i = 0; i < NUM_TRAINS; ++i) {
           trains[i].position += trains[i].speed * 1;
           if (trains[i].position >= strip_length) {
               trains[i].position -= strip_length;
           }
       }

       // clear pixel buffer
       for (int i = 0; i < strip_length; ++i) {
           pixels[i] = {0, 0, 0};
       }

       // draw trains
       for (int i = 0; i < NUM_TRAINS; ++i) {
           int start_pos = static_cast<int>(trains[i].position) % strip_length;
           for (int j = 0; j < trains[i].length; ++j) {
               int pixel_idx = (start_pos + j) % strip_length;
               pixels[pixel_idx] = trains[i].color;
           }
       }

       // draw out pixel buffer
       for (int i = 0; i < strip_length; ++i) {
           uint32_t color = ((uint8_t) (pixels[i].r * 255) << 16) | ((uint8_t) (pixels[i].g * 255) << 8) | (uint8_t) (pixels[i].b * 255);
           set_pixel(i, color);
       }
   }
}

// populate trains
void initialize() {
   strip_length = size();
   for (int i = 0; i < NUM_TRAINS; ++i) {
       trains[i].length = rand() % 15 + 12;
       trains[i].position = static_cast<float>(rand() % strip_length);
       trains[i].speed = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * (3.2f) + 1.0f; 

       if (TRAINS_USE_RAND_RGB) {
           trains[i].color.r = rand() % 256;
           trains[i].color.g = rand() % 256;
           trains[i].color.b = rand() % 256;
       } else {
           // Use hsvToRgb to generate a random color
           double hue = static_cast<double>(rand() % 360);
           trains[i].color = hsvToRgb(hue, (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) * (1.0 - MIN_SATURATION) + MIN_SATURATION, (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) * (1.0 - MIN_VALUE) + MIN_VALUE);
       }
   }
   initialized = true;
}
