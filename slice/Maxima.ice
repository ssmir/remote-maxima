#ifndef __MAXIMA_ICE__
#define __MAXIMA_ICE__

#include "FileServer.ice"

module IARnet {
	module Maxima {
		/** A generic Maxima exception (error during command execution or
			a failure to launch a Maxima process).
		**/
		exception MaximaException {
			string reason;
		};
		
		/** An interface to a Maxima process. Can be created by a 
			[MaximaFactory] object.
		**/
		interface MaximaResource extends Files::FileServer {
			/** Execute a Maxima command and return a result in a form that
				can be passed to [executeCommand]. Trailing ';' or '$' of the
				command can be omitted.
			**/
			["ami"] string executeCommand(string command) throws MaximaException;
			
			/** Execute a Maxima command and return an unformatted Maxima 
				output. Trailing ';' or '$' of the command can be omitted.
			**/
			["ami"] string executeInteractive(string command) throws MaximaException;
			
			/** Sends SIGINT to a Maxima process. **/
			void interrupt();
			
			/** Restarts a Maxima process without destroing its working
				directory.
			**/
			void restart();
			
			/** Deactivate the [MaximaResource] and destroy the corresponding 
				Maxima process.
			**/
			void destroy();
		};
		
		/** [MaximaResource] factory: gives every user his own Maxima instance
		**/
		interface MaximaFactory extends Files::FileServer {
			/** Launches a new Maxima process and returns a proxy to a new 
				[MaximaResource] object which can be used to interact with the
				Maxima process.
			**/
			MaximaResource *launchMaxima() throws MaximaException;
		};

		sequence<MaximaFactory *> Factories;
		
		/** This [MaximaResource] factory acts as a central source of
			[MaximaResource] instances for the user. Slave (basic
			[MaximaFactory]) factories register themselves in the master factory
			for it to be able to delegate [launchMaxima] calls.
		**/
		interface MasterFactory extends MaximaFactory {
			/** Register slave [MaximaFactory].
			**/
			void registerFactory(MaximaFactory *slave);
			
			/** Unregister a slave factory to prevent the master factory from
				launching new Maxima instances using that factory.
			**/
			void unregisterFactory(MaximaFactory *slave);
			
			Factories getSlaveFactories();
		};
	};
};

#endif // __MAXIMA_ICE__

