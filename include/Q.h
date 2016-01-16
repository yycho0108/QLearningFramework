#ifndef __Q_H__
#define __Q_H__

#include "Action.h"
#include "State.h"
#include <unordered_map>

class S;
class A;
class SA;
class Q;

class S{ //serve as containers (to prevent pointer-comparison)
	friend class A;
	friend class Q; //actually should be inside Q
	private:
		State* s;
	public:
		S(State* s=nullptr);
		S(const S&);
		//S(State&&);
		~S();
		//comparator
		bool operator==(const S&) const;
		size_t hash() const;

		//utility
		double reward();
		S next(A);
		void print() const;
};

namespace std{
	template<>
	struct hash<S>{
		size_t operator()(const S& x) const{
			return x.hash();
		}
	};
}

class A{ //serve as containers (to prevent pointer-comparison)
	friend class S;
	friend class Q;
	private:
		Action* a;
	public:
		A(Action* a=nullptr);
		A(const A&);
		//A(Action&&);
		~A();
		bool operator==(const A&) const;
		//return *a==*a
		size_t hash() const;
		void print() const;
};

namespace std{
	template<>
	struct hash<A>{
		size_t operator()(const A& x) const{
			return x.hash();
		}
	};
}

//Perhaps S and A are not strictly necessary?
//

class SA{ //State-Action pair
	friend class Q;
	private:
		S _s;
		A _a;
		S _next; //for convenience, S.next(A);
		size_t _hash; //for convenience, in case hash is costly
		double _reward;
	public:
		SA(S,A);//calculate _hash
		SA(const SA&);
		SA(SA&&) = default; // = member-wise move
		virtual ~SA();
		//comparator
		virtual bool operator==(const SA&) const;
		//in case a special comparison is needed
		//subclass SA
		//and cast S and A to use class-specific functions.
		virtual size_t hash() const;
		virtual void print() const;
		virtual const S& next() const;
		//in case a special hash is needed?
		//getter
		const S& s() const;
		const A& a() const;
		const double& reward() const;
};

namespace std{
	template<>
	struct hash<SA>{
		size_t operator()(const SA& x) const{
			return x.hash();
		}
	};
}

class Q{
	protected:
		//static double alpha;
		static double GAMMA; //learning rat
		size_t t; //epoch
		std::unordered_map<SA,double> qHat; //weight value cache
		//OR REPLACE qHat with neural network? double qH(SA)
		//in this case, SA must be decomposed into attributes.
		std::unordered_map<SA,int> visit; //update count
	public:
		static void setGAMMA(double);
		Q();
		//logic
		virtual double alpha(const SA&);
		virtual double max(const S&);
		virtual void advance(int);
		
		//utility
		virtual void print() const;
		virtual std::unordered_map<S,std::pair<A,double>> policy() const; //is this necessary...?
		virtual void printPolicy() const;
		virtual size_t size() const;
};

//example

/*
 * class TTTQ : public Q{
	private:
		static double gamma;
	public:
		//logic
		virtual double alpha(const SA&);
		virtual double max(const S&);
		virtual void advance();

		//utility
		virtual void print() const;
		virtual size_t size() const;
};
*/
#endif
