#include "Q.h"
#include <limits>
#include <iostream>
/* =========== S ========== */
S::S(State* s):s(s){
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
S S::next(A a){
	return S(s->next(*a.a));
	//have some doubts as to whether or not this would work.
	//the vtable would be gone.... for one thing.
}
/* =========== A ========== */
A::A(Action* a):a(a){
}
A::A(const A& a):a(a.a->copy()){
}
A::~A(){
	delete a;
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

/* =========== SA ========== */
SA::SA(class S s, class A a):
	_s(s),
	_a(a),
	_next(s.next(a)),
	_hash(s.hash()*37 + a.hash()),
	_reward(_next.reward()){
}

/*
SA::SA(SA&& sa): //== default
	s(std::move(sa.s)),
	a(std::move(sa.a)),
	_hash(sa._hash),
	_reward(sa._reward){
}
*/

SA::~SA(){ //s,a will be automatically deleted
}
bool SA::operator==(const SA& o) const{
	return _s==o._s && _a==o._a;
}

size_t SA::hash() const{
	return _hash;
}

void SA::print() const{
	_s.print();
	_a.print();
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
	auto v = s.s->next();
	for(auto &a : v){
		SA sa(s,a); //automatic conversion!
		auto tmp = 0.0;
		if(qHat.find(sa) == qHat.end()){
			qHat.insert(std::make_pair(sa,0));
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
		for(auto& qh : qHat){
			/* alias */
			auto& sa = qh.first;
			auto& util = qh.second;
			
			//auto& state = SA.S();
			//auto& action = SA.A();
			auto& next = sa.next();
			auto& r = sa.reward();
			auto ALPHA = alpha(sa);

			util = (1-ALPHA)*util + ALPHA *(r + GAMMA*max(next));
		}
	}
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
			pi.insert(std::make_pair(s,std::make_pair(a,std::numeric_limits<double>::max())));
		}else{
			auto& elem = pi.at(s);
			if(elem.second < util){
				elem.first = a;
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
	std::cout << "==============" << std::endl;
}
