在虚拟机中使用Keepalived
^^^^^^^^^^^^^^^^^^^^^^^^

``Keepalived`` 同时提供了负载均衡和高可用性框架，其负载均衡框架是基于 ``LVS`` 的。LVS负载均衡类型（``lb_kind``）分直接路由模式 ``DR``、\
网络地址转换模式 ``NAT`` 以及隧道模式 ``TUN`` 三种。LVS客户端（真实服务器）的配置根据其所采用的负载均衡类型有所不同，在下面的示\
例中，为了获得最高的性能，我们选取了直接路由模式 ``DR``。

部署LVS客户端
-------------

我们分别用3台虚拟机来作为LVS客户端，每个虚拟机都在80端口提供HTTP服务

部署服务
>>>>>>>>

1. 我们直接使用Python3标准库中的简单HTTP服务器进行演示::

    python3 -m http.server 80

2. 在HTTP服务的工作目录中创建index.html，内容为虚拟机的固定IP::

    ifconfig eth0 | grep "inet addr" | cut -d ':' -f 2 | cut -d ' ' -f 1 > index.html

3. 在服务所在虚拟机中测试和验证服务，查看输出是否为本机的固定IP地址::

    curl http://localhost/

配置客户端
>>>>>>>>>>

1. 绑定 ``VIP`` 到环回接口 ``lo:0``，**假设VIP为192.168.1.200**::

    ifconfig lo:0 192.168.1.200 netmask 255.255.255.255 broadcast 192.168.1.200 up

.. Note :: 子网掩码为255.255.255.255，使网段只包含一个主机地址，目的是避免IP地址冲突

.. Note :: 直接路由模式的VIP必须与服务器对外提供服务的IP地址在同一个网段

2. 抑制 ``ARP`` 广播，因为有多个服务器配置了相同的VIP，如果不做ARP抑制，会导致乱套::

    echo "1" > /proc/sys/net/ipv4/conf/lo/arp_ignore
    echo "2" > /proc/sys/net/ipv4/conf/lo/arp_announce
    echo "1" > /proc/sys/net/ipv4/conf/all/arp_ignore
    echo "2" > /proc/sys/net/ipv4/conf/all/arp_announce

3. 在虚拟机中执行如下命令验证VIP是否绑定成功，查看输出中lo接口是否有配置的VIP::

    ip addr

4. 在3台真实服务器以外的且处于同一子网的虚拟机中执行如下命令验证ARP抑制是否有效，如果ping不通则证明有效::

    ping 192.168.1.200


部署Keepalived
--------------

我们分别用2台虚拟机来作为LVS转发器（一个MASTER一个BACKUP），以此来实现 ``failover`` （失败切换）。

1. 安装 ``ipvsadm``::

    apt install ipvsadm

2. 安装 ``keepalived``，可以从源码进行编译安装以使用最新版本，也可以从发行版的软件仓库安装旧版本::

    apt install keepalived

3. 配置keepalived，编辑或创建keepalived默认配置文件 ``/etc/keepalived/keepalived.conf``，MASTER转发器的配置内容如下::

    global_defs {
        router_id CK_TEST
    }

    vrrp_sync_group G1 {
        group {
            VI_1
        }
    }

    vrrp_instance VI_1 {
        state MASTER
        interface eth0
        lvs_sync_daemon_interface eth0
        virtual_router_id 50
        nopreempt
        priority 100
        advert_int 1
        authentication {
            auth_type PASS
            auth_pass admin123
        }
        virtual_ipaddress {
            192.168.1.200
        }
    }

    virtual_server 192.168.1.200 80 {
        delay_loop 6
        lb_algo rr
        lb_kind DR
        protocol TCP

        real_server 192.168.1.179 80 {
            weight 1
            HTTP_GET {
                url {
                    path /
                    digest b23518f93b38b0bdeac5a1bf4e205477
                }
                connect_timeout 3
                nb_get_retry 3
                delay_before_retry 3
            }
        }

        real_server 192.168.1.180 80 {
            weight 1
            HTTP_GET {
                url {
                    path /
                    digest b7d8be77ffd1f8f85b8b37fee91d4bb9
                }
                connect_timeout 3
                nb_get_retry 3
                delay_before_retry 3
            }
        }

        real_server 192.168.1.181 80 {
            weight 1
            HTTP_GET {
                url {
                    path /
                    digest 1fca06d0fe272d1ed040eda7b2c8dd77
                }
                connect_timeout 3
                nb_get_retry 3
                delay_before_retry 3
            }
        }
    }

.. Note :: 因为提供的HTTP服务，所以健康检查采用的HTTP_GET，其中的digest使用的keepalived包自带的genhash实用工具生成的，具体方法请自行查看官方文档

4. BACKUP转发器的配置内容与MASTER转发大同小异，需要修改3个部分：

- global_defs块的router_id修改为其他值
- vrrp_instance VI_1块的state值修改为BACKUP
- vrrp_instance VI_1块的priority值修改为小于100的值

5. 分别在2个转发器上启动keepalived守护进程::

    keepalived -D


配置虚拟机端口和安全组
----------------------

**圈重点了，期末要考**。OpenStack的网络服务启用了 ``security group`` （安全组）后，会为虚拟机的网络端口施加一些默认的安全规则，譬如说反欺骗，这就导致\
从虚拟机出来的包如果源MAC地址或源IP地址与对应端口关联的MAC地址和IP地址不一致那么就会被drop掉。而我们使用的NAT负载均衡类型，也就意味着真实服务器是\
直接响应用户请求的，但是响应的数据包源IP地址为VIP，所以这些响应是出不来的；而且LVS转发器只提供了调度和转发功能，其提供服务的VIP通常与其固定IP地址\
不一样，同样会因为反欺骗规则的存在而无法正常工作，因此我们需要通过配置端口的 ``allowed-address-pairs`` 来突破这个限制；另外因为安全组的存在，可能\
会使VRRP数据包无法在转发器直接流转，导致failover机制无法正常运作，典型的现象就是在MASTER转发器正常工作的情况下，BACKUP转发器也会转换进入MASTER状态。

配置allowed-address-pairs
>>>>>>>>>>>>>>>>>>>>>>>>>

对上述3台真实服务器和2台转发器对应的虚拟机，我们都应配置其端口的 ``allowed-address-pairs``，使源IP地址为任意地址的数据包都能从虚拟机中被发出，以下假设\
虚拟机均只有一个端口::

    neutron port-list --device-id <虚拟机UUID> awk -F'|' '{print $2}'  # 输出即为虚拟机的端口ID
    neutron port-update <端口ID> --allowed-address-pairs type=dict list=true ip_address=0.0.0.0/0 

配置转发器安全组
>>>>>>>>>>>>>>>>

为2台转发器对应的虚拟机，各添加一个安全组，用于放行和接收VRRP、ICMP数据包

- 创建安全组，由于新创建的安全组默认带有放行所有数据包的规则，因此我们只用在这个基础上创建2条接收的规则::

    neutron security-group-create enable-vrrp-icmp
    neutron security-group-rule-create --direction ingress --protocol 112 enable-vrrp-icmp  # VRRP协议号为112
    neutron security-group-rule-create --direction ingress --protocol icmp enable-vrrp-icmp

- 虚拟机关联安全组::

    neutron port-update --security-group enable-vrrp-icmp <虚拟机端口ID>  # 这个命令会覆盖端口已有的除了default以外的安全组，如果不想覆盖，就追加原先的安全组


测试验证
--------

负载均衡和故障隔离
>>>>>>>>>>>>>>>>>>

1. 在3台真实服务器和2台转发器以外的且处于同一子网的虚拟机中访问虚拟服务器，多次执行，观察输出是否在3台真实服务器的固定IP中轮转::

    curl http://192.168.1.200/

2. 在IP地址为192.168.1.179的真实服务器上手动关闭HTTP服务，然后继续访问虚拟服务器，多次执行，观察输出是否在其他2台真实服务器的固定IP中轮转

3. 再次启动IP地址为192.168.1.179的真实服务器上的HTTP服务，然后继续访问虚拟服务器，多次执行，观察输出是否在全部3台真实服务器的固定IP中轮转

.. Note :: 关闭或启动服务后可能需要等待几秒，因为健康检查存在间隔时间，真实服务器停止或恢复运转后，Keepalived需要一点时间才能够意识到

失败切换
>>>>>>>>

1. 手动关闭MASTER转发器上的keepalived守护进程::

    killall keepalived

2. 读BACKUP转发器的 ``/var/log/syslog`` 日志文件，查看是否有记录表明BACKUP转发器切换到MASTER状态::

    Sep  4 07:02:21 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) Transition to MASTER STATE                 <- 转换到MASTER状态
    Sep  4 07:02:21 vinzor Keepalived_vrrp[2395]: VRRP_Group(G1) Syncing instances to MASTER state
    Sep  4 07:02:22 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) Entering MASTER STATE
    Sep  4 07:02:22 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) setting protocol VIPs.
    Sep  4 07:02:22 vinzor Keepalived_healthcheckers[2394]: Netlink reflector reports IP 192.168.1.200 added
    Sep  4 07:02:22 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) Sending gratuitous ARPs on eth0 for 192.168.1.200
    Sep  4 07:02:22 vinzor kernel: [12880.868117] IPVS: stopping backup sync thread 2638 ...
    Sep  4 07:02:22 vinzor kernel: [12880.871881] IPVS: sync thread started: state = MASTER, mcast_ifn = eth0, syncid = 50, id = 0

3. 在3台真实服务器和2台转发器以外的且处于同一子网的虚拟机中访问虚拟服务器，观察服务是否正常响应

4. 再次启动MASTER转发器上的keepalived守护进程::

    keepalived -D

5. 读BACKUP转发器的 ``/var/log/syslog`` 日志文件，查看是否有记录表明BACKUP转发器再次切换回BACKUP状态::

    Sep  4 07:04:33 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) Received higher prio advert
    Sep  4 07:04:33 vinzor kernel: [13012.018651] IPVS: stopping master sync thread 2877 ...
    Sep  4 07:04:33 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) Entering BACKUP STATE                      <- 转换回BACKUP状态
    Sep  4 07:04:33 vinzor Keepalived_vrrp[2395]: VRRP_Instance(VI_1) removing protocol VIPs.
    Sep  4 07:04:33 vinzor Keepalived_vrrp[2395]: VRRP_Group(G1) Syncing instances to BACKUP state
    Sep  4 07:04:33 vinzor Keepalived_healthcheckers[2394]: Netlink reflector reports IP 192.168.1.200 removed
    Sep  4 07:04:33 vinzor kernel: [13012.020551] IPVS: sync thread started: state = BACKUP, mcast_ifn = eth0, syncid = 50, id = 0

6. 在3台真实服务器和2台转发器以外的且处于同一子网的虚拟机中访问虚拟服务器，观察服务是否正常响应
