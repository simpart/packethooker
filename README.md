# PacketHooker
PachetHooker is packet capture that has analyze and notify.<br>
It sends POST if packets are matched specified status.

## Analyzer List

|Analyzer Name| Description |
|:------------|:------------|
|TCP-Connection Counter| it counts tcp-connection every server.<br>for example, it sends POST if the counter exceeds specified count.|
|HTTP Responce Checker | not supported yet   |
|Query Responce Checker | not supported yet  |

## Architecture Overview
![arch](https://raw.githubusercontent.com/simpart/packethooker/img/img/architecture.png)

## Supported Environment
- CentOS 7.x
- x86 cpu

## Quick Start
### install packet-hooker
```
git clone https://github.com/simpart/packethooker.git
cd packethooker
./tool/install.sh
```

### capture setting
```
vi /etc/pkthooker/pkthooker.cnf
CAPTURE_IFACE = "ens3"   # add target interface name
```

### notify setting
this is example for slack post setting
```
vi /etc/pkthooker/pkthooker.cnf
HOOK_URI = "https://hooks.slack.com/services/XXXXX/XXXXX"
HOOK_PRM = "channel=#(channel-name),username=(botname),text={@message}"
```

### start packethooker daemon
```
systemctl enable pkthooker
systemctl start pkthooker
systemctl status pkthooker
```

