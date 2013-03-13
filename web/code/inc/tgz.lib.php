<?php
class TGZfile{
	private $file, $tar_size, $filter, $hctx, $create_time;
	function __construct($_file)
	{
		$this->tar_size = 0;
		$this->file = $_file;
		$this->create_time = time();

		//gzip header
		fwrite($this->file, "\x1F\x8B\x08\x00".pack("V",$this->create_time)."\x00\x03");

		$this->filter=stream_filter_append($this->file,'zlib.deflate',STREAM_FILTER_WRITE, -1);
		$this->hctx=hash_init('crc32b');
	}
	private function calc_sum($tar_header)
	{
		$chksum=0;
		for($i=0;$i<512;$i++){
			$chksum+=ord($tar_header{$i});
		}

		return decoct($chksum);
	}
	private function str_modify(&$old, $pos, $new)
	{
		$len = strlen($new);

		for($i=0; $i<$len; $i++)
			$old[$pos++] = $new[$i];
	}
	public function AddFile($name, $content)
	{
		if(strlen($name)>100)
			return FALSE;
		$length=strlen($content);

		$buf=str_pad($name,100,"\x00");
		$buf.=str_pad('0000644',8,"\x00");
		$buf.=str_pad('0001750',8,"\x00"); //uid 1000
		$buf.=str_pad('0001750',8,"\x00"); //gid 1000
		$buf.=str_pad(decoct($length),12,"\x00"); //size
		$buf.=str_pad(decoct($this->create_time), 12, "\x00"); //mtime
		$buf.="\x20\x20\x20\x20\x20\x20\x20\x20"; //chksum
		$buf.="\x30"; //flag
		$buf.=str_repeat("\0",100); //linkname
		$buf.='ustar '; //magic
		$buf.="\x20\x00"; //version GNU tar
		$buf.=str_pad('php',32,"\x00"); //uname
		$buf.=str_pad('php',32,"\x00"); //gname

		$buf.=str_repeat("\0",16); //dev
		$buf.=str_repeat("\0",155); //prefix
		$buf.=str_repeat("\0",12); //padding

		//write checksum
		$this->str_modify($buf, 148, $this->calc_sum($buf));

		$align=($length+511)&(~511); // 512 bytes align
		$buf.=$content;
		$buf.=str_repeat("\0",$align-$length);

		fwrite($this->file, $buf);
		hash_update($this->hctx, $buf);

		$this->tar_size += strlen($buf);

		return TRUE;
	}
	// public function test_string($str)
	// {
	// 	fwrite($this->file, $str);
	// 	hash_update($this->hctx, $str);
	// 	$this->tar_size += strlen($str);
	// }
	public function Close()
	{
		$pad=str_repeat("\0",1024);
		fwrite($this->file, $pad);
		$this->tar_size += 1024;
		hash_update($this->hctx, $pad);

		stream_filter_remove($this->filter);
		$crc=hash_final($this->hctx, TRUE);
		fwrite($this->file, $crc[3].$crc[2].$crc[1].$crc[0], 4);
		fwrite($this->file, pack("V", $this->tar_size), 4);
	}
}