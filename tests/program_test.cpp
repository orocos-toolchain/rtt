
#include "program_test.hpp"
#include <unistd.h>
#include <iostream>
#include <execution/ProgramGraph.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProgramTest );


void 
ProgramTest::setUp()
{
    factory.methodFactory().registerObject("test", this->createMethodFactory() );
}


void 
ProgramTest::tearDown()
{
}


bool ProgramTest::assertBool( bool b) { 
    return b;
}
bool ProgramTest::assertMsg( bool b, const std::string& msg) {
    return b;
}


    MethodFactoryInterface* ProgramTest::createMethodFactory()
    {
        // Add the data of the EE:
        TemplateMethodFactory< ProgramTest >* dat =
            newMethodFactory( this );

        dat->add( "assert", data( &ProgramTest::assertBool,
                                  "Assert", "bool", "") );
        dat->add( "assertMsg", data( &ProgramTest::assertMsg,
                                     "Assert message", "bool", "", "text", "text" ) );
        return dat;
    }


void ProgramTest::testEmptyProgram()
{
    string prog = "";
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, &processor, &factory );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT( false );
        }
    if ( !pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
}

void ProgramTest::testReturnProgram()
{
    string prog = "program x { return \n }";
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, &processor, &factory );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE(exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
    // execute
    CPPUNIT_ASSERT( (*pg_list.begin())->executeToStop() );
}

void ProgramTest::testTypes()
{
    string prog = string("program x {\n") +
        "var int i = 0\n" +
        //"var char c = 'c'\n" +
        "var double d = 0.0\n"+
        "var bool b = false\n"+
        "var string s=\"string\"\n"+
        "var double6d d6 = double6d(0.,0.,0.,0.,0.,0.)\n"+
        "var double6d d6_2 = double6d(0.0)\n"+
        "var vector v = vector(0.,0.,0.)\n"+
        "var rotation r = rotation(0.,0.,0.) \n"+
        "var frame f = frame(v,r) \n"+
        "const int ic = i\n" +
        //"const char cc = c\n" +
        "const double dc = d\n"+
        "const bool bc = b\n"+
        "const string sc=s\n"+
        "const double6d d6c = d6\n"+
        "const vector vc = v\n"+
        "const rotation rc = r\n"+
        "const frame fc = f\n"+
        "set d6[0]=1.0\n"+
        "set d6[1]=d6[0]\n"+
        "set d6[2]=d6[0]\n"+
        "set d6[3]=d6[0]\n"+
        "set d6[4]=d6[0]\n"+
        "set d6[5]=d6[0]\n"+
        "}";
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, &processor, &factory );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE(exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
    // execute
    CPPUNIT_ASSERT( (*pg_list.begin())->executeToStop() );
}

void ProgramTest::testOperators()
{
    string prog = string("program x {\n") +
        "var int i = 0\n" +
        //"var char c = 'c'\n" +
        "var double d = 0.0\n"+
        "var bool b = false\n"+
        "var string s=\"string\"\n"+
        "var double6d d6 = double6d(0.,1.,2.,3.,4.,5.)\n"+
        "var double6d d6_2 = double6d(0.0)\n"+
        "var vector v = vector(0.,0.,0.)\n"+
        "var rotation r = rotation(0.,0.,0.) \n"+
        "var frame f = frame(v,r) \n"+
        "set d = d+1.0-2.0*1.0/2.0 \n"+
        "set b = b or b and true && false || true\n"+
        "set s = s+\"abc\"\n"+
        "set d6 = (d6 + double6d(2.) ) * d6 - d6\n"+
        "}";
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, &processor, &factory );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE(exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
    // execute
    CPPUNIT_ASSERT( (*pg_list.begin())->executeToStop() );
}

    

