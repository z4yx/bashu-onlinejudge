#!/usr/bin/env ruby

require 'net/http'
require 'json'

USER = 'NOI'
PASSWD = 'NOINOI'
LANGS = {"G++" => 0, "GCC" => 1, "Pascal" => 2, "G++(0x)" => 3};
HOST = 'oa.bashu.com.cn'
PORT = 8080
CODE2MSG = {0 => "Correct", 2 => "Time Out", 3 => "MLE", 4 => "Wrong Answer", 5 => "Runtime Error", 99 => "Validator Error"};
CODE2COLOR = {0 => 42, 2 => 43, 3 => 43, 4 => 41, 5 => 46, 99 => 40};

def die message, code = 1
	puts message.to_s
	exit code 	
end

def with_color fore, back = nil
	unless back
		print "\x1B[0;#{fore}m"
	else
		print "\x1B[0;#{fore};#{back}m"
	end
	yield
	puts "\x1B[0m"
end

#Check arguments...
die "Usage: #{__FILE__} filename problem_id lang" if ARGV.size != 3

#Read the F**king Source Code
source = ""
begin
	File.open(ARGV[0], 'r') {|f| source = f.read }
rescue SystemCallError => e
	die e
end

die "No such problem - " + ARGV[1] unless ARGV[1] =~ /^\d+$/
die "No such language : lang = [G++ | GCC | Pascal | G++(0x)]" unless LANGS.has_key? ARGV[2]

#Login
puts "User : #{USER}"
puts 'Login ...Wait...'

conn = Net::HTTP.new HOST, PORT
post_data = URI.encode_www_form('uid' => USER, 'pwd' => PASSWD, 'url' => 'index.php')
resp = conn.post '/code/login.php', post_data, {}
die "Can't connect to OJ, check your network. HTTP Code - " + resp.code if resp.code != '302' && resp.code != '200' 
die 'Invalid username or password. ' if resp.body.size != 0

cookie = resp.response['set-cookie']

#Submit
puts printf 'Submit ...Wait...'
post_data = URI.encode_www_form('source' => source, 'problem' => ARGV[1], 'language' => LANGS[ARGV[2]])
resp = conn.post '/code/submit.php', post_data, {'Cookie' => cookie}
die "Connection closed without indication" if resp.code != '302' && resp.code != '200' 

#Show result
query_key = resp.response['location'][-32 .. -1]
puts 'Judging...' + query_key

retry_times = 3
info_reted = 0
loop do
	begin
		resp = conn.get '/query/query_' + query_key
		raise Exception if resp.code != '200'
	rescue Exception
		die "Connection closed without indication, please contact with ZYX" if retry_times == 0
		retry_times -= 1
	   	retry
	end	

	info = JSON.load resp.body
	detail = info['detail']
	len = detail.size - 1;
	if len > info_reted
		case detail[0][0]
		when 7
			with_color 31 do
				puts "Compile Error"
				with_color(30, 47) {puts detail[0][3]}
			end
		when 100
			with_color 31 do
				puts "System Error"
				with_color(30, 47) {puts "Please contact with the admin."}
			end
		else
			for i in info_reted ... len
				code = detail[i][0]
				with_color(37, CODE2COLOR[code]) do
					print (printf "%3d %15s %6d ms %7d KB", i, CODE2MSG[code], detail[i][1], detail[i][2])
				end
				puts detail[i][3] if (4 .. 5).member? code
				puts "------------------------------------------------\n\n"
			end
		end
		info_reted = len 
	end
	die 'Done...', 0 if info['state'] == 'finish'
end
