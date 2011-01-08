from __future__ import with_statement
from java.lang import *
import re, os, Queue, time, sys
from contextlib import closing
from java.lang import Thread
from java.lang import InterruptedException
from java.lang import Runtime

from ru.isa.dcs.ssmir.maxima import MaximaInstance
from everest.java import JavaServiceI

from MaximaService import packagePath
from threadpool import *

System.load(os.path.abspath("lib/" + System.mapLibraryName("MaximaInstance")))

class MaximaInstance2(MaximaInstance):
    def __init__(self):
        self.queue = Queue.Queue(1)

class MaximaService(JavaServiceI):
    def __init__(self):
        pass

    def launchMaximaInstance(self):
        maxima = MaximaInstance2()
        maxima.initialize(self._maximaPath, packagePath)
        self._maximaPool.put(maxima)

    def init(self, config):
        #self._numWorkers = 1
        self._numWorkers = Runtime.getRuntime().availableProcessors()
        self._maximaPath = config["maximaPath"]
        self._threadPool = ThreadPool(self._numWorkers)
        self._maximaPool = Queue.Queue(self._numWorkers)
        for i in range(self._numWorkers):
            self.launchMaximaInstance()
            
    def destroy(self):
        self._threadPool.dismissWorkers(self._numWorkers, True)
        while True:
            try:
                self._maximaPool.get_nowait().destroy()
            except Queue.Empty:
                break
            
    def workerTask(self, maxima, params, jobDir, status):
        result = None
        try:
            maxima.executeCommand("kill(all)")
            if "script.mac" in params:
                maxima.executeCommand("load(\"%s\");" % params["script.mac"].getAbsolutePath())
            if "data.lsp" in params:
                maxima.executeCommand("load(\"%s\");" % params["data.lsp"].getAbsolutePath())
            maxima.executeCommand("outputFilePath : \"%s\"" % os.path.abspath(jobDir + "/output.lsp"))
            
            result = maxima.executeCommand(params["command"])
            open(os.path.normpath(jobDir + "/output.lsp"), "a").close()
            maxima.queue.put(result)
        except:
            maxima.queue.put(sys.exc_info())
        return None;

    def run(self, params, jobDir, status):
        try:
            self._threadPool.poll()
        except NoResultsPending:
            pass
        
        try:
            while True:
                try:
                    maxima = self._maximaPool.get_nowait()
                    break
                except Queue.Empty:
                    time.sleep(0.1)
        except KeyboardInterrupt:
            raise InterruptedException

        try:
            self._threadPool.putRequest(WorkRequest(self.workerTask, [maxima],
                {"params" : params, "jobDir" : jobDir, "status" : status}))
            
            while True:
                try:
                    result = maxima.queue.get_nowait()
                    break
                except Queue.Empty:
                    time.sleep(0.1)
                    
            if isinstance(result, tuple):
                raise result[0], result[1], result[2]
            return {"result" : result, "output.lsp" : "output.lsp"}
        except KeyboardInterrupt:
            maxima.interruptMaxima()
            maxima.queue.get()
            raise InterruptedException
        finally:
            self._maximaPool.put(maxima)

