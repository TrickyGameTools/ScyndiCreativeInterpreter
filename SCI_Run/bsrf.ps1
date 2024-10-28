# License:
# 
# Build SCI Resource File
# Scyndi's Creative Interpreter
# 
# 
# 
# 	(c) Jeroen P. Broks, 2024
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
# Version: 24.10.10
# End License
# Build SCI Resource File

cd srf
$mydate = date
echo "# SRF Identification file!
# Please note that this file may not live in version control repositories, as it is generated by bsrf.ps1 
# and will as result spook up the repository.

[srf]
Engine=SCI
Description=Resource for Scyndi's Creative Interpreter
Build=$mydate
" > ID/Identify.ini
#cp "Scyndi:/Projects/Applications/Slyvina/Tools/Scyndi/QuickScyndi/QuickScyndi/ScyndiCore.lua" Script
cp "Scyndi:/Projects/Applications/Slyvina/Tools/Scyndi/Script/ScyndiCore.lua" Script
jalondi create -m -y -cm zlib -fc zlib ../../SCI_Exe/SCI_Run.srf
# -puremerge
cd ..
