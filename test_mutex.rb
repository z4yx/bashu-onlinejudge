#!/usr/bin/env ruby

require 'net/http'

USER = 'NOI'
PASSWD = 'NOINOI'
HOST = 'oj.bashu.com.cn'

def die message, code = 1
	puts message.to_s
	exit code 	
end

puts "User : #{USER}"
puts 'Login ...Wait...'

conn = Net::HTTP.new HOST
post_data = URI.encode_www_form('uid' => USER, 'pwd' => PASSWD, 'url' => 'index.php')
resp = conn.post '/code/login.php', post_data, {}
die "Can't connect to OJ, check your network. HTTP Code - " + resp.code if resp.code != '302' && resp.code != '200' 
die 'Invalid username or password. ' if resp.body.size != 0

cookie = resp.response['set-cookie']

(1..1000).each do |i|
  post_data = URI.encode_www_form 'message' => "testing...", 'detail' => "mutex is evil"
  conn.post '/code/postmessage.php', post_data, {'Cookie' => cookie}
  puts i
end
