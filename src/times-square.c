#include <pebble.h>
#include <time.h>

static Window *window;

static Layer *display_layer;

static BitmapLayer *background_image_layer;

#define CELL_WIDTH 8
#define CELL_HEIGHT 8
#define CELL_PADDING_RIGHT 5
#define CELL_PADDING_TOP 5

#define LEFT_OFFSET 33
#define TOP_OFFSET 10

GRect cell_location(int row, int col) {
  return GRect((row * (CELL_WIDTH + CELL_PADDING_RIGHT)) + LEFT_OFFSET, (col * (CELL_HEIGHT + CELL_PADDING_TOP)) + TOP_OFFSET, CELL_WIDTH, CELL_HEIGHT);
}

void display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  graphics_context_set_fill_color(ctx, GColorWhite);

  int minutes = 0;
  int hour = tick_time->tm_hour;
  /* convert 24 hours to 12 hours */
  if (hour > 12) {
    hour = hour - 12;
  }

  if (hour == 0) {
    hour = 12;
  }
  int col, row, height;

  for (col = 0; col < 12; col++) {
    for (row = 0; row < 6; row++) {
      height = col;
      /* Hour mode */
      if (row == 0) {
        if (12 - col > hour) {
          continue;
        }
      } else {
        if (minutes++ < tick_time->tm_min) {
          height = (11 - col);
        } else {
          continue;
        }
      }

      graphics_fill_rect(ctx, cell_location(row, height), 0, GCornerNone);
    }
  }


}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(display_layer);
}




void handle_init(void) {
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);
  display_layer = layer_create(bounds);
  background_image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(background_image_layer, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND));

  layer_add_child(window_layer, bitmap_layer_get_layer(background_image_layer));

  layer_set_update_proc(display_layer, display_layer_update_callback);

  layer_add_child(window_layer, display_layer);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

  layer_mark_dirty(display_layer);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  layer_destroy(bitmap_layer_get_layer(background_image_layer));
  layer_destroy(display_layer);
  window_destroy(window);

}


int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
