/*  remote-maxima - a CAS Maxima C++ API and a Grid-service
	Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published
    by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "cxxtest/TestSuite.h"
#include <MaximaAPI/MaximaInstance.h>
#include <string.h>

using namespace MaximaAPI;

class MaximaInstanceTest : public CxxTest::TestSuite,
    public MaximaInstance::MaximaIOHook
{
    MaximaInstance *maxima;
    
    void launchMaxima(const std::string &workingDir = ".")
    {
        char *maximaPath = getenv("MAXIMA_APP");
        TS_ASSERT(NULL != maximaPath);
        maxima = new MaximaInstance(maximaPath, workingDir);
        TS_ASSERT(NULL != maxima);
    }
    
public:
    void setUp(void)
    {
        maxima = NULL;
    }
    
    void tearDown(void)
    {
        delete maxima;
    }
    
    void testGood(void)
    {
        launchMaxima();
        TS_ASSERT_EQUALS("5", maxima->executeCommand("3+2"));
    }
    
    void testBugWithEscapedBackslashOnTheEnd()
    {
        launchMaxima();
        TS_ASSERT_THROWS_ANYTHING(maxima->executeCommand("\\;"));
    }
    
    void testBugWithDoubleSemicolon()
    {
        launchMaxima();
        TS_ASSERT_THROWS_ANYTHING(maxima->executeCommand("3;;"));
    }
    
    void testBugWithBigMultiexpression()
    {
        launchMaxima();
        TS_ASSERT_THROWS_ANYTHING(maxima->executeCommand("3+4; asksign(a); 4;"));
    }
    
    virtual std::string handle(const std::string &first,
        const std::string &second)
    {
        TS_ASSERT_EQUALS("load", first);
        TS_ASSERT_EQUALS("test123.ma", second);
        return second + "c";
    }
    
    void testMaximaIOHook()
    {
        launchMaxima();
        maxima->setMaximaIOHook("(load)\\s*\\(\"([^\"]*)\"\\)", this);
        std::string result = maxima->executeCommand("load(\"test123.ma\");");
        
        bool good = result == "?test123\\.mac" || result == "\"test123.mac\"";
        if (!good)
        {
            TS_TRACE(result);
        }
        TS_ASSERT(good);
    }
    
    void testWorkingDirectory()
    {
        launchMaxima("..");
        TS_ASSERT_THROWS_ANYTHING(
            maxima->executeCommand("load(\"test123.mac\")"));
    }
    
    void testBadPrompt()
    {
        launchMaxima();
        TS_ASSERT_THROWS_ANYTHING(maxima->executeCommand("asksign(a);"));
    }
    
    void testInterrupt()
    {
        launchMaxima();
        maxima->interruptMaxima();
        TS_ASSERT_EQUALS("123", maxima->executeCommand("123"));
    }
    
    void testInterrupt2()
    {
        launchMaxima();
        TS_ASSERT(maxima->executeAndInterrupt("factorial(100000)", 1));
    }
};

