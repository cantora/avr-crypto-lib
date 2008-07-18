#!/usr/bin/ruby 

require "serialport.so"

if ARGV.size < 5
  STDERR.print <<EOF
  Usage: ruby #{$0} port bps nbits stopb command [target_dir] [additional specifier]
EOF
  exit(1)
end

command=ARGV[4]+" ";
$dir=(ARGV.size>=6)?ARGV[5]:"";
param=(ARGV.size>=7)?ARGV[6]:"";

puts("\nPort: "+ARGV[0]+ "@"+ARGV[1]+" "+ARGV[2]+"N"+ARGV[3]+"\n");
$linewidth = 16
$sp = SerialPort.new(ARGV[0], ARGV[1].to_i, ARGV[2].to_i, ARGV[3].to_i, SerialPort::NONE);
$sp.read_timeout=5000; # 5 seconds
$sp.write(command);

def readTestVector(param)
  fname=$dir;
  lb="";
  buffer="";
  set=0;
  vector=0;
  begin
    lb=$sp.gets();
    if (m=/unknown command/.match(lb) || m=/[Ee][Rr]{2}[Oo][Rr]/.match(lb))
      puts("ERROR: "+lb);
      exit(2);
    end
    if(lb==nil)
      return false;
    end
  end while(m=/\*+/.match(lb));
  
  buffer += lb;
  begin
    lb=$sp.gets();
    if(lb==nil)
      return false;
    end
    buffer+=lb;
  end while(m=/\*.*/.match(lb));

  while(!(m=/Test vectors/.match(lb))) 
    m=/[^:]*:[\s]([A-Za-z0-9_-]*)/.match(lb);
    if(m) 
      fname+=m[1]+".";
    end
    buffer+=lb;
    lb = $sp.gets();
  end
  if(param!="")
    fname+=param+".";
  end
  puts("-> "+fname+"txt");
  file=File.new(fname+"txt", "w");
    buffer+=lb;
    file.write(buffer);
    begin
      if (m=/Test\ vectors\ \-\-\ set[\s]+([0-9]+)/.match(lb))
	set=m[1].to_i;
	print("\nSet "+m[1]+":");
      end
      if (m=/Set [0-9]*, vector#[\s]*([0-9]+):/.match(lb))
        vector=m[1].to_i;
	#print(" "+m[1]);
	if(vector!=0 && vector % $linewidth==0)
	  print("\n      ")
	end
        printf(" %3u", vector);
      end
      lb=$sp.gets();
      if(lb==nil)
        file.close();
        return false;
      end
      file.write(lb);
    end while(!m=/End of test vectors/.match(lb));
    puts("\n");
  file.close();
  return true
end

if(readTestVector(param)==false)
  puts("ERROR: test seem not to be implemented");
  exit(3);
end

while(readTestVector(param))
end

exit(0);


