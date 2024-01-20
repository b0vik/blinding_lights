#include <cstdlib>
#include <cstdint>
#include <emscripten.h>

extern "C" {
    // API functions provided by the host environment
    extern uint32_t size();
    extern uint32_t get_pixel(uint32_t idx);
    extern void set_pixel(uint32_t idx, uint32_t col);
    extern uint64_t time();

    // The tick function that will be called by the host environment
    EMSCRIPTEN_KEEPALIVE void tick(float elapsed_time) {
        uint32_t idx = 0; // First pixel
        uint32_t color = (rand() % 256) << 16 | (rand() % 256) << 8 | (rand() % 256); // Random color
        set_pixel(idx, color);
    }
}