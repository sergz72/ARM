#! /bin/sh

stty -F /dev/ttyACM0 115200 raw -echo   #CONFIGURE SERIAL PORT
exec 3</dev/ttyACM0                     #REDIRECT SERIAL OUTPUT TO FD 3
cat <&3 > ttyDump.dat &                 #REDIRECT SERIAL OUTPUT TO FILE
PID=$!                                  #SAVE PID TO KILL CAT
echo "s" >/dev/ttyACM0                  #SEND COMMAND STRING TO SERIAL PORT
sleep 20s                               #WAIT FOR RESPONSE
kill $PID                               #KILL CAT PROCESS
wait $PID 2>/dev/null                   #SUPRESS "Terminated" output
exec 3<&-                               #FREE FD 3
