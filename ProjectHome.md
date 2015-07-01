The project provides the following items:
  * a C++ API library simplifying programmatic interaction with a [Maxima](http://maxima.sourceforge.net/) process
  * an RPC service based on the [ZeroC Ice](http://www.zeroc.com/ice.html) middleware which allows for accessing Maxima remotely
  * a JNI wrapper for the C++ API library enabling its use from Java
  * an adapter which allows for writing services for [EveREST](https://www.assembla.com/spaces/mathcloud/documents) container in [Jython](http://www.jython.org/)
  * a RESTful web-service for EveREST written in Jython which enables access to Maxima

The goal is to facilitate distributed symbolic computations.

BuildInstructions describes how to build the Maxima Service. UsingMaximaService describes how to start a server that will provide access to a Maxima Service and how to access the service programmatically.

The project has been partially supported by Russian Fund of Basic Researches, grant #07-07-00411-а