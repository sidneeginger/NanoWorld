cd /d %~dp0
cmake -DBUILDDIR="./Server" -P ./cmake/genrev.cmake ./Server