require 'rbconfig'

CONFIG = RbConfig::MAKEFILE_CONFIG


case ARGV[0]
when "archdir"
    puts RbConfig::expand(CONFIG["archdir"])
when "lib"
    puts RbConfig::expand(CONFIG["libdir"])
when "vendorarchdir"
    puts RbConfig::expand(CONFIG["vendorarchdir"])
when "sitearchdir"
    puts RbConfig::expand(CONFIG["sitearchdir"])
end

