#include "engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "item.h"
#include "enemy.h"
#include "player.h"
#include "weapon.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <iostream>
#include <time.h>
#include <sstream>

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);
SDL_Texture* LoadSprite(const char* path);

#define WW 1600
#define WH 1200

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
SDL_Texture* Battex;
SDL_Texture* Gobtex;
Item item[2];
Weapon sword = { NULL, 4 };
Weapon batbgone = { NULL, 4 };
Weapon flail = { NULL, 6 };
SDL_Point mouse;
SDL_Rect tran{ 0, 0, WW, 0};
Player player = { NULL, {WW / 2, (WH / 6) * 3, 64, 128}};
Enemy enemy[15];
bool scrolled = false;
float timeleft = 10;
int timeleftint = 10;
int money = 10000;
float  lastspawn = 40;
int shopitem;
float sec = 0;
double inputcool = 0.3;
std::stringstream moneytxt, pricetxt, item0txt, swdtxt, bbgtxt, timetxt, item1txt, flltxt;
//=============================================================================
void itemLoad()
{
	item[0] = {
		NULL,
		"Holy Handgrenade",
		"Kills everything on screen (Especially rabbits)",
		 (Uint32)rand() % 20 + 80
	};
	item[1] = {
		NULL,
		"Bandage",
		"A regular bandage, heals 5 hp",
		(Uint32)rand() % 20 + 30
	};
}
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
	Battex = LoadSprite("assets/enemys/bat.png");
	Gobtex = LoadSprite("assets/enemys/goblin.png");
	item[0].icon = LoadSprite("assets/items/hhg.png");
	item[1].icon = LoadSprite("assets/items/bnd.png");
	sword.icon = LoadSprite("assets/items/swd.png");
	batbgone.icon = LoadSprite("assets/items/bbg.png");
	flail.icon = LoadSprite("assets/items/flail.png");
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
	if (Battex)
		SDL_DestroyTexture(Battex);
	if (Gobtex)
		SDL_DestroyTexture(Gobtex);
	if (item[0].icon)
		SDL_DestroyTexture(item[0].icon);
	if (item[1].icon)
		SDL_DestroyTexture(item[1].icon);
	if (sword.icon)
		SDL_DestroyTexture(sword.icon);
	if (batbgone.icon)
		SDL_DestroyTexture(batbgone.icon);
	if (flail.icon)
		SDL_DestroyTexture(flail.icon);
}


int main(int argc, char* argv[])
{
	srand(time(NULL));
	shopitem = rand() % 2;
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("The Game", WW, WH))
	{
		return 1;
	}

	itemLoad();
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

void enemySpawn()
{
	int etype = rand() % 2;
	int dir = rand() % 2;
	for (int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
	{
		if (!enemy[i].alive && lastspawn >= 30)
		{
			switch (dir)
			{
			case 0:
				enemy[i].direction = Left;
				break;
			case 1:
				enemy[i].direction = Right;
				break;
			}
			enemy[i].type = (type)etype;
			if (enemy[i].type == Bat)
			{
				enemy[i].texture = Battex;
				switch (enemy[i].direction)
				{
				case(Left):
				{
					enemy[i].body.w = 100;
					enemy[i].body.h = 100;
					enemy[i].alive = true;
					enemy[i].body.x = WW - enemy[i].body.w;
					enemy[i].body.y = 0 + WH / 4;
					break;
				}
				case(Right):
				{
					enemy[i].body.w = 100;
					enemy[i].body.h = 100;
					enemy[i].alive = true;
					enemy[i].body.x = 0;
					enemy[i].body.y = 0 + WH / 4;
					break;
				}
				}
			}
			else if (enemy[i].type == Goblin)
			{
				enemy[i].texture = Gobtex;
				switch (enemy[i].direction)
				{
				case(Left):
				{
					enemy[i].body.w = 100;
					enemy[i].body.h = 100;
					enemy[i].alive = true;
					enemy[i].body.x = WW - enemy[i].body.w;
					enemy[i].body.y = 0 + WH / 2;
					break;
				}
				case(Right):
				{
					enemy[i].body.w = 100;
					enemy[i].body.h = 100;
					enemy[i].alive = true;
					enemy[i].body.x = 0;
					enemy[i].body.y = 0 + WH / 2;
					break;
				}
				}
			}
			enemy[i].speed = 200;
			lastspawn = 0;
			break;
		}
	}
	lastspawn++;
}


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
	SDL_Rect start = { 64, 64, 200, 50 };
	if ((SDL_PointInRect(&mouse, &start)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		curstate = Shop;
	}
}
void shopLoop(float dt)
{
	if (sec >= 1)
	{
		if (sword.cooldown > 0)
		{
			sword.cooldown--;
		}
		if (batbgone.cooldown > 0)
		{
			batbgone.cooldown--;
		}
		if (flail.cooldown > 0)
		{
			flail.cooldown--;
		}
		sec = 0;
	}
	else
	{
		sec += dt;
	}
	int lastitem = shopitem;
	if (IsKeyDown(SDL_SCANCODE_SPACE) && money > item[shopitem].price + 1 && inputcool <= 0)
	{
		inputcool = 0.2;
		money -= item[shopitem].price;
		item[shopitem].ammount++;
		switch (shopitem)
		{
		case(0):
		{
			item[shopitem].price = rand() % 20 + 80;
			break;
		}
		case(1):
		{
			item[shopitem].price = rand() % 20 + 30;
			break;
		}
		}
		shopitem = rand() % 2;
	}
	if (IsKeyDown(SDL_SCANCODE_LALT) && item[shopitem].price > 0 && inputcool <= 0)
	{
		inputcool = 0.2;
		int discount = rand() % 20 + 1;
		if (discount == 0 || item[shopitem].price <= discount)
		{
			return;
		}
		else
		{
			item[shopitem].price -= discount;
		}
	}
	if (IsKeyDown(SDL_SCANCODE_LSHIFT) && inputcool <= 0)
	{ 
		inputcool = 0.2;
		shopitem = rand() % 2;
	}
	timeleft -= dt;
	timeleftint = timeleft + 1;
	inputcool -= dt;
	if (inputcool < 0)
	{
		inputcool == 0;
	}
	if (timeleft <= 0)
	{
		curstate = Dungeon;
		timeleft = 10;
	}
}
void dungeonLoop(float dt)
{
	if (sec >= 1)
	{
		if (sword.cooldown > 0)
		{
			sword.cooldown--;
		}
		if (batbgone.cooldown > 0)
		{
			batbgone.cooldown--;
		}
		if (flail.cooldown > 0)
		{
			flail.cooldown--;
		}
		sec = 0;
	}
	else
	{
		sec += dt;
	}
	enemySpawn();
	for (int i = 0; i < 15; i++)
	{
		if (enemy[i].alive)
		{
			if (enemy[i].body.x > WW || enemy[i].body.x < -enemy[i].body.w || enemy[i].body.y > player.body.y + player.body.h)
			{
				enemy[i].alive = false;
				break;
			}
			switch (enemy[i].direction)
			{
			case(Left):
			{
				enemy[i].body.x -= (int)(enemy[i].speed * dt + 0.5f);
				if (enemy[i].type == Bat && player.body.x + player.body.w + 120 >= enemy[i].body.x)
				{
					enemy[i].body.y += (int)(enemy[i].speed * dt + 0.5f);
				}
				break;
			}
			case(Right):
			{

				enemy[i].body.x += (int)(enemy[i].speed * dt + 0.5f);
				if (enemy[i].type == Bat && player.body.x - enemy[i].body.w - 120 <= enemy[i].body.x)
				{
					enemy[i].body.y += (int)(enemy[i].speed * dt + 0.5f);
				}
				break;
			}
			}
		}
	}
	if (IsKeyDown(SDL_SCANCODE_Q) && item[0].ammount > 0 && inputcool <= 0)
	{
		inputcool = 0.2;
		item[0].ammount--;
		for (int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			enemy[i].alive = false;
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_W) && sword.cooldown <= 0 && inputcool <= 0)
	{
		inputcool = 0.2;
		sword.cooldown = 3;
		for (int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (enemy[i].type == Goblin)
			{
				enemy[i].alive = false;
			}
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_E) && batbgone.cooldown <= 0 && inputcool <= 0)
	{
		inputcool = 0.2;
		batbgone.cooldown = 3;
		for (int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (enemy[i].type == Bat)
			{
				enemy[i].alive = false;
			}
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_R) && flail.cooldown <= 0 && inputcool <= 0)
	{
		inputcool = 0.2;
		SDL_Rect flailbox = { player.body.x - 400, 0, 400 + player.body.w + 400, WH };
		flail.cooldown = 4;
		for (int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (SDL_HasIntersection(&flailbox, &enemy[i].body))
			{
				enemy[i].alive = false;
			}
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_SPACE) && item[1].ammount > 0 && inputcool <= 0)
	{
		inputcool = 0.2;
		item[1].ammount--;
		player.hp += 5;
	}
	
	timeleft -= dt;
	timeleftint = timeleft + 1;
	inputcool -= dt;
	if (timeleft <= 0)
	{
		curstate = Shop;
		timeleft = 10;

	}

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
		shopLoop(dt);
		break;
	}
	case Dungeon:
	{
		dungeonLoop(dt);
		break;
	}
	}

	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();
}

void menuRen()
{
	SDL_Color text = { 0, 0, 0 };
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect bb = { 0, WH - 128, WW, 128 };
	SDL_Rect Start = { 64, 64, 150, 50 };
	SDL_Rect Options = { Start.x, Start.y + 64, 150, 50 };

	SDL_Surface* Startbtn = TTF_RenderText_Solid(SegoeHeader, "Start", text);
	SDL_Surface* Optionsbtn = TTF_RenderText_Solid(SegoeHeader, "Options", text);

	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(gRenderer, &bg);

	SDL_RenderCopy(gRenderer, SDL_CreateTextureFromSurface(gRenderer, Startbtn), NULL, &Start);
	SDL_RenderCopy(gRenderer, SDL_CreateTextureFromSurface(gRenderer, Optionsbtn), NULL, &Options);

	SDL_FreeSurface(Startbtn);
	SDL_FreeSurface(Optionsbtn);

}

void shopRen(SDL_Rect bb, SDL_Rect tb)
{
	SDL_Color text = { 0, 0, 0 };
	SDL_Color wtext = { 255, 255, 255 };
	SDL_Rect bg = { 0, 0, WW, WH };
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
	SDL_Rect Price = { Shopmenu.x + Shopmenu.w - 32 , Shopmenu.y + Shopmenu.h - 32, 32, 32 };
	SDL_Surface* header;
	SDL_Surface* description;
	SDL_Surface* pmoney;
	SDL_Surface* price;
	moneytxt << money;
	pricetxt << item[shopitem].price;
	SDL_RenderCopy(gRenderer, Shopbg, NULL, &bg);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &bb);
	SDL_RenderFillRect(gRenderer, &tb);
	header = TTF_RenderText_Solid(SegoeHeader, item[shopitem].headerstr, text);
	description = TTF_RenderText_Solid(Segoep, item[shopitem].descriptionstr, text);
	pmoney = TTF_RenderText_Solid(Segoep, moneytxt.str().c_str(), text);
	price = TTF_RenderText_Solid(Segoep, pricetxt.str().c_str(), text);
	SDL_Texture* headertxt;
	headertxt = SDL_CreateTextureFromSurface(gRenderer, header);
	SDL_Texture* descriptiontxt;
	descriptiontxt = SDL_CreateTextureFromSurface(gRenderer, description);
	SDL_Texture* pmoneytxt;
	pmoneytxt = SDL_CreateTextureFromSurface(gRenderer, pmoney);
	SDL_Texture* pricetext;
	pricetext = SDL_CreateTextureFromSurface(gRenderer, price);
	
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
	SDL_RenderCopy(gRenderer, item[shopitem].icon, NULL, &Item);
	SDL_RenderCopy(gRenderer, headertxt, NULL, &Header);
	SDL_RenderCopy(gRenderer, descriptiontxt, NULL, &Description);
	SDL_RenderCopy(gRenderer, pmoneytxt, NULL, &PMoney);
	SDL_RenderCopy(gRenderer, pricetext, NULL, &Price);
	SDL_DestroyTexture(headertxt);
	SDL_FreeSurface(header);
	SDL_DestroyTexture(descriptiontxt);
	SDL_FreeSurface(description);
	SDL_DestroyTexture(pmoneytxt);
	SDL_FreeSurface(pmoney);
	SDL_DestroyTexture(pricetext);
	SDL_FreeSurface(price);
}

void enemyRen(float dt)
{

	for (int i = 0; i < 15; i++)
	{
		if (enemy[i].alive)
		{
			switch (enemy[i].direction)
			{
			case(Left):
			{
				SDL_RenderCopy(gRenderer, enemy[i].texture, NULL, &enemy[i].body);
				break;
			}
			case(Right):
			{
				SDL_RenderCopyEx(gRenderer, enemy[i].texture, NULL, &enemy[i].body, 0, 0, SDL_FLIP_HORIZONTAL);
				break;
			}
			}
		}
	}
}

void dungeonRen(float interpolation, SDL_Rect bb, SDL_Rect tb)
{
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
	SDL_RenderFillRect(gRenderer, &bg);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	
	
	SDL_RenderFillRect(gRenderer, &player.body);
	enemyRen(interpolation);
	SDL_RenderFillRect(gRenderer, &bb);
	SDL_RenderFillRect(gRenderer, &tb);
}

void uiRen(SDL_Rect bb, SDL_Rect tb)
{
	SDL_Color text = { 0, 0, 0 };
	SDL_Color wtext = { 255, 255, 255 };
	SDL_Rect Item0ic = { bb.x, bb.y, 60, 60 };
	SDL_Rect Item0amm = { Item0ic.x + 10, Item0ic.y + 55, 40, 60 };
	SDL_Rect Swdic = { bb.x + bb.w - 100, bb.y, 60, 60 };
	SDL_Rect Swdcld = { Swdic.x + 10, Swdic.y + 55, 40, 60 };
	SDL_Rect Bbgic = { Swdic.x - 100, bb.y, 60, 60 };
	SDL_Rect Bbgcld = { Bbgic.x + 10, Bbgic.y + 55, 40, 60 };
	SDL_Rect Timeleft = { WW / 2 - 30, -10, 60, 100 };
	SDL_Rect Item1ic = { Item0ic.x + 100, Item0ic.y, 60, 60 };
	SDL_Rect Item1amm = { Item1ic.x + 10, Item1ic.y + 55, 40, 60 };
	SDL_Rect Fllic = { Bbgic.x - 100, bb.y - 16, 60, 60 };
	SDL_Rect Fllcld = { Fllic.x + 10, Fllic.y + 55, 40, 60 };

	SDL_Surface* item0;
	SDL_Surface* swdcld;
	SDL_Surface* bbgcld;
	SDL_Surface* fllcld;
	SDL_Surface* time;
	SDL_Surface* item1;

	item0txt << item[0].ammount;
	swdtxt << sword.cooldown;
	bbgtxt << batbgone.cooldown;
	flltxt << flail.cooldown;
	timetxt << timeleftint;
	item1txt << item[1].ammount;

	item0 = TTF_RenderText_Solid(SegoeHeader, item0txt.str().c_str(), wtext);
	swdcld = TTF_RenderText_Solid(SegoeHeader, swdtxt.str().c_str(), wtext);
	bbgcld = TTF_RenderText_Solid(SegoeHeader, bbgtxt.str().c_str(), wtext);
	time = TTF_RenderText_Solid(SegoeHeader, timetxt.str().c_str(), wtext);
	item1 = TTF_RenderText_Solid(SegoeHeader, item1txt.str().c_str(), wtext);
	fllcld  = TTF_RenderText_Solid(SegoeHeader, flltxt.str().c_str(), wtext);
	SDL_Texture* item0text;
	item0text = SDL_CreateTextureFromSurface(gRenderer, item0);
	SDL_Texture* swdtext;
	swdtext = SDL_CreateTextureFromSurface(gRenderer, swdcld);
	SDL_Texture* bbgtext;
	bbgtext = SDL_CreateTextureFromSurface(gRenderer, bbgcld);
	SDL_Texture* timetext;
	timetext = SDL_CreateTextureFromSurface(gRenderer, time);
	SDL_Texture* item1text;
	item1text = SDL_CreateTextureFromSurface(gRenderer, item1);
	SDL_Texture* flltext;
	flltext = SDL_CreateTextureFromSurface(gRenderer, fllcld);

	SDL_RenderCopy(gRenderer, item[0].icon, NULL, &Item0ic);
	SDL_RenderCopy(gRenderer, item0text, NULL, &Item0amm);
	SDL_RenderCopy(gRenderer, sword.icon, NULL, &Swdic);
	SDL_RenderCopy(gRenderer, swdtext, NULL, &Swdcld);
	SDL_RenderCopy(gRenderer, batbgone.icon, NULL, &Bbgic);
	SDL_RenderCopy(gRenderer, bbgtext, NULL, &Bbgcld);
	SDL_RenderCopy(gRenderer, timetext, NULL, &Timeleft);
	SDL_RenderCopy(gRenderer, item[1].icon, NULL, &Item1ic);
	SDL_RenderCopy(gRenderer, item1text, NULL, &Item1amm);
	SDL_RenderCopy(gRenderer, flail.icon, NULL, &Fllic);
	SDL_RenderCopy(gRenderer, flltext, NULL, &Fllcld);


	SDL_DestroyTexture(item0text);
	SDL_FreeSurface(item0);
	SDL_DestroyTexture(swdtext);
	SDL_FreeSurface(swdcld);
	SDL_DestroyTexture(bbgtext);
	SDL_FreeSurface(bbgcld);
	SDL_DestroyTexture(timetext);
	SDL_FreeSurface(time);
	SDL_DestroyTexture(item1text);
	SDL_FreeSurface(item1); 
	SDL_DestroyTexture(flltext);
	SDL_FreeSurface(fllcld);

	moneytxt.str(std::string());
	pricetxt.str(std::string());
	item0txt.str(std::string());
	swdtxt.str(std::string());
	bbgtxt.str(std::string());
	timetxt.str(std::string());
	item1txt.str(std::string());
	flltxt.str(std::string());
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
		SDL_Rect bb = { 0, WH - 256, WW, 256 };
		SDL_Rect tb = { 0, 0, WW, 128 };
		dungeonRen(interpolation, bb, tb);
		uiRen(bb, tb);
		break;
	}
	case Shop:
	{
		SDL_Rect bb = { 0, WH - 256, WW, 256 };
		SDL_Rect tb = { 0, 0, WW, 128 };
		shopRen(bb ,tb);
		uiRen(bb, tb);
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