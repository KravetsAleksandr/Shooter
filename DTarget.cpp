//
//  DTarget.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/20/18.
//

#include "DTarget.hpp"

size_t DTarget::_objLeft = 0;

DTarget::DTarget (const std::string& texName, const float& texScale, const std::string& effTraceName, const std::string& effExplosionName, const std::string& texSpline, const float& scaleSpline, const int& window_width, const int& window_height, const float& penetrationPercent) :
MovingObject::MovingObject(texName, texScale, effTraceName, effExplosionName, texSpline, scaleSpline, window_width, window_height, penetrationPercent)
{
	++_objLeft;
}


void DTarget::Draw()
{
	if (!_spline.empty())
	{
		Render::device.PushMatrix();
		Render::device.MatrixTranslate(_splinePos.x - _scaleSpline * (float)_texSplineSize.Width()/2, _splinePos.y - _scaleSpline * (float)_texSplineSize.Height()/2, 0);
		Render::device.MatrixScale(_scaleSpline);
		_texSpline->Draw();
		Render::device.PopMatrix();
	}
	
	if (!_alive)
	{
		_effCont.Draw();
		return;
	}
	
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_objectRect.CenterPoint().x - _texHalfSize.x, _objectRect.CenterPoint().y - _texHalfSize.y, 0);
	FRect rect(_texSize);
	FRect uv(0, 1, 0, 1);
	_tex->TranslateUV(rect, uv);
	Render::device.MatrixScale(_scale);
	_tex->Bind();
	Render::DrawQuad(rect, uv);
	Render::device.PopMatrix();
	
	_effCont.Draw();
}


void DTarget::Update(const float& dt)
{
	_timer += dt * 2;
	while (_timer > 2 * math::PI)
	{
		_timer -= 2 * math::PI;
	}
	_scale += 0.01*sinf(10*_timer); // пульсирование объекта
	
	if (!_spline.empty())
	{
		_splinePos = _spline.getGlobalFrame(math::clamp(0.0f, 1.0f, _timer / 3.0f));
		if ( _splinePos == (_spline.GetKey(_spline.GetKeysCount() - 1)) )
		{
			int incomeSound = MM::manager.PlaySample("income", false, 1.0f);
			_spline.Clear();
		}
	}
	
	if (!_alive)
	{
		_effCont.Update(dt);
		if (!once)
			once = [](){ --_objLeft; return true; }();
		return;
	}
	
	_shift.y -= _gravity; //Ускорение свободного падения
	
	_objectRect.MoveBy(_shift);
	
	CheckFixBorderCollision();
	
	if (_effTrace) _effTrace->SetPos(_objectRect.CenterPoint());
	_effCont.Update(dt);
}



bool DTarget::CheckTargetCollision(const std::shared_ptr<DTarget>& targetToCheckPtr)
{
	if (!_alive || !targetToCheckPtr->isAlive() ) return false;
	
	return ( _objectRect.CenterPoint().GetDistanceTo(targetToCheckPtr->getRect().CenterPoint()) <= _radius + targetToCheckPtr->getRadius() );
}


void DTarget::FixTargetCollision(const std::shared_ptr<DTarget>& targetToFixPtr)
{
	FPoint targetShift = targetToFixPtr->getShift();
	
	// мишени разлетаются вдоль прямой, проходящей через их центры
	FPoint penetrationDirection = (targetToFixPtr->getRect().CenterPoint() - _objectRect.CenterPoint()).Normalized();
	
	//относительная скорость мишеней
	FPoint relativeVelocity =  targetShift - _shift;
	if (penetrationDirection.GetDotProduct(relativeVelocity) > 0)// если объекты отдаляются ничего не делаем
		return;
	
	//тела обмениваются импульсом вдоль нормали контакта
	float exchangeVelocity = -(1 + _bounce) * (penetrationDirection.GetDotProduct(relativeVelocity));
	exchangeVelocity /= _invMass + targetToFixPtr->getInvMass();
	
	FPoint impulse = penetrationDirection * exchangeVelocity;
	
	_shift -= _invMass * impulse;
	targetShift += targetToFixPtr->getInvMass() * impulse;
	
	// глубина проникновения
	float penetrationDepth = _radius + targetToFixPtr->getRadius() - _objectRect.CenterPoint().GetDistanceTo(targetToFixPtr->getRect().CenterPoint());
	FPoint correction = penetrationDirection * penetrationDepth / (_invMass + targetToFixPtr->getInvMass());
	
	// раздвигаем в противоположные стороны вдоль penetrationDirection
	_objectRect.MoveTo(_objectRect.LeftBottom() - _invMass * correction);
	targetToFixPtr->setPosition(targetToFixPtr->getRect().CenterPoint() + targetToFixPtr->getInvMass() * correction, targetShift);
}
