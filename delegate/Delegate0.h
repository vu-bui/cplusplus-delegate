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
#ifndef DELEGATE_DELEGATE0_H
#define DELEGATE_DELEGATE0_H

#include <list>
#include "IComparable.h"

namespace delegate {
//base function pointer template
template<typename R>
struct PointerToFunction0 : IComparable<PointerToFunction<R> > {
	virtual ~PointerToFunction0() {}
	//call the function
	virtual R operator()() = 0;
};

//pointer to static or extern "C" function template
template<typename R>
class PointerToStaticFunction0 : public PointerToFunction0<R> {
private:
	typedef R (*delegate_fn)();
	delegate_fn fn;

public:
	explicit PointerToStaticFunction0(delegate_fn _fn) : fn(_fn) {}
	~PointerToStaticFunction0() {}

	R operator()() {
		return (*fn)();
	}

	bool operator==(const PointerToFunction0<R>& _pts) const {
		const PointerToStaticFunction0<R>* pts = dynamic_cast<const PointerToStaticFunction0<R>*>(&_pts);
		if(pts != 0) {
			return fn == pts->fn;
		}
		return false;
	}
};

//pointer to member function template
template<typename C, typename R, typename delegate_fn = R (C::*)()>
class PointerToMemberFunction0 : public PointerToFunction0<R> {
private:
	C& obj;
	delegate_fn fn;

public:
	explicit PointerToMemberFunction0(C& _obj, delegate_fn _fn) : obj(_obj), fn(_fn) {}
	~PointerToMemberFunction0() {}

	R operator()() {
		return (obj.*fn)(_param);
	}

	bool operator==(const PointerToFunction0<R>& _ptm) const {
		const PointerToMemberFunction0<C, R>* ptm = dynamic_cast<const PointerToMemberFunction0<C, R>*>(&_ptm);
		if(ptm != 0) {
			return &obj == &ptm->obj && fn == ptm->fn;
		}
		return false;
	}
};

template<typename R = void>
class Delegate {
private:
	std::list<PointerToFunction0<R>*> delegates;
	//typename must exist for R are unknown and can be mix up with *
	typedef typename std::list<PointerToFunction0<R>*>::iterator Iterator;

	bool remove(const PointerToFunction0<R>& _pt) {
		for(Iterator it = delegates.begin(); it != delegates.end(); ++it) {
			if(_pt == **it) {
				delete *it;
				delegates.erase(it);
				return true;
			}
		}
		return false;
	}

public:
	//copy constructor
	Delegate(const Delegate<R>& _delegate) {
		operator=(_delegate);
	}
	//assignment operator
	Delegate<R>& operator=(const Delegate<R>& _delegate) {
		delegates.clear();
		for(Iterator it = _delegate.delegates.begin(); it != _delegate.delegates.end(); ++it) {
			delegates.push_back(new PointerToFunction0<R>(*it));
		}
		return *this;
	}

	explicit Delegate() {}
	explicit Delegate(R (*_fn)()) {
		add(_fn);
	}
	template<typename C>
	explicit Delegate(C& _obj, R (C::*_fn)()) {
		add(_obj, _fn);
	}
	~Delegate() {
		for(Iterator it = delegates.begin(); it != delegates.end(); ++it) {
			delete *it;
		}
	}

	//add static function
	void add(R (*_fn)()) {
		delegates.push_back(new PointerToStaticFunction0<R>(_fn));
	}

	//add member function
	template<typename C>
	void add(C& _obj, R (C::*_fn)()) {
		delegates.push_back(new PointerToMemberFunction0<C, R>(_obj, _fn));
	}

	//add const member function
	template<typename C>
	void add(const C& _obj, R (C::*_fn)() const) {
		delegates.push_back(new PointerToMemberFunction0<const C, R, R (C::*)() const>(_obj, _fn));
	}

	//remove static function
	bool remove(R (*_fn)()) {
		return remove(PointerToStaticFunction0<R>(_fn));
	}

	//remove member function
	template<typename C>
	bool remove(C& _obj, R (C::*_fn)()) {
		return remove(PointerToMemberFunction0<C, R>(_obj, _fn));
	}

	//remove const member function
	template<typename C>
	bool remove(const C& _obj, R (C::*_fn)() const) {
		return remove(PointerToMemberFunction0<const C, R, R (C::*)() const>(_obj, _fn));
	}

	//invoke methods
	R operator()() {
		if(delegates.size() > 0) {
			for(Iterator it = delegates.begin(); it != --delegates.end(); ++it) {
				(*it)->operator()();
			}
			return delegates.back()->operator()();
		}
	}
};
}

#endif
