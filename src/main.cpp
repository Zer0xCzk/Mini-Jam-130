#include "engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "item.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <iostream>
#include <time.h>

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);
SDL_Texture* LoadSprite(const char* path);

#define WW 1200
#define WH 600

TTF_Font* SegoeHeader;
TTF_Font* Segoep;
SDL_Texture* Starts;
SDL_Texture* Shopbg;
SDL_Texture* Shopbuy;
SDL_Texture* Shopbuyi;
SDL_Texture* Shopdisc;
SDL_Texture* Shopdisci;
SDL_Texture* Shophag;
SDL_Texture* Shophagi;
Item HolyHandGranade = { NULL, "Holy Handgrenade", "Kills everything on screen (Especially rabbits)", 60};
SDL_Point mouse;
SDL_Rect start = { 64, 64, 165, 48 };
SDL_Rect tran{ 0, 0, WW, 0};
bool scrolled = false;
int timeleft = 600;
int money = 100;
char moneytxt[5];
char pricetxt[5];
//=============================================================================
void textureLoad()
{
	Starts = LoadSprite("assets/menus/Start.png");
	Shopbg = LoadSprite("assets/shops/bg.png");
	Shopbuy = LoadSprite("assets/shops/buy.png");
	Shopbuyi = LoadSprite("assets/shops/space.png");
	Shopdisc = LoadSprite("assets/shops/discard.png");
	Shopdisci = LoadSprite("assets/shops/shift.png");
	Shophag = LoadSprite("assets/shops/haggle.png");
	Shophagi = LoadSprite("assets/shops/alt.png");
	HolyHandGranade.icon = LoadSprite("assets/items/hhg.png");
}
void textureDestroy()
{
	if (Starts)
		SDL_DestroyTexture(Starts);
	if (Shopbg)
		SDL_DestroyTexture(Shopbg);
	if (Shopbuy)
		SDL_DestroyTexture(Shopbuy);
	if (Shopbuyi)
		SDL_DestroyTexture(Shopbuyi);
	if (Shopdisc)
		SDL_DestroyTexture(Shopdisc);
	if (Shopdisci)
		SDL_DestroyTexture(Shopdisci);
	if (Shophag)
		SDL_DestroyTexture(Shophag);
	if (Shophagi)
		SDL_DestroyTexture(Shophagi);
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	HolyHandGranade.price = rand() % 100 + 1;
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("The Game", WW, WH))
	{
		return 1;
	}

	textureLoad();
	SegoeHeader = TTF_OpenFont("assets/font/SEGOEUI.TTF", 20);
	Segoep = TTF_OpenFont("assets/font/SEGOEUI.TTF", 12);
	

	StartLoop(Update, RenderFrame);

	textureDestroy();
	TTF_CloseFont(SegoeHeader);
	TTF_CloseFont(Segoep);

	DeinitSDL();
	return 0;
}

//=============================================================================
enum state {Dungeon, Menu, Shop};
state curstate = Menu;
state laststate = Menu;
state renstate = Menu;


void Transition()
{
	/*
	if (curstate == Shop)
	{
		laststate = curstate;
		renstate = curstate;
		return;
	}
	*/
	if (tran.h >= WH && scrolled == false)
	{
		scrolled = true;
	}
	
	if (tran.h < WH && scrolled == false)
	{
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &tran);
		tran.h +=10;
	}
	else if (tran.h > 0 && scrolled == true)
	{
		renstate = curstate;
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &tran);
		tran.h -= 10;
	}
	else
	{
		scrolled = false;
		laststate = curstate;
	}
}

void menuLoop()
{
	if ((SDL_PointInRect(&mouse, &start)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		curstate = Shop;
	}
}
void shopLoop()
{
	if (IsKeyPressed(SDL_SCANCODE_SPACE) && money > HolyHandGranade.price)
	{
		money -= HolyHandGranade.price;
		HolyHandGranade.price = rand() % 100 + 1;
	}
	if (IsKeyPressed(SDL_SCANCODE_LALT) && HolyHandGranade.price > 0)
	{
		int discount = rand() % 20 + 1;
		if (discount == 0 || HolyHandGranade.price < discount)
		{
			return;
		}
		else
		{
			HolyHandGranade.price -= discount;
		}
	}
	if (IsKeyPressed(SDL_SCANCODE_LSHIFT))
	{ 
		HolyHandGranade.price = rand() % 100 + 1;
	}
	timeleft--;
	if (timeleft <= 0)
	{
		curstate = Menu;
		timeleft = 660;
	}
}
void dungeonLoop()
{
	/*
	timeleft--;
	if (timeleft <= 0)
	{
		curstate = Shop;
		timeleft = 660;
	}
	*/
}

void Update(float dt)
{
	SDL_GetMouseState(&mouse.x, &mouse.y);
	switch (curstate)
	{
	case Menu:
	{
		menuLoop();
		break;
	}
	case Shop:
	{
		shopLoop();
		break;
	}
	case Dungeon:
	{
		dungeonLoop();
		break;
	}
	}

	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();
}

void menuRen()
{
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect bb = { 0, WH - 128, WW, 128 };
	SDL_Rect start = { 64, 64, 165, 48 };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(gRenderer, &bg);

	SDL_RenderCopy(gRenderer, Starts, NULL, &start);
}

void shopRen()
{
	SDL_Color text = { 0, 0, 0 };
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect bb = { 0, WH - 128, WW, 128 };
	SDL_Rect tb = { 0, 0, WW, 64 };
	SDL_Rect player = { WW / 2, (WH / 6) * 3, 64, 128 };
	SDL_Rect Shopmenu = { WW / 2 - 256, WH / 2 - 128, 512, 256 };
	SDL_Rect ItemPedestal = { Shopmenu.x + Shopmenu.w / 2 - 50, Shopmenu.y + 16, 100, 100 };
	SDL_Rect Shopbuyico = { ItemPedestal.x - 150, ItemPedestal.y + 32, 64, 32 };
	SDL_Rect Shopbuytext = { Shopbuyico.x + 64, Shopbuyico.y, 100, 32 };
	SDL_Rect Shopdiscico = { ItemPedestal.x + ItemPedestal.w + 16, ItemPedestal.y, 64, 32 };
	SDL_Rect Shopdisctext = { Shopdiscico.x + 64, Shopdiscico.y, 100, 32 };
	SDL_Rect Shophagico = { ItemPedestal.x + ItemPedestal.w + 16, ItemPedestal.y + ItemPedestal.h - 32, 64, 32 };
	SDL_Rect Shophagtext = { Shophagico.x + 64, Shophagico.y, 100, 32 };
	SDL_Rect Item = { ItemPedestal.x, ItemPedestal.y, 100, 100 };
	SDL_Rect Header = { ItemPedestal.x - 50, ItemPedestal.y + ItemPedestal.h, 200, 32 };
	SDL_Rect Description = { Header.x - 100, Header.y + Header.h + 16, 400, 32 };
	SDL_Rect PMoney = { Shopmenu.x , Shopmenu.y + Shopmenu.h - 32, 32, 32 };
	SDL_Rect Price = { Shopmenu.x + Shopmenu.w - 32 , Shopmenu.y + Shopmenu.h - 32, 32, 32};
	SDL_Surface* header;
	SDL_Surface* description;
	SDL_Surface* pmoney;
	SDL_Surface* price;
	sprintf(moneytxt, "%8d", money);
	sprintf(pricetxt, "%8d", HolyHandGranade.price);
	SDL_RenderCopy(gRenderer, Shopbg, NULL, &bg);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &bb);
	SDL_RenderFillRect(gRenderer, &tb);
	header = TTF_RenderText_Blended(SegoeHeader, HolyHandGranade.headerstr, text);
	description = TTF_RenderText_Blended(Segoep, HolyHandGranade.descriptionstr, text);
	pmoney = TTF_RenderText_Blended(Segoep, moneytxt, text);
	price = TTF_RenderText_Blended(Segoep, pricetxt, text);
	SDL_Texture* headertxt;
	headertxt = SDL_CreateTextureFromSurface(gRenderer, header);
	SDL_Texture* descriptiontxt;
	descriptiontxt = SDL_CreateTextureFromSurface(gRenderer, description);
	SDL_Texture* pmoneytxt;
	pmoneytxt = SDL_CreateTextureFromSurface(gRenderer, pmoney);
	SDL_Texture* pricetxt;
	pricetxt = SDL_CreateTextureFromSurface(gRenderer, price);
	
	
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &player);
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 100);
	SDL_RenderFillRect(gRenderer, &Shopmenu);
	SDL_RenderCopy(gRenderer, Shopbuyi, NULL, &Shopbuyico);
	SDL_RenderCopy(gRenderer, Shopbuy, NULL, &Shopbuytext);
	SDL_RenderCopy(gRenderer, Shopdisci, NULL, &Shopdiscico);
	SDL_RenderCopy(gRenderer, Shopdisc, NULL, &Shopdisctext);
	SDL_RenderCopy(gRenderer, Shophagi, NULL, &Shophagico);
	SDL_RenderCopy(gRenderer, Shophag, NULL, &Shophagtext);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 200);
	SDL_RenderFillRect(gRenderer, &ItemPedestal);
	SDL_RenderCopy(gRenderer, HolyHandGranade.icon, NULL, &Item);
	SDL_RenderCopy(gRenderer, headertxt, NULL, &Header);
	SDL_RenderCopy(gRenderer, descriptiontxt, NULL, &Description);
	SDL_RenderCopy(gRenderer, pmoneytxt, NULL, &PMoney);
	SDL_RenderCopy(gRenderer, pricetxt, NULL, &Price);

	SDL_DestroyTexture(headertxt);
	SDL_FreeSurface(header);
	SDL_DestroyTexture(descriptiontxt);
	SDL_FreeSurface(description);
	SDL_DestroyTexture(pmoneytxt);
	SDL_FreeSurface(pmoney);
	SDL_DestroyTexture(pricetxt);
	SDL_FreeSurface(price);
}

void RenderFrame(float interpolation)
{
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
	switch (renstate)
	{
	case Menu:
	{
		menuRen();
		break;
	}
	case Dungeon:
	{
		SDL_Rect bg = { 0, 0, WW, WH };
		SDL_Rect bb = { 0, WH - 128, WW, 128 };
		SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
		SDL_RenderFillRect(gRenderer, &bg);


		break;
	}
	case Shop:
	{
		shopRen();
		break;
	}
	}
	if (laststate != curstate)
	{
		Transition();
	}
}

SDL_Texture* LoadSprite(const char* path)
{

	SDL_Texture* texture = IMG_LoadTexture(gRenderer, path);
	if (texture == NULL)
	{
		fprintf(stderr, "IMG_LoadTexture error: %s\n", IMG_GetError());
		return NULL;
	}

	return texture;
}