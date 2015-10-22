
#include "stdafx.h"
#include <chrono>
#include <thread>
#include <stdio.h>
#include <sdlcpp/sdlcpp.h>
#undef main

int main(int argc, char* argv [])
{
	sdlcpp::Initializer l_initializer(SDL_INIT_EVERYTHING);
	if (!l_initializer)
		return -1;

	auto l_backbuffer = sdlcpp::setVideoMode(1200, 800, 32, SDL_SWSURFACE);

	sdlcpp::setCaption("BONJOUR STELLA !!!", nullptr);

	sdlcpp::SpriteStrip l_asterixSpriteStrip(
		sdlcpp::loadBmp("C:\\DEV\\GAME\\Asterix\\Asterix\\Sprites\\Asterix.Sprites.Row1.bmp", { 255, 200, 0 }),
		48,
		64
		);

	auto l_asterix = sdlcpp::loadBmp("C:\\DEV\\GAME\\Asterix\\Asterix\\Sprites\\Asterix.Sprite.bmp", { 255, 0, 255 });

	SDL_Rect l_rectVillage = { 0, 0, 1200, 800 };
	sdlcpp::fillRect(l_backbuffer, &l_rectVillage, { 50, 100, 150 });
	
	SDL_Rect l_rectAsterix = { 0, 0, 0, 0 };
	sdlcpp::blitSurface(l_asterix, nullptr, l_backbuffer, &l_rectAsterix);

	sdlcpp::Sprite l_asterixSprite(l_asterixSpriteStrip);
	l_asterixSprite.blit(l_backbuffer, 0, 0);

	sdlcpp::flip(l_backbuffer);

	double l_xAsterix = 0.0;
	double l_yAsterix = 0.0;
	Sint16 l_xTarget = 0;
	Sint16 l_yTarget = 0;

	bool l_end = false;
	sdlcpp::EventPoller l_eventPoller;

	l_eventPoller.onEvent<SDL_QUIT>([&](const SDL_Event& a_event)
	{
		l_end = true;
	});

	l_eventPoller.onEvent<SDL_MOUSEBUTTONDOWN>([&](const SDL_MouseButtonEvent& a_event)
	{
		l_xTarget = a_event.x;
		l_yTarget = a_event.y;
	});

	l_eventPoller.onEvent<SDL_MOUSEMOTION>([&](const SDL_MouseButtonEvent& a_event)
	{
		l_xTarget = a_event.x;
		l_yTarget = a_event.y;
	});

	auto l_lastFlipTimepoint = std::chrono::system_clock::now();
	auto l_prev = std::chrono::system_clock::now();

	SDL_Event l_event = { 0 };
	while (!l_end)
	{
		l_eventPoller.pollEvents();

		if (l_rectAsterix.x != l_xTarget)
		{
			l_xAsterix += (l_xTarget - l_rectAsterix.x) / 400.0;
			l_rectAsterix.x = static_cast<Sint16>(l_xAsterix);
		}

		if (l_rectAsterix.y != l_yTarget)
		{
			l_yAsterix += (l_yTarget - l_rectAsterix.y) / 400.0;
			l_rectAsterix.y = static_cast<Sint16>(l_yAsterix);
		}

		sdlcpp::fillRect(l_backbuffer, &l_rectVillage, { 0, 0, 0 });
		sdlcpp::blitSurface(l_asterix, nullptr, l_backbuffer, &l_rectAsterix);

		auto l_now = std::chrono::system_clock::now();
		if (l_now - l_prev > std::chrono::milliseconds(100))
		{
			l_asterixSprite.next();
			l_prev = l_now;
		}
		l_asterixSprite.blit(l_backbuffer, l_xTarget, l_yTarget);
		sdlcpp::drawPixel(l_backbuffer, l_xTarget - 5, l_yTarget - 5, { 255, 0, 0 });

		sdlcpp::flip(l_backbuffer);

		auto l_flipTimepoint = std::chrono::system_clock::now();
		auto l_frameDuration = l_flipTimepoint - l_lastFlipTimepoint;
		if (l_frameDuration < std::chrono::milliseconds(1000 / 30))
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30) - l_frameDuration);
	}

	return 0;
}
