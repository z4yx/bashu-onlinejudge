<?php
function EnterCriticalSection($lock_file){
	while(!($file=fopen($lock_file,'w+')))
		usleep(50000);
	if(!flock($file,LOCK_EX)){
		fclose($file);
		return false;
	}
	return $file;
}

function LeaveCriticalSection($file){
	flock($file, LOCK_UN);
	fclose($file);
}
?>