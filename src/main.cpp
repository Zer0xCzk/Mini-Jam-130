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

int WW = 1280;
int WH = 720;
double scale = 1;
#define price1 (Uint32)rand() % 20 + 80
#define price2 (Uint32)rand() % 20 + 30
#define cd1 4
#define cd2 6

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);
SDL_Texture* LoadSprite(const char* path);

Mix_Chunk* swd = NULL;
Mix_Chunk* bbg = NULL;
Mix_Chunk* hhg = NULL;
Mix_Chunk* fll = NULL;
Mix_Chunk* bnd = NULL;
Mix_Chunk* shp = NULL;
Mix_Chunk* fgt = NULL;
TTF_Font* SegoeHeader;
TTF_Font* Segoep;
SDL_Texture* Shopbg;
SDL_Texture* Shopbuy;
SDL_Texture* Shopbuyi;
SDL_Texture* Shopdisc;
SDL_Texture* Shopdisci;
SDL_Texture* Shophag;
SDL_Texture* Shophagi;
SDL_Texture* Battex;
SDL_Texture* Gobtex;
SDL_Texture* Heart;
SDL_Texture* Menubg;
SDL_Texture* Dungeonbg;
SDL_Texture* tuttxt;
SDL_Texture* pback;
SDL_Texture* qbtn;
SDL_Texture* wbtn;
SDL_Texture* ebtn;
SDL_Texture* rbtn;
SDL_Texture* tbtn;
Item item[2];
Weapon sword = { NULL, cd1 };
Weapon batbgone = { NULL, cd1 };
Weapon flail = { NULL, 6 };
SDL_Point mouse;
SDL_Rect tran{ 0, 0, WW, 0};
Player player = { NULL, { (int)(WW / 2 - (75 * scale) / 2), (WH / 6) * 3, (int)(75 * scale), (int)(150 * scale) }, 20 };
Enemy enemy[15];
bool scrolled = false;
float timeleft = 10;
int timeleftint = 10;
int money = 100;
double  lastspawn = 40;
double spawnrate = 1;
int shopitem;
int red = 0;
int score = 0;
float sec = 0;
double inputcool = 0.3;
double enspeed = 200;
std::stringstream moneytxt, pricetxt, item0txt, swdtxt, bbgtxt, timetxt, item1txt, flltxt, hammtxt, scoretxt;
//=============================================================================
void itemLoad()
{
	item[0] = {
		NULL,
		"Holy Handgrenade",
		"Kills everything on screen (Especially rabbits)",
		price1,
		0
	};
	item[1] = {
		NULL,
		"Bandage",
		"A regular bandage, heals 2 hp",
		price2,
		0
	};
}
void textureLoad()
{
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
	Heart = LoadSprite("assets/items/heart.png");
	Menubg = LoadSprite("assets/menus/bg.png");
	Dungeonbg = LoadSprite("assets/enemys/bg.png");
	tuttxt = LoadSprite("assets/menus/ttext.png");
	pback = LoadSprite("assets/menus/pback.png");
	qbtn = LoadSprite("assets/menus/qbtn.png");
	wbtn = LoadSprite("assets/menus/wbtn.png");
	ebtn = LoadSprite("assets/menus/ebtn.png");
	rbtn = LoadSprite("assets/menus/rbtn.png");
	tbtn = LoadSprite("assets/menus/tbtn.png");
}
void textureDestroy()
{
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
	if (Heart)
		SDL_DestroyTexture(Heart);
	if (Menubg)
		SDL_DestroyTexture(Menubg);
	if (Dungeonbg)
		SDL_DestroyTexture(Dungeonbg);
	if (tuttxt)
		SDL_DestroyTexture(tuttxt);
	if (qbtn)
		SDL_DestroyTexture(qbtn);
	if (wbtn)
		SDL_DestroyTexture(wbtn);
	if (ebtn)
		SDL_DestroyTexture(ebtn);
	if (rbtn)
		SDL_DestroyTexture(rbtn);
	if (tbtn)
		SDL_DestroyTexture(tbtn);
}


int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));
	shopitem = rand() % 2;
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("Moon's madness", WW, WH))
	{
		return 1;
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Volume(-1, 32);

	itemLoad();
	textureLoad();
	SegoeHeader = TTF_OpenFont("assets/font/SEGOEUI.TTF", 20);
	Segoep = TTF_OpenFont("assets/font/SEGOEUI.TTF", 12);
	swd = Mix_LoadWAV("assets/sfx/swd.wav");
	bbg = Mix_LoadWAV("assets/sfx/bbg.wav");
	fll = Mix_LoadWAV("assets/sfx/fll.wav");
	hhg = Mix_LoadWAV("assets/sfx/hhg.wav");
	bnd = Mix_LoadWAV("assets/sfx/bnd.wav");
	shp = Mix_LoadWAV("assets/sfx/shp.wav");
	fgt = Mix_LoadWAV("assets/sfx/fgt.wav");

	StartLoop(Update, RenderFrame);

	textureDestroy();
	TTF_CloseFont(SegoeHeader);
	TTF_CloseFont(Segoep);
	Mix_FreeChunk(bbg);
	Mix_FreeChunk(fll);
	Mix_FreeChunk(swd);
	Mix_FreeChunk(hhg);
	Mix_FreeChunk(bnd);
	Mix_FreeChunk(shp);
	Mix_FreeChunk(fgt);

	DeinitSDL();
	return 0;
}

//=============================================================================
enum state { Dungeon, Menu, Shop, Options, Tutorial, GameOver };
state curstate = Menu;
state laststate = Menu;
state renstate = Menu;

void enemySpawn()
{
	int etype = rand() % 2;
	int dir = rand() % 2;
	for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
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
			enemy[i].type = (t)etype;
			if (enemy[i].type == Bat)
			{
				enemy[i].texture = Battex;
				switch (enemy[i].direction)
				{
				case(Left):
				{
					enemy[i].body.w = (int)(100 * scale);
					enemy[i].body.h = (int)(50 * scale);
					enemy[i].alive = true;
					enemy[i].body.x = WW - enemy[i].body.w;
					enemy[i].body.y = 0 + WH / 3;
					break;
				}
				case(Right):
				{
					enemy[i].body.w = (int)(100 * scale);
					enemy[i].body.h = (int)(50 * scale);
					enemy[i].alive = true;
					enemy[i].body.x = 0;
					enemy[i].body.y = 0 + WH / 3;
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
					enemy[i].body.w = (int)(150 * scale);
					enemy[i].body.h = (int)(150 * scale);
					enemy[i].alive = true;
					enemy[i].body.x = (WW - enemy[i].body.w);
					enemy[i].body.y = 0 + WH / 2;
					break;
				}
				case(Right):
				{
					enemy[i].body.w = (int)(150 * scale);
					enemy[i].body.h = (int)(150 * scale);
					enemy[i].alive = true;
					enemy[i].body.x = 0;
					enemy[i].body.y = 0 + WH / 2;
					break;
				}
				}
			}
			enemy[i].speed = enspeed;
			lastspawn = 0;
			break;
		}
	}
	lastspawn += spawnrate;
}


void Transition()
{
	if (curstate == Tutorial || curstate == Options || curstate == Menu)
	{
		laststate = curstate;
		renstate = curstate;
		return;
	}
	if (tran.h >= WH && scrolled == false)
	{
		scrolled = true;
	}
	
	if (tran.h < WH && scrolled == false)
	{
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &tran);
		tran.h += (int)(10.0 * scale);
	}
	else if (tran.h > 0 && scrolled == true)
	{
		renstate = curstate;
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &tran);
		tran.h -= (int)(10.0 * scale);
	}
	else
	{
		scrolled = false;
		laststate = curstate;
	}
}

void menuLoop()
{
	SDL_Rect start = { 64, 128, 150, 75 };
	SDL_Rect options = { start.x, start.y + 64, 150, 75 };
	if ((SDL_PointInRect(&mouse, &start)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		curstate = Tutorial;
	}
	if ((SDL_PointInRect(&mouse, &options)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		curstate = Options;
	}
}

void optionsLoop()
{
	SDL_Rect qhd = { 100, 175, 200, 50 };
	SDL_Rect fhd = { 100, 250, 200, 50 };
	SDL_Rect hd = { 100, 325, 200, 50 };
	SDL_Rect back = { 100, WH / 4 * 3 - 64, 150, 50 };
	if ((SDL_PointInRect(&mouse, &qhd)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		WW = 2560;
		WH = 1440;	
		scale = 2;
	}
	if ((SDL_PointInRect(&mouse, &fhd)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		WW = 1920;
		WH = 1080;
		scale = 1.5;
	}
	if ((SDL_PointInRect(&mouse, &hd)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		WW = 1280;
		WH = 720;
		scale = 1;
	}
	if ((SDL_PointInRect(&mouse, &back)) && IsMousePressed(SDL_BUTTON_LMASK))
	{
		curstate = Menu;
	}
	SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowSize(gWindow, WW, WH);
	tran.w = WW;
	player.body.w = (int)(75 * scale);
	player.body.h = (int)(150 * scale);
	player.body.x = WW / 2 - player.body.w / 2;
	player.body.y = (WH / 6) * 3;
	for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
	{
		enemy[i].body.w = (int)(150 * scale);
		enemy[i].body.h = (int)(150 * scale);
	}
}

void shopLoop(float dt)
{
	red++;
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
	if (IsKeyDown(SDL_SCANCODE_SPACE) && (unsigned int)money > item[shopitem].price + 1 && inputcool <= 0)
	{
		inputcool = 0.2;
		money -= item[shopitem].price;
		item[shopitem].ammount++;
		switch (shopitem)
		{
		case(0):
		{
			item[shopitem].price = price1;
			break;
		}
		case(1):
		{
			item[shopitem].price = price2;
			break;
		}
		}
		shopitem = rand() % 2;
	}
	if (IsKeyDown(SDL_SCANCODE_LALT) && inputcool <= 0)
	{
		inputcool = 0.2;
		int discount = rand() % 20 + 1;
		switch (shopitem)
		{
		case(0):
		{
			if (item[shopitem].price - discount < 50)
			{
				return;
			}
		}
		case(1):
		{
			if (item[shopitem].price - discount < 30)
			{
				return;
			}
		}
		}
		if (discount == 0 || item[shopitem].price <= (unsigned int)discount)
		{
			return;
		}
		else
		{
			item[shopitem].price -= discount;
		}
	}
	if (IsKeyPressed(SDL_SCANCODE_LSHIFT) && inputcool <= 0)
	{ 
		inputcool = 0.2;
		shopitem = rand() % 2;
		switch (shopitem)
		{
		case(0):
		{
			item[shopitem].price = price1;
			break;
		}
		case(1):
		{
			item[shopitem].price = price2;
			break;
		}
		}
	}
	if (inputcool > 0)
	{
		inputcool -= dt;
	}
	timeleft -= dt;
	timeleftint = (int)(timeleft + 1);
	if (timeleft <= 0)
	{
		Mix_PlayChannel(-1, fgt, 0);
		curstate = Dungeon;
		timeleft = 10;
	}
}

void dungeonLoop(float dt)
{
	if (player.hp <= 0)
	{
		curstate = GameOver;
		Mix_CloseAudio();
	}
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
			if (SDL_HasIntersection(&player.body, &enemy[i].body))
			{
				enemy[i].alive = false;
				player.hp--;
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
		Mix_PlayChannel(-1, hhg, 0);
		Mix_GetError();
		inputcool = 0.2;
		item[0].ammount--;
		for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (enemy[i].alive)
			{
				enemy[i].alive = false;
				score++;
			}
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_W) && item[1].ammount > 0 && inputcool <= 0)
	{
		Mix_PlayChannel(-1, bnd, 0);
		inputcool = 0.2;
		item[1].ammount--;
		player.hp += 2;
	}
	else if (IsKeyDown(SDL_SCANCODE_E) && sword.cooldown <= 0 && inputcool <= 0)
	{
		Mix_PlayChannel(-1, swd, 0);
		inputcool = 0.2;
		sword.cooldown = cd1;
		for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (enemy[i].type == Goblin && enemy[i].alive)
			{
				money++;
				score++;
				enemy[i].alive = false;
			}
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_R) && batbgone.cooldown <= 0 && inputcool <= 0)
	{
		Mix_PlayChannel(-1, bbg, 0);
		inputcool = 0.2;
		batbgone.cooldown = cd1;
		for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (enemy[i].type == Bat && enemy[i].alive)
			{
				money++;
				score++;
				enemy[i].alive = false;
			}
		}
	}
	else if (IsKeyDown(SDL_SCANCODE_T) && flail.cooldown <= 0 && inputcool <= 0)
	{
		Mix_PlayChannel(-1, fll, 0);
		inputcool = 0.2;
		SDL_Rect flailbox = { player.body.x - 400, 0, 400 + player.body.w + 400, WH };
		flail.cooldown = cd2;
		for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (SDL_HasIntersection(&flailbox, &enemy[i].body) && enemy[i].alive)
			{
				money++;
				score++;
				enemy[i].alive = false;
			}
		}
	}
	
	timeleft -= dt;
	timeleftint = (int)(timeleft + 1);
	inputcool -= dt;
	if (timeleft <= 0)
	{
		Mix_PlayChannel(-1, shp, 0);
		red = 0;
		curstate = Shop;
		timeleft = 10;
		enspeed += 10 * scale;
		spawnrate += spawnrate + 0.1;
		for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Enemy); i++)
		{
			if (enemy[i].alive)
			{
				enemy[i].alive = false;
			}
		}
	}

}

void tutorialLoop()
{
	if (IsKeyDown(SDL_SCANCODE_SPACE))
	{
		curstate = Shop;
		Mix_PlayChannel(-1, shp, 0);
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
	case Options:
	{
		optionsLoop();
		break;
	}
	case Tutorial:
	{
		tutorialLoop();
		break;
	}
	default:
	{
		break;
	}
	}
	

	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();
}

void menuRen()
{
	SDL_Color text = { 255, 255, 255, 255 };
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect Start = { 64, 128, 150, 75 };
	SDL_Rect Options = { Start.x, Start.y + 64, 150, 75 };
	SDL_Rect name = { 64, 0, 300, 100 };

	SDL_Surface* Startbtn = TTF_RenderText_Solid(SegoeHeader, "Start", text);
	SDL_Surface* Optionsbtn = TTF_RenderText_Solid(SegoeHeader, "Options", text);
	SDL_Surface* namesrf = TTF_RenderText_Solid(SegoeHeader, "Moon's madness", text);

	SDL_RenderCopy(gRenderer, Menubg, NULL, &bg);

	SDL_Texture* starttext = SDL_CreateTextureFromSurface(gRenderer, Startbtn);
	SDL_Texture* optionstext = SDL_CreateTextureFromSurface(gRenderer, Optionsbtn);
	SDL_Texture* nametext = SDL_CreateTextureFromSurface(gRenderer, namesrf);

	SDL_RenderCopy(gRenderer, starttext, NULL, &Start);
	SDL_RenderCopy(gRenderer,  optionstext, NULL, &Options);
	SDL_RenderCopy(gRenderer, nametext, NULL, &name);

	SDL_FreeSurface(Startbtn);
	SDL_FreeSurface(Optionsbtn);
	SDL_FreeSurface(namesrf);

	SDL_DestroyTexture(optionstext);
	SDL_DestroyTexture(starttext);
	SDL_DestroyTexture(nametext);
}

void varChange(std::stringstream x, SDL_Texture* textName, SDL_Surface)
{

}

void optionsRen()
{
	SDL_Color text = { 255, 255, 255, 255 };
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect halfbg = { 64, 64, WW / 2 - halfbg.x, WH / 4 * 3 - halfbg.y };
	SDL_Rect resmenu = { 100, 100, 300, 300 };
	SDL_Rect resolution = { 100, 100, 250, 75 };
	SDL_Rect qhd = { 100, 175, 200, 50 };
	SDL_Rect fhd = { 100, 250, 200, 50 };
	SDL_Rect hd = { 100, 325, 200, 50 };
	SDL_Rect back = { 100, WH / 4 * 3 - 64, 150, 50 };

	SDL_Surface* resbtn = TTF_RenderText_Solid(SegoeHeader, "Resolution", text);
	SDL_Surface* qhdbtn = TTF_RenderText_Solid(SegoeHeader, "2560x1440", text);
	SDL_Surface* fhdbtn = TTF_RenderText_Solid(SegoeHeader, "1920x1080", text);
	SDL_Surface* hdbtn = TTF_RenderText_Solid(SegoeHeader, "1280x720", text);
	SDL_Surface* backbtn = TTF_RenderText_Solid(SegoeHeader, "Return", text);
	
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 100);
	SDL_RenderCopy(gRenderer, Menubg, NULL, &bg);
	SDL_RenderFillRect(gRenderer, &halfbg);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 50);
	SDL_RenderFillRect(gRenderer, &back);
	SDL_RenderFillRect(gRenderer, &resmenu);

	SDL_Texture* restxt = SDL_CreateTextureFromSurface(gRenderer, resbtn);
	SDL_Texture* qhdtxt = SDL_CreateTextureFromSurface(gRenderer, qhdbtn);
	SDL_Texture* fhdtxt = SDL_CreateTextureFromSurface(gRenderer, fhdbtn);
	SDL_Texture* hdtxt = SDL_CreateTextureFromSurface(gRenderer, hdbtn);
	SDL_Texture* backtxt = SDL_CreateTextureFromSurface(gRenderer, backbtn);

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderCopy(gRenderer, restxt, NULL, &resolution);
	SDL_RenderCopy(gRenderer, qhdtxt, NULL, &qhd);
	SDL_RenderCopy(gRenderer, fhdtxt, NULL, &fhd);
	SDL_RenderCopy(gRenderer, hdtxt, NULL, &hd);
	SDL_RenderCopy(gRenderer, backtxt, NULL, &back);

	SDL_FreeSurface(resbtn);
	SDL_FreeSurface(qhdbtn);
	SDL_FreeSurface(fhdbtn);
	SDL_FreeSurface(hdbtn);
	SDL_FreeSurface(backbtn);

	SDL_DestroyTexture(restxt);
	SDL_DestroyTexture(qhdtxt);
	SDL_DestroyTexture(fhdtxt);
	SDL_DestroyTexture(hdtxt);
	SDL_DestroyTexture(backtxt);
}

void shopRen(SDL_Rect bb, SDL_Rect tb)
{
	SDL_Color text = { 0, 0, 0, 255 };
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect plr = { WW / 2 - (int)(75.0 * scale) / 2, (WH / 6) * 3, (int)(75.0 * scale), (int)(150.0 * scale) };
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
	SDL_Rect Price = { Shopmenu.x + Shopmenu.w - 32 , Shopmenu.y + Shopmenu.h -32, 32, 32 };
	SDL_Rect tg = { 0, 0, WW, WH };
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
	SDL_RenderCopy(gRenderer, pback , NULL, &plr);
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

	SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, (uint8_t)(red / 25));
	SDL_RenderFillRect(gRenderer, &tg);

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
	SDL_Rect plr = { WW / 2 - (int)(75 * scale) / 2, (WH / 6) * 3, (int)(75.0 * scale), (int)(150 * scale) };
	SDL_RenderCopy(gRenderer, Dungeonbg, NULL, &bg);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	
	
	SDL_RenderCopy(gRenderer, pback, NULL, &plr);
	enemyRen(interpolation);
	SDL_RenderFillRect(gRenderer, &bb);
	SDL_RenderFillRect(gRenderer, &tb);
}

void uiRen(SDL_Rect bb, SDL_Rect tb)
{
	SDL_Color wtext = { 255, 255, 255, 255 };
	SDL_Rect Timeleft = { tb.w / 2 - 30, tb.y + 16, 60, 100 };
	SDL_Rect Hpic = { (bb.w / 2) - 50, bb.y, 100, 100 };
	SDL_Rect Hamm = { Hpic.x + Hpic.w / 2 - 20, Hpic.y + Hpic.h - 8, 40, 60 };
	SDL_Rect Swdic = { Hpic.x + Hpic.w + 16, bb.y, 60, 60 };
	SDL_Rect Swdcld = { Swdic.x + 10, Swdic.y + 55, 40, 60 };
	SDL_Rect Bbgic = { Swdic.x + 100, bb.y, 60, 60 };
	SDL_Rect Bbgcld = { Bbgic.x + 10, Bbgic.y + 55, 40, 60 };
	SDL_Rect Fllic = { Bbgic.x + 100, bb.y, 60, 60 };
	SDL_Rect Fllcld = { Fllic.x + 10, Fllic.y + 55, 40, 60 };
	SDL_Rect Item0ic = { Hpic.x - 180, bb.y, 60, 60 };
	SDL_Rect Item0amm = { Item0ic.x + 10, Item0ic.y + 55, 40, 60 };
	SDL_Rect Item1ic = { Item0ic.x + 100, Item0ic.y, 60, 60 };
	SDL_Rect Item1amm = { Item1ic.x + 10, Item1ic.y + 55, 40, 60 };
	SDL_Rect QButton = { Item0ic.x + 5, Item0amm.y + 60, 50, 50 };
	SDL_Rect WButton = { Item1ic.x + 5, Item1amm.y + 60, 50, 50 };
	SDL_Rect EButton = { Swdic.x + 5, Swdcld.y + 60, 50, 50 };
	SDL_Rect RButton = { Bbgic.x + 5, Bbgcld.y + 60, 50, 50 };
	SDL_Rect TButton = { Fllic.x + 5, Fllcld.y + 60, 50, 50 };

	SDL_Surface* item0;
	SDL_Surface* swdcld;
	SDL_Surface* bbgcld;
	SDL_Surface* fllcld;
	SDL_Surface* time;
	SDL_Surface* item1;
	SDL_Surface* hamm;

	item0txt << item[0].ammount;
	swdtxt << sword.cooldown;
	bbgtxt << batbgone.cooldown;
	flltxt << flail.cooldown;
	timetxt << timeleftint;
	item1txt << item[1].ammount;
	hammtxt << player.hp;

	item0 = TTF_RenderText_Solid(SegoeHeader, item0txt.str().c_str(), wtext);
	swdcld = TTF_RenderText_Solid(SegoeHeader, swdtxt.str().c_str(), wtext);
	bbgcld = TTF_RenderText_Solid(SegoeHeader, bbgtxt.str().c_str(), wtext);
	time = TTF_RenderText_Solid(SegoeHeader, timetxt.str().c_str(), wtext);
	item1 = TTF_RenderText_Solid(SegoeHeader, item1txt.str().c_str(), wtext);
	fllcld  = TTF_RenderText_Solid(SegoeHeader, flltxt.str().c_str(), wtext);
	hamm = TTF_RenderText_Solid(SegoeHeader, hammtxt.str().c_str(), wtext);
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
	SDL_Texture* hammtext;
	hammtext = SDL_CreateTextureFromSurface(gRenderer, hamm);

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
	SDL_RenderCopy(gRenderer, Heart, NULL, &Hpic);
	SDL_RenderCopy(gRenderer, hammtext, NULL, &Hamm);
	SDL_RenderCopy(gRenderer, qbtn, NULL, &QButton);
	SDL_RenderCopy(gRenderer, wbtn, NULL, &WButton);
	SDL_RenderCopy(gRenderer, ebtn, NULL, &EButton);
	SDL_RenderCopy(gRenderer, rbtn, NULL, &RButton);
	SDL_RenderCopy(gRenderer, tbtn, NULL, &TButton);



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
	SDL_DestroyTexture(hammtext);
	SDL_FreeSurface(hamm);

	moneytxt.str(std::string());
	pricetxt.str(std::string());
	item0txt.str(std::string());
	swdtxt.str(std::string());
	bbgtxt.str(std::string());
	timetxt.str(std::string());
	item1txt.str(std::string());
	flltxt.str(std::string());
	hammtxt.str(std::string());
}

void gameOverRen()
{
	SDL_Color text = { 0, 0, 0, 255 };
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect GOtext = { WW / 2 - 100, WH / 2 - 50, 200, 100 };
	SDL_Rect Scoretext = { GOtext.x - 25, GOtext.y + 100, 200, 100 };
	SDL_Rect Score = { Scoretext.x + Scoretext.w + 8, Scoretext.y, 50, 100 };
	SDL_Rect leave = { WW / 2 - 200, WH - 150, 400, 100 };

	scoretxt << score;
	
	SDL_Surface* GObtn = TTF_RenderText_Solid(SegoeHeader, "Game Over", text);
	SDL_Surface* scoretext = TTF_RenderText_Solid(Segoep, "Your kills: ", text);
	SDL_Surface* scoreint = TTF_RenderText_Solid(Segoep, scoretxt.str().c_str(), text);
	SDL_Surface* textsrf = TTF_RenderText_Solid(Segoep, "Press Escape to quit the game", text);


	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(gRenderer, &bg);

	SDL_Texture* gotxt = SDL_CreateTextureFromSurface(gRenderer, GObtn);
	SDL_Texture* scrtxt = SDL_CreateTextureFromSurface(gRenderer, scoretext);
	SDL_Texture* scr = SDL_CreateTextureFromSurface(gRenderer, scoreint);
	SDL_Texture* texttxt = SDL_CreateTextureFromSurface(gRenderer, textsrf);

	SDL_RenderCopy(gRenderer, gotxt, NULL, &GOtext);
	SDL_RenderCopy(gRenderer, scrtxt, NULL, &Scoretext);
	SDL_RenderCopy(gRenderer, scr, NULL, &Score);
	SDL_RenderCopy(gRenderer, texttxt, NULL, &leave);

	SDL_FreeSurface(GObtn);
	SDL_FreeSurface(scoretext);
	SDL_FreeSurface(scoreint);
	SDL_FreeSurface(textsrf);

	SDL_DestroyTexture(gotxt);
	SDL_DestroyTexture(scrtxt);
	SDL_DestroyTexture(scr);
	SDL_DestroyTexture(texttxt);

	scoretxt.str(std::string());
}

void tutorialRen()
{
	SDL_Rect bg = { 0, 0, WW, WH };
	SDL_Rect tut = { 0, 0, WW, WH };
	SDL_Rect tutbg = { WW / 5, 0, WW / 5 * 3, WH };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 100);

	SDL_RenderCopy(gRenderer, Menubg, NULL, &bg);
	SDL_RenderFillRect(gRenderer, &tutbg);
	SDL_RenderCopy(gRenderer, tuttxt, NULL, &tut);
	
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
		SDL_Rect bb = { 0, (WH - 200), WW, (int)(200.0 * scale) };
		SDL_Rect tb = { 0, 0, WW, (int)(100.0 * scale) };
		dungeonRen(interpolation, bb, tb);
		uiRen(bb, tb);
		break;
	}
	case Shop:
	{
		SDL_Rect bb = { 0, (WH - 200), WW, (int)(200.0 * scale) };
		SDL_Rect tb = { 0, 0, WW, (int)(100.0 * scale) };
		shopRen(bb ,tb);
		uiRen(bb, tb);
		break;
	}
	case Options:
	{
		optionsRen();
		break;
	}
	case GameOver:
	{
		gameOverRen();
		break;
	}
	case Tutorial:
	{
		tutorialRen();
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
