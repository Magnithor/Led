#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "testJson.h"
#include "testColor.h"

int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  runner.addTest( JsonTest::suite() );
  runner.addTest( ColorTest::suite() );
  bool wasSuccessful = runner.run( "", false );     
  return !wasSuccessful;
}