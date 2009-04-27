#!/usr/bin/ruby 
# shavs_test.rb
=begin
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
=end

$debug = false
require 'rubygems'
require 'serialport'

def init_system
  sleep 1
  $sp.print("echo  off \r")
  print("DBG i: " + "echo  off \r"+"\n") if $debug
#  line = $sp.readlines()
#  print("DBG 0.0: ")
#  print(line)
#  sleep 1
  $sp.print("shavs_set #{$algo_select} \r")
  print("DBG i: " + "shavs_set #{$algo_select} \r"+"\n") if $debug
#  line = $sp.readlines()
#  print("DBG 0.1: ")
#  print(line)
#  sleep 1
  $sp.print("shavs_test1 \r")
  print("DBG i: " + "shavs_test1 \r"+"\n") if $debug
#  line = $sp.readlines()
#  print("DBG 0.2: ")
#  print(line)
end

def get_md
  begin
    line = $sp.gets()
	line = "" if line==nil
	puts("DBG g: "+line) if $debug
  end while not /[\s]*MD[\s]*=.*/.match(line)
  return line	
end

def send_md(md_string)
  for i in 0..md_string.length-1
    $sp.print(md_string[i].chr)
#	print("DBG s: "+ md_string[i].chr) if $debug
	if(i%20==19)
#		sleep(0.1)
	end		
  end
end

def run_test(filename)
  errors = 0
  line=1
  if not File.exist?(filename)
  	puts("ERROR file "+filename+" does not exist!")
  end
  pos = 0
  file = File.new(filename, "r");
  until file.eof
    begin
      lb=file.gets()
    end while not (file.eof or (/[\s]*Len[\s]*=.*/.match(lb)))
    puts("DBG sending: "+lb) if $debug
	return if file.eof
	$sp.print(lb.strip)
	$sp.print("\r")
    begin
	  lb=file.gets()
    end while not (file.eof or (/[\s]*Msg[\s]*=.*/.match(lb)))
    return if file.eof
    puts("DBG sending: "+lb) if $debug
	send_md(lb.strip)
	avr_md = get_md()
    begin
	  lb=file.gets()
    end while not /[\s]*MD[\s]*=.*/.match(lb)
	a = (/[\s]*MD[\s]*=[\s]*([0-9a-fA-F]*).*/.match(lb))[1];
	b = (/[\s]*MD[\s]*=[\s]*([0-9a-fA-F]*).*/.match(avr_md))[1];
	a.upcase!
	b.upcase!
	printf("\n%4d: ", line) if (pos%$linewidth==0 and $linewidth!=0)
	line += 1               if (pos%$linewidth==0 and $linewidth!=0)
	#putc((a==b)?'*':'!')
	if(a==b)
	  putc('*')
	else
	  putc('!')
	  errors += 1;
	end  
	pos += 1
  end
  return errors
end

if ARGV.size < 6
  STDERR.print <<EOF
  Usage: ruby #{$0} port bps nbits stopb algo_select testfile ...
EOF
  exit(1)
end

puts("\nPort: "+ARGV[0]+ "@"+ARGV[1]+" "+ARGV[2]+"N"+ARGV[3]+"\n");
puts("serial port interface version: " + SerialPort::VERSION);
$linewidth = 64
$params = { "baud"      => ARGV[1].to_i, 
            "data_bits" => ARGV[2].to_i, 
            "stop_bits" => ARGV[3].to_i, 
            "parity"    => SerialPort::NONE }
$sp = SerialPort.new(ARGV[0], $params)
#$sp = SerialPort.new(ARGV[0], ARGV[1].to_i, ARGV[2].to_i, ARGV[3].to_i, SerialPort::NONE);

$sp.read_timeout=1000; # 5 minutes
$sp.flow_control = SerialPort::SOFT
$algo_select = ARGV[4]
#irb

init_system()

for i in (5..(ARGV.size-1))
  errors = run_test(ARGV[i])
  if errors == 0
    puts("\n[ok]")
  else
    puts("\n[errors: "+errors.to_s+"]")
  end
end
 $sp.print("EXIT\r");

#exit(0);


