//
//  SObstacle.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/21/18.
//

#ifndef SObstacle_hpp
#define SObstacle_hpp

#include "StaticObject.hpp"
#include "stdafx.h"

class SObstacle : public StaticObject
{
private:
	
	
public:
	using StaticObject::StaticObject;
	
	void Init(const FPoint&, const float&); //center, angle
	void Update ();
	void Draw() override;
};


#endif /* SObstacle_hpp */
