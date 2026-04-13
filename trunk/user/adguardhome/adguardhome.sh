#!/bin/sh

scriptfilepath=$(cd "$(dirname "$0")"; pwd)/$(basename $0)

# ========== DNS 转发配置 ==========
change_dns() {
    if [ "$(nvram get adg_redirect)" = 1 ]; then
        sed -i '/no-resolv/d' /etc/storage/dnsmasq/dnsmasq.conf
        sed -i '/server=127.0.0.1#5335/d' /etc/storage/dnsmasq/dnsmasq.conf
        cat >> /etc/storage/dnsmasq/dnsmasq.conf << EOF
no-resolv
server=127.0.0.1#5335
EOF
        /sbin/restart_dhcpd
        logger -t "【AdGuardHome】" "添加DNS转发到5335端口"
    fi
}

del_dns() {
    sed -i '/no-resolv/d' /etc/storage/dnsmasq/dnsmasq.conf
    sed -i '/server=127.0.0.1#5335/d' /etc/storage/dnsmasq/dnsmasq.conf
    /sbin/restart_dhcpd
}

# ========== iptables 重定向 ==========
set_iptable() {
    if [ "$(nvram get adg_redirect)" = 2 ]; then
        IPS="`ifconfig | grep "inet addr" | grep -v ":127" | grep "Bcast" | awk '{print $2}' | awk -F : '{print $2}'`"
        for IP in $IPS
        do
            iptables -t nat -A PREROUTING -p tcp -d $IP --dport 53 -j REDIRECT --to-ports 5335 >/dev/null 2>&1
            iptables -t nat -A PREROUTING -p udp -d $IP --dport 53 -j REDIRECT --to-ports 5335 >/dev/null 2>&1
        done

        IPS="`ifconfig | grep "inet6 addr" | grep -v " fe80::" | grep -v " ::1" | grep "Global" | awk '{print $3}'`"
        for IP in $IPS
        do
            ip6tables -t nat -A PREROUTING -p tcp -d $IP --dport 53 -j REDIRECT --to-ports 5335 >/dev/null 2>&1
            ip6tables -t nat -A PREROUTING -p udp -d $IP --dport 53 -j REDIRECT --to-ports 5335 >/dev/null 2>&1
        done
        logger -t "【AdGuardHome】" "重定向53端口"
    fi
}

clear_iptable() {
    OLD_PORT="5335"
    IPS="`ifconfig | grep "inet addr" | grep -v ":127" | grep "Bcast" | awk '{print $2}' | awk -F : '{print $2}'`"
    for IP in $IPS
    do
        iptables -t nat -D PREROUTING -p udp -d $IP --dport 53 -j REDIRECT --to-ports $OLD_PORT >/dev/null 2>&1
        iptables -t nat -D PREROUTING -p tcp -d $IP --dport 53 -j REDIRECT --to-ports $OLD_PORT >/dev/null 2>&1
    done

    IPS="`ifconfig | grep "inet6 addr" | grep -v " fe80::" | grep -v " ::1" | grep "Global" | awk '{print $3}'`"
    for IP in $IPS
    do
        ip6tables -t nat -D PREROUTING -p udp -d $IP --dport 53 -j REDIRECT --to-ports $OLD_PORT >/dev/null 2>&1
        ip6tables -t nat -D PREROUTING -p tcp -d $IP --dport 53 -j REDIRECT --to-ports $OLD_PORT >/dev/null 2>&1
    done
}

# ========== 配置文件生成 ==========
getconfig() {
    adg_file="/etc/storage/adg.sh"
    if [ ! -f "$adg_file" ] || [ ! -s "$adg_file" ] ; then
        cat > "$adg_file" <<-\EEE
http:
  address: 0.0.0.0:3030
auth_name: admin
auth_pass: admin
language: zh-cn
dns:
  bind_host: 0.0.0.0
  port: 5335
  ratelimit: 0
  upstream_dns:
  - tcp://1.0.0.1
  bootstrap_dns: tcp://1.0.0.1
  all_servers: true
tls:
  enabled: false
EEE
        chmod 755 "$adg_file"
    fi
}

# ========== 重启/守护逻辑 ==========
adg_renum=`nvram get adg_renum`

adg_restart () {
    relock="/var/lock/AdGuardHome_restart.lock"
    if [ "$1" = "o" ] ; then
        nvram set adg_renum="0"
        [ -f $relock ] && rm -f $relock
        return 0
    fi
    if [ "$1" = "x" ] ; then
        adg_renum=${adg_renum:-"0"}
        adg_renum=`expr $adg_renum + 1`
        nvram set adg_renum="$adg_renum"
        if [ "$adg_renum" -gt 3 ] ; then
            I=19
            echo $I > $relock
            logger -t "【AdGuardHome】" "多次尝试启动失败，等待【"`cat $relock`"分钟】后自动尝试重新启动"
            while [ $I -gt 0 ]; do
                I=$(($I - 1))
                echo $I > $relock
                sleep 60
                [ "$(nvram get adg_renum)" = "0" ] && break
                [ $I -lt 0 ] && break
            done
            nvram set adg_renum="1"
        fi
        [ -f $relock ] && rm -f $relock
    fi
    start_adg
}

# ========== 查找二进制 ==========
find_bin() {
    SVC_PATH="$(nvram get adg_bin)"
    dirs="/etc/storage/bin /tmp/AdGuardHome /usr/bin"
    if [ -z "$SVC_PATH" ] ; then
        for dir in $dirs ; do
            if [ -f "$dir/AdGuardHome" ] && [ -x "$dir/AdGuardHome" ]; then
                SVC_PATH="$dir/AdGuardHome"
                break
            fi
        done
        [ -z "$SVC_PATH" ] && SVC_PATH="/tmp/AdGuardHome/AdGuardHome"
    fi
}

# ========== 检查二进制是否有效（使用 --help 代替 -h）==========
is_valid_binary() {
    [ -f "$1" ] && [ -x "$1" ] && "$1" --help >/dev/null 2>&1 && [ $? -eq 0 ]
}

# ========== 下载 AdGuardHome（固定链接 + 自动切换软/硬浮点）==========
dl_adg() {
    find_bin
    if is_valid_binary "$SVC_PATH"; then
        logger -t "【AdGuardHome】" "已存在有效二进制: $SVC_PATH"
        return 0
    fi

    logger -t "【AdGuardHome】" "找不到有效的 $SVC_PATH ，开始下载 AdGuardHome 程序"
    
    tag="v0.107.54"
    # 尝试顺序：软浮点 -> 硬浮点
    versions="linux_mipsle_softfloat linux_mipsle"
    
    adg_path=$(dirname "$SVC_PATH")
    [ ! -d "$adg_path" ] && mkdir -p "$adg_path"
    
    success=0
    for version in $versions; do
        download_url="https://github.com/AdguardTeam/AdGuardHome/releases/download/${tag}/AdGuardHome_${version}.tar.gz"
        tmp_dir="/tmp/AdGuardHome_download_${version}"
        rm -rf "$tmp_dir"
        mkdir -p "$tmp_dir"
        cd "$tmp_dir" || continue
        
        logger -t "【AdGuardHome】" "尝试下载版本: ${version}"
        for proxy in $github_proxys ; do
            logger -t "【AdGuardHome】" "从 ${proxy}${download_url} 下载"
            rm -f "AdGuardHome.tar.gz"
            curl -Lkso "AdGuardHome.tar.gz" "${proxy}${download_url}" 2>/dev/null || wget --no-check-certificate -q -O "AdGuardHome.tar.gz" "${proxy}${download_url}" 2>/dev/null
            if [ "$?" = 0 ] && [ -s "AdGuardHome.tar.gz" ]; then
                tar -xzvf "AdGuardHome.tar.gz" >/dev/null 2>&1
                if [ $? -eq 0 ]; then
                    if [ -f "AdGuardHome/AdGuardHome" ]; then
                        cp "AdGuardHome/AdGuardHome" "$SVC_PATH"
                    else
                        find . -maxdepth 1 -name "AdGuardHome" -type f -exec cp {} "$SVC_PATH" \;
                    fi
                    if [ -f "$SVC_PATH" ]; then
                        chmod +x "$SVC_PATH"
                        if is_valid_binary "$SVC_PATH"; then
                            logger -t "【AdGuardHome】" "成功下载并解压: ${version}"
                            success=1
                            break 3
                        else
                            logger -t "【AdGuardHome】" "版本 ${version} 无法运行，尝试下一个"
                            rm -f "$SVC_PATH"
                        fi
                    fi
                else
                    logger -t "【AdGuardHome】" "解压失败"
                fi
            else
                logger -t "【AdGuardHome】" "下载失败"
            fi
        done
        cd /tmp
        rm -rf "$tmp_dir"
    done
    
    if [ $success -eq 0 ]; then
        logger -t "【AdGuardHome】" "所有版本均失败，请手动下载适合你路由器的版本解压到 $SVC_PATH"
        return 1
    fi
    return 0
}

github_proxys="$(nvram get github_proxy)"
[ -z "$github_proxys" ] && github_proxys=" "

# ========== 守护进程 ==========
adg_keep() {
    logger -t "【AdGuardHome】" "守护进程启动"
    if [ -s /tmp/script/_opt_script_check ]; then
        sed -Ei '/【AdGuardHome】|^$/d' /tmp/script/_opt_script_check
        cat >> "/tmp/script/_opt_script_check" <<-OSC
[ -z "\`pidof AdGuardHome\`" ] && logger -t "进程守护" "AdGuardHome 进程掉线" && eval "$scriptfilepath start &" && sed -Ei '/【AdGuardHome】|^$/d' /tmp/script/_opt_script_check #【AdGuardHome】
OSC
    fi
}

# ========== 启动主逻辑 ==========
start_adg() {
    mkdir -p /tmp/AdGuardHome
    mkdir -p /etc/storage/AdGuardHome
    logger -t "【AdGuardHome】" "正在启动..."
    sed -Ei '/【AdGuardHome】|^$/d' /tmp/script/_opt_script_check
    
    find_bin
    if ! is_valid_binary "$SVC_PATH"; then
        dl_adg
        if ! is_valid_binary "$SVC_PATH"; then
            logger -t "【AdGuardHome】" "无法获得有效的 AdGuardHome 二进制，启动失败"
            return 1
        fi
    fi
    
    adgenable=$(nvram get adg_enable)
    if [ "$adgenable" = "1" ] ; then
        getconfig
        change_dns
        set_iptable
        logger -t "【AdGuardHome】" "运行 $SVC_PATH"
        eval "$SVC_PATH -c $adg_file -w /tmp/AdGuardHome -v" &
        sleep 4
        if [ ! -z "`pidof AdGuardHome`" ] ; then
            mem=$(cat /proc/$(pidof AdGuardHome)/status | grep -w VmRSS | awk '{printf "%.1f MB", $2/1024}')
            cpui="$(top -b -n1 | grep -E "$(pidof AdGuardHome)" 2>/dev/null| grep -v grep | awk '{for (i=1;i<=NF;i++) {if ($i ~ /AdGuardHome/) break; else cpu=i}} END {print $cpu}')"
            logger -t "【AdGuardHome】" "运行成功！"
            logger -t "【AdGuardHome】" "内存占用 ${mem} CPU占用 ${cpui}%"
            adg_restart o
        else
            logger -t "【AdGuardHome】" "运行失败，10 秒后自动尝试重新启动"
            sleep 10
            adg_restart x
        fi
        adg_keep
    fi
}

# ========== 停止逻辑 ==========
stop_adg() {
    scriptname=$(basename $0)
    sed -Ei '/【AdGuardHome】|^$/d' /tmp/script/_opt_script_check
    rm -rf /tmp/AdGuardHome
    killall -9 AdGuardHome 2>/dev/null
    killall AdGuardHome 2>/dev/null
    del_dns
    clear_iptable
    logger -t "【AdGuardHome】" "关闭AdGuardHome"
    if [ ! -z "$scriptname" ] ; then
        eval $(ps -w | grep "$scriptname" | grep -v $$ | grep -v grep | awk '{print "kill "$1";";}')
        eval $(ps -w | grep "$scriptname" | grep -v $$ | grep -v grep | awk '{print "kill -9 "$1";";}')
    fi
}

# ========== 入口 ==========
case $1 in
start)
    start_adg &
    ;;
stop)
    stop_adg
    ;;
*)
    echo "Usage: $0 {start|stop}"
    ;;
esac
