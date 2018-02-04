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

    CPPUNIT_ASSERT( o.count () == 0);
    CPPUNIT_ASSERT( o.json() == "{}" );
    o.set(std::string("count"), 4);
    o.set(std::string("d"), 4.2);
    CPPUNIT_ASSERT_MESSAGE(o.json(), o.json() == "{\"count\":4,\"d\":4.2}" );
    CPPUNIT_ASSERT( o.count () == 2);    
  }

  void testParse(){    
    json::Object o;
    size_t pos = 0;
    std::stringstream ss;
    std::string value = std::string(" { \"count\" : 4 } ");
    int p = o.parse(pos, value);
    ss << pos << " " << value << " " << value.length() << " p =" << p << "\n";
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.count() == 1);
    CPPUNIT_ASSERT(o.count () == 1);
    CPPUNIT_ASSERT(o.hasKey(std::string("count")));
   // CPPUNIT_ASSERT( o.get(std::string("count"));
    o.parse(std::string("{ }"));
    CPPUNIT_ASSERT( o.count () == 0);

    ss.clear(); pos = 0;
    value = std::string("{\"d\":2.12, \"green\":30, \"red\":-49, \"false\":false, \"true\":true, \"null\": null, \"str\":\"str\", \"blue\":42, \"brightness\":1}");
    p = o.parse(pos, value);
    ss << pos << " " << value << " " << value.length() << " p =" << p << " count = " << o.count() << "\n";
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.count() == 9);

    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("d")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.get(std::string("d"))->isDouble());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), !o.get(std::string("d"))->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.get(std::string("d"))->getDouble() == 2.12);
    
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("green")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("green")))->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("green")))->isDouble());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("green")))->getInt() == 30);
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.get(std::string("green"))->getDouble() == 30.0);

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


    ss.clear(); pos = 0;
    value = std::string("{\"array\":[30,1], \"object\":{\"n\":49}}");
    p = o.parse(pos, value);
    ss << pos << " " << value << " " << value.length() << " p =" << p << " count = " << o.count() << "\n";
    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.count() == 2);

    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("array")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("array")))->isArray());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("array")))->getArray()->count() == 2);
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("array")))->getArray()->get(0)->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("array")))->getArray()->get(0)->getInt() == 30);
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("array")))->getArray()->get(1)->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("array")))->getArray()->get(1)->getInt() == 1);

    CPPUNIT_ASSERT_MESSAGE(ss.str(), o.hasKey(std::string("object")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("object")))->isObject());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("object")))->getObject()->count() == 1);
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("object")))->getObject()->hasKey(std::string("n")));
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("object")))->getObject()->get(std::string("n"))->isInt());
    CPPUNIT_ASSERT_MESSAGE(ss.str(), (o.get(std::string("object")))->getObject()->get(std::string("n"))->getInt()==49);
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
