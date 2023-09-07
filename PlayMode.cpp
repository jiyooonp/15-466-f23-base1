#include "PlayMode.hpp"

// for the GL_ERRORS() macro:
#include "gl_errors.hpp"

// for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include "load_save_png.hpp"
#include "data_path.hpp"
#include <bitset>

PlayMode::PlayMode()
{

	std::vector<std::string> symbolassetVector = {
		"symbol_01.png",
		"symbol_02.png",
		"symbol_03.png",
		"symbol_04.png"};
	std::vector<std::string> numberAssetVector = {
		"number_00.png",
		"number_01.png",
		"number_02.png",
		"number_03.png",
		"number_04.png",
		"number_05.png",
		"number_06.png",
		"number_07.png",
		"number_08.png",
		"number_09.png",
	};
	std::vector<std::string> characterAssetVector = {
		"tobby_dead.png",
		"tobby_alive.png",
		"tobby_dead.png",
		"tobby_alive.png",

	};
	std::vector<std::string> subCharacterAssetVector = {
		"good_guy.png",
		"bad_guy.png",
	};
	std::vector<std::string> backgroundAssetVector = {
		"bg.png",
		"bg1.png",
		"bg.png"

	};
	std::vector<std::string> paletteAssetVector = {
		"palette1.png",
		"palette2.png",
		"palette3.png"};

	// load main character 0, 0
	for (uint16_t i = 0; i < characterAssetVector.size(); i++)
		loadAssets(characterAssetVector[i], 0, 0 + i);

	// load sub character 1, 10
	for (uint16_t i = 0; i < subCharacterAssetVector.size(); i++)
		loadAssets(subCharacterAssetVector[i], 1, 10 + i);

	// load background 2, 100
	for (uint16_t i = 0; i < backgroundAssetVector.size(); i++)
		loadAssets(backgroundAssetVector[i], 2, 100 + i);

	// load symbol 3, 30
	for (uint16_t i = 0; i < symbolassetVector.size(); i++)
		loadAssets(symbolassetVector[i], 3, 30 + i);

	// load number 4, 40
	for (uint16_t i = 0; i < numberAssetVector.size(); i++)
		loadAssets(numberAssetVector[i], 4, 40 + i);

	// load palette # 6, 7, 8 / 110, 120, 130
	for (uint16_t i = 0; i < paletteAssetVector.size(); i++)
		loadAssets(paletteAssetVector[i], 6 + i, 10 * (11 + i));

	resetTobby();
}

void PlayMode::randomNumberSpeedPosition()
{
	if (number_at.size() == 0)
	{
		// Assign random speed to numbers
		for (int i = 0; i < 10; i++)
		{
			float x = std::rand() % ppu.ScreenWidth;
			float y = std::rand() % ppu.ScreenHeight;

			number_at.push_back(glm::vec4(x, y, 0.0f, 0.0f));
			numberSpeed.push_back(std::rand() % 40);
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			float x = std::rand() % ppu.ScreenWidth;
			float y = std::rand() % ppu.ScreenHeight;

			number_at[i] = glm::vec4(x, y, 0.0f, 0.0f);
			numberSpeed[i] = std::rand() % 40;
		}
	}
}

void PlayMode::randomSymbolPosition()
{
	if (symbol_at.size() == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			float x = std::rand() % ppu.ScreenWidth;
			float y = std::rand() % ppu.ScreenHeight;
			symbol_at.push_back(glm::vec4(x, y, 0.0f, 0.0f));
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			float x = std::rand() % ppu.ScreenWidth;
			float y = std::rand() % ppu.ScreenHeight;

			symbol_at[i] = glm::vec4(x, y, 0.0f, 0.0f);
		}
	}
}

void PlayMode::resetTobby()
{

	tobbyState = 0;
	tobbyTarget = std::rand() % 100;
	tobbyNumber = 0;
	needNumber = true;
	currentSymbol = 0;
	numbersLeft = 10;
	randomNumberSpeedPosition();
	randomSymbolPosition();

	// make all the numbers appear on screen
	for (uint8_t i = 0; i < number_at.size(); i++)
	{
		number_at[i].w = 0.0f;
	}
}

void PlayMode::loadAssets(std::string assetPath, int assetType, int spriteIndex)
{

	// Make one tile for each 8x8 png
	PPU466::Tile tile;

	std::vector<glm::u8vec4> data;
	glm::uvec2 size;

	int colorCount = 1;
	int currentColor = 0;

	// first color in the palette is always transparent
	ppu.palette_table[assetType][0] = glm::u8vec4(0x00, 0x00, 0x00, 0x00);

	load_png(data_path("assets/" + assetPath), &size, &data, LowerLeftOrigin);

	for (uint32_t y = 0; y < 8; ++y)
	{

		// initialize tiles to zero
		tile.bit0[y] = 0;
		tile.bit1[y] = 0;

		for (int x = 0; x < 8; ++x)
		{

			uint16_t pixelIndex = x + y * 8;

			bool alreadyHaveColor = false;

			// assume that the same types have the same palette
			for (uint8_t colorIndex = 0; colorIndex < 4; colorIndex++)
			{
				if (data[pixelIndex] == ppu.palette_table[assetType][colorIndex])
				{
					alreadyHaveColor = true;
					currentColor = colorIndex;
					break;
				}
			}
			if (!alreadyHaveColor && colorCount < 4)
			{
				ppu.palette_table[assetType][colorCount] = data[pixelIndex];
				currentColor = colorCount;
				colorCount++;
			}
			if (colorCount > 4)
			{
				std::cout << "Too many colors in " << assetPath << ". Cannot load assets! Shutting down program!" << std::endl;
				return;
			}

			uint32_t bit0 = 0;
			uint32_t bit1 = 0;

			if (currentColor / 1 == 0)
			{
				bit0 = 0;
				bit1 = 0;
			}
			if (currentColor / 1 == 1)
			{
				bit0 = 1;
				bit1 = 0;
			}
			if (currentColor / 1 == 2)
			{
				bit0 = 0;
				bit1 = 1;
			}
			if (currentColor / 1 == 3)
			{
				bit0 = 1;
				bit1 = 1;
			}

			tile.bit0[y] |= bit0 << x;
			tile.bit1[y] |= bit1 << x;

			ppu.tile_table[spriteIndex] = tile;
		}
	}
}

PlayMode::~PlayMode()
{
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size)
{

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.sym == SDLK_LEFT)
		{
			left.downs += 1;
			left.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RIGHT)
		{
			right.downs += 1;
			right.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_UP)
		{
			up.downs += 1;
			up.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN)
		{
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE)
		{
			PlayerSpeed += 10;
			// std::cout << "Player Speed increased to: " << PlayerSpeed << std::endl;
		}
		else if (evt.key.keysym.sym == SDLK_g)
		{
			// palyer thinks he has gotten the number correct
			if (tobbyNumber == tobbyTarget)
			{
				tobbyScore += 1;
				// start new state
				resetTobby();
				std::cout << "Tobby got it right! Score is now " << tobbyScore << std::endl;
			}
			else
			{
				std::cout << "Tobby got it wrong! Tobby died. Total score is " << tobbyScore << std::endl;
				tobbyState = 1; // tobby is dead
			}
		}
		else if (evt.key.keysym.sym == SDLK_r)
		{
			std::cout << "Restarting the game!" << std::endl;
			resetTobby();
			needNumber = true;
			tobbyScore = 0;
			PlayerSpeed = 30.0f;
		}
	}
	else if (evt.type == SDL_KEYUP)
	{
		if (evt.key.keysym.sym == SDLK_LEFT)
		{
			left.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RIGHT)
		{
			right.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_UP)
		{
			up.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN)
		{
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::updateTobbyNumber(int currentNumber)
{
	needNumber = false;

	numbersLeft--;
	switch (currentSymbol)
	{
	case 0:
		tobbyNumber += currentNumber;
		break;
	case 1:
		tobbyNumber -= currentNumber;
		break;
	case 2:
		tobbyNumber *= currentNumber;
		break;
	case 3:
		tobbyNumber /= currentNumber;
		break;
	default:
		std::cout << " No operator selected" << std::endl;
		break;
	}
	if (numbersLeft <= 0 && tobbyNumber != tobbyTarget)
	{
		tobbyState = 1; // tobby is dead
	}
}

void PlayMode::update(float elapsed)
{
	// check if tobby is dead
	if (tobbyState == 1)
	{
		return;
	}

	if (left.pressed)
		player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed)
		player_at.x += PlayerSpeed * elapsed;
	if (down.pressed)
		player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed)
		player_at.y += PlayerSpeed * elapsed;

	{ // Added after running game multiple times. There are points where Tobby is off screen in x, y, but appears on screen
		if (player_at.x > static_cast<float>(ppu.ScreenWidth))
		{
			player_at.x -= static_cast<float>(ppu.ScreenWidth);
		}
		else if (player_at.x < 0)
		{
			player_at.x += static_cast<float>(ppu.ScreenWidth);
		}
		if (player_at.y > static_cast<float>(ppu.ScreenHeight))
		{
			player_at.y -= static_cast<float>(ppu.ScreenHeight);
		}
		else if (player_at.y < 0)
		{
			player_at.y += static_cast<float>(ppu.ScreenHeight);
		}
	}

	for (uint8_t i = 0; i < number_at.size(); i++)
	{
		number_at[i].x += numberSpeed[i] * elapsed;
		number_at[i].y += numberSpeed[i] * elapsed;

		if (number_at[i].x > ppu.ScreenWidth)
			number_at[i].x = 0;
		if (number_at[i].y > ppu.ScreenHeight)
			number_at[i].y = 0;
	}

	{ // Check if tobby got a number
		for (uint8_t i = 0; i < number_at.size(); i++)
		{
			if (std::abs(player_at.x - number_at[i].x) < 4 && std::abs(player_at.y - number_at[i].y) < 8 && number_at[i].w == 0.0f)
			{
				if (!needNumber)
				{
					// ran into a number when it needed a symbol -> dead
					std::cout << "Tobby is dead cuz it got a number: " << i + 1 << std::endl;
					tobbyState = 1; // tobby is dead
					needNumber = true;
					return;
				}
				else
				{
					number_at[i].w = 1.0f;
					updateTobbyNumber(i);

					// restore all symbols
					for (int i = 0; i < 4; i++)
					{
						symbol_at[i].w = 0.0f;
					}
					// if it collides with two, only one will be accounted for
				}
			}
		}
	}

	{ // Check if tobby got a symbol
	  // symbol order + - * /
		for (uint8_t i = 0; i < symbol_at.size(); i++)
		{
			if (std::abs(player_at.x - symbol_at[i].x) < 8 && std::abs(player_at.y - symbol_at[i].y) < 8 && symbol_at[i].w == 0.0f)
			{
				if (needNumber)
				{
					// ran into a symbol when it needed a number -> dead
					tobbyState = 1; // tobby is dead
					// player_at.w = 0.0f; // make tobby dead
					std::cout << "Tobby is dead cuz it got a symbol" << std::endl;
					needNumber = true;
					return;
				}
				else
				{
					symbol_at[i].w = 1.0f;
					currentSymbol = i;
					needNumber = true;
				}
			}
		}
	}

	// reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size)
{
	//--- set ppu state based on game state ---

	// background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		0xff,
		0xff,
		0xff,
		0xff);

	{ // Draw the background
		uint8_t tileIndexbg = 102;
		uint8_t paletteIndexbg = 2;
		uint16_t backgroundInfo = 0;
		backgroundInfo |= tileIndexbg << 0;
		backgroundInfo |= paletteIndexbg << 8;
		for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y)
		{
			for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x)
			{
				ppu.background[x + PPU466::BackgroundWidth * y] = backgroundInfo;
			}
		}
		// if (needNumber)
		// {
		// 	uint16_t needNumberInfo = 0;
		// 	needNumberInfo |= 100 << 0;
		// 	needNumberInfo |= paletteIndexbg << 8;
		// 	ppu.background[0] = needNumberInfo;
		// }
	}

	{ // Display the target number on the top of the screen
		int firstDigit = tobbyTarget / 10;
		int secondDigit = tobbyTarget % 10;

		uint16_t firstDigitInfo = 0;
		firstDigitInfo |= (40 + firstDigit) << 0;
		firstDigitInfo |= 6 << 8;

		uint16_t secondDigitInfo = 0;
		secondDigitInfo |= (40 + secondDigit) << 0;
		secondDigitInfo |= 6 << 8;

		int index1 = PPU466::BackgroundWidth * 28 + 15;
		int index2 = PPU466::BackgroundWidth * 28 + 16;

		ppu.background[index1] = firstDigitInfo;
		ppu.background[index2] = secondDigitInfo;
	}

	// If dead, display socre in the center
	if (tobbyState == 1)
	{
		int scoreIndex1 = PPU466::BackgroundWidth * 14 + 14;
		int scoreIndex2 = PPU466::BackgroundWidth * 14 + 15;
		int scoreIndex3 = PPU466::BackgroundWidth * 14 + 16;

		int firstScoreDigit = tobbyScore / 100;
		int secondScoreDigit = (tobbyScore % 100) / 10;
		int thirdScoreDigit = tobbyScore % 10;

		uint16_t firstScoreDigitInfo = 0;
		firstScoreDigitInfo |= (40 + firstScoreDigit) << 0;
		firstScoreDigitInfo |= 7 << 8;

		uint16_t secondScoreDigitInfo = 0;
		secondScoreDigitInfo |= (40 + secondScoreDigit) << 0;
		secondScoreDigitInfo |= 7 << 8;

		uint16_t thirdScoreDigitInfo = 0;
		thirdScoreDigitInfo |= (40 + thirdScoreDigit) << 0;
		thirdScoreDigitInfo |= 7 << 8;

		ppu.background[scoreIndex1] = firstScoreDigitInfo;
		ppu.background[scoreIndex2] = secondScoreDigitInfo;
		ppu.background[scoreIndex3] = thirdScoreDigitInfo;
	}

	{ // Draw player sprite:
		ppu.sprites[0].x = int8_t(player_at.x);
		ppu.sprites[0].y = int8_t(player_at.y);
		if (tobbyState == 1)
		{
			// tobby is dead
			ppu.sprites[0].index = 2;
		}
		else
		{
			ppu.sprites[0].index = 1;
		}
		ppu.sprites[0].attributes = 0;
	}

	{ // Draw symbol sprites
		for (uint8_t i = 30; i < 34; ++i)
		{
			ppu.sprites[i].x = symbol_at[i - 30].x;
			ppu.sprites[i].y = symbol_at[i - 30].y;
			ppu.sprites[i].index = i;
			ppu.sprites[i].attributes = 3;
			ppu.sprites[i].attributes |= static_cast<int>(symbol_at[i - 30].w) << 7;
		}
	}
	{ // Draw number sprites
		for (uint8_t i = 40; i < 50; ++i)
		{
			ppu.sprites[i].x = number_at[i - 40].x;
			ppu.sprites[i].y = number_at[i - 40].y;
			ppu.sprites[i].index = i;
			ppu.sprites[i].attributes = 4;
			ppu.sprites[i].attributes |= static_cast<int>(number_at[i - 40].w) << 7;
		}
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
