#!/usr/bin/env python 
# _*_ coding: utf-8 _*_ 

user_id = 'NOI';           #Your user name
user_pwd = 'NOINOI';       #Your password

#================================================#

import httplib;
import urllib;
import json;
import re;
import time;
import sys;

host = 'oj.bashu.com.cn';
subdir = '/code/';

languages = {"G++": 0, "GCC": 1, "Pascal": 2, "G++(0x)": 3};
results = {0: "Correct", 2: "Time Out", 3: "MLE", 4: "Wrong Answer", 5: "Runtime Error", 99: "Validator Error"};

def getCookie(headers):
	ret = '';
	regexp = re.compile('^(path|domain|expires|secure)=', re.IGNORECASE);
	for i in headers:
		if i[0] == 'set-cookie':
			value = i[1];
			for j in value.split(';'):
				if regexp.match(j.strip()):
					continue;
				ret += j + ';';
	return ret;

def getLocation(headers):
	for i in headers:
		if i[0] == 'location':
			return i[1];

def color_st(fore, back=None):
	if back is None:
		sys.stdout.write("\x1B[0;%dm" % fore);
	else:
		sys.stdout.write("\x1B[0;%d;%dm" % (fore, back));

def color_ed():
	sys.stdout.write("\x1B[0m\n");

if len(sys.argv) < 3:
	print "usage: %s code_file problem_id language" % sys.argv[0];
	quit(1);

if sys.argv[3] not in languages:
	print "Invalid language name";
	print "language can be",languages.keys();
	quit(1);
language_id = languages[sys.argv[3]];

problem_id = 0;
try:
	problem_id = int(sys.argv[2]);
except ValueError:
	print "Problem ID must be a number";
	quit(1);

source_code = '';
try:
	f = open(sys.argv[1],'rb');
	source_code = f.read();
	f.close();
except IOError:
	print "Cannot open and read source code file";
	quit(1);

print "User: %s" % user_id;

post_data = urllib.urlencode({'uid': user_id, 'pwd': user_pwd, 'url': 'index.php'});
headers={"Accept": "text/html", "User-Agent": "OJ_Python_Client", "Content-Type": "application/x-www-form-urlencoded"};

print "Login...",
conn = httplib.HTTPConnection(host);
conn.request("POST", subdir + "login.php", post_data, headers);
res = conn.getresponse();

if res.status == 302:
	print "OK";
else:
	print "Failed!";
	quit(1);

Cookie = getCookie(res.getheaders());
conn.close();

headers['Cookie'] = Cookie;
post_data = urllib.urlencode({'source': source_code, 'problem': problem_id, 'language': language_id});

print "Submit...",
conn.connect();
conn.request("POST", subdir + "submit.php", post_data, headers);
res = conn.getresponse();

if res.status == 302:
	print "OK";
else:
	print "Failed!";
	quit(1);

query_key = getLocation(res.getheaders())[-32:];
#print "Key:",query_key;
conn.close();

print "Waiting...";
time.sleep(2);

last_len = 0;
time_wait = 1;
retry = 2;
while True:
	try:
		conn.connect();
		conn.request("GET", '/query/query_' + query_key);
		res = conn.getresponse();
		if res.status != 200:
			raise NameError('error');
	except:
		conn.close();
		if retry == 0:
			print "Cannot get information from server";
			quit(1);
		retry -= 1;
		continue;

	info = json.loads(res.read());
	conn.close();
	detail = info['detail'];
	length = len(detail)-1;
	if length > last_len:
		if detail[0][0] == 7:
			color_st(31);
			print "Compile Error";
			color_st(30, 47)
			print detail[0][3],
			color_ed();
			break;

		if detail[0][0] == 100:
			color_st(31);
			print "System Error";
			color_st(30, 47)
			print detail[0][3];
			print "Please contact administrator.",
			color_ed();
			break;

		for i in xrange(last_len, length):
			err_code = detail[i][0];
			if err_code == 0:
				color_st(37, 42);
			elif err_code == 2 or err_code == 3:
				color_st(37, 43);
			elif err_code == 4:
				color_st(37, 41);
			elif err_code == 5:
				color_st(37, 46);
			else :
				color_st(37, 40);
			print "% 3d % 15s % 6d ms % 7d KB" % (i, results[err_code], detail[i][1], detail[i][2]),
			color_ed();
			if err_code in [4,5]:
				print detail[i][3];

		last_len = length;
		time_wait = 1;
	else:
		time_wait = 2.5;
	if info['state'] == 'finish':
		print "Done."
		break;
	time.sleep(time_wait);
