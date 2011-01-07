/*  Maxima JNI API
    Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>
    
    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file BOOST_LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

package ru.isa.dcs.ssmir.maxima;

public class MaximaInstance {
	public native void initialize(String maximaPath, String utilsDir) throws MaximaException;
	
	public native void destroy() throws MaximaException;
	
	public native String executeCommand(String command) throws MaximaException;
	
	public native void interruptMaxima() throws MaximaException;
	
	private long _ptr;
}

