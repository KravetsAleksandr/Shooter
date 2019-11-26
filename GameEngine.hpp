//
//  GameEngine.hpp
//  Test
//
//  Created by Aleksandr Kravets on 6/14/18.
//

#ifndef GameEngine_hpp
#define GameEngine_hpp

#include "GameControlButton.hpp"
#include "DTarget.hpp"
#include "DBullet.hpp"
#include "SObstacle.hpp"
#include "SCannon.hpp"
#include <Core/Timer.h>
#include "stdafx.h"


class GameEngine
{
public:
	GameEngine(){};
	

	void Draw();
	void Update(const float&); //dt
	void Init(const int&, const int&); // размеры игрового поля, ширина и высота
	
	void MouseDown(const IPoint&, const int&); // 0 - левая, 1 - правая
	void MouseMove(const IPoint&);
	void MouseUp(const IPoint&);
	
	void GamePause();
	
private:
	void GamePlay();
	void GameFinish();
	void DrawMenu();
	void DrawGame();
	
private:
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;

	int _gameStatus;
	enum GameStatus{START, PAUSE, FINISH, GAME};
	
	std::map< const std::string, const int > _menuMap =
		{{ "START", 0 },
		{ "PAUSE", 1 },
		{ "FINISH", 2 },
		{ "GAME", 3 }};

	std::map<const std::string, float> _resultsMap =
		{{ "ISWON", 0 },
		{ "TARGETS LEFT", 0 },
		{ "TIME LEFT", 0 }};
	
	
	bool _isPaused = false;
	
	int _pauseSample;
	
	Core::Timer _gameTimer;
	
	
	Render::Texture* _texBackground;
	Render::Texture* _texCursorNormal;
	Render::Texture* _texCursorActive;
	Render::Texture* _texBeetleHUD;
	Render::Texture* _texBombHUD;
	Render::Texture* _texBulletsHUD;
	Render::Texture* _texClockHUD;
	Render::Texture* _texMenu;
	Render::Texture* _texResults;
	Render::Texture* _texCongrats;
	Render::Texture* _texGameover;
	Render::Texture* _texSpline; // текстура объекта, двигающегося по сплайну
	
	GUI::Cursor _cursor;
	
	IPoint _mousePos;
	
	
	std::list< std::shared_ptr<DTarget> > _targetListPtr;
	typedef std::list< std::shared_ptr<DTarget> >::const_iterator targetListIt;
	
	std::list< std::shared_ptr<DBullet> > _bulletListPtr;
	typedef std::list< std::shared_ptr<DBullet> >::const_iterator bulletListIt;
	
	std::shared_ptr<SCannon> _cannonPtr;
	
	std::shared_ptr<GameControlButton> _controlButtonPtr;
	
	std::list< std::shared_ptr<SObstacle> > _obstacleListPtr;
	typedef std::list< std::shared_ptr<SObstacle> >::const_iterator obstacleListIt;
	
	std::map< std::string, float > _configMap;
};

#endif /* GameEngine_hpp */
