#include <MaximaAPI/MaximaInstance.h>
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

