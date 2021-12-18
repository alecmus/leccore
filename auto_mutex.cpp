//
// auto_mutex.cpp - auto mutex implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "leccore_common.h"
#include <mutex>

class mutex;

// Wrapper for the std::mutex object.
class liblec::leccore::mutex::impl {
	std::mutex _mtx;

public:
	impl() {}
	~impl() {}

	void lock() {
		_mtx.lock();
	}

	void unlock() {
		_mtx.unlock();
	}
};

liblec::leccore::mutex::mutex() : _d(*new impl()) {}
liblec::leccore::mutex::~mutex() { delete& _d; }

class liblec::leccore::auto_mutex::impl {
	mutex& _mtx;

public:
	impl(mutex& mtx) :
		_mtx(mtx) {
		_mtx._d.lock();
	}

	~impl() {
		_mtx._d.unlock();
	}
};

liblec::leccore::auto_mutex::auto_mutex(mutex& mtx) : _d(*new impl(mtx)) {}
liblec::leccore::auto_mutex::~auto_mutex() { delete& _d; }
