# Cobalt
There is a better alternative for other platforms but it is only for private servers (and limited versions) available at [Sinum](https://github.com/projectnovafn/Sinum)
# Private server for latest does not work because of EOS (I believe added in S22 or something?)
SSL Bypass for Fortnite.<br>
Hopefully when I have a lot of time I'll make a new SSL Bypass with a BUNCH of features.

# How to change the port or IP

Go to `settings.h` and change URL_HOST and URL_PORT to what you want.

# How to enable hybrid

Go to `settings.h` and change the variable `CobaltUsage` to `ECobaltUsage::Hybrid`.

# There is no public way to launch latest anymore without the anticheat!

# OUTDATED: You can literally open the game with a proxy open now lol, but I'll keep this here
## Recording requests on latest

[Download LawinServerV1](https://github.com/Lawin0129/LawinServer) and run `install_packages.bat` and then `start.bat`.<br><br>
In Cobalt, change the mode to hybrid ([tutorial](https://github.com/Milxnor/Cobalt#how-to-enable-hybrid)) and make sure `URL_HOST` is `127.0.0.1` (localhost) and `URL_PORT` is 3551 (default LawinServerV1 port) in `settings.h`.

## How to have Fiddler open and connect to XMPP and matchmaker

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

Put this into your FiddlerScript.

```
import Fiddler;

class Handlers
{
    static function OnBeforeRequest(oSession: Session) {
        if (oSession.hostname.StartsWith("xmpp-service")) 
        {
            oSession.oRequest.headers.UriScheme = "https";
        }
        if (oSession.hostname.StartsWith("fortnite-matchmaking")) 
        {
            oSession.oRequest.headers.UriScheme = "https";
        }
        if (oSession.hostname.StartsWith("fortnite-mm")) 
        {
            oSession.oRequest.headers.UriScheme = "https";
        }
    }
        
    static function OnBeforeResponse(oSession: Session) {
        oSession.utilReplaceInResponse("wss://", "ws://");
    }
}
```

# Credits

Memcury - https://github.com/kem0x/Memcury<br>
Neonite++ for the signatures and curl hook - https://github.com/PeQuLeaks/NeonitePP-Fixed/tree/1.4<br>
