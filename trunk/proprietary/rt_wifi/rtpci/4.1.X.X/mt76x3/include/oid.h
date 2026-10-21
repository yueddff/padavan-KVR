/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	oid.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
*/
#ifndef _OID_H_
#define _OID_H_

/*#include <linux/wireless.h> */



/* new types for Media Specific Indications */
/* Extension channel offset */
#define EXTCHA_NONE			0
#define EXTCHA_ABOVE		0x1
#define EXTCHA_BELOW		0x3

/* BW */
#define BAND_WIDTH_20		0
#define BAND_WIDTH_40		1
#define BAND_WIDTH_80		2
#define BAND_WIDTH_160	    3
#define BAND_WIDTH_10		4	/* 802.11j has 10MHz. This definition is for internal usage. doesn't fill in the IE or other field. */
#define BAND_WIDTH_5		5
#define BAND_WIDTH_8080		6
#define BAND_WIDTH_BOTH		7	/* BW20 + BW40 */
#define BAND_WIDTH_25		8
#define BAND_WIDTH_NUM		9


/* SHORTGI */
#define GAP_INTERVAL_400	1	/* only support in HT mode */
#define GAP_INTERVAL_800	0
#define GAP_INTERVAL_BOTH	2

#define NdisMediaStateConnected			1
#define NdisMediaStateDisconnected		0

#define NdisApMediaStateConnected			1
#define NdisApMediaStateDisconnected		0


#define NDIS_802_11_LENGTH_SSID         32

#define MAC_ADDR_LEN			6
#define IEEE80211_ADDR_LEN		6	/* size of 802.11 address */
#define IEEE80211_NWID_LEN		32

#define NDIS_802_11_LENGTH_RATES        8
#define NDIS_802_11_LENGTH_RATES_EX     16

#define OID_P2P_DEVICE_NAME_LEN	32
/*#define MAX_NUM_OF_CHS						49 */ /* 14 channels @2.4G +  12@UNII + 4 @MMAC + 11 @HiperLAN2 + 7 @Japan + 1 as NULL terminationc */
/*#define MAX_NUM_OF_CHS             			54 */ /* 14 channels @2.4G +  12@UNII(lower/middle) + 16@HiperLAN2 + 11@UNII(upper) + 0 @Japan + 1 as NULL termination */
#define MAX_NUMBER_OF_EVENT				10	/* entry # in EVENT table */

#ifdef BB_SOC
#define MAX_NUMBER_OF_MAC				16 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211
#else
#if defined(MT7603_FPGA) || defined(MT7628_FPGA) || defined(MT7636_FPGA)
#define MAX_NUMBER_OF_MAC				4	/* if MAX_MBSSID_NUM is 8, this value can't be larger than 211 */
#elif defined(MT7603)
#ifdef ECONET_ALPHA_RELEASE
#define MAX_NUMBER_OF_MAC				8
#else
#ifdef MAC_REPEATER_SUPPORT
#ifdef MULTI_APCLI_SUPPORT
#define MAX_NUMBER_OF_MAC				(75 - (16 + 1) * 2)
#else
#define MAX_NUMBER_OF_MAC				(75 - (16 + 1) * 1)
#endif /* MULTI_APCLI_SUPPORT */
#else
#define MAX_NUMBER_OF_MAC            	75
#endif /* MAC_REPEATER_SUPPORT */
#endif
#elif defined(MT76x2)
#ifdef MAC_REPEATER_SUPPORT				//((MAX_EXT_MAC_ADDR_SIZE + 1) * MAC_APCLI_NUM)
#define MAX_NUMBER_OF_MAC               (116 - ((16 + 1) * 1))
#else
#define MAX_NUMBER_OF_MAC			   	 116
#endif /* MAC_REPEATER_SUPPORT */
#else
#define MAX_NUMBER_OF_MAC				32	/* if MAX_MBSSID_NUM is 8, this value can't be larger than 211 */
#endif /* defined(MT7603_FPGA) || defined(MT7628_FPGA) */
#endif /* BB_SOC */

#define MAX_NUMBER_OF_ACL				64
#define MAX_LENGTH_OF_SUPPORT_RATES		12	/* 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54 */
#define MAX_NUMBER_OF_DLS_ENTRY			4

#ifdef MAC_REPEATER_SUPPORT
/*seems different chip have different size , use 16 (minimum size) tmply */
#define MAX_NUMBER_OF_MAC_LIST			16  /* MAX_EXT_MAC_ADDR_SIZE */
#endif /* MAC_REPEATER_SUPPORT */


#define RT_QUERY_SIGNAL_CONTEXT				0x0402
#define RT_SET_IAPP_PID                 	0x0404
#define RT_SET_APD_PID						0x0405
#define RT_SET_DEL_MAC_ENTRY				0x0406
#define RT_QUERY_EVENT_TABLE            	0x0407
#ifdef DOT11R_FT_SUPPORT
#define RT_SET_FT_STATION_NOTIFY			0x0408
#define RT_SET_FT_KEY_REQ					0x0409
#define RT_SET_FT_KEY_RSP					0x040a
#define RT_FT_KEY_SET						0x040b
#define RT_FT_DATA_ENCRYPT					0x040c
#define RT_FT_DATA_DECRYPT					0x040d
#define RT_FT_NEIGHBOR_REPORT				0x040e
#define RT_FT_NEIGHBOR_REQUEST				0x040f
#define RT_FT_NEIGHBOR_RESPONSE				0x0410
#define RT_FT_ACTION_FORWARD				0x0411
#endif /* DOT11R_FT_SUPPORT */
/* */
/* IEEE 802.11 OIDs */
/* */
#define	OID_GET_SET_TOGGLE			0x8000
#define	OID_GET_SET_FROM_UI			0x4000

#define	OID_802_11_NETWORK_TYPES_SUPPORTED			0x0103
#define	OID_802_11_NETWORK_TYPE_IN_USE				0x0104
#define	OID_802_11_RSSI_TRIGGER						0x0107
#define	RT_OID_802_11_RSSI							0x0108	/* rt2860 only */
#define	RT_OID_802_11_RSSI_1						0x0109	/* rt2860 only */
#define	RT_OID_802_11_RSSI_2						0x010A	/* rt2860 only */
#define	OID_802_11_NUMBER_OF_ANTENNAS				0x010B
#define	OID_802_11_RX_ANTENNA_SELECTED				0x010C
#define	OID_802_11_TX_ANTENNA_SELECTED				0x010D
#define	OID_802_11_SUPPORTED_RATES					0x010E
#define	OID_802_11_ADD_WEP							0x0112
#define	OID_802_11_REMOVE_WEP						0x0113
#define	OID_802_11_DISASSOCIATE						0x0114
#define	OID_802_11_PRIVACY_FILTER					0x0118
#define	OID_802_11_ASSOCIATION_INFORMATION			0x011E
#define	OID_802_11_TEST								0x011F


#define	RT_OID_802_11_COUNTRY_REGION				0x0507
#define	OID_802_11_BSSID_LIST_SCAN					0x0508
#define	OID_802_11_SSID								0x0509
#define	OID_802_11_BSSID							0x050A
#define	RT_OID_802_11_RADIO							0x050B
#define	RT_OID_802_11_PHY_MODE						0x050C
#define	RT_OID_802_11_STA_CONFIG					0x050D
#define	OID_802_11_DESIRED_RATES					0x050E
#define	RT_OID_802_11_PREAMBLE						0x050F
#define	OID_802_11_WEP_STATUS						0x0510
#define	OID_802_11_AUTHENTICATION_MODE				0x0511
#define	OID_802_11_INFRASTRUCTURE_MODE				0x0512
#define	RT_OID_802_11_RESET_COUNTERS				0x0513
#define	OID_802_11_RTS_THRESHOLD					0x0514
#define	OID_802_11_FRAGMENTATION_THRESHOLD			0x0515
#define	OID_802_11_POWER_MODE						0x0516
#define	OID_802_11_TX_POWER_LEVEL					0x0517
#define	RT_OID_802_11_ADD_WPA						0x0518
#define	OID_802_11_REMOVE_KEY						0x0519
#define	RT_OID_802_11_QUERY_PID						0x051A
#define	RT_OID_802_11_QUERY_VID						0x051B
#define	OID_802_11_ADD_KEY							0x0520
#define	OID_802_11_CONFIGURATION					0x0521
#define	OID_802_11_TX_PACKET_BURST					0x0522
#define	RT_OID_802_11_QUERY_NOISE_LEVEL				0x0523
#define	RT_OID_802_11_EXTRA_INFO					0x0524
#define	RT_OID_802_11_HARDWARE_REGISTER				0x0525
#define OID_802_11_ENCRYPTION_STATUS            OID_802_11_WEP_STATUS
#define OID_802_11_DEAUTHENTICATION                 0x0526
#define OID_802_11_DROP_UNENCRYPTED                 0x0527
#define OID_802_11_MIC_FAILURE_REPORT_FRAME         0x0528
#define OID_802_11_EAP_METHOD						0x0529
#define OID_802_11_ACL_LIST							0x052A
#define OID_802_11_FROAM_ACL_ADD_ENTRY				0x252B
#define OID_802_11_FROAM_ACL_DEL_ENTRY				0x252C

#if (defined(VENDOR_FEATURE6_SUPPORT) || defined(MAP_SUPPORT))
#define OID_802_11_COEXISTENCE						0x0530
#endif

/* For 802.1x daemin using */
#ifdef DOT1X_SUPPORT
#define OID_802_DOT1X_CONFIGURATION					0x0540
#define OID_802_DOT1X_PMKID_CACHE					0x0541
#define OID_802_DOT1X_RADIUS_DATA					0x0542
#define OID_802_DOT1X_WPA_KEY						0x0543
#define OID_802_DOT1X_STATIC_WEP_COPY				0x0544
#define OID_802_DOT1X_IDLE_TIMEOUT					0x0545
#define OID_802_DOT1X_RADIUS_ACL_NEW_CACHE          0x0546
#define OID_802_DOT1X_RADIUS_ACL_DEL_CACHE          0x0547
#define OID_802_DOT1X_RADIUS_ACL_CLEAR_CACHE        0x0548
#define OID_802_DOT1X_QUERY_STA_AID                 0x0549
#endif /* DOT1X_SUPPORT */

#define	RT_OID_DEVICE_NAME							0x0607
#define	RT_OID_VERSION_INFO							0x0608
#define	OID_802_11_BSSID_LIST						0x0609
#define	OID_802_3_CURRENT_ADDRESS					0x060A
#define	OID_GEN_MEDIA_CONNECT_STATUS				0x060B
#define	RT_OID_802_11_QUERY_LINK_STATUS				0x060C
#define	OID_802_11_RSSI								0x060D
#define	OID_802_11_STATISTICS						0x060E
#define	OID_GEN_RCV_OK								0x060F
#define	OID_GEN_RCV_NO_BUFFER						0x0610
#define	RT_OID_802_11_QUERY_EEPROM_VERSION			0x0611
#define	RT_OID_802_11_QUERY_FIRMWARE_VERSION		0x0612
#define	RT_OID_802_11_QUERY_LAST_RX_RATE			0x0613
#define	RT_OID_802_11_TX_POWER_LEVEL_1				0x0614
#define	RT_OID_802_11_QUERY_PIDVID					0x0615
/*for WPA_SUPPLICANT_SUPPORT */
#define OID_SET_COUNTERMEASURES                     0x0616
#define OID_802_11_SET_IEEE8021X                    0x0617
#define OID_802_11_SET_IEEE8021X_REQUIRE_KEY        0x0618
#define OID_802_11_PMKID                            0x0620
#define RT_OID_WPA_SUPPLICANT_SUPPORT               0x0621
#define RT_OID_WE_VERSION_COMPILED                  0x0622
#define RT_OID_NEW_DRIVER                           0x0623
#define	OID_AUTO_PROVISION_BSSID_LIST				0x0624
#define RT_OID_WPS_PROBE_REQ_IE						0x0625

#define	RT_OID_802_11_SNR_0							0x0630
#define	RT_OID_802_11_SNR_1							0x0631
#define	RT_OID_802_11_QUERY_LAST_TX_RATE			0x0632
#define	RT_OID_802_11_QUERY_HT_PHYMODE				0x0633
#define	RT_OID_802_11_SET_HT_PHYMODE				0x0634
#define	OID_802_11_RELOAD_DEFAULTS					0x0635
#define	RT_OID_802_11_QUERY_APSD_SETTING			0x0636
#define	RT_OID_802_11_SET_APSD_SETTING				0x0637
#define	RT_OID_802_11_QUERY_APSD_PSM				0x0638
#define	RT_OID_802_11_SET_APSD_PSM					0x0639
#define	RT_OID_802_11_QUERY_DLS						0x063A
#define	RT_OID_802_11_SET_DLS						0x063B
#define	RT_OID_802_11_QUERY_DLS_PARAM				0x063C
#define	RT_OID_802_11_SET_DLS_PARAM					0x063D
#define RT_OID_802_11_QUERY_WMM              		0x063E
#define RT_OID_802_11_SET_WMM      					0x063F
#define RT_OID_802_11_QUERY_IMME_BA_CAP				0x0640
#define RT_OID_802_11_SET_IMME_BA_CAP				0x0641
#define RT_OID_802_11_QUERY_BATABLE					0x0642
#define RT_OID_802_11_ADD_IMME_BA					0x0643
#define RT_OID_802_11_TEAR_IMME_BA					0x0644
#define RT_OID_DRIVER_DEVICE_NAME                   0x0645
#define RT_OID_802_11_QUERY_DAT_HT_PHYMODE          0x0646
#define OID_WAPP_EVENT					        	0x0647
#define OID_802_11_SET_PSPXLINK_MODE				0x0648
/*+++ add by woody +++*/

#ifdef WH_EZ_SETUP
#define OID_802_11_PASSPHRASE						0x0649
#else
#define OID_802_11_SET_PASSPHRASE					0x0649
#endif
#define RT_OID_802_11_QUERY_TX_PHYMODE                          0x0650
#define RT_OID_802_11_QUERY_MAP_REAL_TX_RATE                          0x0678
#define RT_OID_802_11_QUERY_MAP_REAL_RX_RATE                          0x0679
#define	RT_OID_802_11_SNR_2							0x067A
#define RT_OID_802_11_PER_BSS_STATISTICS			0x067D
#define OID_802_11_MBSS_GET_STA_COUNT					0x067E



#ifdef HOSTAPD_SUPPORT
#define SIOCSIWGENIE	0x8B30
#define OID_HOSTAPD_SUPPORT               0x0661

#define HOSTAPD_OID_STATIC_WEP_COPY   0x0662
#define HOSTAPD_OID_GET_1X_GROUP_KEY   0x0663

#define HOSTAPD_OID_SET_STA_AUTHORIZED   0x0664
#define HOSTAPD_OID_SET_STA_DISASSOC   0x0665
#define HOSTAPD_OID_SET_STA_DEAUTH   0x0666
#define HOSTAPD_OID_DEL_KEY   0x0667
#define HOSTAPD_OID_SET_KEY   0x0668
#define HOSTAPD_OID_SET_802_1X   0x0669
#define HOSTAPD_OID_GET_SEQ   0x0670
#define HOSTAPD_OID_GETWPAIE                 0x0671
#define HOSTAPD_OID_COUNTERMEASURES 0x0672
#define HOSTAPD_OID_SET_WPAPSK 0x0673
#define HOSTAPD_OID_SET_WPS_BEACON_IE 0x0674
#define HOSTAPD_OID_SET_WPS_PROBE_RESP_IE 0x0675

#define	RT_HOSTAPD_OID_HOSTAPD_SUPPORT				(OID_GET_SET_TOGGLE |	OID_HOSTAPD_SUPPORT)
#define	RT_HOSTAPD_OID_STATIC_WEP_COPY				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_STATIC_WEP_COPY)
#define	RT_HOSTAPD_OID_GET_1X_GROUP_KEY				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_GET_1X_GROUP_KEY)
#define	RT_HOSTAPD_OID_SET_STA_AUTHORIZED			(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_STA_AUTHORIZED)
#define	RT_HOSTAPD_OID_SET_STA_DISASSOC				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_STA_DISASSOC)
#define	RT_HOSTAPD_OID_SET_STA_DEAUTH				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_STA_DEAUTH)
#define	RT_HOSTAPD_OID_DEL_KEY						(OID_GET_SET_TOGGLE |	HOSTAPD_OID_DEL_KEY)
#define	RT_HOSTAPD_OID_SET_KEY						(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_KEY)
#define	RT_HOSTAPD_OID_SET_802_1X						(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_802_1X)
#define	RT_HOSTAPD_OID_COUNTERMEASURES				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_COUNTERMEASURES)
#define	RT_HOSTAPD_OID_SET_WPAPSK				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_WPAPSK)
#define	RT_HOSTAPD_OID_SET_WPS_BEACON_IE				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_WPS_BEACON_IE)
#define	RT_HOSTAPD_OID_SET_WPS_PROBE_RESP_IE				(OID_GET_SET_TOGGLE |	HOSTAPD_OID_SET_WPS_PROBE_RESP_IE)

#define IEEE80211_IS_MULTICAST(_a) (*(_a) & 0x01)
#define	IEEE80211_KEYBUF_SIZE	16
#define	IEEE80211_MICBUF_SIZE	(8 + 8)	/* space for both tx+rx keys */
#define IEEE80211_TID_SIZE		17	/* total number of TIDs */

#define	IEEE80211_MLME_ASSOC		    1	/* associate station */
#define	IEEE80211_MLME_DISASSOC		    2	/* disassociate station */
#define	IEEE80211_MLME_DEAUTH		    3	/* deauthenticate station */
#define	IEEE80211_MLME_AUTHORIZE	    4	/* authorize station */
#define	IEEE80211_MLME_UNAUTHORIZE	    5	/* unauthorize station */
#define IEEE80211_MLME_CLEAR_STATS	    6	/* clear station statistic */
#define IEEE80211_1X_COPY_KEY        	7	/* copy static-wep unicast key */

#define	IEEE80211_MAX_OPT_IE	256
#define IWEVEXPIRED	0x8C04

struct ieee80211req_mlme {
	UINT8 im_op;		/* operation to perform */
	UINT8 im_ssid_len;	/* length of optional ssid */
	UINT16 im_reason;	/* 802.11 reason code */
	UINT8 im_macaddr[IEEE80211_ADDR_LEN];
	UINT8 im_ssid[IEEE80211_NWID_LEN];
};

struct ieee80211req_key {
	UINT8 ik_type;		/* key/cipher type */
	UINT8 ik_pad;
	UINT16 ik_keyix;	/* key index */
	UINT8 ik_keylen;	/* key length in bytes */
	UINT8 ik_flags;
	UINT8 ik_macaddr[IEEE80211_ADDR_LEN];
	UINT64 ik_keyrsc;	/* key receive sequence counter */
	UINT64 ik_keytsc;	/* key transmit sequence counter */
	UINT8 ik_keydata[IEEE80211_KEYBUF_SIZE + IEEE80211_MICBUF_SIZE];
	int txkey;
};

struct ieee80211req_del_key {
	UINT8 idk_keyix;	/* key index */
	UINT8 idk_macaddr[IEEE80211_ADDR_LEN];
};

struct default_group_key {
	UINT16 ik_keyix;	/* key index */
	UINT8 ik_keylen;	/* key length in bytes */
	UINT8 ik_keydata[IEEE80211_KEYBUF_SIZE + IEEE80211_MICBUF_SIZE];
};

struct ieee80211req_wpaie {
	UINT8 wpa_macaddr[IEEE80211_ADDR_LEN];
	UINT8 rsn_ie[IEEE80211_MAX_OPT_IE];
};

struct hostapd_wpa_psk {
	struct hostapd_wpa_psk *next;
	int group;
	UCHAR psk[32];
	UCHAR addr[6];
};

#endif /*HOSTAPD_SUPPORT */

#define RT_OID_802_11_QUERY_TDLS_PARAM			0x0676
#define	RT_OID_802_11_QUERY_TDLS				0x0677

/* Ralink defined OIDs */
/* Dennis Lee move to platform specific */

#define	RT_OID_802_11_BSSID					  (OID_GET_SET_TOGGLE |	OID_802_11_BSSID)
#define	RT_OID_802_11_SSID					  (OID_GET_SET_TOGGLE |	OID_802_11_SSID)
#define	RT_OID_802_11_INFRASTRUCTURE_MODE	  (OID_GET_SET_TOGGLE |	OID_802_11_INFRASTRUCTURE_MODE)
#define	RT_OID_802_11_ADD_WEP				  (OID_GET_SET_TOGGLE |	OID_802_11_ADD_WEP)
#define	RT_OID_802_11_ADD_KEY				  (OID_GET_SET_TOGGLE |	OID_802_11_ADD_KEY)
#define	RT_OID_802_11_REMOVE_WEP			  (OID_GET_SET_TOGGLE |	OID_802_11_REMOVE_WEP)
#define	RT_OID_802_11_REMOVE_KEY			  (OID_GET_SET_TOGGLE |	OID_802_11_REMOVE_KEY)
#define	RT_OID_802_11_DISASSOCIATE			  (OID_GET_SET_TOGGLE |	OID_802_11_DISASSOCIATE)
#define	RT_OID_802_11_AUTHENTICATION_MODE	  (OID_GET_SET_TOGGLE |	OID_802_11_AUTHENTICATION_MODE)
#define	RT_OID_802_11_PRIVACY_FILTER		  (OID_GET_SET_TOGGLE |	OID_802_11_PRIVACY_FILTER)
#define	RT_OID_802_11_BSSID_LIST_SCAN		  (OID_GET_SET_TOGGLE |	OID_802_11_BSSID_LIST_SCAN)
#define	RT_OID_802_11_WEP_STATUS			  (OID_GET_SET_TOGGLE |	OID_802_11_WEP_STATUS)
#define	RT_OID_802_11_RELOAD_DEFAULTS		  (OID_GET_SET_TOGGLE |	OID_802_11_RELOAD_DEFAULTS)
#define	RT_OID_802_11_NETWORK_TYPE_IN_USE	  (OID_GET_SET_TOGGLE |	OID_802_11_NETWORK_TYPE_IN_USE)
#define	RT_OID_802_11_TX_POWER_LEVEL		  (OID_GET_SET_TOGGLE |	OID_802_11_TX_POWER_LEVEL)
#define	RT_OID_802_11_RSSI_TRIGGER			  (OID_GET_SET_TOGGLE |	OID_802_11_RSSI_TRIGGER)
#define	RT_OID_802_11_FRAGMENTATION_THRESHOLD (OID_GET_SET_TOGGLE |	OID_802_11_FRAGMENTATION_THRESHOLD)
#define	RT_OID_802_11_RTS_THRESHOLD			  (OID_GET_SET_TOGGLE |	OID_802_11_RTS_THRESHOLD)
#define	RT_OID_802_11_RX_ANTENNA_SELECTED	  (OID_GET_SET_TOGGLE |	OID_802_11_RX_ANTENNA_SELECTED)
#define	RT_OID_802_11_TX_ANTENNA_SELECTED	  (OID_GET_SET_TOGGLE |	OID_802_11_TX_ANTENNA_SELECTED)
#define	RT_OID_802_11_SUPPORTED_RATES		  (OID_GET_SET_TOGGLE |	OID_802_11_SUPPORTED_RATES)
#define	RT_OID_802_11_DESIRED_RATES			  (OID_GET_SET_TOGGLE |	OID_802_11_DESIRED_RATES)
#define	RT_OID_802_11_CONFIGURATION			  (OID_GET_SET_TOGGLE |	OID_802_11_CONFIGURATION)
#define	RT_OID_802_11_POWER_MODE			  (OID_GET_SET_TOGGLE |	OID_802_11_POWER_MODE)
#define RT_OID_802_11_SET_PSPXLINK_MODE		  (OID_GET_SET_TOGGLE |	OID_802_11_SET_PSPXLINK_MODE)
#define RT_OID_802_11_EAP_METHOD			  (OID_GET_SET_TOGGLE |	OID_802_11_EAP_METHOD)
#ifdef WH_EZ_SETUP
#define RT_OID_802_11_SET_PASSPHRASE		  (OID_GET_SET_TOGGLE | OID_802_11_PASSPHRASE)
#else
#define RT_OID_802_11_SET_PASSPHRASE		  (OID_GET_SET_TOGGLE | OID_802_11_SET_PASSPHRASE)
#endif

#ifdef DOT1X_SUPPORT
#define RT_OID_802_DOT1X_PMKID_CACHE		(OID_GET_SET_TOGGLE | OID_802_DOT1X_PMKID_CACHE)
#define RT_OID_802_DOT1X_RADIUS_DATA		(OID_GET_SET_TOGGLE | OID_802_DOT1X_RADIUS_DATA)
#define RT_OID_802_DOT1X_WPA_KEY			(OID_GET_SET_TOGGLE | OID_802_DOT1X_WPA_KEY)
#define RT_OID_802_DOT1X_STATIC_WEP_COPY	(OID_GET_SET_TOGGLE | OID_802_DOT1X_STATIC_WEP_COPY)
#define RT_OID_802_DOT1X_IDLE_TIMEOUT		(OID_GET_SET_TOGGLE | OID_802_DOT1X_IDLE_TIMEOUT)
#endif /* DOT1X_SUPPORT */

#define RT_OID_802_11_SET_TDLS_PARAM			(OID_GET_SET_TOGGLE | RT_OID_802_11_QUERY_TDLS_PARAM)
#define RT_OID_802_11_SET_TDLS				(OID_GET_SET_TOGGLE | RT_OID_802_11_QUERY_TDLS)


#ifdef WAPI_SUPPORT
#define OID_802_11_WAPI_PID					0x06A0
#define OID_802_11_PORT_SECURE_STATE		0x06A1
#define OID_802_11_UCAST_KEY_INFO			0x06A2
#define OID_802_11_MCAST_TXIV				0x06A3
#define OID_802_11_MCAST_KEY_INFO			0x06A4
#define OID_802_11_WAPI_CONFIGURATION		0x06A5
#define OID_802_11_WAPI_IE					0x06A6

#define RT_OID_802_11_WAPI_PID				(OID_GET_SET_TOGGLE | OID_802_11_WAPI_PID)
#define RT_OID_802_11_PORT_SECURE_STATE		(OID_GET_SET_TOGGLE | OID_802_11_PORT_SECURE_STATE)
#define RT_OID_802_11_UCAST_KEY_INFO		(OID_GET_SET_TOGGLE | OID_802_11_UCAST_KEY_INFO)
#define RT_OID_802_11_MCAST_TXIV			(OID_GET_SET_TOGGLE | OID_802_11_MCAST_TXIV)
#define RT_OID_802_11_MCAST_KEY_INFO		(OID_GET_SET_TOGGLE | OID_802_11_MCAST_KEY_INFO)
#define RT_OID_802_11_WAPI_CONFIGURATION	(OID_GET_SET_TOGGLE | OID_802_11_WAPI_CONFIGURATION)
#define RT_OID_802_11_WAPI_IE				(OID_GET_SET_TOGGLE | OID_802_11_WAPI_IE)
#endif /* WAPI_SUPPORT */
#ifdef ACL_BLK_COUNT_SUPPORT
#define OID_802_11_ACL_BLK_REJCT_COUNT_STATICS			0x069b
#endif/*ACL_BLK_COUNT_SUPPORT*/

/*
	The full range (1-4,095) of VLAN IDs must be supported by the 802.1Q
	implementation.
	VLAN ID 0 is reserved.
*/

#define RT_VLAN_8023_HEADER_COPY(__pAd, __VLAN_VID, __VLAN_Priority,		\
								__pHeader8023, __HdrLen, __pData, __TPID)	\
{																			\
	VLAN_8023_Header_Copy(__VLAN_VID, __VLAN_Priority,					\
								__pHeader8023, __HdrLen, __pData,			\
								__TPID);					\
}


#define RT_80211_TO_8023_PACKET(__pAd, __VLAN_VID, __VLAN_Priority,			\
								__pRxBlk, __pHeader802_3,					\
								_wdev_idx, __TPID)					\
{																			\
	wlan_802_11_to_802_3_packet(											\
				get_netdev_from_bssid(__pAd, _wdev_idx),				\
				__pRxBlk->OpMode,										\
				__VLAN_VID, __VLAN_Priority,							\
				__pRxBlk->pRxPacket, __pRxBlk->pData, __pRxBlk->DataSize,	\
				__pHeader802_3, __TPID);					\
}

// TODO: shiang-usw, fix me for pEntry->apidx to func_tb_idx
#define RTMP_L2_FRAME_TX_ACTION(__pAd, __ApIdx, __FrameBuf, __FrameLen)		\
	RTMPL2FrameTxAction(__pAd, get_netdev_from_bssid(__pAd, __ApIdx),		\
						announce_802_3_packet, __ApIdx, __FrameBuf, __FrameLen, __pAd->OpMode)

#define RTMP_UPDATE_OS_PACKET_INFO(__pAd, __pRxBlk, _wdev_idx)		\
	RtmpOsPktInit(__pRxBlk->pRxPacket,									\
					get_netdev_from_bssid(__pAd, _wdev_idx),			\
					__pRxBlk->pData, __pRxBlk->DataSize);

#ifdef SYSTEM_LOG_SUPPORT
/*
	RTMPSendWirelessEvent --> RtmpOsSendWirelessEvent --> RtmpDrvSendWirelessEvent
*/
#define RTMPSendWirelessEvent(__pAd, __Event_flag, __pAddr, _wdev_idx, __Rssi)	\
	RtmpOsSendWirelessEvent(__pAd, __Event_flag, __pAddr, _wdev_idx, __Rssi,		\
							RtmpDrvSendWirelessEvent);
#else
#define RTMPSendWirelessEvent(__pAd, __Event_flag, __pAddr, __BssIdx, __Rssi)
#endif /* SYSTEM_LOG_SUPPORT */

#define RTMP_OS_TASK_INIT(__pTask, __pTaskName, __pAd)		\
	RtmpOSTaskInit(__pTask, __pTaskName, __pAd, &(__pAd)->RscTaskMemList, &(__pAd)->RscSemMemList);
#define VALID_UCAST_ENTRY_WCID(_wcid)  (_wcid < MAX_LEN_OF_MAC_TABLE)
#endif /* __RTMP_DEF_H__ */