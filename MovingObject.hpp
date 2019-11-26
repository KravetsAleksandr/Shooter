//
//  DynamicObject.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/19/18.
//

#ifndef MovingObject_hpp
#define MovingObject_hpp

#include "stdafx.h"

class MovingObject
{
protected:
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;
	
	bool _alive;
	
	float _timer; //для анимаций
	
	//Параметры объекта
	Render::Texture* _tex; // текстура объекта
	FRect _objectRect; // прямоугольник текстуры объекта
	IRect _texSize; //размеры текстуры объекта
	FPoint _texHalfSize; // половиа ширины и высоты оригинальной текстуры (часто используемая величина в расчетах)
	float _radius;
	float _scale;
	
	//Характеристики поведения объекта
	FPoint _shift; //смещение по X,Y после расчета угла
	float _bounce; // коэффициент отскока 0 - 1;
	float _gravity; // гравитация
	float _invMass; // инвертированная масса
	float _angle; // угол

	//Сплайны
	TimedSpline<FPoint> _spline;
	IRect _texSplineSize;
	FPoint _splinePos;
	float _scaleSpline;
	Render::Texture* _texSpline; // текстура объекта, двигающегося по сплайну
	
	//Эффекты трассы и взрыва при столкновении
	EffectsContainer _effCont;
	ParticleEffectPtr _effTrace;
	std::string _effTraceName;
	ParticleEffectPtr _effExplosion;
	std::string _effExplosionName;
	
protected:
	FPoint Rotated(const FPoint&, const FPoint&, const float&); //точка вращения, центр вращения, угол
	void CheckFixBorderCollision();
	virtual void ChildInit() = 0;
	
public:
	explicit MovingObject(const std::string&, const float&, const std::string&, const std::string&, const std::string&, const float&, const int&, const int&, const float&); //имя текстуры, scale текстуры, имя эффекта следа, имя эффекта взрыва, имя тексутры сплайна, scale сплайна, WINDOW_WIDTH, WINDOW_HEIGHT, процент визуального проникновения объектов внутрь
	virtual ~MovingObject(){}
	
	virtual void Init(const FPoint&, const int&, const float&, const float&, const float&); //center, speed, angle, gravity, bounce
	
	FRect getRect() { return _objectRect; }
	FPoint getShift() { return _shift; }
	float getRadius() { return _radius; }
	float getInvMass() { return _invMass; }
	
	void setPosition(FPoint, FPoint&); // CenterPoint, shift
	
	virtual void Draw() = 0;
	virtual void Update(const float&) = 0; //dt
	
	void CheckFixObstacleCollision(const FRect&, const float&);// прямоугольник объекта, угол наклона
	
	void setDisabled();
	bool isAlive() { return _alive; }
	bool isEffectFinished() { return _effCont.IsFinished() && _spline.empty(); }
};


#endif /* MovingObject_hpp */
