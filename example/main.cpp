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
		TTTAction(int i, int j);
		TTTAction(int ID);
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
				size_t hash();
				size_t ID() const;
				bool operator==(const Trait& t);
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
		TTTState(TTTState prev, Action& a);
		static void setSize(int,int);
		
		//T,R
		virtual State* next(Action& a);
		virtual After&& next(); //vector of Action*s, auto management
		virtual double reward();
		virtual double reward(Action& a);

		//utility
		virtual State* copy();
		virtual void print() const;
		void computeReward();
};
class TTTQ : public Q{
	public:
		//logic
		virtual double alpha(const SA&);
		virtual double max(const S&);
		virtual void advance(int i);

		//utility
		virtual void print() const;
		virtual std::unordered_map<S,std::pair<A,double>> policy() const;
		virtual void printPolicy() const;
		virtual size_t size() const;
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
enum TTTState::Cell : char{EMPTY,O,X};
using Cell = TTTState::Cell;

size_t Trait::hash(){
	if(_hash==0){
		auto& board = *this;
		size_t C = board[0] + board[width-1] + board[(height-1)*width] + board[(height*width)-1];
		C <<= 3;
		size_t S = board[1] + board[1*width] + board[2*width-1] + board[2*width+1];
		S <<= 6;
		size_t M = board[1*width + 1]; // just for now
		return _hash = ((17*M + S)*31+C)+7;
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
		for(int j=0;j<=i;++j){
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
	turn = O;
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
After&& TTTState::next(){
	After res;
	for(auto i=0;i<height;++i){
		for(auto j=0;j<width;++j){
			res.push_back(new TTTAction(i,j));
		}
	}
	return std::move(res);
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

		//utility
TTTState::State* TTTState::copy(){
	return new TTTState(*this);
}

void TTTState::print() const{
	return board.print();
}
