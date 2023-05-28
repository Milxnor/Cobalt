# Cobalt
SSL Bypass for Fortnite.

# Changing port or ip.

Go to `settings.h` and change URL_HOST and URL_PORT to what you want.

# Enabling hybrid

Go to `settings.h` and change the variable `CobaltUsage` to `ECobaltUsage::Hybrid`.

# Recording Requests on latest

Download [LawinServerV1](https://github.com/Lawin0129/LawinServer).<br>
Remember to change the mode to hybrid and make sure URL_HOST is right and URL_PORT is 3551 (default LawinServerV1 port).

## How to have fiddler and connect to XMPP

Thanks [Lawin](https://github.com/Lawin0129) for this method.<br>
Go to the `CloudStorage` folder in your LawinServerV1 and open `DefaultEngine.ini` and change it to

```
[OnlineSubsystemMcp.Xmpp]
ServerAddr="ws://xmpp-service-prod.ol.epicgames.com"
Protocol=ws
bUseSSL=false

[OnlineSubsystemMcp.Xmpp Prod]
ServerAddr="ws://xmpp-service-prod.ol.epicgames.com"
Protocol=ws
bUseSSL=false
```

Put this into your fiddler script.

```
import Fiddler;

class Handlers
{
    static function OnBeforeRequest(oSession: Session) {
        if (oSession.hostname.StartsWith("xmpp-service")) 
        {
            oSession.oRequest.headers.UriScheme = "https";
        }
    }
}
```

## Launching latest

Go to your Win64 folder of latest, replace `FortniteClient-Win64-Shipping_BE.exe`, `FortniteClient-Win64-Shipping_EAC.exe`, and `FortniteClient-Win64-Shipping_EAC_EOS.exe` with `FortniteClient-Win64-Shipping.exe`.<br>
![image](https://github.com/Milxnor/Cobalt/assets/84999745/aca1fee6-6751-48c7-b81d-70618e5a46f5)<br>
Launch fiddler, and when you get the error, inject Cobalt and then you should load in lobby a few seconds later.

# Credits

Memcury - https://github.com/kem0x/Memcury<br>
Neonite++ for the signatures and curl hook - https://github.com/PeQuLeaks/NeonitePP-Fixed/tree/1.4<br>
