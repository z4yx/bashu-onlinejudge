#!/usr/bin/env python
import os,sys;
import MySQLdb as mdb;

data_dir='/root/data/';
database_pass='';
max_prob=3542;

def count_in(dir):
	num=0;
	files=os.listdir(dir);
	for line in files:
		line=os.path.join(dir,line);
		if os.path.isfile(line):
			if line.endswith('.in'):
				num=num+1;
#	print dir,num
	return num;

con=mdb.connect('localhost','root',database_pass,'bsoj');

def update(p,s):
	global con;
	print p,s;
	con.cursor().execute("update problem set case_score="+str(s)+" where problem_id="+str(p));

for i in range(1000,max_prob-1):
	n=count_in(data_dir+str(i));
	if n==0:
		continue;
	elif n<40:
		update(i,100/n);
	else:
		update(i,200/n);
