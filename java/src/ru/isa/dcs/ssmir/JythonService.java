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

