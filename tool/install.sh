#!/bin/bash
SCP_DIR=$(cd $(dirname $0);pwd);

init_ttr () {
  cd /usr/local/src
  # get ttr
  git clone https://github.com/simpart/tetraring4c.git
  if [ $? != 0 ]; then
    exit;
  fi
  # build
  cd tetraring4c
  make
  make install
}

init_cnf () {
  mkdir /etc/pkthook
  if [ $? != 0 ]; then
    exit;
  fi
  cp SCP_DIR/pkthook.cnf /etc/pkthook/
  cp SCP_DIR/tcpcon.cnf /etc/pkthook/
}


### start install ###
echo 'packethooker installation start'
echo '==============================='

#init_ttr
init_cnf
