//
//  GameControlButton.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/25/18.
//

#ifndef GameControlButton_hpp
#define GameControlButton_hpp
#include "stdafx.h"

class GameControlButton
{
private:
	Render::Texture* _texStartBtn = Core::resourceManager.Get<Render::Texture>("btnStart_Text");
	Render::Texture* _texReplayBtn = Core::resourceManager.Get<Render::Texture>("Replay");
	Render::Texture* _texContinueBtn = Core::resourceManager.Get<Render::Texture>("Continue");
	Render::Texture* _texPauseBtn = Core::resourceManager.Get<Render::Texture>("Pause");

	FRect _ctrlBtn; // прямоугольник управляющей кнопки
	int _gameStatus; // в зависимости от статуса игры производится перемещение прямоугольника управляющей кнопки
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;
	float _timer; //зацикленный таймер для анимаций
	
private:
	void Move();
	
public:
	GameControlButton(const int&, const int&); // width, height
	
	void operator = ( const int &status) //перегрузка оператора присваивания
	{
		_gameStatus = status;
		Move();
	}
	
	bool operator == (const IPoint &mouse_pos)
	{
		return _ctrlBtn.Contains(mouse_pos);
	}
	
	void Draw();
	void Update(const float& dt);

};

#endif /* GameControlButton_hpp */



