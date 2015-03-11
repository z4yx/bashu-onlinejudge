repair table attend,compileinfo,contest,contest_problem,loginlog,mail quick;
repair table message,privilege,news,problem,solution quick;
repair table source_code,users;

alter table users modify language int NOT NULL DEFAULT 0;
alter table users modify email BLOB;
alter table users modify email varchar(100) CHARACTER SET gbk;
update users set password = decode(password,'PWDforJO2005');
alter table users modify password varchar(90) CHARACTER SET gbk;
alter table users modify nick BLOB;
alter table users modify nick varchar(100) CHARACTER SET gbk;
alter table users modify school BLOB;
alter table users modify school varchar(100) CHARACTER SET gbk;
alter table users ADD COLUMN score int NOT NULL DEFAULT 0 AFTER solved;
alter table users ADD COLUMN experience int NOT NULL DEFAULT 0 AFTER score;
alter table users CONVERT TO CHARACTER SET utf8;

update source_code set source = uncompress(source);
alter table source_code modify source text CHARACTER SET gbk;
alter table source_code CONVERT TO CHARACTER SET utf8;

alter table solution drop COLUMN className;
alter table solution drop COLUMN ip;
alter table solution ADD COLUMN score int NOT NULL DEFAULT 0 AFTER result;
alter table solution ADD COLUMN info varchar(100) DEFAULT NULL AFTER score;
alter table solution ADD COLUMN public_code BOOL NOT NULL DEFAULT 0;
delete from solution where result=10000;
update solution set result=4 where result=1 or result=6;
update solution set result=5 where result=98;
alter table solution CONVERT TO CHARACTER SET utf8;

alter table problem change difficulty has_tex tinyint NOT NULL DEFAULT 0;
alter table problem change error compare_way int NOT NULL DEFAULT 0;
update problem set has_tex=0;
alter table problem modify title BLOB;
alter table problem modify title varchar(200) CHARACTER SET gbk;
alter table problem modify description BLOB;
alter table problem modify description TEXT CHARACTER SET gbk;
alter table problem modify input BLOB;
alter table problem modify input TEXT CHARACTER SET gbk;
alter table problem modify output BLOB;
alter table problem modify output TEXT CHARACTER SET gbk;
alter table problem drop COLUMN input_path;
alter table problem drop COLUMN output_path;
alter table problem modify sample_input BLOB;
alter table problem modify sample_input TEXT CHARACTER SET gbk;
alter table problem modify sample_output BLOB;
alter table problem modify sample_output TEXT CHARACTER SET gbk;
alter table problem modify hint BLOB;
alter table problem modify hint TEXT CHARACTER SET gbk;
alter table problem modify source BLOB;
alter table problem modify source varchar(100) CHARACTER SET gbk;
alter table problem drop COLUMN sample_Program;
alter table problem ADD COLUMN case_score int NOT NULL DEFAULT 0 AFTER memory_limit;
alter table problem CONVERT TO CHARACTER SET utf8;

alter table privilege CONVERT TO CHARACTER SET utf8;

alter table news modify content BLOB;
alter table news modify content TEXT CHARACTER SET gbk;
alter table news modify title BLOB;
alter table news modify title varchar(200) CHARACTER SET gbk;
alter table news CONVERT TO CHARACTER SET utf8;

alter table message modify title BLOB;
alter table message modify title varchar(200) CHARACTER SET gbk;
alter table message modify content BLOB;
alter table message modify content TEXT CHARACTER SET gbk;
alter table message CONVERT TO CHARACTER SET utf8;

alter table mail modify mail_id INT(11) NOT NULL AUTO_INCREMENT;
alter table mail modify title BLOB;
alter table mail modify title varchar(200) CHARACTER SET gbk;
alter table mail modify content BLOB;
alter table mail modify content TEXT CHARACTER SET gbk;
alter table mail change reply flags tinyint UNSIGNED NOT NULL DEFAULT 0;
alter table mail CONVERT TO CHARACTER SET utf8;

drop table loginlog;

alter table contest_problem modify title BLOB;
alter table contest_problem modify title varchar(200) CHARACTER SET gbk;
alter table contest_problem CONVERT TO CHARACTER SET utf8;

alter table contest modify title BLOB;
alter table contest modify title varchar(255) CHARACTER SET gbk;
alter table contest modify description BLOB;
alter table contest modify description TEXT CHARACTER SET gbk;
alter table contest CONVERT TO CHARACTER SET utf8;

alter table compileinfo modify error BLOB;
alter table compileinfo modify error TEXT CHARACTER SET gbk;
alter table compileinfo CONVERT TO CHARACTER SET utf8;

alter table attend modify user_id BLOB;
alter table attend modify user_id varchar(20) CHARACTER SET gbk;
alter table attend modify nick BLOB;
alter table attend modify nick varchar(100) CHARACTER SET gbk;
alter table attend CONVERT TO CHARACTER SET utf8;

update solution set valid=0;
update solution,(select solution_id from(select solution_id,problem_id,user_id,result FROM solution order by solution_id) as t where result=0 group by problem_id,user_id)as s SET valid=1 where solution.solution_id=s.solution_id;

update users,(select user_id as uid,count(1) as s from solution group by user_id) as cnt set users.submit=cnt.s where cnt.uid=users.user_id;
update users,(select user_id as uid,count(distinct problem_id) as s from solution where result=0 group by user_id) as cnt set users.solved=cnt.s where cnt.uid=users.user_id;
CREATE INDEX solve_submit ON users (solved,submit);

CREATE TABLE `preferences` (
  `id` int AUTO_INCREMENT,
  `user_id` varchar(20) NOT NULL DEFAULT '',
  `property` varchar(20) NOT NULL DEFAULT '',
  `value` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `u_p` (`user_id`,`property`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `saved_problem` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `problem_id` int(11) NOT NULL DEFAULT '0',
  `user_id` varchar(20) NOT NULL DEFAULT '',
  `savetime` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `u_p` (`user_id`,`problem_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `user_notes` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `problem_id` int(11) NOT NULL,
  `user_id` varchar(20) NOT NULL,
  `tags` varchar(32) NOT NULL,
  `content` TEXT NOT NULL DEFAULT '',
  `edit_time` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `u_p` (`user_id`,`problem_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `experience_titles` (
  `experience` int NOT NULL,
  `title` varchar(20) NOT NULL,
  PRIMARY KEY (`experience`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
INSERT INTO `experience_titles` VALUES (0,'Default');

CREATE TABLE `level_experience` (
  `level` int(11) NOT NULL,
  `experience` int(11) NOT NULL,
  PRIMARY KEY (`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
INSERT INTO `level_experience` VALUES (0,0),(1,1),(2,2),(3,4),(4,5),(5,10),(6,15),(7,20);

delimiter //
CREATE FUNCTION get_problem_level (pid int)
RETURNS int
NOT DETERMINISTIC
READS SQL DATA
BEGIN
RETURN IFNULL((SELECT (has_tex>>3)&7 FROM problem WHERE problem_id = pid),0);
END//

CREATE FUNCTION problem_flag_to_level (flag int)
RETURNS int
NO SQL
BEGIN
RETURN (flag>>3)&7;
END//
 
delimiter ;
