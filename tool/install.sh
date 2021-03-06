#!/bin/bash
SCP_DIR=$(cd $(dirname $0);pwd);

install_ttr () {
  cd /usr/local/src
  if [ $? != 0 ]; then
    exit;
  fi
  
  # get ttr4c
  git clone https://github.com/simpart/tetraring4c.git
  if [ $? != 0 ]; then
    exit;
  fi
  # build
  cd tetraring4c
  if [ $? != 0 ]; then
    exit;
  fi
  
  make
  if [ $? != 0 ]; then
    exit;
  fi
  make install
  if [ $? != 0 ]; then
    exit;
  fi
  
  # install ttr4php
  cd $SCP_DIR/../src/php/
  git clone https://github.com/simpart/tetraring4php.git ttr
  
}

install_pia () {
  cd /usr/local/src
  if [ $? != 0 ]; then
    exit;
  fi
  # get protopia
  git clone https://github.com/simpart/protopia4c.git
  if [ $? != 0 ]; then
    exit;
  fi
  # build
  cd protopia4c
  if [ $? != 0 ]; then
    exit;
  fi
  
  make
  if [ $? != 0 ]; then
    exit;
  fi

  make install
  if [ $? != 0 ]; then
    exit;
  fi
}

install_pkthkr () {
  cd $SCP_DIR/../
  if [ $? != 0 ]; then
    exit;
  fi
  
  make
  if [ $? != 0 ]; then
    exit;
  fi
  
  make install
  if [ $? != 0 ]; then
    exit;
  fi
  
}

init_cnf () {
  # get ifname
  for IFNAME in $(ls /sys/class/net/)
  do
    if [ "lo" != ${IFNAME} ]; then
      break;
    fi
  done;
  if [ "lo" = ${IFNAME} ]; then
    echo "could not find interface name";
    exit;
  fi
  # set ifname
  sed -i -e "s/CAPTURE_IFACE = \"\"/CAPTURE_IFACE = \"$IFNAME\"/g" $SCP_DIR/pkthooker.cnf
  
  # copy config file
  mkdir /etc/pkthooker
  if [ $? != 0 ]; then
    exit;
  fi
  
  cp $SCP_DIR/pkthooker.cnf /etc/pkthooker/
  if [ $? != 0 ]; then
    exit;
  fi
  
  cp $SCP_DIR/tcpcon.cnf /etc/pkthooker/
  if [ $? != 0 ]; then
    exit;
  fi
  
  # enable library
  ldconfig
}


### start install ###
echo 'packethooker installation start'
echo '==============================='

# install required package
yum install -y gcc make php curl

# install required library
install_ttr
install_pia

# install packethooker
install_pkthkr

# regist to systemd
cp $SCP_DIR/pkthooker.service /etc/systemd/system/

init_cnf
