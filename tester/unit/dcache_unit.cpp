#include "cache.h"
#include "dstring.h"
#include "syslog.h"
#include <stdlib.h>
//------------------

using namespace std;
using DCache::cache;
using DString::dstring;
using Syslog::syslog_message;

int main()
{
	cache *c;
	struct syslog_message *t;

	c = new cache(8192, NULL);
	cerr << "cache created\n";

	cerr << "creating/adding messages: ";
	for( int x = 0; x < 500; x++ )
	{
		cerr << x;
		t = (struct syslog_message*)malloc(sizeof(struct syslog_message));
		t->facility = x;
		t->severity = 0;
		t->date = new dstring("This is the song that doesn't end. Yes it goes on and on my friend. Some people started singing it not knowing what it was, and they'll continue singing it forever just because...");
		t->host = new dstring(t->date);
		t->message = new dstring(t->date);
		t->daemon = new dstring(t->date);
		c->add(t);
		t = NULL;
	}
	cerr << "\ndone\n";
	cerr << "reading data back: ";
	for( int x = 0; x < 500; x++ )
	{
		t = c->get();
		if( t->facility != x )
		{
			cerr << "Not a match ;.; x=" << x << endl;
			exit(1);
		}
		free(t);
	}
	cerr << "done\n";
}


