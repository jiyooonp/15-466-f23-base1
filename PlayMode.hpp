#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	virtual void loadAssets(std::string, int , int );
	virtual void updateTobbyNumber(int);
	virtual void resetTobby();
	virtual void randomNumberSpeed();
	virtual void randomSymbolPosition();
	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);
	std::vector<glm::vec4> number_at;
	std::vector<float> numberSpeed;

	std::vector<glm::vec4> symbol_at;

	// game state
	float PlayerSpeed = 30.0f;

	int tobbyState, tobbyScore = 0, tobbyTarget,  tobbyNumber;
	bool needNumber;
	int currentSymbol;

	// printing 
	std::vector<char> symbolList = {'+', '-', '*', '/'};
	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
