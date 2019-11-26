//
//  ControlButton.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/25/18.
//

#include "GameControlButton.hpp"

GameControlButton::GameControlButton(const int& window_width, const int& window_height)
: WINDOW_WIDTH(window_width)
, WINDOW_HEIGHT(window_height)
, _timer(0.0f) 
{

}

void GameControlButton::Update(const float& dt)
{
	//
	// обновляем таймер для анимации кнопки
	//
	_timer += dt * 2;
	while (_timer > 2 * math::PI)
	{
		_timer -= 2 * math::PI;
	}
}

void GameControlButton::Draw()
{
	
	switch (_gameStatus)
	{
		case 0: // start
		{
			Render::device.PushMatrix();
			Render::device.MatrixTranslate(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0);
			Render::device.MatrixScale(1.8f);
			Render::device.MatrixTranslate(-_texStartBtn->getBitmapRect().Width()/2, -_texStartBtn->getBitmapRect().Height()/2, 0);
			Render::DrawDiagonalBlic(_texStartBtn, -10, -5, 9, _timer/6, 255, Color(255, 255, 255), true);
			Render::device.PopMatrix();
			break;
		}
		case 1: // pause
		{
			Render::device.PushMatrix();
			Render::device.MatrixTranslate(WINDOW_WIDTH/2 + 80, WINDOW_HEIGHT/2 - 100, 0);
			Render::device.MatrixScale(0.8f);
			Render::device.MatrixTranslate(-_texContinueBtn->getBitmapRect().Width()/2, -_texContinueBtn->getBitmapRect().Height()/2, 0);
			Render::BeginColor(Color(255, 255, 0, 185));
			Render::DrawDiagonalBlic(_texContinueBtn, -10, -5, 9, _timer/6, 255, Color(255, 255, 255), true);
			Render::EndColor();
			Render::device.PopMatrix();
			break;
		}
		case 2: // result
		{
			Render::device.PushMatrix();
			Render::device.MatrixTranslate(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 150, 0);
			Render::device.MatrixScale(1.8f);
			Render::device.MatrixTranslate(-_texReplayBtn->getBitmapRect().Width()/2, - _texReplayBtn->getBitmapRect().Height()/2, 0);
			Render::BeginColor(Color(255, 255, 0, 185));
			Render::DrawDiagonalBlic(_texReplayBtn, -10, -5, 9, _timer/6, 255, Color(255, 255, 255), true);
			Render::EndColor();
			Render::device.PopMatrix();
			break;
		}
		case 3: // game
		{
			Render::device.PushMatrix();
			Render::device.MatrixTranslate(WINDOW_WIDTH - _texPauseBtn->getBitmapRect().Width() * 1.5f, WINDOW_HEIGHT - _texPauseBtn->getBitmapRect().Height() * 2.5, 0);
			Render::DrawDiagonalBlic(_texPauseBtn, -10, -5, 9, _timer/6, 255, Color(255, 255, 255), true);
			Render::device.PopMatrix();
			break;
		}
		default:
			;
	}
	
}

void GameControlButton::Move()
{
	switch (_gameStatus)
	{
		case 0: // start window
		{
			_ctrlBtn = (FRect)_texStartBtn->getBitmapRect();
			_ctrlBtn.Scale(1.8f);
			FPoint origin(WINDOW_WIDTH/2 - _ctrlBtn.Width()/2, WINDOW_HEIGHT/2 - _ctrlBtn.Height()/2);
			_ctrlBtn.MoveTo(origin);
			break;
		}
		case  1: // pause window
		{
			_ctrlBtn = (FRect)_texContinueBtn->getBitmapRect();
			_ctrlBtn.Scale(0.8f);
			FPoint origin(WINDOW_WIDTH/2 - _ctrlBtn.Width()/2 + 80, WINDOW_HEIGHT/2 - 100 - _ctrlBtn.Height()/2);
			_ctrlBtn.MoveTo(origin);
			break;
		}
		case 2: // results window
		{
			_ctrlBtn = (FRect)_texReplayBtn->getBitmapRect();
			_ctrlBtn.Scale(1.8f);
			FPoint origin(WINDOW_WIDTH/2 - _ctrlBtn.Width()/2, WINDOW_HEIGHT/2 - 150 - _ctrlBtn.Height()/2);
			_ctrlBtn.MoveTo(origin);
			break;
		}
		case 3: // game
		{
			_ctrlBtn = (FRect)_texPauseBtn->getBitmapRect();
			FPoint origin(WINDOW_WIDTH - _ctrlBtn.Width() * 1.5f, WINDOW_HEIGHT - _ctrlBtn.Height() * 2.5f);
			_ctrlBtn.MoveTo(origin);
			break;
		}
		default:
			;
	}
}
