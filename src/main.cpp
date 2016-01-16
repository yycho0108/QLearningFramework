#include "Utility.h" //for random
#include "Action.h"
#include "Q.h"
#include "State.h"

#include <iostream>
#include <algorithm>
/* *** CLASS DECLARATION *** */
class TTTAction : public Action{
	private:
		static int width,height;
	public:
		TTTAction(int,int);
		TTTAction(int);
		virtual Action* copy();
		static void setSize(int,int);
		//virtual bool operator==(const Action&) const;
		//virtual size_t hash() const;
		virtual void print() const;
};
class TTTState : public State{
	public:	
		using After = State::After;
		enum Cell:char;
		struct Trait : public std::vector<Cell>{
			private://size_t _ID=0;
				size_t _hash=0;
			public:
				size_t hash() const;
				size_t ID() const;
				bool operator==(const Trait&);
				void flip();
				void rotate();
				void transpose();
				void print() const;
				Cell checkH(int) const;
				Cell checkV(int) const;
				Cell checkD(bool) const;
		};
	private:
		static int width,height;
		Cell turn;
		Trait board;
		double _reward;
		bool done;
	public:
		TTTState();
		TTTState(const TTTState&);
		TTTState(TTTState, Action&);
		static void setSize(int,int);
		
		//T,R
		virtual State* next(Action&);
		virtual void next(After&); //vector of Action*s, auto management
		virtual double reward();
		virtual double reward(Action&);

		virtual bool operator==(const State&) const;
		virtual size_t hash() const;
		//utility
		void flip();
		void rotate();
		virtual State* copy();
		virtual void print() const;
		void computeReward();
};

class TTTSA : public SA{
	public:
		virtual bool operator==(const SA&) const;
		TTTSA(S&,A&)=delete;//calculate _hash
		TTTSA(S&& s,A&& a); //forward the constructors!
		TTTSA(const SA& sa);
		TTTSA(SA&& sa);
};

class TTTQ : public Q{
	public:
		//logic
		virtual double alpha(const SA&);
		virtual double max(const S&);
		//virtual void advance(int);

		//utility
		//virtual void print() const;
		//virtual std::unordered_map<S,std::pair<A,double>> policy() const;
		//virtual void printPolicy() const;
		//virtual size_t size() const;
		void init(int,int);
};

/* *** CLASS DEFINITION *** */

// TTTAction

int TTTAction::width;
int TTTAction::height;

TTTAction::TTTAction(int i, int j){
	_ID = i*width + j;
}
TTTAction::TTTAction(int _ID){
	this->_ID = _ID;
}
void TTTAction::setSize(int w, int h){
	width=w,height=h;
}

void TTTAction::print() const{
	std::cout << "(" << _ID/width << "," << _ID%width << ")" << std::endl;
}
Action* TTTAction::copy(){
	return new TTTAction(ID());
}
// TTTState

int TTTState::width=0;
int TTTState::height=0;
void TTTState::setSize(int w, int h){
	width=w,height=h;
}
//Trait
using Trait = TTTState::Trait;
using After = TTTState::After;
enum TTTState::Cell : char{EMPTY=1,O,X}; //primes
using Cell = TTTState::Cell;

size_t Trait::hash() const{
	if(_hash==0){
		// initialize hash
		auto& board = *this;
		size_t C = board[0] + board[width-1] + board[(height-1)*width] + board[(height*width)-1];
		C <<= 3;
		size_t S = board[1] + board[1*width] + board[2*width-1] + board[2*width+1];
		S <<= 6;
		size_t M = board[1*width + 1]; // just for now
		return const_cast<Trait&>(board)._hash = ((17*M + S)*31+C)+7;
	}else{
		return _hash;
	}
}

size_t Trait::ID() const{
	size_t res=0;
	for(auto& c : *this){
		res *= 3;
		res += c;
	}
	return res;	
}

bool Trait::operator==(const Trait& t){
	return ID() == t.ID();
}
void Trait::flip(){
	for(int i=0; i<height; ++i){
		std::reverse(begin()+width*i, begin()+width*(i+1));
	}
}
void Trait::rotate(){
	transpose();
	flip();
}
void Trait::transpose(){
	for(int i=0; i<height;++i){
		for(int j=0;j<i;++j){ //don't need to flip middle
			std::swap((*this)[i*width+j], (*this)[j*width+i]);
		}
	}
}
void Trait::print() const{
	auto& board = *this;
	for(int i=0; i<height; ++i){
		for(int j=0; j<width; ++j){
			switch(board[i*width+j]){
				case EMPTY:
					putchar('-');
					break;
				case O:
					putchar('O');
					break;
				case X:
					putchar('X');
					break;
			}
		}
		std::cout << std::endl;
	}
}
Cell Trait::checkH(int i) const{
	auto& board = *this;
	auto ref = board[i*width]; // j =0;
	for(int j=1;j<width;++j){
		if(board[i*width + j] != ref)
			return EMPTY;
	}
	return ref;
}
Cell Trait::checkV(int j) const{
	auto& board = *this;
	auto ref = board[j]; //i=0;
	for(int i=1;i<height;++i){
		if(board[i*width +j] !=ref)
			return EMPTY;
	}
	return ref;
}
Cell Trait::checkD(bool rise) const{
	auto& board = *this;
	if(rise){
		auto i = height - 1;
		auto j = 0;
		auto ref = board[i*width+j]; //bottom left
		while(j < width){
			if(board[i*width+j] != ref)
				return EMPTY;
			++j, --i;
		}
		return ref;
	}else{
		auto i = 0, j = 0;
		auto ref = board[i*width+j];
		while(j < width){
			if(board[i*width+j] != ref)
				return EMPTY;
			++j, ++i;
		}
		return ref;
	}
	return EMPTY;
}

//TTTState
TTTState::TTTState(){
	done=false;
	for(int i=0;i<height;++i){
		for(int j=0;j<width;++j){
			board.push_back(EMPTY);
			//next.push_back(TTTAction(i,j));
		}
	}
	turn = O;//always start with zero, sir!
	computeReward();
}
TTTState::TTTState(const TTTState& s):
turn(s.turn),
board(s.board),
_reward(s._reward),
done(s.done){
	//nothing
}
TTTState::TTTState(TTTState prev, Action& a){
	std::swap(*this,prev);
	board[a.ID()] = turn;
	turn = (turn == O)?X:O;
}
State* TTTState::next(Action& a){
	return new TTTState(*this,a);
}

void TTTState::next(After& res){
	res.clear();
	for(auto i=0;i<height;++i){
		for(auto j=0;j<width;++j){
			if(board[i*width+j] == EMPTY) //available
				res.push_back(new TTTAction(i,j));
		}
	}
	return;
}

void TTTState::computeReward(){
	done = true;
	for(int i=0;i<height;++i){
		auto tmp = board.checkH(i);
		if(tmp != EMPTY){
			_reward = (tmp==turn)?1.0:-1.0;
			return;
		}
	}
	for(int j=0;j<width;++j){
		auto tmp = board.checkV(j);
		if(tmp != EMPTY){
			_reward = (tmp==turn)?1.0:-1.0;
			return;
		}
	}
	auto tmp = board.checkD(true);
	if(tmp != EMPTY){
		_reward = (tmp==turn)?1.0:-1.0;
		return;
	}
	tmp = board.checkD(false);
	if(tmp != EMPTY){
		_reward = (tmp==turn)?1.0:-1.0;
		return;
	}

	_reward = 0.0;
	done = false;
	return;	
}

double TTTState::reward(){
	return _reward;
}

double TTTState::reward(Action& a){
	auto n = next(a);
	return -n->reward();
	delete n; //definitely not optimal... hmm.
}

size_t TTTState::hash() const{
	return board.hash();
}
bool TTTState::operator==(const State& s) const{
	return board == static_cast<const TTTState&>(s).board;
}
		//utility
TTTState::State* TTTState::copy(){
	return new TTTState(*this);
}

void TTTState::print() const{
	return board.print();
}

void TTTState::flip(){
	board.flip();
}

void TTTState::rotate(){
	board.rotate();
}
/* *** TTTSA *** */
bool TTTSA::operator==(const SA& other) const{

	auto n = static_cast<TTTState*>(const_cast<State*>(this->next().get()));
	auto c = static_cast<TTTState*>(const_cast<State*>(this->s().get()));
	
	for(int i=0;i<4;++i){
		if(*n == *other.next().get() && *c == *other.s().get())
			return true;
		n->rotate();
		c->rotate();
	}
	n->flip();
	c->flip();
	for(int i=0;i<4;++i){
		if(*n == *other.next().get() && *c == *other.s().get())
			return true;
		n->rotate();
		c->rotate();
	}
	n->flip(); //now all is as it was before!
	c->flip();
	//next().flip();
	//std::cout << "FALSE! NOT SAME!" << std::endl;
	return false;
}
TTTSA::TTTSA(S&& s,A&& a):SA(std::move(s),std::move(a)){ // forwarding
};
TTTSA::TTTSA(const SA& sa):SA(sa){
};
TTTSA::TTTSA(SA&& sa):SA(sa){
};

/* *** TTTQ *** */

double TTTQ::alpha(const SA& sa){
	auto v = ++visit[sa];
	return 1.0 / pow(v+1,0.85);
}
double TTTQ::max(const S& s){
	return -Q::max(s);
}
void TTTQ::init(int w, int h){
	TTTAction::setSize(w,h);
	TTTState::setSize(w,h);
	for(int i=0;i<h;++i){
		for(int j=0;j<w;++j){
			qHat.insert(std::make_pair(
						TTTSA(S(new TTTState()),A(new TTTAction(i,j))),
						f() //random weight initialization.
						));
			//perhaps improve so that
			//std::unordered_map<SA,std::pair<double,int>>
			//double = value, int = # visited
			visit.insert(std::make_pair(
						TTTSA(S(new TTTState()),A(new TTTAction(i,j))),
						0 //visited 0 times!
						));

		}
	}
	printPolicy();
	//std::cout << qHat.size() << std::endl;	
}
#include <signal.h>
static volatile bool run = true;
void quitHandler(int){
	run = false;
}
int main(int argc, char* argv[]){
	signal(SIGINT, quitHandler);
	int MAX_ITER=20;
	if(argc==2)
		MAX_ITER=std::atoi(argv[1]);

	TTTQ q;
	q.init(3,3);
	for(int i=0;i<MAX_ITER && run;++i){
		std::cerr << i << std::endl;
		q.advance(1);
	}
	std::cerr << "COMPLETE!" << std::endl;
	//q.print();
	std::cout << std::endl;
	q.printPolicy();
//	std::cout << q.size() << std::endl;
}

