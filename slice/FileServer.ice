#ifndef __FILESERVER_ICE__
#define __FILESERVER_ICE__

module IARnet {
	module Files {
		enum FileExceptionReason {
			MalformedName,
			NotEnoughSpace,
			OtherPutException,
			NotFound,
			OtherGetException
		};
		
		exception FileException {
			FileExceptionReason reason;
			string message;
		};
		
		sequence<byte> FileContent;
		
		interface FileServer;
		
		sequence<FileServer*> SeqOfFS;
		
		interface FileServer {
			["ami"] void putFile(string fileName, FileContent content)
				throws FileException;
				
			idempotent FileContent getFile(string fileName)
				throws FileException;
				
			SeqOfFS dispatch(string fileName, SeqOfFS recipients)
				throws FileException;
		};
	};
};

#endif // __FILESERVER_ICE__

