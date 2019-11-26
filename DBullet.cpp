//
//  DBullet.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/20/18.
//

#include "DBullet.hpp"



void DBullet::ChildInit()
{
	int shootingSound = MM::manager.PlaySample("shooting", false, 0.3f);
}


void DBullet::Draw()
{
	if (!_alive)
	{
		_effCont.Draw();
		return;
	}
	
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_objectRect.CenterPoint().x, _objectRect.CenterPoint().y, 0);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), (_angle-45)); // -45 - чтобы выровнять картинку
	Render::device.MatrixTranslate(-_texHalfSize.x, -_texHalfSize.y, 0);
	FRect rect(_texSize);
	FRect uv(0, 1, 0, 1);
	_tex->TranslateUV(rect, uv);
	Render::device.MatrixScale(_scale);
	_tex->Bind();
	Render::DrawQuad(rect, uv);
	
	Render::device.PopMatrix();
	
	_effCont.Draw();
}

void DBullet::Update(const float& dt)
{
	if (!_alive)
	{
		_effCont.Update(dt);
		_spline.Clear(); // для снаряда сплайн не нужен и обнуляется
		return;
	}
	
	_shift.y -= _gravity; //Ускорение свободного падения
	
	_objectRect.MoveBy(_shift);
	
	CheckFixBorderCollision();
	
	FPoint effPos(_objectRect.CenterPoint().x, _objectRect.CenterPoint().y + _texHalfSize.y);
	effPos = Rotated(effPos, _objectRect.CenterPoint(), (_angle-90)); //-90 приведение градусов в декартову систему координат
	if (_effTrace) _effTrace->SetPos(effPos);
	_effCont.Update(dt);
	
}


bool DBullet::CheckTargetCollision(const std::shared_ptr<DTarget>& targetToCheckPtr)
{
	if (!_alive || !targetToCheckPtr->isAlive() ) return false;
	
	return ( _objectRect.CenterPoint().GetDistanceTo(targetToCheckPtr->getRect().CenterPoint()) <= _radius + targetToCheckPtr->getRadius() );
}
