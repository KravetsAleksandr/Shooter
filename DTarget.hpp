//
//  DTarget.hpp
//  Test
//
//  Created by Aleksandr Kravets on 9/20/18.
//

#ifndef DTarget_hpp
#define DTarget_hpp

#include "MovingObject.hpp"
#include "stdafx.h"

class DTarget : public MovingObject
{
private:
	static size_t _objLeft; //глобальный счетчик активных объектов класса на поле
	bool once = false; //для уменьшения счетчика
	
private:
	void ChildInit() override {};
	
public:
	explicit DTarget (const std::string&, const float&, const std::string&, const std::string&, const std::string&, const float&, const int&, const int&, const float&);
	~DTarget () {};
	
	void Update(const float&) override;
	void Draw() override;
	
	static size_t Count() { return _objLeft; } // выдает количество объектов класса по внешнему запросу
	static void ResetCounter() {_objLeft = 0;}; // обнуляет количество объектов класса

	bool CheckTargetCollision(const std::shared_ptr<DTarget>&); // объект для сверки коллизии
	void FixTargetCollision(const std::shared_ptr<DTarget>&); // объект для сверки коллизии
};

#endif /* DTarget_hpp */
