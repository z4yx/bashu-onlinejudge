<?php
define("PUBLIC_KEY", "-----BEGIN PUBLIC KEY-----\nMFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAMa44v2O2oZIgXL2PtdoxjTJ4ASWfGTL\nd4VZ05MzsQAbNnQ+abT4otHnK7n6Ku4WKCbDAd3FKcsnNSv0eVhehxUCAwEAAQ==\n-----END PUBLIC KEY-----\n");
function my_rsa($value)
{
	$len=strlen($value);
	if($len>64)
		$value=substr($value,0,64);
	else
		$value=str_pad($value,64,"\x00");

	$crypted="";
	openssl_public_encrypt($value,$crypted,PUBLIC_KEY,OPENSSL_NO_PADDING);
	return "\x00".base64_encode($crypted);
}

function password_right($usr, $pwd_in){
    $result=mysql_query("select password from users where user_id='$usr'");
    if(!($row=mysql_fetch_row($result)) || !$row[0])
    	return false;

	$pwd_enc=my_rsa($pwd_in);
	$pwd_real=$row[0];
	if(ord($pwd_real)!=0){ //password in database is not encrypted password
		$pwd_real=my_rsa($pwd_real);
		$pwd_escaped=mysql_escape_string($pwd_real);
		mysql_query("update users set password='$pwd_escaped' where user_id='$usr'");
	}
	if(strcmp($pwd_enc, $pwd_real)!=0)
		return false;
	else
		return true;
}
