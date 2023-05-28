# Cobalt
SSL Bypass for Fortnite.

# Changing port or ip.

Go to `settings.h` and change URL_HOST and URL_PORT to what you want.

# Enabling hybrid

Go to `settings.h` and change the variable `CobaltUsage` to `ECobaltUsage::Hybrid`.

# How to connect to XMPP with fiddler open (credits Lawin)

All you have to do is put this into your fiddler script.

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

# Credits

Memcury - https://github.com/kem0x/Memcury<br>
Neonite++ for the signatures and curl hook - https://github.com/PeQuLeaks/NeonitePP-Fixed/tree/1.4<br>
