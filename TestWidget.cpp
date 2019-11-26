#include "stdafx.h"
#include "TestWidget.h"


enum
{
	WINDOW_WIDTH = 1024, // ширина игрового поля
	WINDOW_HEIGHT = 768 // высота игрового поля
};



TestWidget::TestWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
{
	Init();
}


void TestWidget::Init()
{
	_game.Init(WINDOW_WIDTH, WINDOW_HEIGHT);
}


void TestWidget::Draw()
{
	_game.Draw();
}


void TestWidget::Update(float dt)
{
	_game.Update(dt);
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	if (Core::mainInput.GetMouseRightButton())
		_game.MouseDown(mouse_pos, 1);
	else
		_game.MouseDown(mouse_pos, 0);

	return false;
}

void TestWidget::MouseMove(const IPoint &mouse_pos)
{
	_game.MouseMove(mouse_pos);
}

void TestWidget::MouseUp(const IPoint &mouse_pos)
{
	_game.MouseUp(mouse_pos);
}

void TestWidget::AcceptMessage(const Message& message)
{
	//
	// Виджету могут посылаться сообщения с параметрами.
	//
	//

	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();
}

void TestWidget::KeyPressed(int keyCode)
{
	//
	// keyCode - виртуальный код клавиши.
	// В качестве значений для проверки нужно использовать константы VK_.
	//

	if (keyCode == VK_ESCAPE) {
		// Реакция на нажатие кнопки Esc
		_game.GamePause();
	}
}

void TestWidget::CharPressed(int unicodeChar)
{
	//
	// unicodeChar - Unicode код введённого символа
	//

	if (unicodeChar == L'а') {
		// Реакция на ввод символа 'а'
	}
}
