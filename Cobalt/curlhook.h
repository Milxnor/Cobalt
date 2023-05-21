#pragma once

#include <Windows.h>
#include <regex>

#include "curldefinitions.h"
#include "memcury.h"

// #define HYBRID_ENABLED

auto (*curl_easy_setopt_original)(CURL* Curl, uintptr_t opt, ...)->CURLcode;

std::string FNhost = "127.0.0.1:3551";

//This routine is used for setting up curl. we will be hijacking this to change the values.
auto Hijacked_curl_easy_setopt(CURL* Curl, uintptr_t opt, va_list info) -> CURLcode
{
    int OFF = 0;
    switch (opt)
    {
    case CURLOPT_NOPROXY:
        return curl_easy_setopt_original(Curl, opt, "");
        break;

    case CURLOPT_SSL_VERIFYPEER:
        return curl_easy_setopt_original(Curl, opt, OFF);
        break;
    case CURLOPT_SSL_VERIFYHOST:
        return curl_easy_setopt_original(Curl, opt, OFF);
        break;
    case CURLOPT_PINNEDPUBLICKEY:
        return CURLcode::CURLE_OK;
        break;
    case CURLOPT_URL:
        std::string url = info;
        std::regex Host(("(.*).ol.epicgames.com"));
#ifdef HYBRID_ENABLED
        if (std::regex_search(info, std::regex(("/fortnite/api/cloudstorage/system")))) {
            url = std::regex_replace(info, Host, FNhost);
        }
        else if (std::regex_search(info, std::regex(("/fortnite/api/v2/versioncheck/")))) {
            url = std::regex_replace(info, Host, FNhost);
        }
        else if (std::regex_search(info, std::regex(("/fortnite/api/game/v2/profile")))) {
            url = std::regex_replace(info, Host, FNhost);
        }
        else if (std::regex_search(info, std::regex(("/content/api/pages/fortnite-game")))) {
            url = std::regex_replace(info, Host, FNhost);
        }
        else if (std::regex_search(info, std::regex(("/affiliate/api/public/affiliates/slug")))) {
            url = std::regex_replace(info, Host, FNhost);
        }
        else if (std::regex_search(info, std::regex(("/socialban/api/public/v1")))) {
            url = std::regex_replace(info, Host, FNhost);
        }
        /* else if (std::regex_search(info, std::regex(ENC("player.platform")))) { // idk if this even works
            if (version == S13)
            {
                url = std::regex_replace(info, Host, "IOS");
                Log(ENC("Changed Windows to IOS"));
            }
        } */
#else
        if (std::regex_search(info, Host)) {
            url = std::regex_replace(info, Host, FNhost);
        }
#endif
        return curl_easy_setopt_original(Curl, opt, url.c_str());
        break;
    }
    return curl_easy_setopt_original(Curl, opt, info);
}

__int64 sigscan(const std::string& Str) { return Memcury::Scanner::FindPattern(Str.c_str()).Get(); }