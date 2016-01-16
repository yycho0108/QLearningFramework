#ifndef __STATE_H__
#define __STATE_H__

#include <vector>
#include "Action.h"
class State{
	//subclass & fill in traits as necessary.
	public:
		virtual ~State();
		//State(State&,Action);

		//comparator
		virtual bool operator==(const State&);
		virtual size_t hash() const;
		
		//T,R
		virtual State* next(Action& a);// = return new State(*this,a); ...or something along the lines
		virtual std::vector<Action*> next();	
		virtual double reward();
		virtual double reward(Action& a);
		
		//utility
		virtual State* copy();
		virtual void print() const;
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
		virtual State next(Action& a);
		virtual std::vector<State> next();
		virtual double reward();
		virtual double reward(Action& a);

		//utility
		virtual void print() const;
};

*/
#endif
