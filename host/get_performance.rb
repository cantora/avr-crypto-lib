#!/usr/bin/ruby 
# get_performance.rb
=begin
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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

require 'serialport'

def read_line(error_msg=true)
  s = $sp.gets()
  if s==nil
    puts("ERROR: read timeout!\n") if error_msg
	return nil
  end	
  s.gsub(/\006/, '');	
end

def readPerformanceVector(param)
  lb=""
  buffer=""
  fname=""
  fout=0
  begin
    lb = read_line()
    if lb.match(/End of performance figures/)
      return false
	end
	if m=lb.match(/=== (.*) performance ===/) 
	  fout.close if fout!=0
	  fname=$dir+m[1]
	  fname+="."+param if param != ""
	  fname+=".txt"
	  fout = File.open(fname, "w+")
	  printf("> %s \n", fname)	
	  fout.write(lb)
    else
	  if fout!=0 && lb!=""
	    fout.write(lb)
	  end	
	end
  end while true
end


if ARGV.size < 5
  STDERR.print <<EOF
  Usage: ruby #{$0} port bps nbits stopb command [target_dir] [additional specifier]
EOF
  exit(1)
end

command=ARGV[4]+"\r";
$dir=(ARGV.size>=6)?ARGV[5]:"";
param=(ARGV.size>=7)?ARGV[6]:"";

puts("\nPort: "+ARGV[0]+ "@"+ARGV[1]+" "+ARGV[2]+"N"+ARGV[3]+"\n");
$linewidth = 16
$sp = SerialPort.new(ARGV[0], ARGV[1].to_i, ARGV[2].to_i, ARGV[3].to_i, SerialPort::NONE);
$sp.read_timeout=1000; # 1 secound
$extended_wait=100;
$sp.write(command);

while(readPerformanceVector(param))
end

exit(0);


