# Build SCI Resource File

cd srf
$mydate = date
echo "[srf]
Engine=SCI
Description=Resource for Scyndi's Creative Interpreter
Build=$mydate
" > ID/Identify.ini
NJCR add -doj -cm zlib -fc zlib ../../SCI_Exe/SCI_Run.srf