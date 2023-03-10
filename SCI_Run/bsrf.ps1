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
NJCR add -doj -cm zlib -fc zlib ../../SCI_Exe/SCI_Run.srf
# -puremerge
cd ..
