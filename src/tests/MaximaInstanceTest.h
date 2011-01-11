/* 
Copyright (c) 2007, Sergey Smirnov <sasmir (at) gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of contributors may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "cxxtest/TestSuite.h"
#include <MaximaAPI/MaximaInstance.h>
#include <string.h>

using namespace MaximaAPI;

class MaximaInstanceTest : public CxxTest::TestSuite,
    public MaximaInstance::MaximaIOHook
{
    MaximaInstance *maxima;
    
    void launchMaxima(const std::string &workingDir = MAXIMA_TEST_DIR)
    {
        const char *maximaPath = getenv("MAXIMA_APP");
        maximaPath = maximaPath ? maximaPath : MAXIMA_APP_PATH;
        TS_ASSERT(NULL != maximaPath);
        TS_ASSERT(0 != strlen(maximaPath));
        maxima = new MaximaInstance(maximaPath, workingDir, MAXIMA_TEST_DIR);
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
        launchMaxima(std::string(MAXIMA_TEST_DIR) + "/..");
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

