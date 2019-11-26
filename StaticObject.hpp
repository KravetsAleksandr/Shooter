//
//  StaticObject.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/21/18.
//

#ifndef StaticObject_hpp
#define StaticObject_hpp

#include "stdafx.h"

class StaticObject
{
	
protected:
	Render::Texture* _tex; // текстура объекта
	IRect _texSize; // размеры текстуры объекта
	FRect _objectRect; // прямоугольник объекта
	
	float _angle;
	float _scale;
	float _radius;
	
public:
	explicit StaticObject(const std::string&, const float&); // текстура, скейл, центр, угол наклона
	virtual ~StaticObject(){}
	
	virtual void Draw() = 0;
	
	FRect getRect() { return _objectRect; }
	float getAngle() { return _angle; }
	float getRadius() { return _radius; }
};


#endif /* StaticObject_hpp */
