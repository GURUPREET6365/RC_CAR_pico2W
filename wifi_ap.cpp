#include "wifi_ap.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>

// Tell the C++ compiler that this header belongs to a C file
extern "C" {
    #include "dhcpserver.h"
}

bool start_access_point()
{
    const char* ssid = "atlas car network";
    const char* password = "123456789";

    // Initialize CYW43 Wi-Fi chip + lwIP
    if (cyw43_arch_init())
    {
        printf("Failed to initialize CYW43\n");
        return false;
    }

    printf("CYW43 initialized\n");

    // Start Wi-Fi Access Point
    cyw43_arch_enable_ap_mode(
        ssid,
        password,
        CYW43_AUTH_WPA2_AES_PSK
    );

    printf("Access Point started!\n");
    printf("SSID: %s\n", ssid);

    // --------------------------------------------------
    // DHCP SERVER
    // --------------------------------------------------

    // Must be static so it stays alive after the function ends
    static dhcp_server_t dhcp_server; 

    ip4_addr_t gateway;
    ip4_addr_t netmask;

    ip4_addr_set_u32(&gateway, IPADDR_ANY); 
    // Using Pico's default IP macro (usually 192.168.4.1)
    gateway.addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    netmask.addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

    dhcp_server_init(
        &dhcp_server,
        &cyw43_state.netif[CYW43_ITF_AP],
        (ip_addr_t*)&gateway,
        (ip_addr_t*)&netmask
    );

    printf("DHCP server started! Ready for phones to connect.\n");

    return true;
}