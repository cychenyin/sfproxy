/*
 * hstreambuf.h
 *
 *  Created on: May 30, 2014
 *      Author: asdf
 */

#ifndef HSTREAMBUF_H_
#define HSTREAMBUF_H_

#include <iosfwd> // Accesses filebuf class
#include <streambuf>
#include <fstream>
#include <libio.h>

#include <string.h>
#include <stdio.h> // for sprintf

namespace FinagleRegistryProxy {
using std;

class hstreambuf : public filebuf
{
public:
    hstreambuf( int filed );
    virtual int sync();
    virtual int overflow( int ch );
    ~hstreambuf();
private:
    int column, line, page;
    char* buffer;
    void convert( long cnt );
    void newline( char*& pd, int& jj );
    void heading( char*& pd, int& jj );
    void pstring( char* ph, char*& pd, int& jj );
};
ostream& und( ostream& os );
ostream& reg( ostream& os );


} /* namespace FinagleRegistryProxy */

#endif /* HSTREAMBUF_H_ */
