//
//  StaticObject.cpp
//  Test
//
//  Created by Aleksandr Kravets on 9/21/18.
//

#include "StaticObject.hpp"


StaticObject::StaticObject (const std::string& texName, const float& scale)
: _tex( Core::resourceManager.Get<Render::Texture>(texName) ) //текстура
, _scale(scale)
{
	_texSize = _tex->getBitmapRect(); //параметры прямоугольника текстуры
	_objectRect = (FRect)_texSize;
	_objectRect.Scale(_scale);
	
	_objectRect.Height() > _objectRect.Width() ? _radius = _objectRect.Height()/2 : _radius = _objectRect.Width()/2;
}

