#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "dstring.h"
#include "drarray.h"
#include "syslog.h"
#include "conf.h"
//---------------------
typedef void* (*export_func)(void *args);
int main(int argc, char **argv)
{
	using Syslog::syslog_message;
	using DString::dstring;
	using DRArray::drarray;
	using DConf::conf;
	struct syslog_message *test;
	void *dl;
	export_func once, many, cfg;
	char *e;
	drarray<syslog_message*> *many_test;
	conf *c;
	dstring *tmp;

	if( argc != 2 )
	{
		fprintf( stderr, "%s libmodule.so\n", argv[0] );
		exit(1);
	}

	tmp = new dstring("config.test");
	c = new conf(tmp);
	
	test = (struct syslog_message*)malloc(sizeof(struct syslog_message));
	test->facility = 10;
	test->severity = 0;
	test->date = new dstring("Oct 11 22:14:15");
	test->host = new dstring("mybox");
	test->message = new dstring("su: 'su root' failed for peter on /dev/pts/2");
	test->daemon = new dstring("login failures");
	
	if( ! (dl = dlopen(argv[1], RTLD_NOW)) )
	{
		fprintf( stderr, "Failed to dlopen() the module!\n%s\n", dlerror() );
		exit(1);
	}
	dlerror();
	cfg = (export_func)dlsym(dl, "dryad_config");
	if( e = dlerror() )
	{
		fprintf( stderr, "Failed to resolve symbol dryad_config!\n%s\n", e);
		exit(1);
	}
	dlerror();
	once = (export_func)dlsym(dl, "dryad_once");
	if( e = dlerror() )
	{
		fprintf( stderr, "Failed to resolve symbol dryad_once!\n%s\n", e );
		exit(1);
	}
	dlerror();
	many = (export_func)dlsym(dl, "dryad_many");
	if( e = dlerror() )
	{
		fprintf( stderr, "Failed to resolve symbol dryad_many!\n%s\n", e );
		exit(1);
	}
	dlerror();

	fprintf( stderr, "Sending in the clowns.\n");
	(*cfg)(c);

	fprintf( stderr, "Testing dryad_once function call.\n" );
	(*once)(test); //test get's free'd here.
	
	many_test = new drarray<struct syslog_message*>;
	
	test = (struct syslog_message*)malloc(sizeof(struct syslog_message));
	test->facility = 10;
	test->severity = 0;
	test->date = new dstring("Oct 11 22:14:15");
	test->host = new dstring("mybox");
	test->message = new dstring("su: 'su root' failed for peter on /dev/pts/2");
	test->daemon = new dstring("login failures");

	many_test->pushback(test);

	test = (struct syslog_message*)malloc(sizeof(struct syslog_message));
	test->facility = 10;
	test->severity = 5;
	test->date = new dstring("Oct 12 22:31:42");
	test->host = new dstring("anotherhost");
	test->message = new dstring("su: 'su yourmom' failed for joe on /dev/tty2");
	test->daemon = new dstring("login failures");

	many_test->pushback(test);
	
	
	fprintf( stderr, "Testing dryad_many function call.\n" );
	(*many)(many_test);
	
}

