#include <FastLED.h>

#define piezoPin 3

#define LED_PIN     4
#define NUM_LEDS    15
#define BRIGHTNESS  25
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 10

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

int motion = 0;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
    
    SetupRedAndGreenPalette();
    currentBlending = LINEARBLEND;
    
    pinMode(piezoPin, INPUT);

    GIMSK |= (1 << PCIE);   // pin change interrupt enable
    PCMSK |= (1 << PCINT3); // pin change interrupt enabled for PCINT3
    sei();                  // enable interrupts
}

ISR(PCINT0_vect)
{
    if(motion < 250) ChangePalette();
    motion = 255;
}

void loop()
{
    static uint8_t startIndex = 0;
    startIndex = (startIndex + 1); /* motion speed */
    
    FillLEDsFromPaletteColors(startIndex, motion);

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);

    //startIndex = map(analogRead(piezoPin), 0, 1023, 0, NUM_LEDS);

    if(motion > 0) motion--;
}

void FillLEDsFromPaletteColors(uint8_t colorIndex, uint8_t brightness)
{
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void XmasLEDs(uint8_t colorIndex, uint8_t brightness)
{
    CRGB red = CHSV( HUE_RED, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);

    for( int i = 0; i < NUM_LEDS; i++) {
        if(colorIndex % 2 == i % 2)
            leds[i] = red;
        else
            leds[i] = green;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalette()
{
    static uint8_t next = 0;
    next = (next + 1) % 21;

    if(next % 2 == 1) {
      SetupRedAndGreenPalette();
      currentBlending = LINEARBLEND;
      return;
    }
    
    switch(next / 2) {
        case  0:
          currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; break;
        case  1:
          currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  break;
        case  2:
          currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; break;
        case  3:
          SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; break;
        case  4:
          SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; break;
        case  5:
          SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; break;
        case  6:
          SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; break;
        case  7:
          currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; break;
        case  8:
          currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; break;
        case  9:
          currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  break;
        case 10:
          currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; break;
    }

}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

void SetupRedAndGreenPalette()
{
    CRGB red = CHSV( HUE_RED, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    
    currentPalette = CRGBPalette16(
                                   green,  red,  green,  red,
                                   green,  red,  green,  red,
                                   green,  red,  green,  red,
                                   green,  red,  green,  red );
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
