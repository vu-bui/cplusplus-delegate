/* Delegate2.h
 * Crossword Puzzle Generator
 * Assignment 2, COSC2131 Programming Using C++
 * RMIT International University Vietnam
 *
 * Copyright 2011 Bui Luan Vu (s3245903)
 *
 * Refer to the NOTICE file in the root of the source
 * acknowledgements of third party works used in this
 *
 * Date created: 20/04/2011
 * Date last modified: 20/04/2011
 */
#ifndef DELEGATE_DELEGATE2_H
#define DELEGATE_DELEGATE2_H

#include <list>

namespace delegate {
//base function pointer template
template<typename P1, typename P2, typename R>
struct PointerToFunction2 {
	virtual ~PointerToFunction2() {}
	//call the function
	virtual R operator()(P1, P2) = 0;
	//compare two functions
	virtual bool operator==(const PointerToFunction2<P1, P2, R>&) const = 0;
};

//pointer to static or extern "C" function template
template<typename P1, typename P2, typename R>
class PointerToStaticFunction2 : public PointerToFunction2<P1, P2, R> {
private:
	typedef R (*delegate_fn)(P1, P2);
	delegate_fn fn;

public:
	explicit PointerToStaticFunction2(delegate_fn _fn) : fn(_fn) {}
	~PointerToStaticFunction2() {}

	R operator()(P1 _param1, P2 _param2) {
		return (*fn)(_param1, _param2);
	}

	bool operator==(const PointerToFunction2<P1, P2, R>& _pts) const {
		const PointerToStaticFunction2<P1, P2, R>* pts = dynamic_cast<const PointerToStaticFunction2<P1, P2, R>*>(&_pts); 
		if(pts != 0) {
			return fn == pts->fn;
		}
		return false;
	}
};

//pointer to member function template
template<typename C, typename P1, typename P2, typename R>
class PointerToMemberFunction2 : public PointerToFunction2<P1, P2, R> {
private:
	typedef R (C::*delegate_fn)(P1, P2);
	C& obj;
	delegate_fn fn;

public:
	explicit PointerToMemberFunction2(C& _obj, delegate_fn _fn) : obj(_obj), fn(_fn) {}
	~PointerToMemberFunction2() {}

	R operator()(P1 _param1, P2 _param2) {
		return (obj.*fn)(_param1, _param2);
	}

	bool operator==(const PointerToFunction2<P1, P2, R>& _ptm) const {
		const PointerToMemberFunction2<C, P1, P2, R>* ptm = dynamic_cast<const PointerToMemberFunction2<C, P1, P2, R>*>(&_ptm);
		if(ptm != 0) {
			return &obj == &ptm->obj && fn == ptm->fn;
		}
		return false;
	}
};

template<typename P1 = void, typename P2 = void, typename R = void>
class Delegate2 {
private:
	std::list<PointerToFunction2<P1, P2, R>*> delegates;
	//typename must exist for P and R are unknown and can be mix up with *
	typedef typename std::list<PointerToFunction2<P1, P2, R>*>::iterator Iterator;

public:
	//copy constructor
	Delegate2(const Delegate2<P1, P2, R>& _delegate) {
		for(Iterator it = _delegate.delegates.begin(); it != _delegate.delegates.end(); it++) {
			delegates.push_back(new PointerToFunction2<P1, P2, R>(*it));
		}
	}
	//assignment operator
	Delegate2<P1, P2, R>& operator=(const Delegate2<P1, P2, R>& _delegate) {
		delegates.clear();
		for(Iterator it = _delegate.delegates.begin(); it != _delegate.delegates.end(); it++) {
			delegates.push_back(new PointerToFunction2<P1, P2, R>(*it));
		}
		return *this;
	}

	explicit Delegate2() {}
	explicit Delegate2(R (*_fn)(P1, P2)) {
		add(_fn);
	}
	template<typename C>
	explicit Delegate2(C& _obj, R (C::*_fn)(P1, P2)) {
		add(_obj, _fn);
	}
	~Delegate2() {
		for(Iterator it = delegates.begin(); it != delegates.end(); it++) {
			delete *it;
		}
	}

	//add static function
	void add(R (*_fn)(P1, P2)) {
		delegates.push_back(new PointerToStaticFunction2<P1, P2, R>(_fn));
	}

	//add member function
	template<typename C>
	void add(C& _obj, R (C::*_fn)(P1, P2)) {
		delegates.push_back(new PointerToMemberFunction2<C, P1, P2, R>(_obj, _fn));
	}

	//remove static function
	bool remove(R (*_fn)(P1, P2)) {
		PointerToStaticFunction2<P1, P2, R> pts(_fn);
		for(Iterator it = delegates.begin(); it != delegates.end(); it++) {
			if(pts == **it) {
				delegates.erase(it);
				return true;
			}
		}
		return false;
	}

	//remove member function
	template<typename C>
	bool remove(C& _obj, R (C::*_fn)(P1, P2)) {
		PointerToMemberFunction2<C, P1, P2, R> ptm(_obj, _fn);
		for(Iterator it = delegates.begin(); it != delegates.end(); it++) {
			if(ptm == **it) {
				delegates.erase(it);
				return true;
			}
		}
		return false;
	}

	//invoke methods
	R operator()(P1 _param1, P2 _param2) {
		if(delegates.size() > 0) {
			for(Iterator it = delegates.begin(); it != --delegates.end(); it++) {
				(*it)->operator()(_param1, _param2);
			}
			return delegates.back()->operator()(_param1, _param2);
		}
	}
};
}

#endif
