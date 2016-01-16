#ifndef __ACTION_H__
#define __ACTION_H__

#include <cstddef>
#include <functional> //hash
class Action{ //subclass as needed
	protected:
		size_t _ID; //this seems to be necessary for comparison.
		//esp. for an action, there are only so much actions available,
		//so an _ID should be sufficient for comparison.
	public:
		//Action(); -- do not know how it will be initialized
		virtual ~Action();
		virtual Action* copy()=0;
		virtual bool operator==(const Action&) const;
		//provide some identification
		virtual size_t hash() const;
		size_t ID() const;
		//visualization for convenience
		virtual void print() const;
};
// example
/*
class TTTAction : public Action{
	private:
		static int w,h;
		size_t _hash;
	public:
		static void setSize(int,int);
		virtual bool operator==(const Action&) const;
		virtual size_t hash() const;
		virtual void print() const;
};
*/
namespace std{
	template<>
	struct hash<Action>{
		size_t operator()(const Action& x) const{
			return x.hash();
		}
	};
}
#endif
