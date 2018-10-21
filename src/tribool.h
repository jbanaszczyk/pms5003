#ifndef _JB_LIBRARIES_TRIBOOL_H_
#define _JB_LIBRARIES_TRIBOOL_H_

// Class contains Arduino port of Boost.Tribool
// http://www.boost.org/doc/libs/1_67_0/doc/html/tribool.html
//
// Changes
//   not implemented: macro BOOST_TRIBOOL_THIRD_STATE(Name)
//   indeterminate state is named "unknown"
//
// Ported by https://github.com/jbanaszczyk

#include <inttypes.h>

namespace jb {
	namespace logic {
		struct unknown_t {};

		class tribool;
		constexpr bool unknown(tribool arg, unknown_t dummy = unknown_t());
		typedef bool(*unknown_keyword_t)(tribool, unknown_t);

		class tribool {
		public:
			enum : int8_t { false_value, true_value, unknown_value } value;
			constexpr tribool() : value(unknown_value) {}
			constexpr tribool(const bool v) : value(v ? true_value : false_value) {};
			constexpr tribool(unknown_keyword_t) : value(unknown_value) {}
			constexpr operator bool() const {
				return value == true_value ? true : false;
			};

			constexpr tribool operator!() const {
				return value == false_value
					? tribool(true)
					: value == true_value ? tribool(false) : tribool(unknown);
			};

			bool isBool() const { return value == false_value || value == true_value; }

			explicit constexpr operator char() const {return value == true_value ? '1' : value == false_value ? '0' : '?'; }
		};

		constexpr tribool operator&&(const tribool lhs, const tribool rhs) {
			return (static_cast<bool>(!lhs) || static_cast<bool>(!rhs))
				? tribool(false)
				: ((static_cast<bool>(lhs) && static_cast<bool>(rhs)) ? tribool(true) : unknown)
				;
		}

		constexpr tribool operator&&(tribool lhs, bool rhs) { return rhs ? lhs : tribool(false); }
		constexpr tribool operator&&(bool lhs, tribool rhs) { return lhs ? rhs : tribool(false); }
		constexpr tribool operator&&(unknown_keyword_t, tribool lhs) { return !lhs ? tribool(false) : tribool(unknown); }
		constexpr tribool operator&&(tribool lhs, unknown_keyword_t) { return !lhs ? tribool(false) : tribool(unknown); }

		constexpr tribool operator||(tribool lhs, tribool rhs) {
			return (static_cast<bool>(!lhs) && static_cast<bool>(!rhs))
				? tribool(false)
				: ((static_cast<bool>(lhs) || static_cast<bool>(rhs)) ? tribool(true) : tribool(unknown))
				;
		}

		constexpr tribool operator||(tribool lhs, bool rhs) { return rhs ? tribool(true) : lhs; }
		constexpr tribool operator||(bool lhs, tribool rhs) { return lhs ? tribool(true) : rhs; }
		constexpr tribool operator||(unknown_keyword_t, tribool lhs) { return lhs ? tribool(true) : tribool(unknown); }
		constexpr tribool operator||(tribool lhs, unknown_keyword_t) { return lhs ? tribool(true) : tribool(unknown); }

		constexpr tribool operator==(tribool lhs, tribool rhs) {
			return (unknown(lhs) || unknown(rhs))
				? unknown
				: ((lhs && rhs) || (!lhs && !rhs))
				;
		}
		constexpr tribool operator==(tribool lhs, bool rhs) { return lhs == tribool(rhs); }
		constexpr tribool operator==(bool lhs, tribool rhs) { return tribool(lhs) == rhs; }
		constexpr tribool operator==(unknown_keyword_t, tribool lhs) { return tribool(unknown) == lhs; }
		constexpr tribool operator==(tribool lhs, unknown_keyword_t) { return tribool(unknown) == lhs; }

		constexpr tribool operator!=(tribool lhs, tribool rhs) {
			return (unknown(lhs) || unknown(rhs))
				? unknown
				: !((lhs && rhs) || (!lhs && !rhs))
				;
		}
		constexpr tribool operator!=(tribool lhs, bool rhs) { return lhs != tribool(rhs); }
		constexpr tribool operator!=(bool lhs, tribool rhs) { return tribool(lhs) != rhs; }
		constexpr tribool operator!=(unknown_keyword_t, const tribool lhs) { return tribool(unknown) != lhs; }
		constexpr tribool operator!=(const tribool lhs, unknown_keyword_t) { return lhs != tribool(unknown); }

		constexpr bool unknown(tribool arg, unknown_t dummy) { return arg.value == tribool::unknown_value; };
	}
}

#endif
