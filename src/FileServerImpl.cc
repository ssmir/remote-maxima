#include "FileServerImpl.h"

#include <ios>
#include <fstream>
#include <iterator>
#include <set>

using namespace IARnet::Files;

class Transaction : public IceUtil::Monitor<IceUtil::Mutex>
{
    std::set<FileServerPrx> _successful;
    size_t _repliesLeft;
    
    public:
        Transaction(std::set<FileServerPrx> &successful)
        :   _successful(successful)
        {
        }
        
        void run(const std::string &fileName, const FileContent &file);
        
        void replyReceived(FileServerPrx prx, bool success)
        {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
            
            if (!success)
            {
                _successful.erase(prx);
            }
            
            if (--_repliesLeft == 0)
            {
                notify();
            }
        }
};

class AMI_FileServer_putFileI : public AMI_FileServer_putFile
{
    Transaction *_master;
    FileServerPrx _prx;
    
    public:
        AMI_FileServer_putFileI(FileServerPrx prx, Transaction *master)
        :   _master(master),
            _prx(prx)
        {
        }
        
        virtual void ice_response()
        {
            _master->replyReceived(_prx, true);
        }
        
        virtual void ice_exception(const Ice::Exception &e)
        {
            _master->replyReceived(_prx, false);
        }
};

void Transaction::run(const std::string &fileName, const FileContent &file)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    
    _repliesLeft = _successful.size();
    for (std::set<FileServerPrx>::const_iterator it = _successful.begin();
        it != _successful.end(); ++it)
    {
        (*it)->putFile_async(new AMI_FileServer_putFileI(*it, this),
            fileName, file);
    }
    
    do
    {
        wait();
    }
    while (_repliesLeft > 0);
}

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
    
    Transaction(successful).run(fileName, file);
    
    return SeqOfFS(successful.begin(), successful.end());
}

