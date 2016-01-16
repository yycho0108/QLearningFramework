#ifndef __STATE_H__
#define __STATE_H__

#include <vector>
#include "Action.h"
class State{
	//subclass & fill in traits as necessary.
	public:
		struct After:public std::vector<Action*>{
			After();
			After(After&&);
			After(const After&)=delete;
			~After();
		};
		virtual ~State();
		//State(State&,Action);

		//comparator
		virtual bool operator==(const State&) const=0;
		virtual size_t hash() const=0;
		
		//T,R
		virtual State* next(Action& a)=0;// = return new State(*this,a);
		virtual void  next(After&)=0;// = return vector of new actions	
		virtual double reward()=0;
		virtual double reward(Action& a)=0;
		
		//utility
		virtual State* copy()=0;
		virtual void print() const=0;
		//double max();
};

//example

/*
 * class TTTState : public State{
	private:
		static int w,h;
		enum Cell:char;
		struct Trait : public std::vector<Cell>{
			bool operator==(const Trait& t);
			size_t hash() const;
			void flip();
			void rotate();
			void transpose();
			void print() const;
			Cell checkH(int) const;
			Cell checkV(int) const;
			Cell CheckD(int) const;
		};
		Cell turn;
		Trait board;
	public:
		static void setSize(int,int);
		
		//T,R
		virtual State* next(Action& a);
		virtual std::vector<Action*> next();
		virtual double reward();
		virtual double reward(Action& a);

		//utility
		virtual State* copy();
		virtual void print() const;
};

*/
#endif
