#!/usr/bin/env python

import httplib, time
from contextlib import closing

for i in range(20):
    with closing(httplib.HTTPConnection("localhost", 9999)) as conn:
        conn.request("POST", "/maxima", '{"command" : "factorial(100000)"}',
            {"Content-Type" : "application/json"})
        print "Request was made"
        response = conn.getresponse()
        location = response.getheader("Location")
        print "Response:", response.status, response.reason, "Location:", location
    
    #time.sleep(2)
    
    with closing(httplib.HTTPConnection("localhost", 9999)) as conn:
        conn.request("DELETE", location)
        print "Made DELETE request"
        r1 = conn.getresponse()
        print r1.status, r1.reason
        print r1.read()

