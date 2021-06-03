//
// auto_mutex.cpp - auto mutex implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "leccore_common.h"
#include <mutex>

class mutex;

// Wrapper for the std::mutex object.
class liblec::leccore::mutex::impl {
	std::mutex mtx_;

public:
	impl() {}
	~impl() {}

	void lock() {
		mtx_.lock();
	}

	void unlock() {
		mtx_.unlock();
	}
};

liblec::leccore::mutex::mutex() : d_(*new impl()) {}
liblec::leccore::mutex::~mutex() { delete& d_; }

class liblec::leccore::auto_mutex::impl {
	mutex& mtx_;

public:
	impl(mutex& mtx) :
		mtx_(mtx) {
		mtx_.d_.lock();
	}

	~impl() {
		mtx_.d_.unlock();
	}
};

liblec::leccore::auto_mutex::auto_mutex(mutex& mtx) : d_(*new impl(mtx)) {}
liblec::leccore::auto_mutex::~auto_mutex() { delete& d_; }
