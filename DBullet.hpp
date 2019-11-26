//
//  DBullet.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/20/18.
//

#ifndef DBullet_hpp
#define DBullet_hpp


#include "MovingObject.hpp"
#include "DTarget.hpp"
#include "stdafx.h"

class DBullet : public MovingObject
{
	
private:
	void ChildInit() override;
	
public:
	using MovingObject::MovingObject; // использование конструктора базового класса
	
	void Update(const float&) override;
	void Draw() override;
	
	
	bool CheckTargetCollision(const std::shared_ptr<DTarget>&); // объект для сверки коллизии
};


#endif /* DBullet_hpp */
