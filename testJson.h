#include <cppunit/extensions/HelperMacros.h>
#include "json.h"
#include <string>

class JsonTest : public CppUnit::TestFixture  {
private:
  //Complex *m_10_1, *m_1_1, *m_11_2;
public:
  void setUp()
  {
    /*m_10_1 = new Complex( 10, 1 );
    m_1_1 = new Complex( 1, 1 );
    m_11_2 = new Complex( 11, 2 );  */
  }

  void tearDown() 
  {
    /*delete m_10_1;
    delete m_1_1;
    delete m_11_2;*/
  }

  void testSerialize()
  {
    json::Object o;

    CPPUNIT_ASSERT( o.getCount() == 0);
    CPPUNIT_ASSERT( o.json() == "{}" );
    o.set(std::string("count"), 4);
    CPPUNIT_ASSERT( o.json() == "{\"count\":4}" );
    CPPUNIT_ASSERT( o.getCount() == 1);    
  }

  void testParse(){    
    json::Object o;
    size_t pos = 0;
    std::stringstream ss;
    std::string value = std::string(" { \"count\" : 4 } ");
    int p = o.parse(pos, value);
    ss << pos << " " << value << " " << value.length() << " p =" << p << "\n";
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.getCount() == 1);
    CPPUNIT_ASSERT(o.getCount() == 1);
    CPPUNIT_ASSERT(o.hasKey(std::string("count")));
   // CPPUNIT_ASSERT( o.get(std::string("count"));
    o.parse(std::string("{ }"));
    CPPUNIT_ASSERT( o.getCount() == 0);

    ss.clear(); pos = 0;
    value = std::string("{\"green\":30, \"red\":-49, \"false\":false, \"true\":true, \"null\": null, \"str\":\"str\", \"blue\":42, \"brightness\":1}");
    p = o.parse(pos, value);
    ss << pos << " " << value << " " << value.length() << " p =" << p << " count = " << o.getCount() << "\n";
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.getCount() == 8);
    
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("green")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("green")))->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("green")))->getInt() == 30);

    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("red")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("red")))->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("red")))->getInt() == -49);
  
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("false")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("false")))->isBool());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), !(o.get(std::string("false")))->getBool());

    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("true")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("true")))->isBool());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("true")))->getBool());
    
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("null")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("null")))->isNull());

    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("str")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("str")))->isString());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("str")))->getString().compare(std::string("str")) == 0);
  }

  void testParseString(){
    std::string str = std::string("\"Prófun\"");
    std::string str2 =  std::string("Prófun");
    std::string value;
    std::stringstream ss;
    size_t pos = 0;
    json::parseString(pos, str, value);

    int compare = value.compare(str2);
    ss << "\"" << value << "\" " << compare << " " << str;
    CPPUNIT_ASSERT_MESSAGE(ss.str(), compare == 0);
    ss.clear();
  }

  static CppUnit::Test *suite()
  {
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Json" );
    suiteOfTests->addTest( new CppUnit::TestCaller<JsonTest>( 
                                   "testEqtestSerializeuality", 
                                   &JsonTest::testSerialize ) );
    suiteOfTests->addTest( new CppUnit::TestCaller<JsonTest>(
                                   "testParse",
                                   &JsonTest::testParse ) );
    suiteOfTests->addTest( new CppUnit::TestCaller<JsonTest>(
                                   "testParseString",
                                   &JsonTest::testParseString ) );
    return suiteOfTests;
  }
};