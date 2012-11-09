<?php
class php_mutex {
	private $resource;
	private $use_sysv;
	public function __construct($lock_file){
		$this->use_sysv=extension_loaded("sysvsem");
		if($this->use_sysv){
			$key=crc32($lock_file);
			$this->resource=sem_get($key, 1);
			if(!$this->resource || !sem_acquire($this->resource))
				die('Could not get lock');
		}else{
			$i=0;
			while(!($this->resource=fopen($lock_file,'w+'))){
				usleep(50000);
				$i++;
				if($i>10)
					die('Lock wait timeout exceeded');
			}
			if(!flock($this->resource,LOCK_EX)){
				fclose($this->resource);
				die('Could not get lock');
			}
		}
	}

	public function release_mutex(){
		if($this->use_sysv){
			sem_release($this->resource);
			//sem_remove($this->resource);
		}else{
			flock($this->resource, LOCK_UN);
			fclose($this->resource);
		}
	}

	public function __destruct(){

	}
}