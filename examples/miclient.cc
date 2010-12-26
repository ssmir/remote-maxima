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

#include <Ice/Application.h>
#include <MaximaAPI/MaximaInstance.h>
#include <iostream>

using std::cout;
using std::endl;

class MaximaApplication : public Ice::Application
{
public:
    MaximaApplication()
    {
        _done = false;
    }
    
    ~MaximaApplication()
    {
    }
    
    virtual int run(int argc, char *argv[])
    {
        callbackOnInterrupt();
        
        char *maximaPath = getenv("MAXIMA_APP");
        assert(maximaPath != NULL);
        
        _maxima.reset(new MaximaAPI::MaximaInstance(maximaPath));
        
        while (!_done) {
            try {
                if (argc < 2) {
                    cout << "> ";
                }
                std::string s;
                std::getline(std::cin, s);
                if (s == "q") {
                    break;
                }
                if (s == "i") {
                    _maxima->interruptMaxima();
                    continue;
                }
                if (s == "it") {
                    cout << _maxima->executeAndInterrupt("factorial(100000)", 1);
                    continue;
                }
                if (argc < 2) {
                    cout << _maxima->executeCommand(s) << endl;
                } else {
                    cout << _maxima->executeInteractive(s);
                }
            } catch (const std::exception &e) {
                cout << "exception caught: " << e.what() << endl; 
            }
        }
        
        return 0;
    }
    
    virtual void interruptCallback(int)
    {
        _done = true;
        _maxima->interruptMaxima();
    }
    
private:
    bool _done;
    boost::scoped_ptr<MaximaAPI::MaximaInstance> _maxima;
};

int main(int argc, char **argv)
{
    MaximaApplication app;
    return app.main(argc, argv);
}

