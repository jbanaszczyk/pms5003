#ifndef _JB_LIBRARIES_MINI_MUTEX_H_
#define _JB_LIBRARIES_MINI_MUTEX_H_

#include <util/atomic.h>
#include <tribool.h>

// Class contains Arduino implementation of very basic approach to mutex
//
// Created by https://github.com/jbanaszczyk

namespace jb {
    namespace threads {

        using namespace jb::logic;

        class mutex {
        public:
            mutex(const mutex &) = delete;
            mutex & operator = (const mutex &) = delete;
            mutex() : unlocked(true) {};

            bool try_lock() {
                static_assert (sizeof(unlocked) == 1, "locked : expected single byte");
                bool result;
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    result = unlocked;
                    unlocked = false;
                }
                return result;
            }

            void unlock() {
                unlocked = true;
            }

        public:
            volatile bool unlocked;
        };

        template <typename T = uint8_t, T LOCKED = T{ 0xC3 }, T UNLOCKED = T{ 0x3C } >
        class safe_mutex {
        public:
            safe_mutex(const safe_mutex&) = delete;
            safe_mutex& operator = (const safe_mutex &) = delete;
            safe_mutex() : state(UNLOCKED) {};
            ~safe_mutex() {
                static_assert (mixer(LOCKED, UNLOCKED) != LOCKED, "Function mixer() in destructor should return something different from LOCKED and UNLOCKED value (LOCKED)");
                static_assert (mixer(LOCKED, UNLOCKED) != UNLOCKED, "Function mixer() in destructor should return something different from LOCKED and UNLOCKED value (UNLOCKED)");
                static_assert (LOCKED != UNLOCKED, "LOCKED and UNLOCKED parameters should be different");
                state = mixer(LOCKED, UNLOCKED);
            }

            tribool try_lock() {
                tribool result = unknown;
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    if ((state == LOCKED) || (state == UNLOCKED)) {
                        result = state == UNLOCKED;
                        state = LOCKED;
                    }
                }
                return result;
            }

            void unlock() {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    if (state == LOCKED) {
                        state = UNLOCKED;
                    }
                }
            }

        protected:
            volatile T state;

        private:
            static constexpr T mixer(T lhs, T rhs) {
                return (!lhs) ^ (!rhs);
            }
        };
    }
}

#endif
