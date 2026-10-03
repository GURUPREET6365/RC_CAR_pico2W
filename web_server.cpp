#include "web_server.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

// Global variables for the main loop to read
volatile CarCommand current_command = CMD_STOP;
volatile int current_gear = 1;

// HTTP Response headers
const char* http_ok_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
const char* http_ok_json = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n{}";

// Your HTML (Compressed slightly to save memory buffer space)
const char* html_page = R"=====(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"><title>ATLAS CAR</title>
<style>
*{box-sizing:border-box;user-select:none}
body{background:#151515;color:#fff;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;font-family:sans-serif}
.ctrl{text-align:center;width:100%;max-width:400px}
h1{margin:0 0 5px;letter-spacing:2px}
.stat{color:#999;margin-bottom:20px}
#cg{color:#4da3ff}
.gears{display:flex;justify-content:center;gap:10px;margin-bottom:30px}
.g{width:70px;height:50px;background:#242424;border:2px solid #3d3d3d;color:#fff;border-radius:10px;font-size:20px;font-weight:bold}
.g.sel{background:#087ff5;border-color:#4da3ff}
.pad{display:flex;justify-content:space-around}
.col{display:flex;flex-direction:column;gap:15px}
.btn{width:100px;height:100px;background:#292929;color:#fff;border:none;border-radius:25px;font-size:40px;box-shadow:0 6px 0 #0c0c0c}
.btn.act{background:#087ff5;transform:translateY(4px);box-shadow:0 2px 0 #0c0c0c}
.stop{width:220px;height:60px;margin-top:30px;background:#d32f2f;color:#fff;border:none;border-radius:15px;font-size:20px;font-weight:bold;box-shadow:0 6px 0 #791919}
.stop:active{transform:translateY(4px);box-shadow:0 2px 0 #791919}
</style></head><body>
<div class="ctrl"><h1>ATLAS CAR</h1><div class="stat">Gear: <strong id="cg">1</strong></div>
<div class="gears"><button class="g sel" data-g="1">1</button><button class="g" data-g="2">2</button><button class="g" data-g="3">3</button></div>
<div class="pad"><div class="col"><button class="btn" data-c="/left">◀</button><button class="btn" data-c="/right">▶</button></div>
<div class="col"><button class="btn" data-c="/forward">▲</button><button class="btn" data-c="/backward">▼</button></div></div>
<button class="stop" id="st">STOP</button></div>
<script>
let act=null,cg=1,st=document.getElementById('st'),gb=document.querySelectorAll('.g'),bb=document.querySelectorAll('.btn');
const snd=c=>fetch(c).catch(()=>{});
gb.forEach(b=>b.onclick=()=>{gb.forEach(x=>x.classList.remove('sel'));b.classList.add('sel');document.getElementById('cg').innerText=b.dataset.g;snd('/gear/'+b.dataset.g);});
const stop=()=>{if(act){act.classList.remove('act');act=null;snd('/stop');}};
bb.forEach(b=>{b.onpointerdown=e=>{e.preventDefault();b.setPointerCapture(e.pointerId);if(act&&act!==b)stop();act=b;b.classList.add('act');snd(b.dataset.c);};
b.onpointerup=e=>{e.preventDefault();stop();};b.onpointercancel=e=>{e.preventDefault();stop();};b.onlostpointercapture=()=>stop();});
st.onpointerdown=e=>{e.preventDefault();stop();};
window.onblur=stop;document.onvisibilitychange=()=>document.hidden&&stop();document.oncontextmenu=e=>e.preventDefault();document.ontouchmove=e=>e.preventDefault();
</script></body></html>
)=====";


// Callback when data arrives from the browser
// Callback when data arrives from the browser
static err_t server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        // Browser closed the connection
        tcp_close(tpcb);
        return ERR_OK;
    }

    // 1. CRITICAL FIX: Tell the lwIP network stack we successfully received the data!
    // Without this, the connection hangs and the browser says "Site can't be reached"
    tcp_recved(tpcb, p->tot_len);

    char* request = (char*)p->payload;

    // 2. Check if the browser is asking for the main web page
    if (strstr(request, "GET / ") || strstr(request, "GET /index.html")) {
        // 3. MEMORY FIX: Changed TCP_WRITE_FLAG_COPY to 0. 
        // This prevents the Pico from running out of RAM when sending the large HTML string.
        tcp_write(tpcb, http_ok_html, strlen(http_ok_html), 0);
        tcp_write(tpcb, html_page, strlen(html_page), 0);
    } 
    // 4. Check for Drive Commands
    else if (strstr(request, "GET /forward")) { current_command = CMD_FORWARD; tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    else if (strstr(request, "GET /backward")){ current_command = CMD_BACKWARD;tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    else if (strstr(request, "GET /left"))    { current_command = CMD_LEFT;    tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    else if (strstr(request, "GET /right"))   { current_command = CMD_RIGHT;   tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    else if (strstr(request, "GET /stop"))    { current_command = CMD_STOP;    tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    // 5. Check for Gear Commands
    else if (strstr(request, "GET /gear/1"))  { current_gear = 1;              tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    else if (strstr(request, "GET /gear/2"))  { current_gear = 2;              tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    else if (strstr(request, "GET /gear/3"))  { current_gear = 3;              tcp_write(tpcb, http_ok_json, strlen(http_ok_json), 0); }
    
    // Send the data and close the connection
    tcp_output(tpcb);
    pbuf_free(p);
    tcp_close(tpcb);

    return ERR_OK;
}

// Callback when a new device connects to Port 80
static err_t server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, server_recv);
    return ERR_OK;
}

// Initialization function
void start_web_server() {
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 80); // Bind to port 80 (standard HTTP port)
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, server_accept);
    
    printf("Web server is running! Navigate to http://192.168.4.1\n");
}