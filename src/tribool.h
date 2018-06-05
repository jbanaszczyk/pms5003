#pragma once

#include <Arduino.h>

// Class contains Arduino port of Boost.Tribool
// http://www.boost.org/doc/libs/1_67_0/doc/html/tribool.html
//
// Changes
//   indeterminate state is named unknown
//   there is a lack of macro BOOST_TRIBOOL_THIRD_STATE(Name)
//   no namespaces
//
// Ported by https://github.com/jbanaszczyk

struct unknown_t {};

class tribool;
constexpr bool unknown(tribool arg, unknown_t dummy = unknown_t());
typedef bool(*unknown_keyword_t)(tribool, unknown_t);

class tribool {
public:
	enum : int8_t { false_v, true_v, unknown_v } value;
	constexpr tribool() : value(unknown_v) {}
	constexpr tribool(bool v) : value(v ? true_v : false_v) {};
	constexpr tribool(unknown_keyword_t) : value(unknown_v) {}
	constexpr operator bool() const {
		return value == tribool::true_v ? true : false;
	};

	constexpr tribool operator!() const {
		return value == false_v
			? tribool(true)
			: value == true_v ? tribool(false) : tribool(unknown);
	};

	bool isBool() const { return value == false_v || value == true_v; }
};

constexpr tribool operator&&(tribool lhs, tribool rhs) {
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
constexpr tribool operator!=(unknown_keyword_t, tribool lhs) { return tribool(unknown) != lhs; }
constexpr tribool operator!=(tribool lhs, unknown_keyword_t) { return lhs != tribool(unknown); }

constexpr bool unknown(tribool arg, unknown_t dummy) { return arg.value == tribool::unknown_v; };
