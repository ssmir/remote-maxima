#!/usr/bin/env python
import sys, Ice

comm = Ice.initialize(sys.argv)

ssp = comm.stringToProxy(sys.argv[1])

ctx = {}
#ctx['IARnet.User'] = 'ssmir'
#ctx['IARnet.Password'] = 'qwerty'

#print 'Context: ', ctx;

print 'Trying to call ice_ids for proxy "', ssp.ice_toString(), '":', \
	ssp.ice_ids(ctx)

