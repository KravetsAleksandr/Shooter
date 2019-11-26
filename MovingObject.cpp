//
//  DynamicObject.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/19/18.
//

#include "MovingObject.hpp"


MovingObject::MovingObject(const std::string& texName, const float& texScale, const std::string& effTraceName, const std::string& effExplosionName, const std::string& texSpline, const float& scaleSpline, const int& window_width, const int& window_height, const float& penetrationPercent)
: _tex(Core::resourceManager.Get<Render::Texture>(texName))
, _scale(texScale)
, _effTraceName(effTraceName)
, _effExplosionName (effExplosionName)
, _scaleSpline(scaleSpline)
, WINDOW_WIDTH(window_width)
, WINDOW_HEIGHT(window_height)
, _timer(0.0f)

{
	_texSize = _tex->getBitmapRect();
	_objectRect = (FRect)_texSize;
	_objectRect.Scale(_scale);
	_texHalfSize.x = _objectRect.Width()/2; // запоминаем исходные размеры для Draw, расчетный прямоугольник будет меньше
	_texHalfSize.y = _objectRect.Height()/2;
	// уменьшаем размер расчетного прямоугольника чтобы достичь эффекта наложения текстур
	_objectRect.Height() > _objectRect.Width() ?
	_objectRect.Inflate(-2 * _objectRect.Width() / 100 * penetrationPercent) :
	_objectRect.Inflate(-2 * _objectRect.Height() / 100 * penetrationPercent);
	
	_objectRect.Height() > _objectRect.Width() ? _radius = _objectRect.Height()/2 : _radius = _objectRect.Width()/2;
	_radius > 0 ? _invMass = 1/_radius : _invMass = 0;
	
	if ( !texSpline.empty() )
	{
		_texSpline = Core::resourceManager.Get<Render::Texture>(texSpline);
		_texSplineSize = _texSpline->getBitmapRect();
	}

	_effTrace = nullptr;
	_effExplosion = nullptr;
}


void MovingObject::Init(const FPoint& center, const int& speed, const float& angle, const float& gravity, const float& bounce)
{
	_alive = true;
	
	_bounce = bounce;
	_gravity = gravity;
	_angle = angle;
	
	_objectRect.MoveTo(FPoint(center.x - _objectRect.Width()/2, center.y - _objectRect.Height()/2));
	
	_shift.x = math::cos(angle/180.f * math::PI) * speed; //шаг перемещения мишени по Х
	_shift.y = math::sin(angle/180.f * math::PI) * speed; //шаг перемещения мишени по Y

	CheckFixBorderCollision();
	
	_effCont.KillAllEffects(); // в случае множественной переинициализации из-за коллизий на старте
	if ( !_effTraceName.empty() )
	{
		_effTrace = _effCont.AddEffect(_effTraceName, _objectRect.CenterPoint());
		_effTrace->Reset();
	}
	else
	{
		_effTrace = nullptr;
	}
	_spline.Clear();// в случае множественной переинициализации из-за коллизий на старте
	
	ChildInit();
}


void MovingObject::setPosition(FPoint center, FPoint& shift)
{
	_objectRect.MoveTo( FPoint(center.x - _objectRect.Width()/2, center.y - _objectRect.Height()/2) );
	_shift = shift;
}


void MovingObject::setDisabled()
{
	_alive = false;
	
	if (_effTrace)
	{
		_effTrace->Kill();
		_effTrace = nullptr;
	}
	
	if ( !_effExplosionName.empty() )
	{
		_effExplosion = _effCont.AddEffect(_effExplosionName, _objectRect.CenterPoint());
		_effExplosion->Reset();
		int explosionSound = MM::manager.PlaySample("explosion", false, 0.5f);
	}
	
	if ( _spline.empty() && _texSpline )
	{
		_spline.addKey(0.0f, _objectRect.CenterPoint());
		_spline.addKey(0.5f, FPoint(_objectRect.CenterPoint().x + (WINDOW_WIDTH - 100/2 - _objectRect.CenterPoint().x)/3,
									WINDOW_HEIGHT - 50/2 - (WINDOW_HEIGHT - 50/2 - _objectRect.CenterPoint().y)/3));
		_spline.addKey(1.0f, FPoint(WINDOW_WIDTH - 100/2, WINDOW_HEIGHT - 50/2));
		_spline.CalculateGradient();
		_splinePos = _objectRect.CenterPoint(); // определяем стартовую позицию для Draw до апдейта позиции по запросу
		_timer = 0.00f; // для старта в начале, а не по случайному зацикленному значению таймера
	}
}


void MovingObject::CheckFixObstacleCollision(const FRect& obstacle, const float& angle)
{
	// не проверяем если объекты далеко друг от друга
	float distance = obstacle.CenterPoint().GetDistanceTo(obstacle.LeftBottom()) + _radius; // 2 радиуса
	if ( !_alive || obstacle.CenterPoint().GetDistanceTo(_objectRect.CenterPoint()) > distance)
		return;
	
	//находим угол поворота объекта относительно базового положения объекта
	float correctionAngle;
	if (angle >= 90 && angle <= 180)
		correctionAngle = 270 + (180 - angle);
	else
		correctionAngle = 90 - angle;
	
	// вращаем центр мишени, соотносим с базовым положением объекта (стороны параллельны осям координат)
	FPoint rotatedObsCenter = Rotated(_objectRect.CenterPoint(), obstacle.CenterPoint(), correctionAngle);
	
	// направление откуда приехали
	FPoint rotatedPrevCenter = Rotated((_objectRect.CenterPoint() - _shift), obstacle.CenterPoint(), correctionAngle);
	
	float EPS = 5.0f; //  допустимая глубина проникновения
	FRect obRect = obstacle.Inflated(-EPS); // изменяем прямоугольник на глубину проникновения со всех сторон
	
	// расширяем полученный объект на радиус
	FRect Left(obRect.LeftBottom().x - _radius, obRect.LeftBottom().x, obRect.LeftBottom().y - _radius, obRect.LeftTop().y + _radius);
	FRect Right(obRect.RightBottom().x, obRect.RightBottom().x + _radius, obRect.RightBottom().y - _radius, obRect.RightTop().y + _radius);
	FRect Top(obRect.LeftTop().x, obRect.RightTop().x, obRect.CenterPoint().y, obRect.RightTop().y + _radius);
	FRect Bottom(obRect.LeftTop().x, obRect.RightTop().x, obRect.LeftBottom().y - _radius, obRect.CenterPoint().y);
	
	
	bool isCollision = false;
	FPoint reflection; // точка отражения
	if (Left.Contains(rotatedObsCenter))
	{
		if (rotatedPrevCenter.x < rotatedObsCenter.x)
			reflection.x = rotatedPrevCenter.x;
		else
			reflection.x = obRect.LeftBottom().x - _radius;
		if (rotatedPrevCenter.y <= rotatedObsCenter.y)
			reflection.y = rotatedObsCenter.y + (rotatedObsCenter.y - rotatedPrevCenter.y);
		else
			reflection.y = rotatedObsCenter.y - (rotatedPrevCenter.y - rotatedObsCenter.y);
		isCollision = true;
	}
	else if (Right.Contains(rotatedObsCenter))
	{
		if (rotatedPrevCenter.x > rotatedObsCenter.x)
			reflection.x = rotatedPrevCenter.x;
		else
			reflection.x = obRect.RightBottom().x + _radius;
		if (rotatedPrevCenter.y <= rotatedObsCenter.y)
			reflection.y = rotatedObsCenter.y + (rotatedObsCenter.y - rotatedPrevCenter.y);
		else
			reflection.y = rotatedObsCenter.y - (rotatedPrevCenter.y - rotatedObsCenter.y);
		isCollision = true;
	}
	else if (Top.Contains(rotatedObsCenter))
	{
		if (rotatedPrevCenter.y > rotatedObsCenter.y)
			reflection.y = rotatedPrevCenter.y;
		else
			reflection.y = obRect.RightTop().y + _radius;
		if (rotatedPrevCenter.x <= rotatedObsCenter.x)
			reflection.x = rotatedObsCenter.x + (rotatedObsCenter.x - rotatedPrevCenter.x);
		else
			reflection.x = rotatedObsCenter.x - (rotatedPrevCenter.x - rotatedObsCenter.x);
		isCollision = true;
	}
	else if (Bottom.Contains(rotatedObsCenter))
	{
		if (rotatedPrevCenter.y < rotatedObsCenter.y)
			reflection.y = rotatedPrevCenter.y;
		else
			reflection.y = obRect.LeftBottom().y - _radius;
		if (rotatedPrevCenter.x <= rotatedObsCenter.x)
			reflection.x = rotatedObsCenter.x + (rotatedObsCenter.x - rotatedPrevCenter.x);
		else
			reflection.x = rotatedObsCenter.x - (rotatedPrevCenter.x - rotatedObsCenter.x);
		isCollision = true;
	}
	
	if (isCollision)
	{
		FPoint shift = (reflection - rotatedObsCenter).Normalized() * _shift.Length();
		rotatedObsCenter += (shift * _bounce);
		
		FPoint backReflectionPoint = Rotated(reflection, obstacle.CenterPoint(), -correctionAngle);
		_shift = (backReflectionPoint - _objectRect.CenterPoint()).Normalized() * _shift.Length();
		
		if ( !Bottom.Contains(rotatedObsCenter) && !Top.Contains(rotatedObsCenter) &&
			!Right.Contains(rotatedObsCenter) && !Left.Contains(rotatedObsCenter) )
		{
			_shift *= _bounce;
			return;
		}
		else
		{
			rotatedObsCenter -= (shift * _bounce); // при bounce вылета не получилось, чекаем без него
			rotatedObsCenter += shift;
			
			if ( !Bottom.Contains(rotatedObsCenter) && !Top.Contains(rotatedObsCenter) &&
				!Right.Contains(rotatedObsCenter) && !Left.Contains(rotatedObsCenter) )
			{
				_objectRect.MoveBy(_shift);
				_shift *= _bounce;
			}
			else
				if (_shift.y >= 0 && _shift.y <= _gravity)
				{
					_shift.y += _gravity; // обнуление действия гравитации при следующем апдейте пока в состоянии коллизии
					_shift.x > 0 ? _shift.x += 1 : _shift.x -= 1;
				}
		}
	}
	
}

void MovingObject::CheckFixBorderCollision()
{
	bool isBorderCollision = false;
	
	FPoint center = _objectRect.CenterPoint();
	
	if ( _objectRect.CenterPoint().x >= (WINDOW_WIDTH - _texHalfSize.x) )
	{
		center.x = WINDOW_WIDTH - _texHalfSize.x;
		_shift.x *= -_bounce;
		isBorderCollision = true;
	}
	else
		if ( _objectRect.CenterPoint().x <= _texHalfSize.x )
		{
			center.x = _texHalfSize.x;
			_shift.x *= -_bounce;
			isBorderCollision = true;
		}
	
	if ( _objectRect.CenterPoint().y >= (WINDOW_HEIGHT - _texHalfSize.y) )
	{
		center.y = WINDOW_HEIGHT - _texHalfSize.y;
		_shift.y *= -_bounce;
		isBorderCollision = true;
	}
	else
		if ( _objectRect.CenterPoint().y <= _texHalfSize.y )
		{
			center.y = _texHalfSize.y;
			_shift.y *= -_bounce;
			_shift.x *= _bounce;
			if (math::abs(_shift.y) < _gravity && _alive)
				setDisabled();
			isBorderCollision = true;
		}
	
	if (isBorderCollision)
		_objectRect.MoveTo( FPoint(center.x - _objectRect.Width()/2, center.y - _objectRect.Height()/2) );
}


FPoint MovingObject::Rotated(const FPoint& pointToRotate, const FPoint& rotationCenter, const float& angle)
{
	FPoint rotatedPoint;
	
	rotatedPoint.x = rotationCenter.x +
	(pointToRotate.x - rotationCenter.x) * math::cos(angle * math::PI / 180) -
	(pointToRotate.y - rotationCenter.y) * math::sin(angle * math::PI / 180);
	
	rotatedPoint.y = rotationCenter.y +
	(pointToRotate.y - rotationCenter.y) * math::cos(angle * math::PI / 180) +
	(pointToRotate.x - rotationCenter.x) * math::sin(angle * math::PI / 180);
	
	return rotatedPoint;
}
