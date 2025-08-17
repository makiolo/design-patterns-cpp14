#!/usr/bin/env node

var os = require('os')
var fs = require('fs');
var path = require('path')
var shelljs = require('shelljs');
var is_win = (os.platform() === 'win32');

if(!process.env.CMAKI_PWD)
{
	if (fs.existsSync(path.join("..", "..", "node_modules", "npm-mas-mas"))) {
		shelljs.env['CMAKI_PWD'] = path.join(process.cwd(), '..', '..');
		process.env['CMAKI_PWD'] = path.join(process.cwd(), '..', '..');
	} else {
		shelljs.env['CMAKI_PWD'] = path.join(process.cwd());
		process.env['CMAKI_PWD'] = path.join(process.cwd());
	}
}
else
{
	shelljs.env['CMAKI_PWD'] = process.env['CMAKI_PWD'];
}

if(!process.env.CMAKI_INSTALL)
{
	shelljs.env['CMAKI_INSTALL'] = path.join(process.env['CMAKI_PWD'], 'bin');
	process.env['CMAKI_INSTALL'] = path.join(process.env['CMAKI_PWD'], 'bin');
}
else
{
	shelljs.env['CMAKI_INSTALL'] = process.env['CMAKI_INSTALL'];
}

if(!process.env.NPP_SERVER)
{
	shelljs.env['NPP_SERVER'] = 'http://artifacts.myftp.biz'
	process.env['NPP_SERVER'] = 'http://artifacts.myftp.biz'
}
else
{
	shelljs.env['NPP_SERVER'] = process.env['NPP_SERVER'];
}

if(!process.env.NPP_CACHE)
{
	shelljs.env['NPP_CACHE'] = 'TRUE'
	process.env['NPP_CACHE'] = 'TRUE'
}
else
{
	shelljs.env['NPP_CACHE'] = process.env['NPP_CACHE'];
}

if(is_win)
{
	cmaki_identifier = 'cmaki_identifier.cmd'
}
else
{
	cmaki_identifier = 'cmaki_identifier.sh'
}


// no check in cmaki_identifier for avoid recursion
if( process.cwd().replace(/\\/g, "/").search("/cmaki_identifier") == -1 )
{
	if(!fs.existsSync( path.join( process.env['CMAKI_INSTALL'], cmaki_identifier) ))
	{
		dir_identifier = path.join(process.env['CMAKI_PWD'], 'node_modules', 'npm-mas-mas', 'cmaki_identifier');

		backup1 = shelljs.env['CMAKI_PWD'];
		backup2 = process.env['CMAKI_PWD'];

		shelljs.env['CMAKI_PWD'] = dir_identifier;
		process.env['CMAKI_PWD'] = dir_identifier;

		shelljs.cd( dir_identifier );

		if (shelljs.exec('npm install').code !== 0) {
			shelljs.echo('Error detecting compiler (compiling cmaki_identifier ...)');
			shelljs.exit(1);
		}

		shelljs.env['CMAKI_PWD'] = backup1;
		process.env['CMAKI_PWD'] = backup2;
	}
}

if(!process.env.MODE)
{
	shelljs.env['MODE'] = 'Debug';
	process.env['MODE'] = 'Debug';
}
else
{
	shelljs.env['MODE'] = process.env['MODE'];
}

function trim(s)
{
	return ( s || '' ).replace( /^\s+|\s+$/g, '' );
}

var environment_vars = [];
next_is_environment_var = false;
process.argv.forEach(function(val, index, array)
{
	if(next_is_environment_var)
	{
		environment_vars.push(val);
	}
	next_is_environment_var = (val == '-e');
});
environment_vars.forEach(function(val, index, array)
{
	var chunks = val.split("=");
	if( chunks.length == 2 )
	{
		shelljs.env[chunks[0]] = chunks[1];
		process.env[chunks[0]] = chunks[1];
	}
	else
	{
		console.log("Error in -e with value: " + val);
	}
});

////////////////////////////////////////////////////////////////////////////////
// change cwd
shelljs.cd( process.env['CMAKI_PWD'] );
////////////////////////////////////////////////////////////////////////////////


var dir_script;
var script = process.argv[2];
if (is_win)
{
	if(fs.existsSync(path.join(process.cwd(), script+".cmd")))
	{
		dir_script = process.cwd();
	}
	else
	{
		dir_script = path.join(process.env['CMAKI_PWD'], 'node_modules', 'npm-mas-mas', 'cmaki_scripts');
	}
}
else
{
	if(fs.existsSync(path.join(process.cwd(), script+".sh")))
	{
		dir_script = process.cwd();
	}
	else
	{
		dir_script = path.join(process.env['CMAKI_PWD'], 'node_modules', 'npm-mas-mas', 'cmaki_scripts');
	}
}

if (is_win)
{
	script_execute = path.join(dir_script, script+".cmd");
	exists = fs.existsSync(script_execute);
	caller_execute = "cmd /c ";
	script_execute = script_execute.replace(/\//g, "\\");
}
else
{
	script_execute = path.join(dir_script, script+".sh");
	exists = fs.existsSync(script_execute);
	caller_execute = "bash ";
	script_execute = script_execute.replace(/\\/g, "/");
}

console.log("Execute: " + caller_execute + script_execute);

if(exists)
{
	var child = shelljs.exec(caller_execute + script_execute, {async:true, silent:true}, function(err, stdout, stderr) {
		process.exit(err);
	});
	child.stdout.on('data', function(data) {
		console.log(trim(data));
	});
	child.stderr.on('data', function(data) {
		console.log(trim(data));
	});
}
else
{
	console.log("[error] dont exits: " + script_execute);
	process.exit(1);
}

