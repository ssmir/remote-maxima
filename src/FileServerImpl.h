#ifndef __FILESERVERIMPL_H__
#define __FILESERVERIMPL_H__

#include "Maxima.h"

#include <boost/filesystem.hpp>

namespace fs = ::boost::filesystem;

namespace IARnet
{   
namespace Files
{

class FileServerImpl
{
public:
    FileServerImpl(const fs::path &directory, bool removeOnDestroy = true);
    ~FileServerImpl();
    
    void putFile(const std::string &fileName, const FileContent &file);

    FileContent getFile(const std::string &fileName);
    
    SeqOfFS dispatch(const std::string &fileName, const SeqOfFS &servers);
    
    static FileContent loadFile(const fs::path &path);
    
    static void saveFile(const fs::path &path, const FileContent &content);
    
private:
    const fs::path _directory;
	const bool _removeOnDestroy;
};

}
}

#endif // __FILESERVERIMPL_H__

