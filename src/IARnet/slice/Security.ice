#include <Ice/Identity.ice>

module IARnet {
	module Security {
		
		exception AccessDeniedException {
			string reason;
		};
		
		struct RequestInfo {
			string serverName;
			string serviceId;
			Ice::Identity objectId;
			string facet;
			string remoteHost;			
		};
		
		sequence<string> StringSeq;
		
		interface AuthorizationService {
			idempotent bool authorize(string userId, string password, RequestInfo request, out string reason);
		};

		interface SSLAuthorizationService {
			idempotent bool authorize(StringSeq certs, RequestInfo request, out string reason);
		};
		
	};
};