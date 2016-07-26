

#include <pebble.h>
#include <math.h>

static Window *gluggle, *s_intro_window, *s_calculate_window, *s_glass_window, *s_meter_window;
static TextLayer *s_text_layer, *s_text_layer_2, *s_calculate_layer, *s_glass_layer, *s_meter_layer, *s_volume_layer;

static int weight;
static char enterWeight[30];

static int waterNeeded;
static char waterAmount[100];

static int cupVolume;
static char enterVolume[100];

static BitmapLayer *gluggle_layer, *s_meter_bitmap_layer, *s_glass_bitmap_layer, *s_background_layer;
static GBitmap *gluggle_bitmap, *s_meter_bitmap, *s_glass_bitmap, *s_background_bitmap;
static Layer *meter;

int waterIntake;
int percentage;
static char percent[100];
static char volume[100];

int height;

bool hasBeenLaunchedBefore;


static void tick_handler(struct tm *tick_time, TimeUnits changed) {
	static char oldDate[50];
	strftime(oldDate, sizeof(oldDate), "%a %d %b", tick_time);
	persist_read_string(5, oldDate, sizeof(oldDate));
	static char date_buffer[50];
	strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
	if(oldDate != date_buffer) {
		waterIntake = 0;
		persist_write_int(4, waterIntake);
	}
	persist_write_string(5, date_buffer);
}


static void gluggle_load(Window *window){
	gluggle_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_compositing_mode(gluggle_layer, GCompOpSet);
	bitmap_layer_set_alignment(gluggle_layer, GAlignCenter);
	bitmap_layer_set_background_color(gluggle_layer, GColorClear);
	
	gluggle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GLUGGLE);
	bitmap_layer_set_bitmap(gluggle_layer, gluggle_bitmap);
	layer_add_child(window_get_root_layer(gluggle), bitmap_layer_get_layer(gluggle_layer));
}

static void gluggle_unload(Window *window){
	bitmap_layer_destroy(gluggle_layer);
}


static void intro_window_load(Window *window){
	
	s_text_layer = text_layer_create(GRect(10, 50, 125, 125));
	text_layer_set_background_color(s_text_layer, GColorClear);
 	text_layer_set_text_color(s_text_layer, GColorBlack);
	text_layer_set_font(s_text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOSEFIN_18)));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
	text_layer_set_text(s_text_layer, "Enter your weight:");
	layer_add_child(window_get_root_layer(s_intro_window), text_layer_get_layer(s_text_layer));
	
	s_text_layer_2 = text_layer_create(GRect(10, 70, 125, 125));
	text_layer_set_background_color(s_text_layer_2, GColorClear);
 	text_layer_set_text_color(s_text_layer_2, GColorBlack);
	text_layer_set_font(s_text_layer_2, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOSEFIN_18)));
	text_layer_set_text_alignment(s_text_layer_2, GTextAlignmentCenter);
	text_layer_set_text(s_text_layer_2, "0");
	layer_add_child(window_get_root_layer(s_intro_window), text_layer_get_layer(s_text_layer_2));

	
	weight = persist_read_int(1);
	
	snprintf(enterWeight, sizeof(enterWeight), "%d lb", weight);
	text_layer_set_text(s_text_layer_2, enterWeight);
	layer_add_child(window_get_root_layer(s_intro_window), text_layer_get_layer(s_text_layer_2));
	
	//hasBeenLaunchedBefore = false;
	hasBeenLaunchedBefore = persist_read_bool(0);
  	//hasBeenLaunchedBefore = false;
	//if it hasnt been launched before, we have to do a little setup
	if (!hasBeenLaunchedBefore) {
    	//first we write create a space for our number of glasses to be stored
    	StatusCode intStatus = persist_write_int(1, weight);
    	//then we tell the watch that we have launched for the first time
    	//next we output some info about whether or not the info was saved, this can be ignored
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "Writing... int code %d" , intStatus);
		
		
	}  else {
		text_layer_set_text(s_text_layer, "");
		text_layer_set_text(s_text_layer_2, "Press select");
	}
	

	
}

static void intro_window_unload(Window *window){
	text_layer_destroy(s_text_layer);
	text_layer_destroy(s_text_layer_2);
}


static void calculate_water_needed(int weight){
	waterNeeded = weight/2;
	StatusCode intStatus = persist_write_int(2, waterNeeded);
	snprintf(waterAmount, sizeof(waterAmount), "You need %d ounces of water a day!", waterNeeded);
	text_layer_set_text(s_calculate_layer, waterAmount);
	layer_add_child(window_get_root_layer(s_calculate_window), text_layer_get_layer(s_calculate_layer));
}

static void calculate_window_load(Window *window){
	s_calculate_layer = text_layer_create(GRect(10, 50, 125, 125));
	text_layer_set_background_color(s_calculate_layer, GColorClear);
 	text_layer_set_text_color(s_calculate_layer, GColorBlack);
	text_layer_set_font(s_calculate_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOSEFIN_18)));
	text_layer_set_text_alignment(s_calculate_layer, GTextAlignmentCenter);
	
	calculate_water_needed(weight);
	layer_add_child(window_get_root_layer(s_calculate_window), text_layer_get_layer(s_calculate_layer));
}

static void calculate_window_unload(Window *window){
	text_layer_destroy(s_calculate_layer);
}


static void glass_window_load(Window *window){
	s_glass_layer = text_layer_create(GRect(10, 50, 125, 125));
	text_layer_set_background_color(s_glass_layer, GColorClear);
 	text_layer_set_text_color(s_glass_layer, GColorBlack);
	text_layer_set_font(s_glass_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOSEFIN_18)));
	text_layer_set_text_alignment(s_glass_layer, GTextAlignmentCenter);
	
	snprintf(enterVolume, sizeof(enterVolume), "Set the volume of your cup: %d oz", cupVolume);
	text_layer_set_text(s_glass_layer, enterVolume);
	layer_add_child(window_get_root_layer(s_glass_window), text_layer_get_layer(s_glass_layer));
	
	hasBeenLaunchedBefore = persist_read_bool(0);
  
	//if it hasnt been launched before, we have to do a little setup
	if (!hasBeenLaunchedBefore) {
    	//first we write create a space for our number of glasses to be stored
    	StatusCode intStatus = persist_write_int(3, cupVolume);
    	//then we tell the watch that we have launched for the first time
    	//next we output some info about whether or not the info was saved, this can be ignored
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "Writing... int code %d" , intStatus);
	}
	
	cupVolume = persist_read_int(3);
	
	snprintf(enterVolume, sizeof(enterVolume), "Set the volume of your cup: %d oz", cupVolume);
	text_layer_set_text(s_glass_layer, enterVolume);
	layer_add_child(window_get_root_layer(s_glass_window), text_layer_get_layer(s_glass_layer));
}

static void glass_window_unload(Window *window){
	text_layer_destroy(s_glass_layer);
}


static void draw_meter(Layer *layer, GContext *ctx){
	waterNeeded = persist_read_int(2);
	height = round((float)waterIntake/(float)waterNeeded*168);
	
	if(height > 168){
		height = 168;
	}
	
	graphics_context_set_fill_color(ctx, GColorCeleste);
	graphics_fill_rect(ctx, GRect(105, 0, 39, height), 0, GCornerNone);
}

static void meter_window_load(Window *window){
  
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG2);
  s_background_layer = bitmap_layer_create(GRect(0, -6, 144, 180));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  //bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
	s_meter_layer = text_layer_create(GRect(50, 3, 60, 148));
	text_layer_set_background_color(s_meter_layer, GColorClear);
 	text_layer_set_text_color(s_meter_layer, GColorBlack);
	text_layer_set_font(s_meter_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOSEFIN_28)));
	text_layer_set_text_alignment(s_meter_layer, GTextAlignmentCenter);
	
	cupVolume = persist_read_int(3);
	waterNeeded = persist_read_int(2);
	waterIntake = persist_read_int(4);
	
	waterNeeded = persist_read_int(2);
	percentage = ((float)waterIntake/(float)waterNeeded)*100;
	snprintf(percent, sizeof(percent), "%d%%", percentage);
	text_layer_set_text(s_meter_layer, percent);
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, percent);
	
	//hasBeenLaunchedBefore = persist_read_bool(0);
  
    //StatusCode intStatus = persist_write_int(3, percentage);
    	
	//persist_write_int(3, percentage);
	//percentage = persist_read_int(3);
	
	//snprintf(percent, sizeof(percent), "%d%%", percentage);
	//text_layer_set_text(s_meter_layer, percent);
  
	layer_add_child(window_get_root_layer(s_meter_window), text_layer_get_layer(s_meter_layer));
	
	meter = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(meter, draw_meter);
	layer_add_child(window_get_root_layer(s_meter_window), meter);
	
	s_volume_layer = text_layer_create(GRect(5, 130, 110, 38));
	text_layer_set_background_color(s_volume_layer, GColorClear);
 	text_layer_set_text_color(s_volume_layer, GColorBlack);
	text_layer_set_font(s_volume_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_JOSEFIN_18)));
	//text_layer_set_text_alignment(s_volume_layer, GTextAlignmentCenter);
	
	snprintf(volume, sizeof(volume), "%d oz / %d oz", waterIntake, waterNeeded);
	text_layer_set_text(s_volume_layer, volume);
	
	
	s_meter_bitmap_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_compositing_mode(s_meter_bitmap_layer, GCompOpSet);
	//bitmap_layer_set_alignment(s_meter_bitmap_layer, GAlignCenter);
	//bitmap_layer_set_background_color(s_meter_bitmap_layer, GColorClear);
	
	//s_meter_bitmap = gbitmap_create_with_resource(RESOURCE_ID_METER);
  s_meter_bitmap = gbitmap_create_with_resource(RESOURCE_ID_METER2);
	bitmap_layer_set_bitmap(s_meter_bitmap_layer, s_meter_bitmap);
	
	s_glass_bitmap_layer = bitmap_layer_create(GRect(-80, -70, 200, 200));
	bitmap_layer_set_compositing_mode(s_glass_bitmap_layer, GCompOpSet);
	//bitmap_layer_set_alignment(s_glass_bitmap_layer, GAlignLeft);
	//bitmap_layer_set_background_color(s_glass_bitmap_layer, GColorClear);
	
	s_glass_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GLASS);
	bitmap_layer_set_bitmap(s_glass_bitmap_layer, s_glass_bitmap);
	
	//layer_set_update_proc(meter, draw_meter);
	
	layer_add_child(window_get_root_layer(s_meter_window), text_layer_get_layer(s_meter_layer));
	layer_add_child(window_get_root_layer(s_meter_window), bitmap_layer_get_layer(s_meter_bitmap_layer));
	layer_add_child(window_get_root_layer(s_meter_window), bitmap_layer_get_layer(s_glass_bitmap_layer));
	layer_add_child(window_get_root_layer(s_meter_window), text_layer_get_layer(s_volume_layer));
	
}

static void meter_window_unload(Window *window){
	text_layer_destroy(s_meter_layer);
	text_layer_destroy(s_volume_layer);
	bitmap_layer_destroy(s_meter_bitmap_layer);
	bitmap_layer_destroy(s_glass_bitmap_layer);
}


static void select_single_click_handler0(ClickRecognizerRef recognizer, void *context){
	window_stack_push(s_intro_window, true);
}

static void click_config_provider0(void *context){
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler0);
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	
	bool hasBeenLaunchedBefore = persist_read_bool(0);
	
	if(!hasBeenLaunchedBefore) {
		window_stack_push(s_calculate_window, true);
	} else {
		window_stack_push(s_meter_window, true);
	}
	 
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	weight+=5;
	snprintf(enterWeight, sizeof(enterWeight), "%d lb", weight);
	text_layer_set_text(s_text_layer_2, enterWeight);
	layer_add_child(window_get_root_layer(s_intro_window), text_layer_get_layer(s_text_layer_2));
	StatusCode intStatus = persist_write_int(1, weight);
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	weight-=5;
	if(weight<0){
		weight = 0;
	}
	snprintf(enterWeight, sizeof(enterWeight), "%d lb", weight);
	text_layer_set_text(s_text_layer_2, enterWeight);
	layer_add_child(window_get_root_layer(s_intro_window), text_layer_get_layer(s_text_layer_2));
	StatusCode intStatus = persist_write_int(1, weight);
}

static void click_config_provider(void *context) {
	// Subcribe to button click events here
	//window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}


static void select_single_click_handler2(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	window_stack_push(s_glass_window, true);
}

static void click_config_provider2(void *context){
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler2);
}


static void select_single_click_handler3(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	window_stack_push(s_meter_window, true);
	StatusCode boolStatus = persist_write_bool(0, true);
}

static void up_single_click_handler3(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	cupVolume = persist_read_int(3);
	cupVolume++;
	snprintf(enterVolume, sizeof(enterVolume), "Set the volume of your cup: %d oz", cupVolume);
	text_layer_set_text(s_glass_layer, enterVolume);
	layer_add_child(window_get_root_layer(s_glass_window), text_layer_get_layer(s_glass_layer));
	StatusCode intStatus = persist_write_int(3, cupVolume);
}

static void down_single_click_handler3(ClickRecognizerRef recognizer, void *context) {
	// A single click has just occured
	cupVolume = persist_read_int(3);
	cupVolume--;
	
	if(cupVolume < 0){
		cupVolume = 0;
	}
	
	snprintf(enterVolume, sizeof(enterVolume), "Set the volume of your cup: %d oz", cupVolume);
	text_layer_set_text(s_glass_layer, enterVolume);
	layer_add_child(window_get_root_layer(s_glass_window), text_layer_get_layer(s_glass_layer));
	StatusCode intStatus = persist_write_int(3, cupVolume);
}

static void click_config_provider3(void *context){
	
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler3);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler3);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler3);
}


static void select_single_click_handler4(ClickRecognizerRef recognizer, void *context) {
	cupVolume = persist_read_int(3);
	waterNeeded = persist_read_int(2);
	waterIntake = persist_read_int(4);
	waterIntake += cupVolume;
	
	
	meter = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(meter, draw_meter);
	layer_add_child(window_get_root_layer(s_meter_window), meter);
	layer_add_child(window_get_root_layer(s_meter_window), bitmap_layer_get_layer(s_meter_bitmap_layer));
	
	percentage = ((float)waterIntake/(float)waterNeeded)*100;
	//StatusCode intStatus = persist_write_int(3, percentage);
	snprintf(percent, sizeof(percent), "%d%%", percentage);
	//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, percent);
	text_layer_set_text(s_meter_layer, percent);
	layer_add_child(window_get_root_layer(s_meter_window), text_layer_get_layer(s_meter_layer));
	
	snprintf(volume, sizeof(volume), "%d oz / %d oz", waterIntake, waterNeeded);
	text_layer_set_text(s_volume_layer, volume);
	layer_add_child(window_get_root_layer(s_meter_window), text_layer_get_layer(s_volume_layer));
	
	StatusCode intStatus = persist_write_int(4, waterIntake);
}

static void click_config_provider4(void *context){
	
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler4);
}


static void init(){
	
	//if(weight == 0 || cupVolume == 0){
	
	gluggle = window_create();
	s_intro_window = window_create();
	s_calculate_window = window_create();
	s_glass_window = window_create();
	s_meter_window = window_create();
	
	window_set_window_handlers(gluggle, (WindowHandlers) {
    .load = gluggle_load,
    .unload = gluggle_unload
 	});
	window_set_background_color(gluggle, GColorWhite);
	
	window_set_window_handlers(s_intro_window, (WindowHandlers) {
    .load = intro_window_load,
    .unload = intro_window_unload
 	});
	window_set_background_color(s_intro_window, GColorWhite);
	
	window_set_window_handlers(s_calculate_window, (WindowHandlers) {
    .load = calculate_window_load,
    .unload = calculate_window_unload
 	});
	window_set_background_color(s_calculate_window, GColorWhite);
	
	window_set_window_handlers(s_glass_window, (WindowHandlers) {
    .load = glass_window_load,
    .unload = glass_window_unload
 	});
	window_set_background_color(s_glass_window, GColorWhite);

	
	window_stack_push(gluggle, true);
	//}
		
	// Use this provider to add button click subscriptions
	window_set_click_config_provider(gluggle, click_config_provider0);
	window_set_click_config_provider(s_intro_window, click_config_provider);
	window_set_click_config_provider(s_calculate_window, click_config_provider2);
	window_set_click_config_provider(s_glass_window, click_config_provider3);
	window_set_click_config_provider(s_meter_window, click_config_provider4);
		
	window_set_window_handlers(s_meter_window, (WindowHandlers) {
    .load = meter_window_load,
    .unload = meter_window_unload
 	});
	window_set_background_color(s_meter_window, GColorWhite);
	
	//tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(){
	window_destroy(s_intro_window);
	window_destroy(s_calculate_window);
	window_destroy(s_glass_window);
	window_destroy(s_meter_window);
}

int main(void){
	init();
	app_event_loop();
	deinit();
}