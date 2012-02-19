
#include "FWCore/Utilities/interface/Exception.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <assert.h>

using namespace cms;

struct Thing
{
  Thing():x() { }
  explicit Thing(int xx):x(xx) { }
  int x;
};

std::ostream& operator<<(std::ostream& os, const Thing& t)
{
  os << "Thing(" << t.x << ")";
  return os;
}

const char expected[] =   "---- InfiniteLoop BEGIN\n"
			   "In func1\n"
			   "---- DataCorrupt BEGIN\n"
			   "This is just a test: \n"
			   "double: 1.11111\n"
			   "float:  2.22222\n"
			   "uint:   75\n"
			   "string: a string\n"
			   "char*:  a nonconst pointer\n"
			   "char[]: a c-style array\n"
			   "Thing:  Thing(4)\n"
			   "\n"
			   "double: 1.111110e+00\n"
			   "float:  2.22e+00\n"
			   "char*:  ..a nonconst pointer\n"
			   "\n"
			   "---- DataCorrupt END\n"
			   "Gave up\n"
			   "---- InfiniteLoop END\n";

void func3()
{
  double d = 1.11111;
  float f = 2.22222;
  unsigned int i = 75U;
  std::string s("a string");
  char* c1 = const_cast<char *>("a nonconst pointer");
  char c2[] = "a c-style array";
  Thing thing(4);

  //  throw cms::Exception("DataCorrupt") 
  cms::Exception e("DataCorrupt");
  e << "This is just a test: \n"
    << "double: " << d << "\n"
    << "float:  " << f << "\n"
    << "uint:   " << i << "\n"
    << "string: " << s << "\n"
    << "char*:  " << c1 << "\n"
    << "char[]: " << c2 << "\n"
    << "Thing:  " << thing << "\n"
    << std::endl
    << "double: " << std::scientific << d << "\n"
    << "float:  " << std::setprecision(2) << f << "\n"
    << "char*:  " << std::setfill('.') << std::setw(20) << c1 << "\n"
    << std::endl;

  throw e;
}

void func2()
{
  func3();
}

void func1()
{
  try 
    {
      func2();
    }
  catch (Exception& e)
    {
      throw Exception("InfiniteLoop","In func1",e) << "Gave up";
    }
  
}

// const char answer[] = 
//   "---- InfiniteLoop BEGIN\n"
//   "In func2\n"
//   "---- DataCorrupt BEGIN\n"
//   "This is just a test: \n" 
//   "double: 1.11111\n"
//   "float:  2.22222\n"
//   "uint:   4294967295\n"
//   "string: a string\n"
//   "char*:  a nonconst pointer\n"
//   "char[]: a c-style array\n"
//   "Thing:  Thing(4)\n"
//   "\n"
//   "double: 1.111110e+00\n"
//   "float:  2.22e+00\n"
//   "uint:   4294967295\n"
//   "string: a string\n"
//   "char*:  ..a nonconst pointer\n"
//   "char[]: a c-style array\n"
//   "Thing:  Thing(4)\n"
//   "---- DataCorrupt END\n"
//   "Gave up\n"
//   "---- InfiniteLoop END\n"

const char* correct[] = { "InfiniteLoop","DataCorrupt" };

int main()
{
  try {
      func1();
  }
  catch (Exception& e) {
      std::cerr << "*** main caught Exception, output is ***\n"
	   << "(" << e.explainSelf() << ")"
	   << "*** After exception output ***"
	   << std::endl;


      if(e.explainSelf() != expected) {
	  std::cerr << "not right answer\n(" << expected << ")\n"
	       << std::endl;
	  abort();
      }


      std::cerr << "\nCategory name list:\n";

      Exception::CategoryList::const_iterator i(e.history().begin()),
	b(e.history().end());
      
      //if(e.history().size() !=2) abort();
      assert (e.history().size() == 2);

      for(int j=0; i != b; ++i,++j) {
	std::cout << "  " << *i << "\n";
	if(*i != correct[j]) {
	  std::cerr << "bad category " << *i << std::endl; abort();
        }
      }
    }
  return 0; 
}
