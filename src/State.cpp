#include "State.h"

using After = State::After;

After::After():std::vector<Action*>(){
}

After::After(After&& a):std::vector<Action*>(std::move(a)){
	//a.swap(*this);
}

After::~After(){
	for(auto e:*this){
		delete e;
	}
}

State::~State(){
}
