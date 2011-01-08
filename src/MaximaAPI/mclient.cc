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

#include <MaximaInstance.h>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char **argv) {
    char *maximaPath = getenv("MAXIMA_APP");
    assert(maximaPath != NULL);
    
    boost::scoped_ptr<MaximaAPI::MaximaInstance> maxima(
        new MaximaAPI::MaximaInstance(maximaPath));
	
	while (true) {
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
                maxima->interruptMaxima();
                continue;
            }
			if (argc < 2) {
				cout << maxima->executeCommand(s) << endl;
            } else {
				cout << maxima->executeInteractive(s);
			}
		} catch (const std::exception &e) {
			cout << "exception caught: " << e.what() << endl; 
		}
	}
	
	return 0;
}

