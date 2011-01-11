/* Jython adapter for RESTful web-services in EveREST

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
*/

package ru.isa.dcs.ssmir;

import org.python.core.PyObject;
import org.python.core.PyString;
import org.python.util.PythonInterpreter;
import java.util.Map;
import java.io.File;

import everest.java.JavaServiceI;
import everest.JobStatus;

public class JythonService implements JavaServiceI {
	@Override
	public void init(Map<String, String> config) {
		if (!config.containsKey("pythonClass")) {
			throw new RuntimeException("Missing 'pythonClass' option in the config");
		}
		String pythonClass = config.get("pythonClass");
		String pythonPackage = config.containsKey("pythonPackage") ?
			config.get("pythonPackage") : pythonClass;
		
		_interpreter = new PythonInterpreter();
		_interpreter.exec("import sys; sys.path.append('" + new File("lib").getAbsolutePath() + "')");
        _interpreter.exec("from " + pythonPackage + " import " + pythonClass);
        PyObject serviceClass = _interpreter.get(pythonClass);
        PyObject implObj = serviceClass.__call__();
        _impl = (JavaServiceI)implObj.__tojava__(JavaServiceI.class);
        
        _impl.init(config);
        
        _interpreter.exec("print sys.path");
	}
	
	@Override
	public void run(Map<String, Object> inputs, Map<String, Object> outputs,
		String jobId, String jobDir, JobStatus status) throws Exception
	{
		 _impl.run(inputs, outputs, jobId, jobDir, status);
	}
	
	@Override
	public void destroy() {
		_impl.destroy();
		_impl = null;
		_interpreter.cleanup();
		_interpreter = null;
	}
	
	private JavaServiceI _impl;
	private PythonInterpreter _interpreter;
}

