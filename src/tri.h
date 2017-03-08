#ifndef _tri_h_
#define _tri_h_

#include <arduino.h>

class tri;

struct unknown_t { };
constexpr inline bool unknown( tri arg, unknown_t dummy = unknown_t() );
typedef bool( *unknown_keyword_t )( tri, unknown_t );

class tri {
public:
    enum : int8_t { false_v, true_v, unknown_v } value;
    constexpr tri() : value( unknown_v ) { }
    constexpr tri( bool v ) : value( v ? true_v : false_v ) { };
    constexpr tri( unknown_keyword_t ) : value( unknown_v ) { }
    constexpr inline operator bool() const {
        return value == tri::true_v ? true : false;
    };

    constexpr inline tri operator!() const {
        return value == false_v
            ? tri( true )
            : value == true_v ? tri( false ) : tri( unknown );
    };

    bool isBool() const { return value == false_v || value == true_v; }
};

constexpr inline tri operator&&( tri lhs, tri rhs ) {
    return ( static_cast<bool>( !lhs ) || static_cast<bool>( !rhs ) )
        ? tri( false )
        : ( ( static_cast<bool>( lhs ) && static_cast<bool>( rhs ) ) ? tri( true ) : unknown )
        ;
}

constexpr inline tri operator&&( tri lhs, bool rhs ) { return rhs ? lhs : tri( false ); }
constexpr inline tri operator&&( bool lhs, tri rhs ) { return lhs ? rhs : tri( false ); }
constexpr inline tri operator&&( unknown_keyword_t, tri lhs ) { return !lhs ? tri( false ) : tri( unknown ); }
constexpr inline tri operator&&( tri lhs, unknown_keyword_t ) { return !lhs ? tri( false ) : tri( unknown ); }

constexpr inline tri operator||( tri lhs, tri rhs ) {
    return ( static_cast<bool>( !lhs ) && static_cast<bool>( !rhs ) )
        ? tri( false )
        : ( ( static_cast<bool>( lhs ) || static_cast<bool>( rhs ) ) ? tri( true ) : tri( unknown ) )
        ;
}

constexpr inline tri operator||( tri lhs, bool rhs ) { return rhs ? tri( true ) : lhs; }
constexpr inline tri operator||( bool lhs, tri rhs ) { return lhs ? tri( true ) : rhs; }
constexpr inline tri operator||( unknown_keyword_t, tri lhs ) { return lhs ? tri( true ) : tri( unknown ); }
constexpr inline tri operator||( tri lhs, unknown_keyword_t ) { return lhs ? tri( true ) : tri( unknown ); }

constexpr inline tri operator==( tri lhs, tri rhs ) {
    return ( unknown( lhs ) || unknown( rhs ) )
        ? unknown
        : ( ( lhs && rhs ) || ( !lhs && !rhs ) )
        ;
}
constexpr inline tri operator==( tri lhs, bool rhs ) { return lhs == tri( rhs ); }
constexpr inline tri operator==( bool lhs, tri rhs ) { return tri( lhs ) == rhs; }
constexpr inline tri operator==( unknown_keyword_t, tri lhs ) { return tri( unknown ) == lhs; }
constexpr inline tri operator==( tri lhs, unknown_keyword_t ) { return tri( unknown ) == lhs; }

constexpr inline tri operator!=( tri lhs, tri rhs ) {
    return ( unknown( lhs ) || unknown( rhs ) )
        ? unknown
        : !( ( lhs && rhs ) || ( !lhs && !rhs ) )
        ;
}
constexpr inline tri operator!=( tri lhs, bool rhs ) { return lhs != tri( rhs ); }
constexpr inline tri operator!=( bool lhs, tri rhs ) { return tri( lhs ) != rhs; }
constexpr inline tri operator!=( unknown_keyword_t, tri lhs ) { return tri( unknown ) != lhs; }
constexpr inline tri operator!=( tri lhs, unknown_keyword_t ) { return lhs != tri( unknown ); }

#endif
