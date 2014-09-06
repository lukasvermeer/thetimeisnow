/*

"We Are Here Now"

Simple Pebble watchface intended to encourage mindfulness of the present.

Created by Lukas Vermeer - www.lukasvermeer.nl
Inspired by Durr - http://skreksto.re/products/durr

*/

#include <pebble.h>

Window *my_window;
TextLayer *text_layer;
static GFont s_time_font;

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "WE00 0ARE HERE NOW0";
  
  snprintf(buffer, 20, "WE%01d%01d %01dARE HERE NOW%01d", 
           tick_time->tm_hour / 10, 
           tick_time->tm_hour % 10, 
           tick_time->tm_min / 10,
           tick_time->tm_min % 10); 
  
  text_layer_set_text(text_layer, buffer);
  
  if (tick_time->tm_min % 5 == 0) {
    static const uint32_t const segments[] = { 200, 100, 400, 200, 200 };
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
    };
    vibes_enqueue_custom_pattern(pat);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  text_layer = text_layer_create(GRect(0, -5, 144, 173));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorClear);
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VERA_MONO_BOLD_42));
  text_layer_set_font(text_layer, s_time_font);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  
  update_time();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void handle_init(void) {
  my_window = window_create();

  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(my_window, true);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}