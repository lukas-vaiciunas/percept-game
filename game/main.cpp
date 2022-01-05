#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "Driver.h"
#include "DriverEvents.h"
#include "Config.h"
#include "FontData.h"

#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif
#define _CRTDBG_MAP_ALLOC

void configure();

int main(void)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	const ALLEGRO_COLOR clearColor = al_map_rgb(25, 25, 75);

	ALLEGRO_DISPLAY *display = nullptr;
	ALLEGRO_EVENT_QUEUE *eventQueue = nullptr;
	ALLEGRO_TIMER *timer = nullptr;

	bool redraw = true;

	if (!al_init())
		return -1;

	if (!al_install_keyboard())
		return -1;

	if (!al_install_mouse())
		return -1;

	if (!al_init_primitives_addon())
		return -1;

	if (!al_init_image_addon())
		return -1;

	if (!al_init_font_addon())
		return -1;

	if (!al_init_ttf_addon())
		return -1;

	//al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	configure();

	display = al_create_display(Config::displayWidth, Config::displayHeight);
	if (!display)
		return -1;

	eventQueue = al_create_event_queue();
	if (!eventQueue)
		return -1;

	timer = al_create_timer(1.0 / 60);
	if (!timer)
		return -1;

	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	al_register_event_source(eventQueue, al_get_mouse_event_source());

	FontData::instance().emplace(Config::smallFontSize, al_load_ttf_font("fonts/chary___.ttf", Config::smallFontSize, 0));
	FontData::instance().emplace(Config::mediumFontSize, al_load_ttf_font("fonts/chary___.ttf", Config::mediumFontSize, 0));
	FontData::instance().emplace(Config::largeFontSize, al_load_ttf_font("fonts/chary___.ttf", Config::largeFontSize, 0));

	Driver driver;

	al_start_timer(timer);

	while (!driver.isDone())
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(eventQueue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			driver.updateOnTick();

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			driver.updateOnMousePress(ev.mouse.button, ev.mouse.x, ev.mouse.y);
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			driver.updateOnKeyPress(ev.keyboard.keycode);
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			driver.updateOnKeyRelease(ev.keyboard.keycode);
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			driver.eventQueue().send(new EventQuit());
		}
		
		if (redraw && al_is_event_queue_empty(eventQueue))
		{
			redraw = false;

			al_clear_to_color(clearColor);
			driver.render();
			al_flip_display();
		}
	}

	al_uninstall_keyboard();
	al_uninstall_mouse();

	al_destroy_display(display);
	al_destroy_event_queue(eventQueue);
	al_destroy_timer(timer);

	return 0;
}

void configure()
{
	ALLEGRO_MONITOR_INFO monitorInfo;
	al_get_monitor_info(0, &monitorInfo);

	//Config::displayWidth = monitorInfo.x2 - monitorInfo.x1;
	//Config::displayHeight = monitorInfo.y2 - monitorInfo.y1;

	Config::displayWidth = 1280;
	Config::displayHeight = 720;

	Config::gameScale = std::floorf((Config::displayWidth + Config::displayHeight) * 0.5f * 0.005f);
	Config::hudScale = Config::gameScale + 2.0f;

	Config::smallFontSize = std::floorf((Config::displayWidth + Config::displayHeight) * 0.5f * 0.025f / 16.0f) * 16.0f;
	Config::mediumFontSize = Config::smallFontSize * 2;
	Config::largeFontSize = Config::mediumFontSize * 4;
}