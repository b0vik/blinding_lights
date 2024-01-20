import os
import time
from wasmer import engine, Store, Module, Instance, ImportObject, Function, Type, FunctionType
from wasmer_compiler_cranelift import Compiler
from pyray import *


# Load the WASM module
store = Store()
with open('effect2.wasm', 'rb') as f:
   wasm_bytes = f.read()
module = Module(store, wasm_bytes)

# # Get the exported functions
# size = instance.exports.size
# get_pixel = instance.exports.get_pixel
# set_pixel = instance.exports.set_pixel
# tick = instance.exports.tick

# Initialize the LED strip
num_pixels = 750
led_strip = [0]*num_pixels # Assuming the LED strip has 100 pixels

def size() -> int:
  return len(led_strip)

def int_to_rgb(color):
   red = (color >> 16) & 0xFF
   green = (color >> 8) & 0xFF
   blue = color & 0xFF
   return red, green, blue, 255 # max opacity by default

def get_pixel(idx: int) -> int:
  return led_strip[idx]

def set_pixel(idx: int, col: int):
  # print(f"set pixel color {idx} to {int_to_rgb(col)}")
  led_strip[idx] = col

def current_time() -> int:
  return int(time.time() * 1000)
  
set_pixel_type = FunctionType(params=[Type.I32, Type.I32], results=[])
get_pixel_type = FunctionType(params=[Type.I32], results=[Type.I32])
size_type = FunctionType(params=[], results=[Type.I32])
time_type = FunctionType(params=[], results=[Type.I64])
  
import_object = ImportObject()
import_object.register(
    "env",
    {
        "set_pixel": Function(store, set_pixel, set_pixel_type),
        "get_pixel": Function(store, get_pixel, get_pixel_type),
        "size": Function(store, size, size_type),
        "time": Function(store, current_time, time_type)
    }
)

# Create the instance with the imports
instance = Instance(module, import_object)
tick = instance.exports.tick

# Initialize PyRay
init_window(2*num_pixels+2*2, 10*3, "emulator.py")
set_target_fps(60)

while not window_should_close():
  begin_drawing()
  clear_background(BLACK)

  # Draw the LED strip
  for i, color in enumerate(led_strip):
      draw_rectangle(i*2 + 2, 10, 8, 10, int_to_rgb(color))
  
  tick(get_time())
  end_drawing()


close_window()
