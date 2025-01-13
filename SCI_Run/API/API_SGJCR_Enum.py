# License:
# 
# Scyndi's Creative Interpreter
# Enum Data Generator
# 
# 
# 
# 	(c) Jeroen P. Broks, 2023, 2025
# 
# 		This program is free software: you can redistribute it and/or modify
# 		it under the terms of the GNU General Public License as published by
# 		the Free Software Foundation, either version 3 of the License, or
# 		(at your option) any later version.
# 
# 		This program is distributed in the hope that it will be useful,
# 		but WITHOUT ANY WARRANTY; without even the implied warranty of
# 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# 		GNU General Public License for more details.
# 		You should have received a copy of the GNU General Public License
# 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
# 	Please note that some references to data like pictures or audio, do not automatically
# 	fall under this licenses. Mostly this is noted in the respective files.
# 
# Version: 25.01.13
# End License
"""   -- Start License block
Scyndi's Creative Interpreter
Enum Data Generator



(c) Jeroen P. Broks, 2023

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

Please note that some references to data like pictures or audio, do not automatically
fall under this licenses. Mostly this is noted in the respective files.

Version: 23.01.21
-- End License block   """
# Quick Python script. Will safe me a lot of work when expanding the SGJCR system


HPP = "// Generated header\n// Do not modify manually but use the API_SGJCR_Enum.py script for Python in stead!\n\n\n#include <map>\n#include <string>\n\nnamespace Scyndi_CI {\n\n"

ENUM = [
	"ID",
	"TxtData",
	"BinData",
	"RPGStat"
]

First = True
HPP += "\tenum class SGJCR_Types { "
for E in ENUM:
	if not First: HPP+=", "
	First = False
	HPP += E
HPP +="};\n\n";

HPP += "\tstatic std::map<SGJCR_Types, std::string> En2Nm {\n"
First = True
for E in ENUM:
	if not First: HPP+=",\n"
	First = False
	HPP += "\t\t{SGJCR_Types::%s,\"%s\"}"%(E,E)
HPP += "\n\t};\n\n"


HPP += "\tstatic std::map<std::string, SGJCR_Types> Nm2En {\n"
First = True                                                  
for E in ENUM:                                                 
	if not First: HPP+=",\n"                                 
	First = False
	HPP += "\t\t{\"%s\",SGJCR_Types::%s}"%(E.upper(),E)           
HPP += "\n\t};\n\n}\n\n"                                             

print(HPP)

print("\n\nSaving header file")

text_file = open("API_SGJCR_Enum.hpp", "w")
n = text_file.write(HPP)
text_file.close()
print("Done!")
