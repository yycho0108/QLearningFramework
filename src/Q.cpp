#include "Utility.h"
#include "Q.h"
#include <limits>
#include <iostream>


/* =========== S ========== */
using After = State::After;
S::S():s(nullptr){
}
/*S::S(State*& s):s(s){
	s = nullptr; //not this->s.
}*/

S::S(State*&& s):s(s){
	s = nullptr; //not this->s.
}

S::S(const S& s):s(s.s->copy()){ //to enforce derived class-type vtable
}

/*
 * S::S(State&& s):s(new State(std::move(s))){

}
*/
S::~S(){
	delete s;
}
S& S::operator=(S& s){
	if(this != &s){
		delete this->s;
		this->s = nullptr;
		std::swap(this->s, s.s);
	}
	return *this;
}
S& S::operator=(S&& s){
	delete this->s;
	this->s = nullptr;
	std::swap(this->s, s.s);
	return *this;
}
bool S::operator==(const S& o) const{
	return *s == *o.s;
}
size_t S::hash() const{
	return s->hash();
}
double S::reward(){
	return s->reward();
}
void S::print() const{
	return s->print();
}
void S::next(After& a){
	return s->next(a);
	//let us be explicit.
}
S S::next(A& a){
	return S(s->next(*a.a));
	//will it work?
}
State* S::get() const{
	return this->s;
}

/* =========== A ========== */
/*A::A(Action*& a):a(a){
	//transfer ownership!
	a = nullptr; //not this->A
}*/
A::A(Action*&& a):a(a){ //make it explicit if you want to transfer that ownership.
	//transfer ownership!
	a = nullptr; //not this->A
}
A::A(const A& a):a(a.a->copy()){
}
A::~A(){
	delete a;
}
A& A::operator=(A& a){
	if(this != &a){
		delete this->a;
		this->a = nullptr;
		std::swap(this->a, a.a);
	}
	return *this;
}
A& A::operator=(A&& a){
	delete this->a;
	this->a = nullptr;
	std::swap(this->a, a.a);
	return *this;
}
bool A::operator==(const A& o) const{
	return *a == *o.a;
}
size_t A::hash() const{
	return a->hash();
}
void A::print() const{
	return a->print();
}
Action* A::get() const{
	return a;
}
/* =========== SA ========== */

/*SA::SA(class S& s, class A& a):
	_s(s),
	_a(a), //eats both.
	_next(s.next(a)),
	_hash(s.hash()*37 + a.hash()),
	_reward(_next.reward()){
}*/
SA::SA(class S&& _s, class A&& _a): //hopefully, this works.
	_s(_s),
	_a(_a) //conducts move operations autonomously
{
	_next = this->_s.next(this->_a);
	_hash = this->_s.hash()*37 + this->_a.hash();
	_reward = this->_s.reward();
}

SA::SA(const SA& sa)
	:_s(sa._s.s->copy()),
	_a(sa._a.a->copy()),
	_next(sa._next.s->copy()),
	_hash(sa._hash),
	_reward(sa._reward)
{
	//create new object, basically.
}

SA::SA(SA&& sa)
	:_s(sa._s),_a(sa._a) //steal
{
	_next = this->_s.next(this->_a);
	_hash = this->_s.hash() ^ this->_next.hash();
	//_hash = this->_s.hash()*37 + this->_a.hash();
	_reward = this->_s.reward();
	//_s = nullptr, _a = nullptr;
}
SA::~SA(){ //s,a will be automatically deleted
}
bool SA::operator==(const SA& o) const{
	return s()==o.s() && a()==o.a();
}

size_t SA::hash() const{
	return _hash;
}

void SA::print() const{
	s().print();
	a().print();
}

const S& SA::next() const{
	return _next;
}
const S& SA::s() const{
	return _s;
}
const A& SA::a() const{
	return _a;
}
const double& SA::reward() const{
	return _reward;
}

/* =========== Q ========== */

double Q::GAMMA = 0.8; //default value
void Q::setGAMMA(double g){
	GAMMA = g;
}
Q::Q(){
	t = 0;
}

double Q::alpha(const SA& sa){
	return 1.0/ ++visit.at(sa); //default implementation
}

double Q::max(const S& s){ //default implementation
	auto res = -std::numeric_limits<double>::max();
	After v;
	s.s->next(v);//store to v //list of next available actions (* vector elements heap memory *)
	for(auto &a : v){
		SA sa(s.s->copy(),std::move(a));//both rvalues.
		//create new copy to avoid corrupting s
		//and transfer ownership to sa.
		
		//perhaps I should have considered using a smart pointer...
		// but a can be transferred without problem.
		// the ownership gets transferred to sa
		// and subsequently to qHat if not found.
		auto tmp = 0.0;
		if(qHat.find(sa) == qHat.end()){
			qHat.insert(std::make_pair(
						sa,
						f() // random value between 0.0 and 1.0
						//std::numeric_limits<double>::max()
						)); //explicit r-value transfer
			visit.insert(std::make_pair(sa,0));
		}else{
			tmp = qHat.at(sa);
		}
		res = std::max(res,tmp);
	}
	return res;
}
void Q::advance(int n){ //default behavior.
	for(int i=0; i<n; ++i){
		//int s = qHat.size();
		//std::cout << s << std::endl;
		for(auto& qh : qHat){
			/* alias */
			auto& sa = qh.first;
			auto& util = qh.second;
			
			//auto& state = SA.S();
			//auto& action = SA.A();
			auto& next = sa.next();//immediate successor
			auto& r = sa.reward();
			auto ALPHA = alpha(sa);

			util = (1-ALPHA)*util + ALPHA *(r + GAMMA*max(next));
			//if(--s<=0) //prevent running for too long on one run... for now.
			//	break;
		}
	}
}

size_t Q::size() const{
	return qHat.size();
}
void Q::print() const{
	std::cout << "=== PRINT ===" << std::endl;
	for(auto& qh : qHat){
		auto& sa = qh.first;
		auto& util = qh.second;
		sa.print();
		std::cout << "WEIGHT : " << util << std::endl;
	}
	std::cout << "=============" << std::endl;
}

std::unordered_map<S,std::pair<A,double>> Q::policy() const{
	std::unordered_map<S,std::pair<A,double>> pi;
	for(auto& qh : qHat){
		auto& sa = qh.first;
		auto& s = sa.s();
		auto& a = sa.a();
		auto& util = qh.second;
		if(pi.find(s) == pi.end()){
			//optimistic search!!
			//new exploration!!
			pi.insert(std::make_pair(s.s->copy(),std::make_pair(a.a->copy(),-std::numeric_limits<double>::max())));
			//do NOT corrupt s or a.
		}else{
			auto& elem = pi.at(s);
			if(elem.second < util){
				elem.first = A(a.a->copy()); //new copy
				elem.second = util;
			}
		}
	}
	return pi;
}

void Q::printPolicy() const{
	std::cout << "=== POLICY ===" << std::endl;
	auto pi = policy();
	for(auto& p : pi){
		auto& s = p.first;
		auto& a = p.second.first;
		auto& util = p.second.second;
		s.print();
		a.print();
		std::cout << "WEIGHT : " << util << std::endl;
	}
	std::cout << "STACK SIZE : " << pi.size() << std::endl;
	std::cout << "==============" << std::endl;
}
