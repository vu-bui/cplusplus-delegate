/* Delegate.h
 * Microcontroller Emulation
 * Assignment 1, COSC2131 Programming Using C++
 * RMIT International University Vietnam
 *
 * Copyright 2011 Bui Luan Vu (s3245903)
 *
 * Refer to the NOTICE file in the root of the source
 * acknowledgements of third party works used in this
 *
 * Date created: 22/03/2011
 * Date last modified: 22/03/2011
 */
#ifndef DELEGATE_DELEGATE1_H
#define DELEGATE_DELEGATE1_H

#include <list>

namespace delegate {
//base function pointer template
template<typename P, typename R>
struct PointerToFunction1 {
	virtual ~PointerToFunction1() {}
	//call the function
	virtual R operator()(P) = 0;
	//compare two functions
	virtual bool operator==(const PointerToFunction1<P, R>&) const = 0;
};

//pointer to static or extern "C" function template
template<typename P, typename R>
class PointerToStaticFunction1 : public PointerToFunction1<P, R> {
private:
	typedef R (*delegate_fn)(P);
	delegate_fn fn;

public:
	explicit PointerToStaticFunction1(delegate_fn _fn) : fn(_fn) {}
	~PointerToStaticFunction1() {}

	R operator()(P _param) {
		return (*fn)(_param);
	}

	bool operator==(const PointerToFunction1<P, R>& _pts) const {
		const PointerToStaticFunction1<P, R>* pts = dynamic_cast<const PointerToStaticFunction1<P, R>*>(&_pts);
		if(pts != 0) {
			return fn == pts->fn;
		}
		return false;
	}
};

//pointer to member function template
template<typename C, typename P, typename R, typename delegate_fn = R (C::*)(P)>
class PointerToMemberFunction1 : public PointerToFunction1<P, R> {
private:
	C& obj;
	delegate_fn fn;

public:
	explicit PointerToMemberFunction1(C& _obj, delegate_fn _fn) : obj(_obj), fn(_fn) {}
	~PointerToMemberFunction1() {}

	R operator()(P _param) {
		return (obj.*fn)(_param);
	}

	bool operator==(const PointerToFunction1<P, R>& _ptm) const {
		const PointerToMemberFunction1<C, P, R>* ptm = dynamic_cast<const PointerToMemberFunction1<C, P, R>*>(&_ptm);
		if(ptm != 0) {
			return &obj == &ptm->obj && fn == ptm->fn;
		}
		return false;
	}
};

template<typename P = void, typename R = void>
class Delegate1 {
private:
	std::list<PointerToFunction1<P, R>*> delegates;
	//typename must exist for P and R are unknown and can be mix up with *
	typedef typename std::list<PointerToFunction1<P, R>*>::iterator Iterator;

public:
	//copy constructor
	Delegate1(const Delegate1<P, R>& _delegate) {
		for(Iterator it = _delegate.delegates.begin(); it != _delegate.delegates.end(); ++it) {
			delegates.push_back(new PointerToFunction1<P, R>(*it));
		}
	}
	//assignment operator
	Delegate1<P, R>& operator=(const Delegate1<P, R>& _delegate) {
		delegates.clear();
		for(Iterator it = _delegate.delegates.begin(); it != _delegate.delegates.end(); ++it) {
			delegates.push_back(new PointerToFunction1<P, R>(*it));
		}
		return *this;
	}

	explicit Delegate1() {}
	explicit Delegate1(R (*_fn)(P)) {
		add(_fn);
	}
	template<typename C>
	explicit Delegate1(C& _obj, R (C::*_fn)(P)) {
		add(_obj, _fn);
	}
	~Delegate1() {
		for(Iterator it = delegates.begin(); it != delegates.end(); ++it) {
			delete *it;
		}
	}

	//add static function
	void add(R (*_fn)(P)) {
		delegates.push_back(new PointerToStaticFunction1<P, R>(_fn));
	}

	//add member function
	template<typename C>
	void add(C& _obj, R (C::*_fn)(P)) {
		delegates.push_back(new PointerToMemberFunction1<C, P, R>(_obj, _fn));
	}

	//add const member function
	template<typename C>
	void add(const C& _obj, R (C::*_fn)(P) const) {
		delegates.push_back(new PointerToMemberFunction1<const C, P, R, R (C::*)(P) const>(_obj, _fn));
	}

	//remove static function
	bool remove(R (*_fn)(P)) {
		PointerToStaticFunction1<P, R> pts(_fn);
		for(Iterator it = delegates.begin(); it != delegates.end(); ++it) {
			if(pts == **it) {
				delete *it;
				delegates.erase(it);
				return true;
			}
		}
		return false;
	}

	//remove member function
	template<typename C>
	bool remove(C& _obj, R (C::*_fn)(P)) {
		PointerToMemberFunction1<C, P, R> ptm(_obj, _fn);
		for(Iterator it = delegates.begin(); it != delegates.end(); ++it) {
			if(ptm == **it) {
				delete *it;
				delegates.erase(it);
				return true;
			}
		}
		return false;
	}

	//remove const member function
	template<typename C>
	bool remove(const C& _obj, R (C::*_fn)(P) const) {
		PointerToMemberFunction1<const C, P, R, R (C::*)(P) const> ptm(_obj, _fn);
		for(Iterator it = delegates.begin(); it != delegates.end(); ++it) {
			if(ptm == **it) {
				delete *it;
				delegates.erase(it);
				return true;
			}
		}
		return false;
	}

	//invoke methods
	R operator()(P _param) {
		if(delegates.size() > 0) {
			for(Iterator it = delegates.begin(); it != --delegates.end(); ++it) {
				(*it)->operator()(_param);
			}
			return delegates.back()->operator()(_param);
		}
	}
};
}

#endif
