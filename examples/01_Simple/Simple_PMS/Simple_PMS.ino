#include <Arduino.h>

#include <pms.h>


////////////////////////////////////////

#if defined PMS_DYNAMIC
Pms5003 *_pms;
#define pms (*_pms)
#else
Pms5003 pms;
#endif

////////////////////////////////////////

auto lastRead = millis();

void setup( void ) {
    Serial.begin( 115200 );
    while ( !Serial ) { };
    Serial.println( "PMS5003" );

#if defined PMS_DYNAMIC
    _pms = new Pms5003();
#else
    pms.begin();
#endif

    //    pms.write( Pms5003::cmdWakeup );
    //    pms.write( Pms5003::cmdModePassive );

    //    Serial.println( pms.getDataSize() );
    //    pms.write( Pms5003::cmdModeActive );
}

////////////////////////////////////////

void loop( void ) {


    const int n = Pms5003::Reserved;
    Pms5003::pmsData data[ n ];

    Pms5003::PmsStatus status = pms.read( data, n );

    switch ( status ) {
        case Pms5003::OK:
        {

            Serial.println( "_________________" );
            auto newRead = millis();
            Serial.print( "Wait time " );
            Serial.println( newRead - lastRead );
            lastRead = newRead;


            for ( size_t i = Pms5003::PM1dot0; i < n; ++i ) {
                Serial.print( data[ i ] );
                Serial.print( "\t" );
                Serial.print( Pms5003::dataNames[ i ] );
                Serial.print( " [" );
                Serial.print( Pms5003::metrics[ i ] );
                Serial.print( "]" );
                Serial.println();
            }
            break;
        }
        case Pms5003::noData:
            break;
        default:
            Serial.println( "_________________" );
            Serial.println( Pms5003::errorMsg[ status ] );
    };
}
