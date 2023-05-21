#pragma once

#include <Windows.h>
#include <regex>

#include "memcury.h"
#include "settings.h"

#include "../vendor/cURL/curl.h"
#include "url.h"

// #define HYBRID_ENABLED

inline CURLcode(*CurlSetOpt)(struct Curl_easy*, CURLoption, va_list) = nullptr;
inline CURLcode(*CurlEasySetOpt)(struct Curl_easy*, CURLoption, ...) = nullptr;

inline CURLcode CurlSetOpt_(struct Curl_easy* data, CURLoption option, ...)
{
	va_list arg;
	va_start(arg, option);

	const auto result = CurlSetOpt(data, option, arg);

	va_end(arg);
	return result;
}

#define XOR

inline CURLcode CurlEasySetOptDetour(struct Curl_easy* data, CURLoption tag, ...)
{
	va_list arg;
	va_start(arg, tag);

	CURLcode result = {};

	if (!data) return CURLE_BAD_FUNCTION_ARGUMENT;

	//Ssl bypass
	if (tag == CURLOPT_SSL_VERIFYPEER)
	{
		result = CurlSetOpt_(data, tag, 0);
	}

	//Disable Proxy
	if (tag == CURLOPT_PROXY)
	{
		result = CurlSetOpt_(data, tag, "");
	}

	//URL redirection
	else if (tag == CURLOPT_URL)
	{
		std::string url = va_arg(arg, char*);

		Uri uri = Uri::Parse(url);

#ifdef URL_HOST

		if (uri.Host.ends_with(XOR("ol.epicgames.com"))
			|| uri.Host.ends_with(XOR(".akamaized.net"))
			|| uri.Host.ends_with(XOR("on.epicgames.com"))
			|| uri.Host.ends_with(XOR("game-social.epicgames.com"))
			|| uri.Host.contains(XOR("superawesome.com")))
		{
			url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
		}

#endif
		result = CurlSetOpt_(data, tag, url.c_str());
	}	

	else
	{
		result = CurlSetOpt(data, tag, arg);
	}

	va_end(arg);
	return result;
}


__int64 sigscan(const std::string& Str) { return Memcury::Scanner::FindPattern(Str.c_str()).Get(); }