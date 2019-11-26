//
//  SCannon.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/21/18.
//

#include "SCannon.hpp"


void SCannon::Init(const FPoint& center, const float& angle, const int& shoot_limit, const float& delay)
{
	_timer = 0.0f;
	_rechargeTimer = 0.0f;
	_shootCounter = 0;
	_angle = angle;
	
	_shootLimit = shoot_limit;
	_delay = delay;
	_initScale = _scale;

	_objectRect.MoveTo(FPoint(center.x - _objectRect.Width()/2, center.y - _objectRect.Height()/2));
	
	_fireStartPos.x = _objectRect.CenterPoint().x;
	_fireStartPos.y = _objectRect.CenterPoint().y + _objectRect.Height()/2;
	_firePos = _fireStartPos;
}


void SCannon::Update(const IPoint& mouse_pos, const float& dt)
{
	_timer += dt * 2;
	while (_timer > math::PI)
	{
		_timer -= math::PI;
	}
	
	//получаем угол между центром пушки и позицией мыши, что = угол наклона пушки относительно оси Х
	// т.е. при _angle = 90 пушка смотрит вверх
//	std::function<float(IPoint, FPoint)> Angle = [](IPoint dot2, FPoint dot1)
//	{
//		float atan = math::atan( (float)(dot2.y - dot1.y), (float)(dot2.x - dot1.x) );
//		return atan * 180 / math::PI;
//	};
//	_angle = Angle( mouse_pos, _objectRect.CenterPoint() );
	
	_angle = [](IPoint dot2, FPoint dot1)
	{
		float atan = math::atan( (float)(dot2.y - dot1.y), (float)(dot2.x - dot1.x) );
		return atan * 180 / math::PI;
	} ( mouse_pos, _objectRect.CenterPoint() );
	
	// считаем точку вылета снаряда, -90 это угол поворота объекта относительно базового состояния объекта
	_firePos = Rotated(_fireStartPos, _objectRect.CenterPoint(), _angle-90);
	
	// если пушка отстреляла лимит запускается таймер перезарядки, по завершению таймера обнуляется лимит
	if (_shootCounter >= _shootLimit)
	{
		_rechargeTimer += dt * 2;
		_scale += 0.012 * sinf( 2.5 * _rechargeTimer); // при перезарядке
		
		while (_rechargeTimer > _delay)
		{
			_rechargeTimer = 0;
			_shootCounter = 0;
			_scale = _initScale;
			int charged = MM::manager.PlaySample("charged", false, 1.0f);
		}
	}
}


FPoint SCannon::Rotated(const FPoint& pointToRotate, const FPoint& rotationCenter, const float& angle)
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


bool SCannon::isReadyToShoot(const IPoint& mouse_pos)
{
	bool isReady = false;
	
	if ( _objectRect.CenterPoint().GetDistanceTo((FPoint)mouse_pos) < _radius || _shootCounter >= _shootLimit)
	{
		int shootingSound = MM::manager.PlaySample("rechargeCannon", false, 1.0f);
		return isReady;
	}
	
	if (_shootCounter >= 0 && _shootCounter < _shootLimit)
	{
		_shootCounter++;
		isReady = true;
	}
	
	return isReady;
}


void SCannon::Draw()
{
	Render::device.PushMatrix();
	
	Render::device.MatrixTranslate(_objectRect.CenterPoint().x, _objectRect.CenterPoint().y, 0);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), (_angle-90)); // коррекция угла для отрисовки текстуры вдоль наклона
	Render::device.MatrixTranslate(_objectRect.LeftBottom().x - _objectRect.CenterPoint().x, _objectRect.LeftBottom().y - _objectRect.CenterPoint().y, 0);
	Render::device.MatrixScale(_scale);
	
	FRect rect(_texSize);
	FRect uv(0, 1, 0, 1);
	
	if (_rechargeTimer > 0)
		Render::DrawDiagonalBlic(_tex, 10, 10, 9, _timer/6, 255, Color(255, 255, 255), true);
	else
	{
		_tex->TranslateUV(rect, uv);
		_tex->Bind();
		Render::DrawQuad(rect, uv);
	}
	
	Render::device.PopMatrix();
}

