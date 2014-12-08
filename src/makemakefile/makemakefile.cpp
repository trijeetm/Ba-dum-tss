#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main( int argc, char ** argv )
{
    string word;
    vector<string> words;

    // loop
    do {
        // output
        // cerr << "input .cpp module name[s] ('q' to stop): ";
        // get
        if( cin >> word && word != "q" )
        {
            // append
            words.push_back( word );
        }
        else { break; }
    } while( true );
    
    // sanity
    if( words.size() == 0 ) exit(0);
    
    // generate object
    cout << "OBJS=";
    // loop over
    for( int i = 0; i < words.size(); i++ )
    {
        // print
        cout << words[i] << ".o ";
        
        // mod
        if( i % 4 == 3 )
        {
            cout << "\\" << endl << "\t";
        }
    }
    cout << endl << endl;
    
    // linker
    cout << words[0] << ": $(OBJS)" << endl;
    cout << "\t" << "$(CXX) -o " << words[0] << " $(OBJS) $(LIBS)" << endl;
    cout << endl;
    
    // objects
    for( int i = 0; i < words.size(); i++ )
    {
        // print
        cout << words[i] << ".o: " << (i==0?"":words[i]) << (i==0?"":".h ") << words[i] << ".cpp" << endl;;
        cout << "\t" << "$(CXX) -o " << words[i] << ".o $(FLAGS) " << words[i] << ".cpp" << endl;
        cout << endl;
    }
    
    cout << "clean:" << endl;
    cout << "\t" << "rm -f *~ *# *.o */*.o " << words[0] << endl;
    cout << endl;

    return 0;
}
