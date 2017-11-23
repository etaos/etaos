#!/usr/bin/env ruby

#
# avrdude frontend
#
# Author: Michel Megens
# E-mail: dev@bietje.net
#

require 'ostruct'
require 'optparse'
require 'optparse/time'

module ETAOS
  VERSION = '0.1.0'

  class << self
    AVRDUDE_CMD = "avrdude -DqV ".freeze

    def upload(options)
      fix_tty(options[:port]) if options[:tty_fix] or ENV['TTY_FIX'].eql? 'true'

      mcu = options[:mcu]
      port = options[:port]
      prog = options[:programmer]
      hex = options[:hex]
      baud = options[:baud]
      conf = options[:config]
      eeprom = options[:eeprom]

      avrdude_args = AVRDUDE_CMD
      avrdude_args += "-p #{mcu} -c #{prog} -b #{baud} -P #{port} -C #{conf} "
      avrdude_args += "-U flash:w:#{hex}:i" unless eeprom
      avrdude_args += "-U eeprom:w:#{hex}:i" if eeprom

      `#{avrdude_args}`
    end

    private
    STTY_FIX_COMMAND = "stty 115200 raw ignbrk hup".freeze

    def fix_tty(port)
      fix_cmd = "#{STTY_FIX_COMMAND} < #{port}"
      system(fix_cmd)
    end
  end
end

def parse(args)
  options = OpenStruct.new

  options.tty_fix = false
  options.hex = nil
  options.mcu = nil
  options.programmer = nil
  options.port = nil
  options.baud = 115200
  options.config = "/etc/avrdude.conf"
  options.eeprom = false

  parser = OptionParser.new do |p|
    p.banner = "Usage: avr_upload [options]"
    p.separator ""
    p.separator "Specific options:"

    p.on('-f', '--tty-fix', "Reset the TTY config before uploading") do
      options.tty_fix = true
    end

    p.on('-e', '--eeprom', "Write to the internal EEPROM") do
      options.eeprom = true
    end

    p.on('-H [FILE]', '--hex [FILE]',
         'Hex file to upload to the AVR program memory') do |file|
      options.hex = file
    end

    p.on('-m [MCU]', '--mcu [MCU]',
         'The type of MCU you are trying to upload to') do |mcu|
      options.mcu = mcu
    end

    p.on('-p [PROGRAMMER]', '--programmer [PROGRAMMER]',
         'Programmer to use to upload the hex file to the AVR') do |prog|
      options.programmer = prog
    end

    p.on('-P [PORT]', '--port [PORT]',
         'Serial port to which the board is connected') do |port|
      options.port = port
    end

    p.on('-b [BAUD]', '--baud [BAUD]',
         'Serial baud rate to use') do |baud|
      options.baud = baud
    end

    p.on('-c [FILE]', '--config [FILE]',
         'Config file for avrdude, defaults to /etc/avrdude.conf') do |conf|
      options.config = conf
    end

    p.separator ""
    p.separator "Common options:"

    p.on_tail('-h', '--help', "Show this message") do
      puts p
      exit
    end

    p.on_tail('-v', '--version', 'Show the version number') do
      puts "AVR uploader #{ETAOS::VERSION}"
      exit
    end
  end

  parser.parse!
  mandatory = [:programmer, :hex, :mcu, :port]
  missing = mandatory.select do |param|
    param if options[param].nil?
  end

  unless missing.empty?
    puts "Missing mandatory options: #{missing.to_s}"
    puts ""
    puts parser
    exit 1
  end

  options
end

options = parse ARGV
ETAOS.upload options
