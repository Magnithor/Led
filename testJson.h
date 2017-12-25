#include <cppunit/extensions/HelperMacros.h>

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

  void testEquality()
  {
    CPPUNIT_ASSERT( true );
  }

  void testAddition()
  {
    CPPUNIT_ASSERT( true );
  }

  static CppUnit::Test *suite()
  {
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Json" );
    suiteOfTests->addTest( new CppUnit::TestCaller<JsonTest>( 
                                   "testEquality", 
                                   &JsonTest::testEquality ) );
    suiteOfTests->addTest( new CppUnit::TestCaller<JsonTest>(
                                   "testAddition",
                                   &JsonTest::testAddition ) );
    return suiteOfTests;
  }
};