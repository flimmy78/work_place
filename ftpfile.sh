#!/bin/sh
#sh getfile.sh ipAddr user password remotePath fileName
cd ../tmp
echo "open $1 21
      user $2 $3
      pwd
      cd $4
      get $5
      close
      bye" | ftp -i -in
cd ../proc

