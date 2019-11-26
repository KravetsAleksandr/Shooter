//
//  GameEngine.cpp
//  Test
//
//  Created by Aleksandr Kravets on 6/14/18.
//

#include "GameEngine.hpp"
using namespace std;


void GameEngine::Init(const int& windowwidth, const int& windowheight)
{
	WINDOW_WIDTH = windowwidth;
	WINDOW_HEIGHT = windowheight;
	
	srand( TickCount() );
	
	//
	// Фоновый трек меню
	//
	MM::manager.StopAll();
	int backTrack = MM::manager.PlayTrack("menutrack", true, 0.7f);

	//
	// Текстуры
	//
	_texCursorNormal = Core::resourceManager.Get<Render::Texture>("cur_normal");
	_texCursorActive = Core::resourceManager.Get<Render::Texture>("cur_active");
	_texBackground = Core::resourceManager.Get<Render::Texture>("Background");
	_texBeetleHUD = Core::resourceManager.Get<Render::Texture>("Beetle");
	_texBombHUD = Core::resourceManager.Get<Render::Texture>("Bomb2");
	_texBulletsHUD = Core::resourceManager.Get<Render::Texture>("Bullets");
	_texClockHUD = Core::resourceManager.Get<Render::Texture>("Clock");
	_texMenu = Core::resourceManager.Get<Render::Texture>("Menu");
	_texResults = Core::resourceManager.Get<Render::Texture>("Results");
	_texCongrats = Core::resourceManager.Get<Render::Texture>("Congratulations");
	_texGameover = Core::resourceManager.Get<Render::Texture>("Gameover");
	_texSpline = Core::resourceManager.Get<Render::Texture>("Star");
	
	
	//
	// Управляющай кнопка меню (Start, Replay) для отработки кликов по ней
	// Инициализация объекта Кнопка
	//
	_controlButtonPtr = std::make_shared<GameControlButton>(WINDOW_WIDTH, WINDOW_HEIGHT);

	
	//
	// ЗАГРУЗКА КОНФИГУРАЦИИ ИЗ ФАЙЛА____________________________
	//
	//дефолтные значения на случай проблем с чтением файла
	_configMap.insert( std::pair<std::string, float>("GAME_SESSION_TIMER", 30) );// таймер одной игровой сессии в секундах
	_configMap.insert( std::pair<std::string, float>("COUNT_TARGET", 5) );// количество мишеней
	_configMap.insert( std::pair<std::string, float>("TARGET_SPEED", 6) ); // начальная скорость мишеней до первого столкновения, далее пересчитывается
	_configMap.insert( std::pair<std::string, float>("TARGET_GRAVITY", 0.0f) ); // коэф. гравитации мишени
	_configMap.insert( std::pair<std::string, float>("TARGET_BOUNCE", 1.0f) ); // коэф. отскока мишеней от стен и других мишеней
	_configMap.insert( std::pair<std::string, float>("COUNT_OBSTACLE", 3) ); //количество мишеней
	_configMap.insert( std::pair<std::string, float>("BULLET_SPEED", 20) ); // начальная скорость пули с дальнейшим влиянием гравитации
	_configMap.insert( std::pair<std::string, float>("BULLET_GRAVITY", 0.35f) ); // [0..1] коэф. гравитации снаряда
	_configMap.insert( std::pair<std::string, float>("BULLET_BOUNCE", 0.4) ); // коэф. отскока снаряда от стен, понижение скорости при каждом столкновении до полной остановки
	_configMap.insert( std::pair<std::string, float>("SHOOT_LIMIT", 3) ); // количество снарядов в обойме, после отстрела обоймы идет перезарядка
	_configMap.insert( std::pair<std::string, float>("SHOOT_DELAY", 4 * math::PI) ); // условное время перезарядки обоймы пушки
	
	//вычитка из файла и перезаписывание параметров новыми полученными значениями
	IO::InputStreamPtr streamPtr = Core::fileSystem.OpenRead("input.txt");
	IO::TextReader text(streamPtr.get());
	
	std::string param = text.ReadAsciiLine();
	while ( !param.empty() )
	{
		param.erase(std::remove(param.begin(), param.end(), ' '), param.end());
		
		if ( param.find_first_of("=") != std::string::npos && param.find_first_of("0123456789") == param.find_first_of("=") + 1 )
		{
			float value = std::stof( param.substr( param.find_first_of("=") + 1, param.find_first_of(";") - param.find_first_of("=") - 1 ) );
			param.assign( param, 0, param.find_first_of("=") );
			if ( _configMap.find(param) != _configMap.end() )
				_configMap.at(param) = value;
			else
				_configMap.insert( std::pair<std::string, float>(param, value) );
		} 
		
		param = text.ReadAsciiLine();
	}
	
	_gameStatus = _menuMap.at("START");
	//_gameStatus = START;
	
	//Управляющая кнопка на Стартовое окно
	(*_controlButtonPtr) = _menuMap.at("START");

}


void GameEngine::GamePlay()
{
	_gameTimer.Sync();
	_gameTimer.Start();
	
	//
	// Фоновый трек
	//
	MM::manager.StopAll();
	int backTrack = MM::manager.PlayTrack("gametrack", true, 0.4f, 1.f);

	
	//
	//Инициализация объекта ПУШКА____________________________
	//
	_cannonPtr = std::make_shared<SCannon>("Cannon", 1.f);
	FPoint obstacleCenter(WINDOW_WIDTH/2, _cannonPtr->getRadius());
	_cannonPtr->Init( obstacleCenter, 90, static_cast<int>(_configMap.at("SHOOT_LIMIT")), _configMap.at("SHOOT_DELAY") );
	
	
	//
	//Инициализация объектов ПРЕПЯТСТВИЕ_______________________
	//
	_configMap.at("COUNT_OBSTACLE") = static_cast<int>(_configMap.at("COUNT_OBSTACLE"));
	for (size_t i=0; i<_configMap.at("COUNT_OBSTACLE"); ++i)
	{
		std::shared_ptr<SObstacle> sptr = std::make_shared<SObstacle>("Obstacle", 1.f);
		_obstacleListPtr.push_back(sptr);
	}
	
	for (obstacleListIt it = _obstacleListPtr.begin(); it != _obstacleListPtr.end(); ++it)
	{
		bool initSuccess = true;
		do
		{
			initSuccess = true;
			
			obstacleCenter.x = (*it)->getRadius() + rand() % ( WINDOW_WIDTH - (int)(*it)->getRadius() * 2 );
			obstacleCenter.y = (*it)->getRadius() + rand() % ( WINDOW_HEIGHT - (int)(*it)->getRadius() * 2 );
			(*it)->Init(obstacleCenter, -180 + rand() % 360);
			
			if ((*it)->getRect().CenterPoint().GetDistanceTo(_cannonPtr->getRect().CenterPoint()) <
				(*it)->getRadius() + _cannonPtr->getRadius())
			{
				initSuccess = false;
				continue;
			}
			
			for (obstacleListIt it_check = _obstacleListPtr.begin(); it_check != it; ++it_check)
			{
				if ( (*it)->getRect().CenterPoint().GetDistanceTo((*it_check)->getRect().CenterPoint()) < 2 * (*it)->getRadius() )
				{
					initSuccess = false;
					break;
				}
			}
		} while (!initSuccess);
	}

	//
	//Инициализация объектов МИШЕНЬ_________________________
	//
	_configMap.at("COUNT_TARGET") = static_cast<int>(_configMap.at("COUNT_TARGET"));
	for (size_t i=0; i<_configMap.at("COUNT_TARGET"); ++i)
	{
		std::shared_ptr<DTarget> sptr = std::make_shared<DTarget>("Ball", (0.07f * (4 + rand() % 7)), "", "FindItem2", "Star", 0.15f, WINDOW_WIDTH, WINDOW_HEIGHT, 10);
		_targetListPtr.push_back(sptr);
	}
	
	for (targetListIt it = _targetListPtr.begin(); it != _targetListPtr.end(); ++it)
	{
		bool initSuccess;
		do
		{
			initSuccess = true;
			(*it)->Init( FPoint(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT), _configMap.at("TARGET_SPEED"), (1 + rand() % 360), _configMap.at("TARGET_GRAVITY"), _configMap.at("TARGET_BOUNCE") );
			
			for (targetListIt it_check = _targetListPtr.begin(); it_check != it; ++it_check)
				if ( (*it)->CheckTargetCollision( (*it_check) ) )
				{
					initSuccess = false;
					break;
				}
			
			(*it)->CheckFixObstacleCollision(_cannonPtr->getRect(), _cannonPtr->getAngle());
			
			for(auto &sptr : _obstacleListPtr)
				(*it)->CheckFixObstacleCollision((*sptr).getRect(), (*sptr).getAngle());
			
		} while (!initSuccess);
	}

	
	_gameStatus = _menuMap.at("GAME");
	
	// Управляющая кнопка на Игру
	(*_controlButtonPtr) = _menuMap.at("GAME");
}


void GameEngine::GameFinish()
{
	_gameTimer.Pause(); // почему-то функция НЕ останавливает таймер?

	MM::manager.StopAll();
	
	// Отлавливаем неактивные мишени (еще не удален из-за анимации) для правильного итогового учета

	_resultsMap.at("TARGETS LEFT") = DTarget::Count();//size;

	// Записываем оставшееся время
	_resultsMap.at("TIME LEFT") = math::floor(_configMap.at("GAME_SESSION_TIMER") - math::floor(_gameTimer.getElapsedTime()));
	
	// Проверяем условие выигрыш или проигрыш
	if ( _resultsMap.at("TIME LEFT") >= 0 && _resultsMap.at("TARGETS LEFT") == 0 )
	{
		_resultsMap.at("ISWON") = 1;
		int missioncomplete = MM::manager.PlaySample("missioncomplete", false, 0.5f, 1.f);
		int backTrack = MM::manager.QueueSample("menutrack", missioncomplete, true, 0.7f);
	}
	else
	{
		_resultsMap.at("ISWON") = 0;
		int missionfail = MM::manager.PlaySample("fail", false, 0.7f, 1.f);
		int backTrack = MM::manager.QueueSample("menutrack", missionfail, true, 0.7f);
	}
	
	// Удаляем все объекты
	DTarget::ResetCounter(); // обнуляем счетчик объектов класса
	_targetListPtr.clear();
	_bulletListPtr.clear();
	_obstacleListPtr.clear();
	_cannonPtr.reset();
	
	
	// Меняем статус игры на Завершено для прекращения апдейта игровых элементов и отрисовки экрана Результаты
	_gameStatus = _menuMap.at("FINISH");
	
	//Управляющая кнопка на Финиш
	(*_controlButtonPtr) =	_menuMap.at("FINISH");
}


void GameEngine::GamePause()
{
	if (_isPaused == false)
	{
		if (_gameStatus != _menuMap.at("GAME"))
			return;
		
		_gameTimer.Pause();
		//для заморозки отражения времени при паузе
		_resultsMap.at("TIME LEFT") = math::floor(_configMap.at("GAME_SESSION_TIMER") - math::floor(_gameTimer.getElapsedTime()));
		
		MM::manager.PauseAll(true, false);
		_pauseSample = MM::manager.PlaySample("pause", true, 0.7f);
		
		_gameStatus = _menuMap.at("PAUSE");
		
		//Управляющая кнопка на Пауза
		(*_controlButtonPtr) =	_menuMap.at("PAUSE");
		
		_isPaused = true;
	}
	else
	{
		_gameTimer.Resume();

		MM::manager.PauseAll(false, false);
		MM::manager.StopSample(_pauseSample);
		
		_gameStatus = _menuMap.at("GAME");
		
		//Управляющая кнопка на Игра
		(*_controlButtonPtr) =	_menuMap.at("GAME");

		_isPaused = false;
	}
	
}


void GameEngine::Draw()
{
	//Бекграунд
	Render::device.PushMatrix();
	FRect rect(_texBackground->getBitmapRect());
	FRect uv(0, 1, 0, 1);
	_texBackground->TranslateUV(rect, uv);
	_texBackground->Bind();
	Render::DrawQuad(rect, uv);
	Render::device.PopMatrix();

	// Отрисовка Игра и Меню если есть

	if ( _gameStatus != _menuMap.at("GAME") )
		DrawMenu();
	else
		DrawGame();
	
	if (_gameStatus == _menuMap.at("PAUSE"))
	{
		DrawGame();
		DrawMenu();
	}
	
	//
	// В пределах игрового поля прячем системный курсор и рисуем кастомный,
	//
	if (_mousePos.x > 0 && _mousePos.x < WINDOW_WIDTH && _mousePos.y > 0 && _mousePos.y < WINDOW_HEIGHT)
	{
		_cursor.HideSystem();
		if ( (*_controlButtonPtr) == _mousePos )
		{
			_texCursorActive->Draw(_mousePos.x-20, _mousePos.y - _texCursorActive->getBitmapRect().Height()+20);
		}
		else
		{
			_texCursorNormal->Draw(_mousePos.x-20, _mousePos.y - _texCursorNormal->getBitmapRect().Height()+20);
		}
	}
	else
		_cursor.ShowSystem();
}


void GameEngine::Update(const float& dt)
{
	//
	// Апдейт управляющей кнопки
	//
	_controlButtonPtr->Update(dt);
	
	
	//
	// Если не активная игровая сессия - НЕ производим апдейт игровых элементов и покидаем апдейт
	//
	if ( _gameStatus != _menuMap.at("GAME") )
		return;
	
	
	//
	// Если осталось менее 15 сек включаем громко тикайющий звук
	//
	if (_configMap.at("GAME_SESSION_TIMER") - _gameTimer.getElapsedTime() <= 15)
	{
		MM::manager.SetTrackVolume(0.2); // приглушаем фоновый трек чтобы выделить звук тикающего таймера
		MM::manager.PlaySample("ticking", true, 1.f);
	}
	
	
	//
	// Если время игровой сессии вышло...
	//
	if (_gameTimer.getElapsedTime() >= _configMap.at("GAME_SESSION_TIMER"))
	{
		GameFinish();
		return;
	}
	
	
	//
	// Апдейт препятствия
	//
	for(auto &sptr : _obstacleListPtr)
	{
		(*sptr).Update();
	}
	
	
	//
	// Апдейт пушки
	//
		_cannonPtr->Update(_mousePos, dt);
	
	
	//
	// Здесь задается новая координата пули, проверка на коллизии с препятствиями
	//
	for(auto &sptr : _bulletListPtr)
	{
		(*sptr).Update(dt);
		for(auto &ptr : _obstacleListPtr)
			(*sptr).CheckFixObstacleCollision((*ptr).getRect(), (*ptr).getAngle());
	}
	
	
	//
	// Здесь задается новая координата шаров, проверка на коллизии с пушкой и препятствиями
	//
	for(auto &sptr : _targetListPtr)
	{
		(*sptr).Update(dt);
		(*sptr).CheckFixObstacleCollision(_cannonPtr->getRect(), _cannonPtr->getAngle());
		for(auto &ptr : _obstacleListPtr)
			(*sptr).CheckFixObstacleCollision((*ptr).getRect(), (*ptr).getAngle());
	}
	
	
	//
	// Проверка всех пуль со всеми мишенями на коллизию
	//
	for (auto &sptr_bullet : _bulletListPtr)
	{
		bool isCollision = false;
		
		for (auto &sptr_ball : _targetListPtr)
		{
			if ( (*sptr_bullet).CheckTargetCollision(sptr_ball) )
			{
				isCollision = true;
				(*sptr_ball).setDisabled();
			}
		}
		
		if (isCollision)
			(*sptr_bullet).setDisabled();
	}
	
	
	//
	// Проверка всех мишеней со всеми мишенями на коллизию
	//
	for (targetListIt it = _targetListPtr.begin(); it != _targetListPtr.end(); ++it)
		for (targetListIt it_check = it; it_check != _targetListPtr.end(); ++it_check)
		{
			if (it_check == it) continue;
			if ( (*it)->CheckTargetCollision(*it_check) )
			{
				(*it)->FixTargetCollision(*it_check);
			}
		}
	
	//
	// Удаление неактивных пуль из контейнера List (у листа объекты удаляются ремувом)
	//
	_bulletListPtr.remove_if([](const std::shared_ptr<DBullet> &bulletPtr) {return !bulletPtr->isAlive() && bulletPtr->isEffectFinished();});
	
	
	//
	// Удаление неактивных мишеней из контейнера List
	//
	_targetListPtr.remove_if([](const std::shared_ptr<DTarget> &ballPtr) {return !ballPtr->isAlive() && ballPtr->isEffectFinished();});
	
	
	//
	// когда убит последний шар
	//
	if ( _targetListPtr.empty() )
	{
		GameFinish();
		return;
	}
	

}


void  GameEngine::MouseMove(const IPoint& mouse_pos)
{
	_mousePos = mouse_pos;
}


void  GameEngine::MouseDown(const IPoint& mouse_pos, const int& direction)
{
	if (direction) // ПРАВАЯ кнопка
	{
		GameFinish(); // тестовый режим перезапуска игры, только при активной игре
	}
	else // ЛЕВАЯ кнопка
	{
		
		if ( _gameStatus == _menuMap.at("GAME") && (*_controlButtonPtr) == mouse_pos )
		{
			GamePause();
			return;
		}
	
		if ( _gameStatus == _menuMap.at("GAME") && !((*_controlButtonPtr) == mouse_pos) )
		{
			//
			//Инициализация объектов bullet_________________________
			//
			if ( _cannonPtr->isReadyToShoot(mouse_pos) )
			{
		
				std::shared_ptr<DBullet> sptr = std::make_shared<DBullet>("Bomb", 0.3f, "Iskra2", "", "", 0.15f, WINDOW_WIDTH, WINDOW_HEIGHT, 10);
				
				std::function<float(FPoint)> Angle = [&mouse_pos](FPoint point)
				{
					float atan = math::atan( (float)(mouse_pos.y - point.y), (float)(mouse_pos.x - point.x) );
					return atan * 180 / math::PI;
				};
				
				sptr->Init(_cannonPtr->getFirePos(), _configMap.at("BULLET_SPEED"), Angle(_cannonPtr->getFirePos()), _configMap.at("BULLET_GRAVITY"), _configMap.at("BULLET_BOUNCE") );
			
				_bulletListPtr.push_back(sptr);
			}
		}
	
		if ( _gameStatus == _menuMap.at("PAUSE") && (*_controlButtonPtr) == mouse_pos )
			GamePause();
	
		if ( _gameStatus == _menuMap.at("START") && (*_controlButtonPtr) == mouse_pos )
			GamePlay();
		
		if ( _gameStatus == _menuMap.at("FINISH") && (*_controlButtonPtr) == mouse_pos )
			GamePlay();
	}
}


void  GameEngine::MouseUp(const IPoint& mouse_pos)
{
	
}


void GameEngine::DrawGame()
{
	// HUD
	Render::BeginColor(Color(255, 255, 0, 165));
	_texBeetleHUD->Draw( 900 - _texBeetleHUD->getBitmapRect().Width(), WINDOW_HEIGHT - _texBeetleHUD->getBitmapRect().Height() );
	_texBombHUD->Draw( 750, WINDOW_HEIGHT - _texBombHUD->getBitmapRect().Height()) ;
	_texClockHUD->Draw( 624, WINDOW_HEIGHT - _texClockHUD->getBitmapRect().Height()) ;
	Render::EndColor();
	
	Render::BindFont("waltographUI");
	Render::BeginColor(Color(255, 255, 50, 255));
	
	_gameStatus == _menuMap.at("PAUSE")
	?
	Render::PrintString( 624 + _texClockHUD->getBitmapRect().Width(), WINDOW_HEIGHT - _texClockHUD->getBitmapRect().Height()/2, 						std::string(" ") +
						utils::lexical_cast(_resultsMap.at("TIME LEFT")), 2.2f, LeftAlign, CenterAlign )
	:
	Render::PrintString( 624 + _texClockHUD->getBitmapRect().Width(), WINDOW_HEIGHT - _texClockHUD->getBitmapRect().Height()/2, 						std::string(" ") +
						utils::lexical_cast(math::floor(_configMap.at("GAME_SESSION_TIMER") - math::floor(_gameTimer.getElapsedTime()))), 2.2f, LeftAlign, CenterAlign );
	
	Render::PrintString( 750 + _texBombHUD->getBitmapRect().Width(), WINDOW_HEIGHT - _texBombHUD->getBitmapRect().Height()/2, 								std::string(" ") +
						utils::lexical_cast(_configMap.at("SHOOT_LIMIT") - _cannonPtr->getShootCounter()), 2.2f, LeftAlign, CenterAlign );
	
	Render::PrintString( 900, WINDOW_HEIGHT - _texBeetleHUD->getBitmapRect().Height()/2,
						std::string(" ") +
						utils::lexical_cast(_configMap.at("COUNT_TARGET") - _targetListPtr.size()) + //DTarget::Count())
						std::string(" / ") +
						utils::lexical_cast(_configMap.at("COUNT_TARGET")),
						2.2f, LeftAlign, CenterAlign );
	
	Render::EndColor();
	

	
	// Управляющая кнопка
	_controlButtonPtr->Draw();

	
	//Пушка
	_cannonPtr->Draw();
	
	
	//Препятствия
	for(auto &sptr : _obstacleListPtr)
	{
		(*sptr).Draw();
	}
	
	//Мишени
	for(auto &sptr : _targetListPtr)
	{
		(*sptr).Draw();
	}
	
	// Снаряды
	for(auto &sptr : _bulletListPtr)
	{
		(*sptr).Draw();
	}
	
	
	// Перезарядка если есть
	if ( _cannonPtr->getShootCounter() >= _configMap.at("SHOOT_LIMIT") )
	{
		Render::BeginColor(Color(255, 255, 50, 125));
		Render::DrawFromLeftToRight(_texBulletsHUD, FPoint(WINDOW_WIDTH/2 - _texBulletsHUD->getBitmapRect().Width()/2,WINDOW_HEIGHT/2 - _texBulletsHUD->getBitmapRect().Height()/2), _cannonPtr->getTimeLeft(), 30);
		Render::EndColor();
	}
	
	
	//
	// Затемняем фон при паузе
	//
	if (_gameStatus == _menuMap.at("PAUSE"))
	{
		Render::device.SetTexturing(false);
		Render::BeginColor(Color(0, 0, 0, 185));
		Render::DrawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		Render::EndColor();
		Render::device.SetTexturing(true);
	}

}


void GameEngine::DrawMenu()
{
	
	switch (_gameStatus)
	{
		case 0: // start window
		{
			_texMenu->Draw(WINDOW_WIDTH/2 -_texMenu->getBitmapRect().Width()/2, WINDOW_HEIGHT/2 - _texMenu->getBitmapRect().Height()/2);
			_controlButtonPtr->Draw();
			break;
		}
		case  1: // pause window
		{
			_texMenu->Draw(WINDOW_WIDTH/2 -_texMenu->getBitmapRect().Width()/2, WINDOW_HEIGHT/2 - _texMenu->getBitmapRect().Height()/2);
			Render::BindFont("waltographUI");
			Render::BeginColor(Color(255, 255, 0, 255));
			Render::PrintString( WINDOW_WIDTH/2, WINDOW_HEIGHT/2, std::string(" GAME IS PAUSED! "), 2.2f, CenterAlign, CenterAlign );
			Render::EndColor();
			
			_controlButtonPtr->Draw();
			break;
		}
		case 2: // results window
		{
			_texResults->Draw(WINDOW_WIDTH/2 -_texResults->getBitmapRect().Width()/2,
							  WINDOW_HEIGHT/2 - _texResults->getBitmapRect().Height()/2);
			Render::BindFont("waltographUI");
			Render::BeginColor(Color(0, 0, 0, 225));
			Render::PrintString( WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 25,
								std::string("\n\n targets hit: ") +
								utils::lexical_cast(_configMap.at("COUNT_TARGET") - _resultsMap.at("TARGETS LEFT")) +
								std::string(" of ") +
								utils::lexical_cast(_configMap.at("COUNT_TARGET")) +
								std::string(" pcs.") +
								std::string("\n\n\n time left: ") +
								utils::lexical_cast(_resultsMap.at("TIME LEFT")) +
								std::string(" sec."),
								3.0f, CenterAlign, CenterAlign );
			Render::EndColor();
			
			if( _resultsMap.at("ISWON") == 1 )
			{
				Render::BeginColor(Color(255, 255, 255, 215));
				_texCongrats->Draw(WINDOW_WIDTH/2 - _texCongrats->getBitmapRect().Width()/2,
								   WINDOW_HEIGHT/2 + 130 - _texCongrats->getBitmapRect().Height()/2);
				Render::EndColor();
			}
			else
				{
					Render::BeginColor(Color(255, 255, 255, 185));
					_texGameover->Draw(WINDOW_WIDTH/2 -_texGameover->getBitmapRect().Width()/2,
								   WINDOW_HEIGHT/2 + 115 - _texGameover->getBitmapRect().Height()/2);
					Render::EndColor();
				}
			
			_controlButtonPtr->Draw();
			break;
		}
		default:
			;
	}
}


