//
//  SCannon.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/21/18.
//

#ifndef SCannon_hpp
#define SCannon_hpp

#include "StaticObject.hpp"
#include "stdafx.h"

class SCannon : public StaticObject
{
private:
	FPoint _fireStartPos; // базовая точка вылета снаряда до перерасчетов угла наклона
	FPoint _firePos; // расчетная точка вылета снаряда после поворота
	
	float _timer;
	float _delay;
	float _initScale;
	float _rechargeTimer;
	int _shootCounter;
	int _shootLimit;
	
public:
	using StaticObject::StaticObject;
	
	void Draw() override;
	
	void Update(const IPoint&, const float&); // позиция мыши, dt
	void Init(const FPoint&, const float&, const int&, const float&); //center, angle, shoot_limit, delay
	
	FPoint Rotated(const FPoint&, const FPoint&, const float&); //точка вращения, центр вращения, угол
	bool isReadyToShoot (const IPoint&); // позиция мыши
	
	FPoint getFirePos() {  return _firePos; }
	int getShootCounter() { return _shootCounter; } // для хедера показывать остаток снарядов в обойме
	float getTimeLeft() { return 1 - _rechargeTimer/_delay; } // для анимации перезарядки
	
};

#endif /* SCannon_hpp */
