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

