<?php
require_once 'inc/checkpwd.php';
function login($user, $is_cookie, $pwd='')
{
	$user=mysql_real_escape_string($user);
	$res=mysql_query("select password,user_id,language,defunct from users where user_id='$user'");
	$r=mysql_fetch_row($res);
	if(!$r)
		return ("No such user");
	if($r[3]!='N')
		return ("User is disabled");

	if(!$is_cookie && !password_right($user, $pwd))
		return ("Password is incorrect");

	session_unset();
	setcookie('SID', '', 31415926);
	$_SESSION['user']=$r[1];
	$_SESSION['lang']=$r[2];
	
	$res=mysql_query("select rightstr from privilege where user_id='$user'");
	while($r=mysql_fetch_row($res)){
		if($r[0]=='administrator' || $r[0]=='source_browser' || $r[0]=='insider')
			$_SESSION[$r[0]]=true;
	}
	
	require_once 'inc/preferences.php';
	$pref=new preferences();
	$res=mysql_query("select property,value from preferences where user_id='$user'");
	while($r=mysql_fetch_row($res)){
		$property=$r[0];
		$pref->$property=$r[1];
	}
	$_SESSION['pref']=serialize($pref);

	$ip=mysql_escape_string($_SERVER["REMOTE_ADDR"]);
	mysql_query("update users set accesstime=NOW(),ip='$ip' where user_id='$user'");

	return TRUE;
}