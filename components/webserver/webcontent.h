/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#ifndef UNODE_WEBCONTENT_H
#define UNODE_WEBCONTENT_H

static const char mime_html[] = "text/html";

static const char web_common_header[] = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>&mu;Node</title> <style>body{padding: 10px} body, form{text-align: center; font-family: Verdana, Geneva, Tahoma, sans-serif; display: flex; flex-direction: column; max-width: 400px; margin: auto}h1{margin-bottom: 0}input{margin-bottom: 10px}label, div{text-align: left}a{text-decoration: none; color: #33C3F0}a:hover{color: #289abd}.btn{background-color: #33C3F0; color: white; border-radius: 4px; border: none; height: 38px; line-height: 38px; margin-top: 10px}.btn:hover{background-color: #289abd; color: white}</style></head><body> <h1>&mu;Node</h1> <i>by vivedo</i><br/> <small>";
static const char web_common_header2[] = "</small><a href=\"/\">&larr; Back</a>";
static const char web_index[] = "</small><a href=\"/wifi\" class=\"btn\">WiFi Settings</a> <a href=\"/dmx\" class=\"btn\">DMX Settings</a> </body></html>";
static const char web_wifi_settings[] = "<form action=\"/wifi\" method=\"POST\"> <h2>WiFi Settings</h2> <label>SSID</label> <input type=\"text\" name=\"wifi_ssid\" placeholder=\"SSID\" required value=\"%s\"> <label>Password</label> <input type=\"text\" name=\"wifi_pswd\" placeholder=\"Password\" required> <input type=\"submit\" value=\"Apply\" class=\"btn\"> </form> <small><i> Applying will disconnect the &mu;Node from WiFi.<br>If connection to new network fails the &mu;Node will fallback to the default AP mode. </i></small></body></html>";
static const char web_dmx_settings[] = "<form action=\"/dmx\" method=\"POST\"> <h2>DMX Config</h2> <label>Protocol</label> <div> <input type=\"radio\" name=\"pr\" id=\"pa\" value=\"a\" %s> <label for=\"pa\">ArtNet 4</label> <input type=\"radio\" name=\"pr\" id=\"ps\" value=\"s\" %s> <label for=\"ps\">E1.31/sACN</label> </div><label>Port A Universe</label> <input type=\"number\" name=\"au\" min=\"0\" value=\"%d\"> <label>Port B Universe</label> <input type=\"number\" name=\"bu\" min=\"0\" value=\"%d\"> <input type=\"submit\" value=\"Save\" class=\"btn\"> </form></body></html>";


#endif //UNODE_WEBCONTENT_H
