#include <pebble.h>

#define TIME_BUFFER_LEN 8
#define FORMAT_STRING_LEN 10

static Window *s_main_window;
static TextLayer *s_time_layer;

// Format and display tick_time onto s_time_layer
static void update_time(struct tm *tick_time){
    static char s_buffer[TIME_BUFFER_LEN];
    static char s_format_string[FORMAT_STRING_LEN];
    
    if (clock_is_24h_style()){
        strcpy(s_format_string, "%H:%M");
    } else {
        strcpy(s_format_string, "%I:%M");
    }
    
    strftime(s_buffer, sizeof(s_buffer), (char *)s_format_string, tick_time);
    text_layer_set_text(s_time_layer, s_buffer);  
    
    return;
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(tick_time);
}


// Window handler functions
static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

    // Improve the layout to be more like a watchface
    text_layer_set_background_color(s_time_layer, GColorWhite);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_time_layer);
}



static void init(){
    // Create a Window element
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    
    // Subscribe to the tick timer service
    // The time will be updated every minute
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, false);
    
    // Get the initial time
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
    update_time(tick_time);

}

static void deinit(){
    // Destroy Window
    window_destroy(s_main_window);
}


int main(void){
    init();
    app_event_loop();
    deinit();
  
    return 0;
}