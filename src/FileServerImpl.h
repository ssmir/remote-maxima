/*  remote-maxima - a CAS Maxima C++ API and a Grid-service
	Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published
    by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __FILESERVERIMPL_H__
#define __FILESERVERIMPL_H__

#include "FileServer.h"

#include <IceUtil/Mutex.h>
#include <boost/filesystem.hpp>

namespace fs = ::boost::filesystem;

namespace IARnet
{   
namespace Files
{

class FileServerImpl : public IceUtil::Mutex
{
public:
    FileServerImpl(const fs::path &directory, bool removeOnDestroy = true);
    ~FileServerImpl();
    
    void putFile(const std::string &fileName, const FileContent &file);

    FileContent getFile(const std::string &fileName);
    
    SeqOfFS dispatch(const std::string &fileName, const SeqOfFS &servers);
    
    static FileContent loadFile(const fs::path &path);
    
    static void saveFile(const fs::path &path, const FileContent &content);

    void write(const std::string &fileName, int offset, const FileContent &chunk);
    
    void close(const std::string &fileName);
        
    FileContent read(const std::string &fileName, int offset, int chunkSize);

private:
    const fs::path _directory;
	const bool _removeOnDestroy;
	std::map<std::string, FILE *> _openedFiles;
};

}
}

#endif // __FILESERVERIMPL_H__

