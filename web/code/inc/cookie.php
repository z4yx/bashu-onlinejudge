<?php
define("cookie_key","random string 133 "); //Please enter a random string
define("cookie_expire",31536000); //about one year

function check_cookie()
{
	if(!isset($_COOKIE['SID']))
		return false;
	$cookie = decrypt(cookie_key, $_COOKIE['SID']);
	if($cookie === false)
		return false;
	$arr = unserialize($cookie);
	//print_r($arr);

	if(!isset($arr['web'],$arr['user'],$arr['lang']) || $arr['web']!="bsoj")
		return false;
	$user = $arr['user'];
	if(preg_match('/\W/',$user))
		return false;

	$_SESSION['user'] = $user;
	$_SESSION['lang'] = intval($arr['lang']);

	if(isset($arr['administrator']))
		$_SESSION['administrator']=true;
	if(isset($arr['source_browser']))
		$_SESSION['source_browser']=true;
	return true;
}
function write_cookie()
{
	$arr = array('web'=>'bsoj');
	$arr['user']=$_SESSION['user'];
	$arr['lang']=$_SESSION['lang'];
	if(isset($_SESSION['administrator']))
		$arr['administrator']=true;
	if(isset($_SESSION['source_browser']))
		$arr['source_browser']=true;
	$data = encrypt(cookie_key, serialize($arr));
	setcookie('SID', $data, time()+cookie_expire);
}

function encrypt($key, $plain_text) {
	$iv='7284565820000000';
	$key=hash('sha256',$key,true);
	return openssl_encrypt($plain_text,'aes-256-cbc',$key,false,$iv);
}
function decrypt($key, $c_t) { 
	$iv='7284565820000000';
	$key=hash('sha256',$key,true);
	return openssl_decrypt($c_t,'aes-256-cbc',$key,false,$iv);
}