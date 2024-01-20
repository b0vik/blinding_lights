#include <cstdlib>
#include <cstdint>
#include <emscripten.h>
#include <cmath>

// Assuming the CHSV to RGB conversion function is provided elsewhere
bool initialized = false;
uint64_t strip_length = 0;

struct RGB {
   double r;
   double g;
   double b;
};

void initialize_stars();

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

struct Star {
    float position;
    float speed;
    RGB color;
};

// Assuming a maximum number of stars
#define NUM_STARS 100

Star stars[NUM_STARS];

extern "C" {
    // API functions provided by the host environment
    extern uint32_t size();
    extern uint32_t get_pixel(uint32_t idx);
    extern void set_pixel(uint32_t idx, uint32_t col);
    extern uint64_t time();

    // The tick function that will be called by the host environment
    EMSCRIPTEN_KEEPALIVE void tick(float elapsed_time) {
        if (!initialized) {
            initialize_stars(); // Initialize stars with random positions and colors
            initialized = true;
        }
        for (int i = 0; i < strip_length; ++i) {
                set_pixel(i, 0x000000);
            }
        for (int i = 0; i < NUM_STARS; i++) {
            stars[i].position += stars[i].speed;
            if (stars[i].position >= strip_length) {
                stars[i].position -= strip_length;
            }

            // Convert position to an index
            uint32_t idx = static_cast<uint32_t>(stars[i].position);

            // Set the star color at the calculated index
            uint32_t color = ((uint8_t) (stars[i].color.r * 255) << 16) | ((uint8_t) (stars[i].color.g * 255) << 8) | (uint8_t) (stars[i].color.b * 255);
            set_pixel(idx, color);
        }
    }
}

void initialize_stars() {
    strip_length = size();
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].position = static_cast<float>(rand() % strip_length);
        stars[i].speed = static_cast<float>(rand() % 100) / 20000.0f; // Random speed from 0.0 to 0.01
        stars[i].color = hsvToRgb(static_cast<double>(rand() % 360), 0, 0.05);
    }
}