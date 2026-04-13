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

# ========== 获取系统架构 ==========
get_arch() {
    arch=$(uname -m)
    case "$arch" in
        mips|mipsle)
            # 判断是否支持软浮点
            if [ -f /lib/libc.so.0 ] && grep -q "soft-float" /lib/libc.so.0 2>/dev/null; then
                echo "linux_mipsle_softfloat"
            else
                echo "linux_mipsle"
            fi
            ;;
        armv7l|armv6l)
            echo "linux_armv7"
            ;;
        aarch64)
            echo "linux_arm64"
            ;;
        x86_64)
            echo "linux_amd64"
            ;;
        i*86)
            echo "linux_386"
            ;;
        *)
            logger -t "【AdGuardHome】" "未知架构: $arch，尝试使用 mipsle_softfloat"
            echo "linux_mipsle_softfloat"
            ;;
    esac
}

# ========== 查找或下载二进制 ==========
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

# 检查二进制是否有效
is_valid_binary() {
    [ -f "$1" ] && [ -x "$1" ] && "$1" -h >/dev/null 2>&1 && [ $? -eq 0 ]
}

dl_adg() {
    find_bin
    if is_valid_binary "$SVC_PATH"; then
        logger -t "【AdGuardHome】" "已存在有效二进制: $SVC_PATH"
        return 0
    fi

    logger -t "【AdGuardHome】" "找不到有效的 $SVC_PATH ，开始下载 AdGuardHome 程序"
    get_tag
    arch=$(get_arch)
    filename="AdGuardHome_${arch}.tar.gz"
    download_url="https://github.com/AdguardTeam/AdGuardHome/releases/download/${tag}/${filename}"
    
    adg_path=$(dirname "$SVC_PATH")
    [ ! -d "$adg_path" ] && mkdir -p "$adg_path"
    
    tmp_dir="/tmp/AdGuardHome_download"
    mkdir -p "$tmp_dir"
    cd "$tmp_dir" || return 1
    
    logger -t "【AdGuardHome】" "下载 ${tag} 版本，架构: ${arch}"
    for proxy in $github_proxys ; do
        logger -t "【AdGuardHome】" "尝试从 ${proxy}${download_url} 下载"
        curl -Lkso "AdGuardHome.tar.gz" "${proxy}${download_url}" || wget --no-check-certificate -q -O "AdGuardHome.tar.gz" "${proxy}${download_url}"
        if [ "$?" = 0 ] && [ -s "AdGuardHome.tar.gz" ]; then
            # 解压，去除顶层目录
            tar -xzvf "AdGuardHome.tar.gz" --strip-components=1 -C "$adg_path" >/dev/null 2>&1
            if [ $? -eq 0 ]; then
                rm -f "AdGuardHome.tar.gz"
                chmod +x "$SVC_PATH"
                if is_valid_binary "$SVC_PATH"; then
                    logger -t "【AdGuardHome】" "下载并解压成功: $SVC_PATH"
                    # 清理多余文件
                    rm -f "$adg_path"/{LICENSE.txt,README.md,CHANGELOG.md,AdGuardHome.sig}
                    cd /tmp
                    rm -rf "$tmp_dir"
                    return 0
                else
                    logger -t "【AdGuardHome】" "下载的文件无法运行，可能架构不匹配"
                    rm -f "$SVC_PATH"
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
    logger -t "【AdGuardHome】" "所有下载源均失败，请手动下载 ${download_url} 解压到 $SVC_PATH"
    return 1
}

get_tag() {
    user_agent='Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36'
    curltest=`which curl`
    logger -t "【AdGuardHome】" "开始获取最新版本..."
    if [ -z "$curltest" ] || [ ! -s "`which curl`" ] ; then
        tag="$( wget --no-check-certificate -T 5 -t 3 --user-agent "$user_agent" --output-document=-  https://api.github.com/repos/AdguardTeam/AdGuardHome/releases/latest 2>&1 | grep 'tag_name' | cut -d\" -f4 )"
        [ -z "$tag" ] && tag="$( wget --no-check-certificate -T 5 -t 3 --user-agent "$user_agent" --quiet --output-document=-  https://api.github.com/repos/AdguardTeam/AdGuardHome/releases/latest  2>&1 | grep 'tag_name' | cut -d\" -f4 )"
    else
        tag="$( curl -k --connect-timeout 3 --user-agent "$user_agent"  https://api.github.com/repos/AdguardTeam/AdGuardHome/releases/latest 2>&1 | grep 'tag_name' | cut -d\" -f4 )"
        [ -z "$tag" ] && tag="$( curl -Lk --connect-timeout 3 --user-agent "$user_agent" -s  https://api.github.com/repos/AdguardTeam/AdGuardHome/releases/latest  2>&1 | grep 'tag_name' | cut -d\" -f4 )"
    fi
    [ -z "$tag" ] && logger -t "【AdGuardHome】" "无法获取最新版本，使用默认版本 v0.107.54" && tag="v0.107.54"
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
    # 确保二进制有效，否则下载
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
    # 杀掉自身其他实例
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
