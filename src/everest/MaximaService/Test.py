"""  Maxima service for EveREST container

Copyright (c) 2009, Sergey Smirnov <sasmir (at) gmail.com>
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
"""

import unittest
from MaximaService import MaximaService
from MaximaService import packagePath
from ru.isa.dcs.ssmir.maxima import MaximaException
from ru.isa.dcs.ssmir.maxima import MaximaInstance

maximaPath = "/opt/local/bin/maxima"

class MaximaServiceTest(unittest.TestCase):
    def setUp(self):
        self._service = MaximaService()
        self._service.init({"maximaPath" : maximaPath})
        
    def tearDown(self):
        self._service.destroy()
        
    def oneCommand(self):
        self.assertEquals(self._service.run({"command" : "123"}, ".", None),
            {"result" : "123", "output.lsp" : "output.lsp"});
        
    def testOneInvocation(self):
        self.oneCommand()
        
    def testTenInvocations(self):
        for i in range(10):
            self.oneCommand()
            
    def testWorkerTask(self):
        maxima = self._service._maximaPool.get_nowait()
        try:
            self._service.workerTask(maxima, {"command" : ""}, ".", None)
        finally:
            self._service._maximaPool.put(maxima)
        result = maxima.queue.get_nowait()
        self.assertEquals(result[0], MaximaException)
            
    def testSimpleError(self):
        self.assertRaises(MaximaException, lambda: self._service.run({"command" : ""}, ".", None))
        
    def testVeryBadNumber(self):
        self.assertRaises(MaximaException, lambda: self._service.run({"command" : "454sdsd"}, ".", None))

class MaximaInstanceTest(unittest.TestCase):
    def setUp(self):
        self.maxima = MaximaInstance()
        self.maxima.initialize(maximaPath, packagePath)
        
    def tearDown(self):
        self.maxima.destroy()
        
    def testOk(self):
        self.assertEquals(self.maxima.executeCommand("123"), "123")

    def testSimpleError(self):
        self.assertRaises(MaximaException, lambda: self.maxima.executeCommand(""))
        
    def testVeryBadNumber(self):
        self.assertRaises(MaximaException, lambda: self.maxima.executeCommand("454sdsd"))


def startTest():
    suite = unittest.TestLoader().loadTestsFromTestCase(MaximaInstanceTest)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(MaximaServiceTest))
    unittest.TextTestRunner(verbosity=2).run(suite)

