#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

PBL_APP_INFO("Pimpin Ain't Easy", "Gazler", 1, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

Layer display_layer;

TextLayer timeLayer; // The word indicators

#define CELL_WIDTH 8
#define CELL_HEIGHT 8
#define CELL_PADDING_RIGHT 5
#define CELL_PADDING_TOP 5

#define LEFT_OFFSET 33
#define TOP_OFFSET 10

GRect cell_location(int row, int col) {
  return GRect((row * (CELL_WIDTH + CELL_PADDING_RIGHT)) + LEFT_OFFSET, (col * (CELL_HEIGHT + CELL_PADDING_TOP)) + TOP_OFFSET, CELL_WIDTH, CELL_HEIGHT);
}

void display_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  PblTm t;

  get_time(&t);

  graphics_context_set_fill_color(ctx, GColorWhite);

  int minutes = 0;
  int hour = t.tm_hour;
  /* convert 24 hours to 12 hours */
  if (hour > 12) {
    hour = hour - 12;
  }
  int i, j, height;

  for (i = 0; i < 12; i++) {
    for (j = 0; j < 6; j++) {
      height = i;
      /* Hour mode */
      if (j == 0) {
        if (12 - i > hour) {
          continue;
        }
      } else {
        if (minutes++ < t.tm_min) {
          height = (11 - i);
        } else {
          continue;
        }
      }

      graphics_fill_rect(ctx, cell_location(j, height), 0, GCornerNone);
    }
  }

}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;

  layer_mark_dirty(&display_layer);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Pimpin Ain't Easy");
  window_stack_push(&window, true);

  window_set_background_color(&window, GColorBlack);

  // Init the layer for the display
  layer_init(&display_layer, window.layer.frame);
  display_layer.update_proc = &display_layer_update_callback;
  layer_add_child(&window.layer, &display_layer);


}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
