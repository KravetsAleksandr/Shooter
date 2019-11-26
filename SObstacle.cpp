//
//  SObstacle.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/21/18.
//

#include "SObstacle.hpp"


void SObstacle::Init(const FPoint& center, const float& angle)
{
	_angle = angle;
	
	_objectRect.MoveTo(FPoint(center.x - _objectRect.Width()/2, center.y - _objectRect.Height()/2));
}

void SObstacle::Update()
{
	_angle > 0 ? _angle += 0.5f :_angle -= 0.5f;
	
	while (_angle < -360)
	{
		_angle += 360;
	}
	
	while (_angle > 360)
	{
		_angle -= 360;
	}
}


void SObstacle::Draw()
{
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_objectRect.CenterPoint().x, _objectRect.CenterPoint().y, 0);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), (_angle - 90)); // коррекция угла для отрисовки текстуры вдоль наклона
	Render::device.MatrixTranslate(_objectRect.LeftBottom().x - _objectRect.CenterPoint().x, _objectRect.LeftBottom().y - _objectRect.CenterPoint().y, 0);
	Render::device.MatrixScale(_scale);
	
	FRect rect(_texSize);
	FRect uv(0, 1, 0, 1);
	_tex->TranslateUV(rect, uv);
	_tex->Bind();
	Render::DrawQuad(rect, uv);
	
	
	//Render::DrawDiagonalGlare(rect, 10, 10, _angle/100,
	//							   0.5f, 1.0f, 255, Color(255, 255, 255));
	
	Render::device.PopMatrix();
}

