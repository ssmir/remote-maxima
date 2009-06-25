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

#include "FileServerImpl.h"

#include <ios>
#include <fstream>
#include <iterator>
#include <set>

using namespace IARnet::Files;

FileServerImpl::FileServerImpl(const fs::path &directory, bool removeOnDestroy)
:   _directory(directory),
	_removeOnDestroy(removeOnDestroy)
{
    fs::create_directory(_directory);
}

FileServerImpl::~FileServerImpl()
{
	if (_removeOnDestroy)
	{
		try
		{
			fs::remove_all(_directory);
		}
		catch(const std::exception &)
		{
		}
	}
}

FileContent FileServerImpl::loadFile(const fs::path &path)
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    file.open(path.external_file_string().c_str(), std::ios::binary);
    
    file.seekg (0, std::ios::end);
    
    FileContent result(file.tellg());
    file.seekg (0, std::ios::beg);
    
    file.read((char *)&*(result.begin()), (std::streamsize)result.size());
    
    
    /*result.reserve(file.tellg());
    
    file.seekg (0, std::ios::beg);
    
    typedef std::istream_iterator<FileContent::value_type> Iter;
    
    result.insert(result.begin(), Iter(file), Iter());*/
    return result;
}

void FileServerImpl::saveFile(const fs::path &path, const FileContent &content)
{
    std::ofstream f;
    f.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    
    f.open(path.external_file_string().c_str(), std::ios::binary);
    
    std::copy(content.begin(), content.end(),
        std::ostream_iterator<FileContent::value_type>(f));
}

FileContent FileServerImpl::getFile(const std::string &fileName)
{
    try
    {
        return loadFile(_directory / fileName);
    }
    catch (const std::exception &e)
    {
        throw FileException(OtherGetException, e.what());
    }
}

void FileServerImpl::putFile(const std::string &fileName, const FileContent &file)
{
    try
    {
        saveFile(_directory / fileName, file);
    }
    catch (const std::exception &e)
    {
        throw FileException(OtherPutException, e.what());
    }
}

SeqOfFS FileServerImpl::dispatch(const std::string &fileName,
    const SeqOfFS &servers)
{
    FileContent file = getFile(fileName);
    std::set<FileServerPrx> successful(servers.begin(), servers.end());
    
    for (SeqOfFS::const_iterator it = servers.begin();
        it != servers.end(); ++it)
    {
        try 
        {
            (*it)->putFile(fileName, file);
        }
        catch (const FileException &)
        {
            successful.erase(*it);
        }
    }
    
    return SeqOfFS(successful.begin(), successful.end());
}

