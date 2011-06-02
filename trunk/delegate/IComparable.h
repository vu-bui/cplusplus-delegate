#ifndef DELEGATE_ICOMPARABLE_H
#define DELEGATE_ICOMPARABLE_H

namespace delegate {
template<typename T>
struct IComparable {
	virtual bool operator==(const T&) const = 0;
};
}

#endif
