#include "Action.h"
#include <iostream>
Action::~Action(){
}
bool Action::operator==(const Action& a) const{
	return _ID == a._ID;
}
size_t Action::hash() const{
	return _ID;
}
size_t Action::ID() const{
	return _ID;
}
void Action::print() const{
	std::cout << "ID : " << _ID << std::endl;
}
