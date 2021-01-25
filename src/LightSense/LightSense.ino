#include <FastLED.h>
#include "pitches.h"

#define LED_PIN_1   5
#define LED_PIN_2   6
#define NUM_LEDS    60
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

#define SPEAKER_PIN 8
#define MELODIES 90 // includes no pitch

int melody[] = {
  0, NOTE_B0, NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1, NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2, NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3, NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7, NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8
};

#define PHOTO_PIN_1 A0
#define PHOTO_PIN_2 A1

#define MIN_PHOTO_VAL 301
#define MAX_PHOTO_VAL 780

int photo1, photo2;

void setup() {
    Serial.begin(9600);
    Serial.println("starting up...");
    delay( 3000 ); // power-up safety delay

    Serial.println("setup leds...");
    FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

void loop()
{
    // retrieve input from sensor
    int photo1 = analogRead(PHOTO_PIN_1);
    int photo2 = analogRead(PHOTO_PIN_2);

    // clamp values
    if (photo1 < MIN_PHOTO_VAL)
    {
      photo1 = MIN_PHOTO_VAL;
    }

    if (photo1 > MAX_PHOTO_VAL)
    {
      photo1 = MAX_PHOTO_VAL;
    }

    // clamp values
    if (photo2 < MIN_PHOTO_VAL)
    {
      photo2 = MIN_PHOTO_VAL;
    }

    if (photo2 > MAX_PHOTO_VAL)
    {
      photo2 = MAX_PHOTO_VAL;
    }

    // setup LEDs 
    ChangePalettePeriodically();

    FillLEDsFromPaletteColors( map (photo1, MIN_PHOTO_VAL, MAX_PHOTO_VAL, 0, NUM_LEDS), leds1);
    FillLEDsFromPaletteColors( map (photo2, MIN_PHOTO_VAL, MAX_PHOTO_VAL, 0, NUM_LEDS), leds2);
    
    FastLED.show();
    FastLED.delay( 1000 / UPDATES_PER_SECOND);
    
    // setup speaker (piezo)
    int photoPart = map(photo1, MIN_PHOTO_VAL, MAX_PHOTO_VAL, 0, 8);
    int photoPart2 = map(photo2, MIN_PHOTO_VAL, MAX_PHOTO_VAL, 0, 9)000;
    int noteIndex = photoPart * 10 + photoPart2;

    int noteDuration = 1000 / 8; // eighth note 
    tone( SPEAKER_PIN, melody[noteIndex], noteDuration);

    // Help the brain distinguish the notes, 
    // set a minimum time between them.
    int pauseBetweenNotes = noteDuration * 1.10;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    noTone(8);
}

void FillLEDsFromPaletteColors( int numLeds, CRGB leds[])
{
    uint8_t brightness = 255;
    uint8_t colorIndex = 1;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
      if (i < numLeds)
      {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
      }
      else 
      {
        leds[i] = 0;
      }
    }
}

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
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


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
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
