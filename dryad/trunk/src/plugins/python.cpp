/***************************************************************************
 *   Copyright (C) 2004 by Peter Snoblin                                   *
 *   pas@truman.edu                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <Python.h>
#include <plugin.h>
namespace python_plugin
{
using namespace plugin;
extern "C"
{
	
	struct mappings {
		dstring *daemon;
		PyObject *func;
	};
	
	dstring *script;
	PyObject *module, *dict, *fallback;
	drarray<struct mappings*> *dtof;

	int dryad_config(conf *c)
	{
		dstring *t;
		PyObject *file, *func_setup;
		drarray<dstring*> *funcs;
		struct mappings *temp;
		
		funcs = new drarray<dstring*>;
		dtof = new drarray<struct mappings*>;
		
		/* init the python interpretor */
		Py_Initialize();
		
		/* get script name from config */
		script = c->daemon_get("libpython_module", "script");
		if( script == NULL )
		{
			cerr << "No script specified for libpython!\nAborting!\n";
			exit(1);
		}
		/* import the given file */
		file = PyString_FromString(script->ascii());
		module = PyImport_Import(file);
		Py_XINCREF(module);
		Py_DECREF(file);
		if( module == NULL )
		{
			cerr << "Failed to import the specified script for libpython!\nAborting!\n";
			exit(1);
		}
		dict = PyModule_GetDict(module);
		Py_XINCREF(dict);
		if( NULL == dict )
		{
			cerr << "Failed to export the dictionary for the specified script in libpython!\nAborting!\n";
			exit(1);
		}
		/* export the default function. it has to exist. */
		t = c->daemon_get("libpython_module", "fallback");
		if( t == NULL )
		{
			cerr << "fallback function not specified!\nAborting!\n";
			exit(1);
		}
		fallback = PyDict_GetItemString(dict, t->ascii());
		Py_XINCREF(fallback);
		if( ! fallback )
		{
			cerr << "fallback function (" << t->ascii() << ") is not defined in the python code!\nAborting!\n";
			exit(1);
		}
		if( !PyCallable_Check(fallback) )
		{
			cerr << "fallback 'function'(" << t->ascii() << ") is not callable!\nAborting!\n";
			exit(1);
		}
		delete t;
		
		/* export the map of functions */
		if( NULL != c->daemon_get("libpython_module", "map", funcs) )
		{
			for( int q = 0; q < funcs->length(); q++ )
			{
				temp = (struct mappings*)malloc(sizeof(struct mappings));
				temp->daemon = funcs->at(q)->prior(' ');
				t = funcs->at(q)->following(' ');
				temp->func = PyDict_GetItemString(dict, t->ascii());
				Py_XINCREF(temp->func);
				if( ! temp->func || ! PyCallable_Check(temp->func) )
				{
					cerr << "The " << temp->daemon->ascii() << " mapping in libpython is either not callable or not defined!\nAborting!\n";
					exit(1);
				}
				delete t;
			}
		}
		
		/* if there is a setup function defined, call it */
		t = c->daemon_get("libpython_module", "setup");
		if( t != NULL )
		{
			func_setup = PyDict_GetItemString(dict, t->ascii());
			if( ! func_setup || !PyCallable_Check(func_setup) )
			{
				cerr << "Setup function not defined in python file, or not callable!\nAborting!\n";
				exit(1);
			}
			Py_DECREF(PyObject_CallObject(func_setup, NULL));
		}
		delete t;
		/* and we're done! */
	}
	
	int dryad_once(struct syslog_message *m)
	{
		PyObject *args;
		PyObject *a;
		args = PyList_New(6);
		PyList_SetItem(args, 0, PyString_FromString(m->daemon->ascii()));
		PyList_SetItem(args, 1, PyString_FromString(m->date->ascii()));
		PyList_SetItem(args, 2, PyString_FromString(m->host->ascii()));
		PyList_SetItem(args, 3, PyString_FromString(m->message->ascii()));
		PyList_SetItem(args, 4, PyInt_FromLong(m->facility));
		PyList_SetItem(args, 5, PyInt_FromLong(m->severity));
		a = PyTuple_New(1);
		PyTuple_SetItem(a, 0, args);
		/* first we check to see if we need to use the default */
		if( 0 == strcmp(m->daemon->ascii(), "NULL") )
		{
			Py_DECREF(PyObject_CallObject(fallback, a ));
			return true;
		}
		for( int c = 0; c < dtof->length(); c++ )
		{
			if( 0 == strcmp(m->daemon->ascii(), dtof->at(c)->daemon->ascii()) )
			{
				Py_DECREF(PyObject_CallObject(dtof->at(c)->func, a));
				return true;
			}
		}
		Py_DECREF(PyObject_CallObject(fallback, a));
		return true;
	}
	
	int dryad_many(drarray<struct syslog_message*> *m)
	{
		PyObject *temp, *args, *a;
		args = PyList_New(m->length());
		for( int c = 0; c < m->length(); c++ )
		{
			temp = PyList_New(6);
			PyList_SetItem(temp, 0, PyString_FromString(m->at(c)->daemon->ascii()));
			PyList_SetItem(temp, 1, PyString_FromString(m->at(c)->date->ascii()));
			PyList_SetItem(temp, 2, PyString_FromString(m->at(c)->host->ascii()));
			PyList_SetItem(temp, 3, PyString_FromString(m->at(c)->message->ascii()));
			PyList_SetItem(temp, 4, PyInt_FromLong(m->at(c)->facility));
			PyList_SetItem(temp, 5, PyInt_FromLong(m->at(c)->severity));
			PyList_SetItem(args, c, temp);
		}
		a = PyTuple_New(1);
		PyTuple_SetItem(a, 0, args);
		
		if( 0 == strcmp(m->at(0)->daemon->ascii(), "NULL") )
		{
			Py_DECREF(PyObject_CallObject(fallback, a ));
			return true;
		}
		for( int c = 0; c < dtof->length(); c++ )
		{
			if( 0 == strcmp(m->at(0)->daemon->ascii(), dtof->at(c)->daemon->ascii()) )
			{
				Py_DECREF(PyObject_CallObject(dtof->at(c)->func, a));
				return true;
			}
		}
		Py_DECREF(PyObject_CallObject(fallback, a));
		return true;
	}

}
}
