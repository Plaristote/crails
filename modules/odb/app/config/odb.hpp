#ifndef  CONFIG_ODB_HPP
# define CONFIG_ODB_HPP

// Enable schema versioning
# pragma db model version(1,1)

// Map your odb types here
# pragma db map type("INTEGER\\[\\]") as("TEXT") to("(?)::INTEGER[]") from("(?)::TEXT")

#endif
