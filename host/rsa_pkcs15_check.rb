#!/usr/bin/ruby
# rsa_pkcs15_check.rb
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

require 'rubygems'
require 'serialport'
require 'getopt/std'

$buffer_size = 0 # set automatically in init_system
$conffile_check = Hash.new
$conffile_check.default = 0
$debug = false
$logfile = nil

################################################################################
# readconfigfile                                                               #
################################################################################

def read_line_from_device()
  repeat_counter = 10000
  l = nil
  s = ''
  begin
    l = $sp.gets()
    repeat_counter -= 1
  end while !l && repeat_counter > 0
  t = Time.new
  $logfile.printf("DBG: (%02d:%02d:%02d)<< %s\n", t.hour, t.min, t.sec, l.inspect) if $debug
  if l && l.include?("AVR-Crypto-Lib")
    $logfile.printf("DBG: system crashed !!!\n")
    exit(false)
  end
  return l
end

def readconfigfile(fname, conf)
  return conf if $conffile_check[fname]==1
  $conffile_check[fname]=1
  section = "default"
  if not File.exists?(fname)
    return conf
  end
  file = File.open(fname, "r")
  until file.eof
    line = file.gets()
	  next if /[\s]*#/.match(line)
  	if m=/\[[\s]*([^\s]*)[\s]*\]/.match(line)
	    section=m[1]
	    conf[m[1]] = Hash.new
	    next
	  end
	  next if ! /=/.match(line)
	  m=/[\s]*([^\s]*)[\s]*=[\s]*([^\s]*)/.match(line)
	  if m[1]=="include"
	    Dir.glob(m[2]){ |fn| conf = readconfigfile(fn, conf) }
	  else
  	  conf[section][m[1]] = m[2]
	  end
  end
  file.close()
  return conf
end

################################################################################
# reset_system                                                                 #
################################################################################

def reset_system
  $sp.print("\r")
  sleep 0.1
  $sp.print("\r")
  sleep 0.1
  $sp.print("echo off\r")
  sleep 0.1
end


def read_block(f)
  d = Array.new
  begin
    l = f.gets
    x = l.split.collect { |e| e.to_i(16) }
    d += x
  end while x.length == 16
  return d
end

=begin
# Modulus: 
# Exponent: 
# Modulus: 
# Public exponent: 
# Exponent: 
# Prime 1: 
# Prime 2: 
# Prime exponent 1: 
# Prime exponent 2: 
# Coefficient: 
# Message:
# Seed:
# Encryption:

=end

def get_next_block(f)
  ret = Hash.new
  data = Array.new
  begin
    l = f.gets
  end while l && ! m= l.match(/^#[\s](.*):[\s]*$/)
  return nil if ! l
  ret['tag'] = m[1]
  ret['line'] = f.lineno
  data = read_block(f)
  ret['data'] = data
  return ret
end

$key_sequence = [
  'Modulus',             # 0
  'Exponent',            # 1
  'Modulus',             # 2
  'Public exponent',     # 3
  'Exponent',            # 4
  'Prime 1',             # 5
  'Prime 2',             # 6
  'Prime exponent 1',    # 7
  'Prime exponent 2',    # 8
  'Coefficient',         # 9
]

def key_consitency_check(k)
  return true
end

def process_file(f, skip_key=1, skip_vec=1)
  a = get_next_block(f)
  key_no = 0
  ok_counter = 0
  fail_counter = 0
  begin
    if !a || ! a['tag'] == 'Modulus'
      printf("ERROR: a = %s %d\n", a.inspect, __LINE__)
      return
    end
    k_seq = Array.new
    k_seq[0] = a
    (1..($key_sequence.length-1)).each do |i|
      a = get_next_block(f)
      if ! a || a['tag'] != $key_sequence[i]
        printf("ERROR: (expecting: %s) a = %s %d\n", $key_sequence[i], a.inspect, __LINE__)
      end
      k_seq[i] = a
    end
    key = convert_key(k_seq)
    printf("ERROR: %d\n", __LINE__) if ! key
    key_no += 1
    vec_no = 0
    printf("\n run %3d: ", key_no)
    skip_key_flag = (key_no < skip_key)
    load_key(key) if ! skip_key_flag
    test_seq = Array.new
    a = get_next_block(f)
    printf("ERROR: %d\n", __LINE__) if ! a 
    begin
      vec_no += 1
      b = get_next_block(f)
      c = get_next_block(f)
      tv = Hash.new
      tv['msg'] = a['data']
      tv['seed'] = b['data']
      tv['enc'] = c['data'] 
      skip_vec_flag = (skip_key_flag || (key_no == skip_key && vec_no < skip_vec))
      if skip_vec_flag
        printf('o')
      else
        v = check_tv(tv)
        if(v == true)
          printf('*')
          $logfile.printf("[[Test %2d.%02d = OK]]\n", key_no, vec_no)
          ok_counter += 1
        else
          printf('%c', v ? '*' : '!')
          $logfile.printf("[[Test %2d.%02d = FAIL]]\n", key_no, vec_no)
          fail_counter += 1
        end
      end
      a = get_next_block(f)
    end while a && a['tag'] == 'Message'
  end while a && a['tag'] = 'Modulus'
#  printf("\nResult: %d OK / %d FAIL ==> %s \nFinished\n", ok_counter, fail_counter, fail_counter==0 ? ':-)' : ':-(')
  return ok_counter,fail_counter
end

def convert_key(k_seq)
  l = ['n', 'e', 'd', 'p', 'q', 'dP', 'dQ', 'qInv']
  r = Hash.new
  return nil if k_seq[0]['data'] != k_seq[2]['data']
  return nil if k_seq[1]['data'] != k_seq[3]['data']
  8.times do |i|
    r[l[i]] = k_seq[2 + i]['data'] 
  end
  return r
end

def wait_for_dot
  begin
    s = $sp.gets()
  end while !s || !s.include?('.')
end

def load_bigint(d)
  $sp.printf("%d\r", d.length)
  while l = read_line_from_device()
    break if /data:/.match(l)
  end
  printf "ERROR: got no answer from system!" if !l
  i = 0
  d.each do |e|
    $sp.printf("%02x", e)
    i += 1
    if i % 60 == 0
# we should now wait for incomming dot
      wait_for_dot()
      print('.')
    end
  end
end

def hexdump(a)
  i = 0
  a.each do |e|
    printf("\n\t") if i % 16 == 0
    printf('%02x ', e)
    i += 1
  end
  puts('') if i % 16 != 1
end

def str_hexdump(a)
  i = 0
  s = ''
  a.each do |e|
    s += "\n\t" if i % 16 == 0
    s += sprintf('%02x ', e)
    i += 1
  end
  s += "\n" if i % 16 != 1
  return s
end

def load_key(k)
  $sp.print("load-key\r")
  sleep 0.1
  v = ['n', 'e', 'p', 'q', 'dP', 'dQ', 'qInv']  
  v.each do |e|
    load_bigint(k[e])
    $logfile.printf("DBG: loaded %s\n", e) if $debug
  end 
  while l = read_line_from_device()
    break if />/.match(l)
  end
end

def strip_leading_zeros(a)
  loop do
    return [] if a.length == 0
    return a if a[0] != 0
    a.delete_at(0)
  end
end

def check_tv(tv)
  sleep 0.1
  $sp.print("seed-test\r")
  sleep 0.1
  load_bigint(tv['msg'])
  $logfile.printf("DBG: loaded %s\n", 'msg') if $debug
  sleep 0.1
  tv['seed'].each { |e| $sp.printf(" %02x", e) } 
  while l = read_line_from_device() 
    break if /ciphertext:/.match(l)
  end
  test_enc = ''
  loop do 
    l = read_line_from_device()
    break if ! /([0-9A-Fa-f]{2}\s*)+/.match(l)
    test_enc += l if l
  end
  test_enc_a = Array.new
  test_enc = test_enc.split(/[\W\r\n]+/)
  test_enc.each do |e|
    v = e.sub(/[^0-9A-Fa-f]/, '') 
    test_enc_a << v if v.length == 2
  end
  test_enc_a.collect!{ |e| e.to_i(16) }
  strip_leading_zeros(test_enc_a)
  strip_leading_zeros(tv['enc'])
  enc_ok = (test_enc_a == tv['enc'])
  if !enc_ok
    $logfile.printf("DBG: ref = %s test = %s\n", str_hexdump(tv['enc']) , str_hexdump(test_enc_a))
  end
  m = nil
  loop do 
    l = read_line_from_device() 
    m = /(>>OK<<|ERROR)/.match(l)
    break if m
  end
  return true if enc_ok && (m[1] == '>>OK<<') 
  return false
end

########################################
# MAIN
########################################


opts = Getopt::Std.getopts('dc:f:il:s:')

conf = Hash.new
conf = readconfigfile("/etc/testport.conf", conf)
conf = readconfigfile("~/.testport.conf", conf)
conf = readconfigfile("testport.conf", conf)
conf = readconfigfile(opts["c"], conf) if opts["c"]

#puts conf.inspect

puts("serial port interface version: " + SerialPort::VERSION);
$linewidth = 64
params = { "baud"       => conf["PORT"]["baud"].to_i,
            "data_bits" => conf["PORT"]["databits"].to_i,
            "stop_bits" => conf["PORT"]["stopbits"].to_i,
            "parity"    => SerialPort::NONE }
params["paraty"] = SerialPort::ODD   if conf["PORT"]["paraty"].downcase == "odd"
params["paraty"] = SerialPort::EVEN  if conf["PORT"]["paraty"].downcase == "even"
params["paraty"] = SerialPort::MARK  if conf["PORT"]["paraty"].downcase == "mark"
params["paraty"] = SerialPort::SPACE if conf["PORT"]["paraty"].downcase == "space"

puts("\nPort: "+conf["PORT"]["port"]+"@"    +
                params["baud"].to_s      +
                " "                      +
                params["data_bits"].to_s +
                conf["PORT"]["paraty"][0,1].upcase +
                params["stop_bits"].to_s +
                "\n")

$sp = SerialPort.new(conf["PORT"]["port"], params)

$sp.read_timeout=1000; # 5 minutes
$sp.flow_control = SerialPort::SOFT

$debug = true if opts['d']

if opts['l']
  $logfile = File.open(opts['l'], 'w')
end

$logfile = STDOUT if ! $logfile
reset_system()

if opts['s'] && m = opts['s'].match(/([\d]+\.([\d]+))/)
  sk = m[1].to_i
  sv = m[2].to_i
else
  sk = 1
  sv = 1
end

f = File.open(opts['f'], "r")
exit if !f
ok,fail = process_file(f,sk,sv)
printf("\nOK: %d FAIL: %d :-%s\n",ok,fail, fail==0 ? ')':'(')


