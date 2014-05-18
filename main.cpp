/*
- audio
- aggiungere gestione delle vite: visualizzazione numero attuale, respawn alla morte
- fare il pulsante per diventare un tile e morire
- capire perchè si ferma di lato
- aggiungere gestione delle vite: visualizzazione numero attuale, respawn alla morte
*/
#include <irrlicht.h>
#include "driverChoice.h"
#include "levelMatrix.h"
#include "input.h"
using namespace irr;
using namespace core;

#define _CRT_SECURE_NO_WARNINGS 1

FILE *logData;

//gestore input
MastEventReceiver *input;

//textures
video::ITexture* terreno;
video::ITexture* player;

//stato player
struct {
	vector2d<float> ine;
	vector2d<float> oldIne;
	vector2d<float> posFloat;
	vector2d<float> oldPosFloat;
	vector2d<int> pos;
	vector2d<int> size;
	bool inVolo;

} statoPlayer;

void caricaSprites(video::IVideoDriver* driver)
{
	terreno = driver->getTexture("sprites/tile.bmp");
	player = driver->getTexture("sprites/char2.bmp");
	driver->makeColorKeyTexture(player, position2d<s32>(0, 0));
}

void inizializzaPlayer()
{
	statoPlayer.posFloat = vector2d<float>(0.0f, 100.0f);
	statoPlayer.oldPosFloat = statoPlayer.posFloat;
	statoPlayer.pos = vector2d<int>(0, 0);
	statoPlayer.size = vector2d<int>(8, 10);
	statoPlayer.ine = vector2d<float>(0.0f, 0.0f);
	statoPlayer.oldIne = statoPlayer.ine;
}

void muoviPlayer(u32 frameTime)
{
	float timeCorrection = float(frameTime) / 50;

	//memorizzo posizione vecchia
	statoPlayer.oldPosFloat = statoPlayer.posFloat;
	statoPlayer.oldIne = statoPlayer.ine;

	//applico l'inerzia del turno precedente
	fprintf(logData, "inerzia:%f \n", statoPlayer.ine.X);
	statoPlayer.posFloat += statoPlayer.ine;

	//calcolo l'inerzia
	//statoPlayer.ine.X-=0.001f*timeCorrection; //smorzo
	//if (statoPlayer.ine.X<0.001) statoPlayer.ine.X=0.0f;

	//applico la gravita
	if (statoPlayer.inVolo){
		statoPlayer.ine.Y += 0.040f*timeCorrection;
	}

	bool muovoLateralmente = false;
	if (input->keyDown(EKEY_CODE::KEY_RIGHT)) //applico gli spostamenti laterali
	{
		muovoLateralmente = true;
		if (!statoPlayer.inVolo) statoPlayer.ine.X += 0.025f*timeCorrection;
		else statoPlayer.ine.X += 0.008f*timeCorrection;
	}
	if (input->keyDown(EKEY_CODE::KEY_LEFT))
	{
		muovoLateralmente = true;
		if (!statoPlayer.inVolo) statoPlayer.ine.X -= 0.025f*timeCorrection;
		else statoPlayer.ine.X -= 0.008f*timeCorrection;
	}
	if (input->keyPressed(EKEY_CODE::KEY_SPACE))
	{
		if (!statoPlayer.inVolo) statoPlayer.ine.Y = -0.5f;
		fprintf(logData, "Salto!\n");
	}
	if (!muovoLateralmente && !statoPlayer.inVolo)
	{
		if (statoPlayer.ine.X > 0.0f)
		{
			statoPlayer.ine.X -= 0.025f*timeCorrection;
			if (statoPlayer.ine.X<0.0f) statoPlayer.ine.X = 0.0f;
		}
		else if (statoPlayer.ine.X<0.0f)
		{
			statoPlayer.ine.X += 0.025f*timeCorrection;
			if (statoPlayer.ine.X>0.0f) statoPlayer.ine.X = 0.0f;
		}
	}
	if (!muovoLateralmente && statoPlayer.inVolo)
	{
		if (statoPlayer.ine.X>0.0f)
		{
			statoPlayer.ine.X -= 0.010f*timeCorrection;
			if (statoPlayer.ine.X<0.0f) statoPlayer.ine.X = 0.0f;
		}
		else if (statoPlayer.ine.X<0.0f)
		{
			statoPlayer.ine.X += 0.010f*timeCorrection;
			if (statoPlayer.ine.X>0.0f) statoPlayer.ine.X = 0.0f;
		}
	}
	if (statoPlayer.ine.X>0.19f) statoPlayer.ine.X = 0.19f;   //limito velocità massima
	if (statoPlayer.ine.X<-0.19f) statoPlayer.ine.X = -0.19f;
	if (statoPlayer.ine.Y>0.5f) statoPlayer.ine.Y = 0.5f;

	//converto il movimento in float in movimento in intero
	vector2d<int> posCandidate = vector2d<int>(0, 0);
	posCandidate.X = int(statoPlayer.posFloat.X);
	posCandidate.Y = int(statoPlayer.posFloat.Y);

	bool posizioneOK = true;
	bool collisione = false;

	//verifico se la posizione è buona, se è buona accetto il movimento
	//calcolo per difetto la cella in cui potrei essere, e verifico se devo controllare le successive
	//calcolo le posizioni, in termini di cella, dei quattro vertici del rettangolo del giocatore usando statoPlayer.size.X
	vector2d<int> vertice[4];
	vertice[0] = vector2d<int>(posCandidate.X / 10, posCandidate.Y / 10);
	vertice[1] = vector2d<int>((posCandidate.X + statoPlayer.size.X - 1) / 10, posCandidate.Y / 10);
	vertice[2] = vector2d<int>(posCandidate.X / 10, (posCandidate.Y + statoPlayer.size.Y - 1) / 10);
	vertice[3] = vector2d<int>((posCandidate.X + statoPlayer.size.X - 1) / 10, (posCandidate.Y + statoPlayer.size.Y - 1) / 10);
	//verifico se sono vuote
	for (int i = 0; i < 4; i++){
		fprintf(logData, "%d:%d,%d--", i, vertice[i].X, vertice[i].Y);
		if (levelMatrix(vertice[i].Y, vertice[i].X) != 0)
		{
			collisione = true;
			posizioneOK = false;
		}
	}
	fprintf(logData, "\n");

	//se non è buona cerco la prima posizione buona TODO
	//aggiorno anche quella float

	if (posizioneOK){
		statoPlayer.pos = posCandidate;
		//statoPlayer.posFloat.X=float(posCandidate.X);
		//statoPlayer.posFloat.Y=float(posCandidate.Y);
	}

	//TODO riconoscere esattamente il tipo di collisione in base a cosa ho attorno come faccio
	//con la inAir, non guardando la velocita
	if (collisione){
		fprintf(logData, "collisione\n");
		if (statoPlayer.oldIne.X != 0.0f)
		{
			fprintf(logData, "collisione Laterale %f\n", statoPlayer.ine.X);
			statoPlayer.posFloat.X = statoPlayer.oldPosFloat.X; //aggiusto la posizione //TODO METTERCI LA VECCHIA posFLOAT
			statoPlayer.ine.X = 0.0;						  //elimino il movimento
		}
		if (statoPlayer.oldIne.Y != 0.0f)
		{
			fprintf(logData, "collisione verticale %f\n", statoPlayer.ine.Y);
			statoPlayer.posFloat.Y = statoPlayer.oldPosFloat.Y;
			//statoPlayer.ine.Y=0.0f;
		}
	}

	//verifico se ho qualcosa sotto i piedi ai vertici bassi per sapere se sono in volo
	vector2d<int> verticeCheckVolo[2];
	verticeCheckVolo[0] = vector2d<int>(posCandidate.X / 10, (posCandidate.Y + statoPlayer.size.Y + 1) / 10);
	verticeCheckVolo[1] = vector2d<int>((posCandidate.X + statoPlayer.size.X) / 10, (posCandidate.Y + statoPlayer.size.Y + 1) / 10);
	statoPlayer.inVolo = true;
	for (int i = 0; i < 2; i++){
		if (levelMatrix(verticeCheckVolo[i].Y, verticeCheckVolo[i].X) != 0)
		{
			statoPlayer.inVolo = false;
		}
	}
	if (statoPlayer.inVolo)
	{
		fprintf(logData, "inVolo\n");
	}
	else
	{
		if (statoPlayer.ine.Y > 0.0) statoPlayer.ine.Y = 0.0f;
	}
}

void disegnaLivello(video::IVideoDriver* driver)
{
	for (int i = 0; i < LEV_ROW; i++)
		for (int j = 0; j < LEV_COL; j++)
		{
			if (levelMatrix(i, j) == 1) driver->draw2DImage(terreno, core::position2d<s32>(j * 10, i * 10), core::rect<s32>(0, 0, 10, 10), 0, video::SColor(255, 255, 255, 255), false);
		}
}
void disegnaPlayer(video::IVideoDriver* driver)
{
	driver->draw2DImage(player, core::position2d<s32>(statoPlayer.pos.X, statoPlayer.pos.Y), core::rect<s32>(0, 0, statoPlayer.size.X, statoPlayer.size.Y), 0, video::SColor(255, 255, 255, 255), true);
}


#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

/*
At first, we let the user select the driver type, then start up the engine, set
a caption, and get a pointer to the video driver.
*/
int main()
{
	//open
	logData = fopen("log.txt", "w");


	//leggo la matrice del livello
	readMatrix();


	//creo la classe per gli input
	input = new MastEventReceiver();

	// ask user for driver
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	// create device
	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(500, 500), 16,
		false, false, false, input);

	if (device == 0)
		return 1; // could not create selected driver.

	device->setWindowCaption(L"Multi-Platform");

	video::IVideoDriver* driver = device->getVideoDriver();

	/*
	All 2d graphics in this example are put together into one texture,
	2ddemo.png. Because we want to draw colorkey based sprites, we need to
	load this texture and tell the engine, which part of it should be
	transparent based on a colorkey.

	In this example, we don't tell it the color directly, we just say "Hey
	Irrlicht Engine, you'll find the color I want at position (0,0) on the
	texture.". Instead, it would be also possible to call
	driver->makeColorKeyTexture(images, video::SColor(0,0,0,0)), to make
	e.g. all black pixels transparent. Please note that
	makeColorKeyTexture just creates an alpha channel based on the color.
	*/
	caricaSprites(driver);
	video::ITexture* images = driver->getTexture("sprites/tile.bmp");
	driver->makeColorKeyTexture(images, core::position2d<s32>(0, 0));
	printf("FIN QUI OK 2.5\n");
	/*
	To be able to draw some text with two different fonts, we first load
	them. Ok, we load just one. As the first font we just use the default
	font which is built into the engine. Also, we define two rectangles
	which specify the position of the images of the red imps (little flying
	creatures) in the texture.
	*/
	/*gui::IGUIFont* font = device->getGUIEnvironment()->getBuiltInFont();
	gui::IGUIFont* font2 =
	device->getGUIEnvironment()->getFont("../../media/fonthaettenschweiler.bmp");

	core::rect<s32> imp1(349,15,385,78);
	core::rect<s32> imp2(387,15,423,78);*/

	/*
	Prepare a nicely filtering 2d render mode for special cases.
	*/
	driver->getMaterial2D().TextureLayer[0].BilinearFilter = true;
	driver->getMaterial2D().AntiAliasing = video::EAAM_FULL_BASIC;

	/*
	Everything is prepared, now we can draw everything in the draw loop,
	between the begin scene and end scene calls. In this example, we are
	just doing 2d graphics, but it would be no problem to mix them with 3d
	graphics. Just try it out, and draw some 3d vertices or set up a scene
	with the scene manager and draw it.
	*/
	printf("FIN QUI OK 2\n");
	inizializzaPlayer();
	input->init();
	ITimer* timer = device->getTimer();
	u32 lastTime;
	u32 frameTime = 100;
	timer->getTime();
	while (device->run() && driver)
	{
		input->endEventProcess();

		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(255, 120, 102, 136));

			/*
			First, we draw 3 sprites, using the alpha channel we
			created with makeColorKeyTexture. The last parameter
			specifies that the drawing method should use this alpha
			channel. The last-but-one parameter specifies a
			color, with which the sprite should be colored.
			(255,255,255,255) is full white, so the sprite will
			look like the original. The third sprite is drawn
			with the red channel modulated based on the time.
			*/

			//disegno livello

			muoviPlayer(frameTime);

			disegnaLivello(driver);
			disegnaPlayer(driver);

			// draw fire & dragons background world
			/*driver->draw2DImage(images, core::position2d<s32>(50,50),
				core::rect<s32>(0,0,342,224), 0,
				video::SColor(255,255,255,255), true);

				// draw flying imp
				driver->draw2DImage(images, core::position2d<s32>(164,125),
				(time/500 % 2) ? imp1 : imp2, 0,
				video::SColor(255,255,255,255), true);

				// draw second flying imp with colorcylce
				driver->draw2DImage(images, core::position2d<s32>(270,105),
				(time/500 % 2) ? imp1 : imp2, 0,
				video::SColor(255,(time) % 255,255,255), true);*/

			/*
			Drawing text is really simple. The code should be self
			explanatory.
			*/

			// draw some text
			/*if (font)
				font->draw(L"This demo shows that Irrlicht is also capable of drawing 2D graphics.",
				core::rect<s32>(130,10,300,50),
				video::SColor(255,255,255,255));

				// draw some other text
				if (font2)
				font2->draw(L"Also mixing with 3d graphics is possible.",
				core::rect<s32>(130,20,300,60),
				video::SColor(255,time % 255,time % 255,255));*/

			/*
			Next, we draw the Irrlicht Engine logo (without
			using a color or an alpha channel). Since we slightly scale
			the image we use the prepared filter mode.
			*/
			/*driver->enableMaterial2D();
			driver->draw2DImage(images, core::rect<s32>(10,10,108,48),
			core::rect<s32>(354,87,442,118));
			driver->enableMaterial2D(false);*/

			/*
			Finally draw a half-transparent rect under the mouse cursor.
			*/
			/*core::position2d<s32> m = device->getCursorControl()->getPosition();
			driver->draw2DRectangle(video::SColor(100,255,255,255),
			core::rect<s32>(m.X-20, m.Y-20, m.X+20, m.Y+20));*/
			input->startEventProcess();
			driver->endScene();
			frameTime = timer->getTime() - lastTime;
			lastTime = timer->getTime();

		}
	}

	device->drop();
	fclose(logData);
	return 0;
}

/*
That's all. I hope it was not too difficult.
**/
