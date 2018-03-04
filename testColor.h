#include <cppunit/extensions/HelperMacros.h>
#include "json.h"
#include "PlayBack/colorParse.h"
#include "PlayBack/colorSolid.h"
#include "PlayBack/colorPattern.h"
#include <string>

class ColorTest : public CppUnit::TestFixture  {
private:
public:
  void setUp()
  {
  }

  void tearDown() 
  {
  }

  void testSolidColor(std::string test, bool hasRed, uint8_t red, bool hasGreen, uint8_t green, bool hasBlue, uint8_t blue, bool hasBrightness, uint8_t brightness) {
    json::Object o;
    std::stringstream ss;
    ss << " { \"solidColor\" : { ";
    bool hasValue = false;
    if (hasRed) { ss << "\"red\":"; ss << (int)red; hasValue = true; }
    if (hasGreen) { if (hasValue) { ss << " ,"; } else { hasValue = true; } ss << "\"green\":"; ss << (int)green; }
    if (hasBlue) { if (hasValue) { ss << " ,"; } else { hasValue = true; } ss << "\"blue\":"; ss << (int)blue; }
    if (hasBrightness) { if (hasValue) { ss << " ,"; } else { hasValue = true; } ss << "\"brightness\":"; ss << (int)brightness; }
    ss << "} }";

    std::string json = ss.str();
    int p = o.parse(json);

    CPPUNIT_ASSERT_MESSAGE(test, p == 0);
    Color* color = ColorParse::Parse(&o);
    CPPUNIT_ASSERT_MESSAGE(test, color != NULL);
    ColorSolid* colorSolid = dynamic_cast<ColorSolid*>(color);
    CPPUNIT_ASSERT_MESSAGE(test, colorSolid != NULL);

    if (hasRed) {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->red == red); 
    } else {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->red == 0);       
    }

    if (hasGreen) {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->green == green); 
    } else {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->green == 0);       
    }

    if (hasBlue) {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->blue == blue); 
    } else {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->blue == 0);       
    }

    if (hasBrightness) {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->brightness == brightness); 
    } else {
      CPPUNIT_ASSERT_MESSAGE(test, colorSolid->brightness == 0);       
    }

    delete color;
    color = NULL;
    colorSolid = NULL;
  }

  void testSolid()
  {
    this->testSolidColor(std::string("1,2,3,4"), true, (uint8_t) 1, true, (uint8_t) 2, true, (uint8_t) 3, true, (uint8_t) 4);
    this->testSolidColor(std::string("1,225,3,4"), true, (uint8_t) 1, true, (uint8_t) 255, true, (uint8_t) 3, false, (uint8_t) 4);
  }

  void testPattern() 
  {
   // CPPUNIT_ASSERT(false);
  }


  static CppUnit::Test *suite()
  {
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Color" );

    suiteOfTests->addTest( new CppUnit::TestCaller<ColorTest>( 
                                   "test solidColor", 
                                   &ColorTest::testSolid ) );
    suiteOfTests->addTest( new CppUnit::TestCaller<ColorTest>( 
                                   "test patternColor", 
                                   &ColorTest::testPattern ) );
    
    return suiteOfTests;
  }
};
