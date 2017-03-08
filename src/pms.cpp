
#include <pms.h>

////////////////////////////////////////

#if ! defined NOMINMAX

#if defined min
#undef min
#endif
template <class T>
inline const T& __attribute__( ( always_inline ) ) min( const T& a, const T& b ) {
    return !( b < a ) ? a : b;
}

#endif

////////////////////////////////////////

inline void __attribute__( ( always_inline ) ) swapEndianBig16( uint16_t *x ) {
    constexpr union {
        // endian.test16 == 0x0001 for low endian
        // endian.test16 == 0x0100 for big endian
        // should be properly optimized by compiler
        uint16_t test16;
        uint8_t test8[ 2 ];
    } endian = { .test8 = { 1,0 } };

    if ( endian.test16 != 0x0100 ) {
        uint8_t hi = ( *x & 0xff00 ) >> 8;
        uint8_t lo = ( *x & 0xff );
        *x = lo << 8 | hi;
    }
}

////////////////////////////////////////

void sumBuffer( uint16_t *sum, const uint8_t *buffer, uint16_t cnt ) {
    for ( ; cnt > 0; --cnt, ++buffer ) {
        *sum += *buffer;
    }
}

inline void sumBuffer( uint16_t *sum, const uint16_t data ) {
    *sum += ( data & 0xFF ) + ( data >> 8 );
}

////////////////////////////////////////

void Pms5003::setTimeout( decltype( timeout ) timeout ) {
    pmsSerial.setTimeout( timeout );
    this->timeout = timeout;
};

decltype(Pms5003::timeout) Pms5003::getTimeout( void ) {
    return timeout;
};

Pms5003::Pms5003() : dataSize{ 0 }, passive( tri( unknown ) ), sleep( tri( unknown ) ) {
#if defined PMS_DYNAMIC
    begin();
#endif
};

Pms5003::~Pms5003() {
#if defined PMS_DYNAMIC
    end();
#endif
}

bool Pms5003::begin( void ) {
    pmsSerial.setTimeout( Pms5003::timeoutPassive );
    if ( !pmsSerial.begin( 9600 ) ) {
        return false;
    }
    return true;
};

void Pms5003::end( void ) {
    pmsSerial.end();
};

int Pms5003::available( void ) {
    return pmsSerial.available();
}

Pms5003::PmsStatus Pms5003::read( pmsData *data, const size_t nData ) {

    while ( available() ) {
        if ( pmsSerial.peek() != sig[ 0 ] ) {
            pmsSerial.read();
        } else {
            break;
        }
    }

    if ( static_cast<size_t>( available() ) < ( dataSize + 2 ) * sizeof( pmsData ) + sizeof( sig ) ) {
        return noData;
    }

    const pmsData maxFrameLen = 2 * 0x1c;    // arbitrary
    if ( !pmsSerial.find( (uint8_t*) &sig[ 0 ], sizeof( sig ) ) ) {
        return noData;
    }
    uint16_t sum{ 0 };
    sumBuffer( &sum, (uint8_t *) &sig, sizeof( sig ) );

    pmsData thisFrameLen{ 0x1c };
    if ( pmsSerial.readBytes( (uint8_t*) &thisFrameLen, sizeof( thisFrameLen ) ) != sizeof( thisFrameLen ) ) {
        return readError;
    };

    if ( thisFrameLen % 2 != 0 ) {
        return frameLenMismatch;
    }
    sumBuffer( &sum, thisFrameLen );

    swapEndianBig16( &thisFrameLen );
    if ( thisFrameLen > maxFrameLen ) {
        return frameLenMismatch;
    }

    size_t toRead{ min( thisFrameLen - 2, nData * sizeof( pmsData ) ) };
    if ( data == nullptr ) {
        toRead = 0;
    }
    if ( toRead ) {
        if ( pmsSerial.readBytes( (uint8_t*) data, toRead ) != toRead ) {
            return readError;
        }
        sumBuffer( &sum, (uint8_t*) data, toRead );

        for ( size_t i = 0; i < nData; ++i ) {
            swapEndianBig16( &data[ i ] );
        }
    }

    pmsData crc;
    for ( ; toRead < thisFrameLen; toRead += 2 ) {
        if ( pmsSerial.readBytes( (uint8_t*) &crc, sizeof( crc ) ) != sizeof( crc ) ) {
            return readError;
        };

        if ( toRead < thisFrameLen - 2 )
            sumBuffer( &sum, crc );
    }

    swapEndianBig16( &crc );

    if ( sum != crc ) {
        return sumError;
    }

    dataSize = thisFrameLen / 2 - 1;
    return OK;
}

Pms5003::pmsData Pms5003::getDataSize( void ) {

    //        auto timeout_ = getTimeout();

    while ( dataSize == 0 ) {

        if ( passive ) {
            write( cmdReadData );
        }


        //@TODO

        pmsSerial.setTimeout( timeoutActive );



        auto err = read( nullptr, 0 );
        if ( err == noData ) {
            passive = true;
            delay( wakeupTime );
        }
    };
    return dataSize;
}

bool Pms5003::write( PmsCmd cmd ) {
    switch ( cmd ) {
        case cmdReadData:
            break;
        case cmdModePassive:
            passive = tri( true );
            break;
        case cmdModeActive:
            passive = tri( false );
            break;
        case cmdSleep:
            sleep = tri( true );
            break;
        case cmdWakeup:
            sleep = tri( false );
            break;
        default:
            return false;
    }
    static_assert( sizeof( cmd ) >= 3, "Wrong definition of PmsCmd (too short)" );

    if ( pmsSerial.write( sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return false;
    }
    size_t cmdSize = 3;
    if ( pmsSerial.write( (uint8_t*) &cmd, cmdSize ) != cmdSize ) {
        return false;
    }

    uint16_t sum{ 0 };
    sumBuffer( &sum, sig, sizeof( sig ) );
    sumBuffer( &sum, (uint8_t*) &cmd, cmdSize );
    swapEndianBig16( &sum );
    if ( pmsSerial.write( (uint8_t*) &sum, sizeof( sum ) ) != sizeof( sum ) ) {
        return false;
    }

    switch ( cmd ) {
        case cmdModePassive:
//@@@            setTimeout( timeoutPassive );
            break;
        case cmdModeActive:
            setTimeout( timeoutActive );
            break;
        case cmdWakeup:
            delay( wakeupTime );
            break;
        default:
            break;
    }
    return true;
}

const char * Pms5003::errorMsg[ nValues_PmsStatus ]{
    "OK",
    "noData",
    "readError",
    "frameLenMismatch",
    "sumError"
};

const char *Pms5003::metrics[ ]{
    "mcg/m3",
    "mcg/m3",
    "mcg/m3",

    "mcg/m3",
    "mcg/m3",
    "mcg/m3",

    "/0.1L",
    "/0.1L",
    "/0.1L",
    "/0.1L",
    "/0.1L",
    "/0.1L",

    "?"
};

const char *Pms5003::dataNames[ ]{
    "PM1.0, CF=1",
    "PM2.5, CF=1",
    "PM10.  CF=1",
    "PM1.0",
    "PM2.5",
    "PM10.",

    "Particles < 0.3 micron",
    "Particles < 0.5 micron",
    "Particles < 1.0 micron",
    "Particles < 2.5 micron",
    "Particles < 5.0 micron",
    "Particles < 10. micron",

    "Reserved data[0]"
};
